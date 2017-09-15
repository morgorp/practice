#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
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

	struct pollfd *fdarr = malloc(MAXOPEN * sizeof(struct pollfd));
	fdarr[0].fd = serv_fd;
	fdarr[0].events = POLLIN;
	int num = 1;
	for(;;) {
		int ret;
		if((ret=poll(fdarr, num, -1)) < 0) break;
		if(fdarr[0].revents & POLLIN) {
			struct sockaddr_in clnt_addr = {0};
			socklen_t socklen = sizeof(clnt_addr);
			int clnt_fd = accept(serv_fd, (struct sockaddr *)&clnt_addr, &socklen);
			if(clnt_fd >= 0) {
				fdarr[num].fd = clnt_fd;
				fdarr[num].events = POLLIN | POLLHUP;
				++num;
				puts("New client.");
			}
			if(ret == 1) continue;
		}
		for(int i=1; i<num; ++i) {
			if((fdarr[i].revents & POLLIN) || (fdarr[i].revents & POLLHUP)) {
				char buf[1024];
				int len = read(fdarr[i].fd, buf, sizeof(buf));
				if(len <= 0 || (fdarr[i].revents & POLLHUP)) {
					close(fdarr[i].fd);
					for(int j=i+1; j<num; ++j) fdarr[j-1] = fdarr[j];
					--num;
					--i;
				} else {
					write(fdarr[i].fd, buf, len);
				}
			}
		}
	}
	free(fdarr);
	close(serv_fd);
	return 0;
}

