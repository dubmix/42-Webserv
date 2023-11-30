#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>

# define IP_ADDRESS 2130706433
# define SERVER_PORT 18000
# define BUFSIZE 4000

int setup_server(int port, int backlog)
{
    struct sockaddr_in  server_addr;
    int                 server_socket;

    (void)backlog;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        printf("Error: could not create socket\n");
        exit(1);
    }
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(IP_ADDRESS);
    server_addr.sin_port = htons(port);

    // this lets us reuse an IP address or port which has been closed recently
    int on = 1;
    if (setsockopt(server_socket, SOL_SOCKET,  SO_REUSEADDR | SO_REUSEPORT,
								&on, sizeof(int)) < 0)
    {
        printf("error: setsockopt() failed\n");
        close(server_socket);
        exit(1);
    }

    if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        printf("Error: could not bind\n");
        return (0);
    }
    if(listen(server_socket, 10) < 0)
    {
        printf("Error: could not listen\n");
        return (0);
    }
    return (server_socket);
}

int accept_new_connection(int server_socket)
{
    int                 addr_size = sizeof(struct sockaddr_in);
    struct sockaddr_in  client_addr;
    int                 client_socket;


    client_socket = accept(server_socket, 
            (struct sockaddr *)&client_addr, 
            (socklen_t *)&addr_size);
    return (client_socket);
}

char    *request_parser(int client_socket)
{
    char    buffer[BUFSIZE];
    size_t  bytes_read;
    int     msgsize = 0;
    // char    actualpath[PATH_MAX + 1];

    while ((bytes_read = read(client_socket, buffer+msgsize, sizeof(buffer) - msgsize - 1)) > 0)
    {
            msgsize += bytes_read;
            if (msgsize > BUFSIZE - 1 || buffer[msgsize-1] == '\n')
                break;
    }
    buffer[msgsize - 1] = 0;
    printf("%s\n", buffer);
    fflush(stdout);
    return (NULL);
}

void    response_builder(int client_socket, char *buffer)
{
    int     file;
    size_t  bytes_read;
    
    file = open("index.html", O_RDONLY);
    if (file < 0)
    {
        printf("could not open file\n");
        exit(1);
    }
    while ( (bytes_read = read(file, buffer, BUFSIZE - 1)) > 0)
    {
        write (client_socket, buffer, bytes_read);
    }
    close(client_socket);
    close(file);
}

void    *handle_connection(int client_socket)
{
    char    buffer[BUFSIZE];
    size_t  bytes_read;
    int     msgsize = 0;
    // int     file;
    // char    actualpath[PATH_MAX + 1];

    while ((bytes_read = read(client_socket, buffer+msgsize, sizeof(buffer) - msgsize - 1)) > 0)
    {
            msgsize += bytes_read;
            if (msgsize > BUFSIZE - 1 || buffer[msgsize-1] == '\n')
                break;
    }
    buffer[msgsize - 1] = 0;
    printf("%s\n", buffer);
    fflush(stdout);

    snprintf((char *)buffer, sizeof(buffer), "HTTP/1.0 200 OK\r\n\r\nHello");
    write(client_socket, (char *)buffer, strlen((char *)buffer));
    // file = open("index.html", O_RDONLY);
    // if (file < 0)
    // {
    //     printf("could not open file\n");
    //     exit(1);
    // }
    // while ( (bytes_read = read(file, buffer, BUFSIZE - 1)) > 0)
    // {
    //     write (client_socket, buffer, bytes_read);
    // }
    close(client_socket);
    // close(file);
    return (NULL);
}

int main(void)
{
    int     server_socket;
    int     client_socket;
    fd_set  current_sockets;
    fd_set  ready_sockets;
    // char    *request;

    FD_ZERO(&current_sockets);
    server_socket = setup_server(SERVER_PORT, 10);
    FD_SET(server_socket, &current_sockets);

    printf("server listening on port: %i\n", SERVER_PORT);
    while (42)
    {
        ready_sockets = current_sockets;

        if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
        {
            printf("Error, select failed\n");
            exit (1);
        }

        for (int i = 0; i < FD_SETSIZE; i++)
        {
            if (FD_ISSET(i, &ready_sockets))
            {
                // two cases:
                // 1. new connection that we want to accept
                if (i == server_socket)
                {
                    client_socket = accept_new_connection(server_socket);
                    FD_SET(client_socket, &current_sockets);
                }
                else // 2. read data from an existing connection
                {
                    // request = request_parser(client_socket);
                    // response_builder(client_socket, request);
                    handle_connection(client_socket);
                    FD_CLR(i, &current_sockets);
                }
            }
        }
    }
    return (0);
}