NAME  = ./tcpserver
SRCS = tcpserver.c
CFLAGS = -Wall -Werror -Wextra
OBJS = $(SRCS:.cpp=.o) 

all: $(NAME)

$(NAME): $(OBJS)
	cc $(CFLAGS) $(OBJS)-o $(NAME)

%.o: %.cpp
	cc $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

GET:
	curl http://127.0.0.1:18000
