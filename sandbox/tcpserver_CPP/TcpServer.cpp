#include "TcpServer.hpp"


/**************************************************************/
/*                      CANONICAL FORM                        */
/**************************************************************/

TcpServer::TcpServer()
	: _serverPort(SERVER_PORT), _serverIp(IP_ADDRESS), _maxConnections(MAX_CONNECTIONS), 
	_maxGetSize(MAX_GET_SIZE), _serverFd(-1)
{
	_setupServer();
	std::cout << "server initialized & listening on port: " << _serverPort << std::endl;
	_initFdSet();
}

TcpServer::~TcpServer()
{
	if ( _serverFd != -1 )
		close(_serverFd);
	/* if (_client_fd != -1)
		close(_client_fd); */
}

/**************************************************************/
/*                     PRIVATE METHODS                        */
/**************************************************************/

void	TcpServer::_setupServer()
{
	// Create socket
	if ( (_serverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		std::cout << "Error: setup server (socket() failed)" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	// Reusable Port and Socket
	int on = 1;
	if ( setsockopt(_serverFd, SOL_SOCKET,  SO_REUSEADDR | SO_REUSEPORT,
				&on, sizeof(int)) < 0 )
	{
		std::cout << "Error: setup server (setsockopt() failed)" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	// Set socket to non-blocking
	if(fcntl(_serverFd, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "Error: setup server (fcntl() failed)" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	// Identify a socket (assigning transport address to socket)
	struct sockaddr_in	server_addr;

	memset( &server_addr, 0, sizeof(server_addr) );
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(_serverIp);
	server_addr.sin_port = htons(_serverPort);
	if ( bind(_serverFd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0 )
	{
		std::cout << "Error: setup server (bind() failed)" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	// Listen for connections on a socket (mark socket as passive)
	if( listen(_serverFd, _maxConnections) < 0 )
    {
		std::cout << "Error: setup server (listen() failed)" << std::endl;
		return ;
	}
}

void	TcpServer::_initFdSet()
{
	FD_ZERO(&_fdSet);
	FD_SET(_serverFd, &_fdSet);
}

int	TcpServer::_acceptNewConnection()
{
	int					addr_size = sizeof(struct sockaddr_in);
    struct sockaddr_in	client_addr;
    int					clientFd;

	clientFd = accept( _serverFd, 
			(struct sockaddr *)&client_addr, 
			(socklen_t *)&addr_size);
	return clientFd;
}

void	TcpServer::_handleConnection( int clientFd )
{
	char	buffer[_maxGetSize];
	ssize_t	bytesRead;

	if ((bytesRead = recv(clientFd, &buffer, 1023, 0)) < 0)
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
	
	close( clientFd );
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

	int		client_socket;
	fd_set	fdSet_tmp;

	while (42)
	{
		FD_ZERO(&fdSet_tmp);
		fdSet_tmp = _fdSet; // copy _fdSet b/c select() modifies it

		// select for non-blocking I/O
		if ( select(FD_SETSIZE, &fdSet_tmp, NULL, NULL, NULL) < 0 )
		{
			std::cerr << "Error select(): " << strerror(errno) << std::endl;
			std::exit(EXIT_FAILURE);
		}

		// check all file descriptors
		for ( int i = 0; i < FD_SETSIZE; i++ )
		{
			if ( FD_ISSET(i, &fdSet_tmp) )
			{
				// two cases:
				if (i == _serverFd) // 1. new connection that we want to accept
				{
					client_socket = this->_acceptNewConnection();
					FD_SET(client_socket, &fdSet_tmp);
				}
				else // 2. read data from an existing connection
				{
					this->_handleConnection( client_socket );
					FD_CLR(i, &fdSet_tmp);
				}
			}
		}
	}
}
