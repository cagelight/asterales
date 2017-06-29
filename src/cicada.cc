#include "asterid/cicada.hh"

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>
#include <netdb.h>
#include <fcntl.h>

using namespace asterid::cicada;

socket::~socket() {
	if (FD != -1) {
		shutdown(FD, SHUT_RDWR);
		close(FD);
	}
}

connection::connection(socket && sock) : socket(sock) {
	sock.FD = -1;
}

listener::listener(uint16_t port) {
	
}
