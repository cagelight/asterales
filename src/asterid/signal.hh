#pragma once

#include <functional>

#include "asterid/synchro.hh"

namespace asterid {
	struct connection;
	struct observer;
	template <typename ... T> struct signal;
}

struct asterid::connection {
	virtual ~connection() = default;
};

struct asterid::observer {
	std::vector<std::unique_ptr<connection>> connections;
};

template <typename ... T> struct asterid::signal {
	using func_t = std::function<void(T ...)>;
	using id_t = uint16_t;
	using cb_t = std::pair<id_t, func_t>;
	
	struct shared_data {
		shared_data(signal * parent) : parent(parent) {}
		signal * parent;
		asterid::rw_spinlock accessor;
	};
	
	struct connection : public asterid::connection {
		id_t id;
		std::shared_ptr<shared_data> data;
		
		connection(id_t id, std::shared_ptr<shared_data> data) : id(id), data(data) {}
		
		~connection() {
			data->accessor.read_access();
			if (data->parent) {
				data->parent->disconnect(id);
			}
			data->accessor.read_done();
		}
	};
	
	signal() = default;
	~signal() {
		data->accessor.write_lock();
		data->parent = nullptr;
		data->accessor.write_unlock();
	}
	
	void connect(observer & ob, func_t f) {
		mutex.lock();
		id_t id = id_incrementor++;
		cbs.emplace_back(id, std::move(f));
		ob.connections.emplace_back(std::make_unique<connection>(id, data));
		mutex.unlock();
	}
		
	
	void disconnect(id_t id) {
		mutex.lock();
		cbs.erase(std::remove_if(cbs.begin(), cbs.end(), [id](auto const & v){return v.first == id;}));
		mutex.unlock();
	}
	
	void fire(T ... args) {
		mutex.lock();
		for (cb_t & cb : cbs) {
			cb.second(args ...);
		}
		mutex.unlock();
	}
	
private:
	
	std::vector<cb_t> cbs;
	id_t id_incrementor = 0;
	asterid::spinlock mutex;
	std::shared_ptr<shared_data> data = std::make_shared<shared_data>(this);
};
