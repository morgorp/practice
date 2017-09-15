#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAXOPEN 1024

int main(int argc, char *argv[])
{
	int port;
	sscanf(argv[1], "%d", &port);

	int serv_fd = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serv_addr = {0};
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);
	if(bind(serv_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("bind");
		return 1;
	}

	listen(serv_fd, 50);

	int ep_fd = epoll_create(MAXOPEN);
	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = serv_fd;
	epoll_ctl(ep_fd, EPOLL_CTL_ADD, serv_fd, &event);

	struct epoll_event *ep_events = malloc(MAXOPEN * sizeof(struct epoll_event));
	for(;;) {
		int num = epoll_wait(ep_fd, ep_events, MAXOPEN, -1);
		if(num < 0) break;
		for(int i=0; i<num; ++i) {
			if(ep_events[i].data.fd == serv_fd) {
				struct sockaddr_in clnt_addr = {0};
				socklen_t socklen = sizeof(clnt_addr);
				int clnt_fd = accept(serv_fd, (struct sockaddr *)&clnt_addr, &socklen);
				if(clnt_fd >= 0) {
					event.events = EPOLLIN;
					event.data.fd = clnt_fd;
					epoll_ctl(ep_fd, EPOLL_CTL_ADD, clnt_fd, &event);
					puts("New client.");
				}
			} else {	
				char buf[1024];
				int len = read(ep_events[i].data.fd, buf, sizeof(buf));
				if(len <= 0) {
					close(ep_events[i].data.fd);
					epoll_ctl(ep_fd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
				} else {
					write(ep_events[i].data.fd, buf, len);
				}
			}
		}
	}
	close(serv_fd);
	close(ep_fd);
	free(ep_events);
	return 0;
}

