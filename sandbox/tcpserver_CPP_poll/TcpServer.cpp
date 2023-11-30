#include "TcpServer.hpp"


/**************************************************************/
/*                      CANONICAL FORM                        */
/**************************************************************/

TcpServer::TcpServer()
	: _serverPorts(serverPorts), _serverIp(serverIPs), _nbrServers(nbrServers), 
	_maxConnections(MAX_CONNECTIONS), _maxGetSize(MAX_GET_SIZE)
{
	// Initialize all FD with -1
	memset( _serverFds, -1, sizeof(_serverFds) );

	// Initialize all servers
	for( int i = 0; i < _nbrServers; ++i ) {
		_setupServer( i );
		_initFdSet( i );
	}

	std::cout << "server initialized & listening on port" << std::endl;
}

TcpServer::~TcpServer()
{
	for( int i = 0; i < _nbrServers; ++i ) {
		if ( _serverFds[i] != -1 )
			close(_serverFds[i]);
	}
	/* if (_client_fd != -1)
		close(_client_fd); */
}

/**************************************************************/
/*                     PRIVATE METHODS                        */
/**************************************************************/

void	TcpServer::_setupServer( int i )
{
	// Create socket
	if ( (_serverFds[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		std::cout << "Error: setup server (socket() failed)" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	// Reusable Port and Socket
	int on = 1;
	if ( setsockopt(_serverFds[i], SOL_SOCKET,  SO_REUSEADDR | SO_REUSEPORT,
				&on, sizeof(int)) < 0 )
	{
		std::cout << "Error: setup server (setsockopt() failed)" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	// Set socket to non-blocking
	if( fcntl(_serverFds[i], F_SETFL, O_NONBLOCK) < 0 )
	{
		std::cerr << "Error: setup server (fcntl() failed)" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	// Identify a socket (assigning transport address to socket)
	struct sockaddr_in	server_addr;

	memset( &server_addr, 0, sizeof(server_addr) );
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(_serverIp[i]);
	server_addr.sin_port = htons(_serverPorts[i]);
	if ( bind(_serverFds[i], (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0 )
	{
		std::cout << "Error: setup server (bind() failed)" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	// Listen for connections on a socket (mark socket as passive)
	if( listen(_serverFds[i], _maxConnections) < 0 )
    {
		std::cout << "Error: setup server (listen() failed)" << std::endl;
		return ;
	}
}

void	TcpServer::_initFdSet( int i )
{
	_fdSet[i].fd = _serverFds[i];	// set descriptor fd to to listening socket
	_fdSet[i].events = 0;			// Clear the bit array
	_fdSet[i].events = _fdSet[i].events | POLLIN;	// Set the POLLIN bit	
}

int	TcpServer::_acceptNewConnection( int serverFd )
{
	int	clientFd;
	
	errno = 0;

	clientFd = accept(serverFd, NULL, NULL);
	if ( clientFd < 0 && errno != EWOULDBLOCK )
	{
		std::cerr << "error: accept() failed" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	return clientFd;
}

void	TcpServer::_handleConnection( int clientFd )
{
	char	buffer[_maxGetSize];
	ssize_t	bytesRead;

	if ( (bytesRead = recv(clientFd, &buffer, 1023, 0)) < 0 )
	{
		std::cerr << "error: recv()" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	else
	{
		buffer[bytesRead] = '\0';
		std::cout << buffer << std::endl;
	}
	fflush( stdout );

	if (send(clientFd, _responseBuilder().c_str(), strlen(_responseBuilder().c_str()), 0) < 0)
		std::cout << "Error: handle connection (send() failed)" << std::endl;
}

std::string	TcpServer::_responseBuilder()
{
	std::string	crlf = "\r\n";
	std::string	response;

	response += "HTTP/1.1 200 OK" + crlf;			// Status-Line
	response += "Content-Type: text/html" + crlf;	// Entity-Header-Field
	response += "Content-Length: 12" + crlf;		// Entity-Header-Field
	response += crlf;
	response += "Hello World" + crlf;				// message-body
	response += crlf;
	
	return response;
}


/**************************************************************/
/*                      PUBLIC METHODS                        */
/**************************************************************/

void	TcpServer::serverRun()
{
	std::cout << "server running and responding..." << std::endl;

	int	clientSocket;
	int	timeout = TIMEOUT_POLL;
	int	status = 0;

	while (42)
	{
		while ( status == 0 )
		{
			// Verify if a new connection is available
			if ( (status = poll(_fdSet, _nbrServers, timeout)) < 0)
			{
				std::cerr << "error: poll() failed";
				std::exit(EXIT_FAILURE);
			}
		}

		// Verify which server has a new connection
		for (int i = 0; i < _nbrServers; ++i)
		{
			// If the server has a new connection ready
			if ( (_fdSet[i].revents & POLLIN) == POLLIN )
			{
				std::cout << "\r" << "Client connected on server: " << _fdSet[i].fd << "(index: " << i << ")" << std::endl;

				if ( (clientSocket = _acceptNewConnection(_fdSet[i].fd)) >= 0 )
				{
					this->_handleConnection( clientSocket );

					// close the socket
					close( clientSocket );
				}
			}
		}
		status = 0;
	}
}
