#pragma once

#include <iostream>
#include <cstdlib> // for exit()
#include <sys/socket.h> // for socket()
#include <netdb.h> // for struct sockaddr_in
#include <string.h> // for bzero(), strlen()
#include <unistd.h> // for close(), read(), write()
#include <stdio.h> // for printf(), snprintf(), fflush()
#include <fcntl.h> // for open(), O_RDONLY
#include <cerrno> // for errno
#include <poll.h> // for poll

# define MAX_GET_SIZE 8192
# define MAX_CONNECTIONS 20
#define TIMEOUT_POLL 500

const int serverIPs[] = {2130706433, 2130706433, 2130706434}; 	/* 127.0.0.1 converted to int */
const int serverPorts[] = {18000, 20000, 19000};
const int nbrServers = sizeof(serverPorts) / sizeof(serverPorts[0]);

/* Class to run a TCP-Server
*	- default constructor initializes the server attributes
*	- setup_server() creates a socket and binds it to a port
*
*/
class TcpServer
{
	private:
		const int*		_serverPorts;
		const int*		_serverIp;
		const int		_nbrServers;
		const int		_maxConnections;
		const int		_maxGetSize;
		int				_serverFds[nbrServers];
		struct pollfd	_fdSet[nbrServers];

		void	_setupServer( int i );
		void	_initFdSet( int i );

		int			_acceptNewConnection( int serverFd );
		void		_handleConnection( int client_socket );
		std::string	_responseBuilder();

	public:
		TcpServer();
		~TcpServer();
		
		void	serverRun();

};
