CC = gcc
OBJFILES = echo_client echo_server_multiproc echo_server_multithread\
	echo_server_select echo_server_poll echo_server_epoll

all : $(OBJFILES)
.PHONY : all

echo_client :
echo_server_multiproc :
echo_server_multithread :
	$(CC) -o $@ -pthread $@.c
echo_server_select :
echo_server_poll :
echo_server_epoll :

client :
	./echo_client 127.0.0.1 12345
.PHONY : client

mp :
	./echo_server_multiproc 12345
.PHONY : mp

mt :
	./echo_server_multithread 12345
.PHONY : mt

select :
	./echo_server_select 12345
.PHONY : select

poll :
	./echo_server_poll 12345
.PHONY : poll

epoll :
	./echo_server_epoll 12345
.PHONY : epoll

clean :
	rm -f $(OBJFILES)
.PHONY : clean

