#pragma once

// N-THREADED ASYNCHRONOUS TCP SERVICES

#include <netinet/in.h>

namespace asterid::cicada {
	
	struct socket {
		int FD;
		struct sockaddr_in6 addr;
		
		socket() = default;
		virtual ~socket();
	};
	
	struct connection : public socket {
		connection() = delete;
		connection(socket &&);
		connection(connection const & other) = delete;
		connection(connection && other) = default;
		virtual ~connection() = default;
	};
	
	struct listener : public socket {
		listener() = delete;
		listener(uint16_t);
		virtual ~listener() = default;
	};
	
	struct server {
		
	};
	
}
