#pragma once

#include <cstddef>
#include <cstring>
#include <vector>
#include <cstdint>

namespace asterid::chameleon {
	
	typedef double element_t;
	typedef uint_fast32_t dim_t;
	typedef std::vector<element_t> storage_t;
	
	struct image {
		image() = default;
		image(dim_t width, dim_t height) : width_(width), height_(height) {
			data_.resize(width * height);
			memset(data_.data(), 0, width * height);
		}
		
		element_t & at(dim_t x, dim_t y) { return data_[y * height_ + x]; }
		element_t const & at(dim_t x, dim_t y) const { return data_[y * height_ + x]; }
		
		dim_t width() const { return width_; }
		dim_t height() const { return height_; }
		
		storage_t const & data() const { return data_; }
		storage_t & data() { return data_; }
		
	private:
		dim_t width_ = 0, height_ = 0;
		storage_t data_;
	};
	
	typedef std::vector<image> compound_storage_t;
	
	struct compound_image {
		compound_image(size_t num_channels, dim_t width, dim_t height) : width_(width), height_(height) {
			for (size_t i = 0; i < num_channels; i++) channels.emplace_back(width, height);
		}
		
		struct accessor {
			compound_image * parent;
			dim_t x, y;
			void operator = (element_t e) { for(size_t i = 0; i < parent->num_channels(); i++) parent->channels[i].at(x, y) = e; }
			element_t & operator [] (size_t i) { return parent->channels[i].at(x, y); }
		};
		
		accessor at(dim_t x, dim_t y) { return accessor { this, x, y }; }
		
		struct const_accessor {
			compound_image const * parent;
			dim_t x, y;
			element_t const & operator [] (size_t i) { return parent->channels[i].at(x, y); }
		};
		
		const_accessor at(dim_t x, dim_t y) const { return const_accessor { this, x, y }; }
		
		dim_t width() const { return width_; }
		dim_t height() const { return height_; }
		size_t num_channels() const { return channels.size(); }
		
	private:
		dim_t width_ = 0, height_ = 0;
		compound_storage_t channels;
	};
}
