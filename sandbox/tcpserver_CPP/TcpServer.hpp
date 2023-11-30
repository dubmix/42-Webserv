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

# define IP_ADDRESS 2130706433	/* 127.0.0.1 converted to int */
# define SERVER_PORT 18000
# define MAX_CONNECTIONS 10
# define MAX_GET_SIZE 8192


/* Class to run a TCP-Server
*	- default constructor initializes the server attributes
*	- setup_server() creates a socket and binds it to a port
*
*/
class TcpServer
{
	private:
		const int	_serverPort;
		const int	_serverIp;
		const int	_maxConnections;
		const int	_maxGetSize;
		int			_serverFd;
		fd_set		_fdSet;

		void	_setupServer();
		void	_initFdSet();

		int			_acceptNewConnection();
		void		_handleConnection( int client_socket );
		std::string	_responseBuilder();

	public:
		TcpServer();
		~TcpServer();
		
		void	serverRun();

};
