#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/socket.h>

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

	fd_set readset, tmpset;
	FD_ZERO(&readset);
	FD_SET(serv_fd, &readset);
	int maxfd = serv_fd;
	for(;;) {
		tmpset = readset;
		int num;
		if((num = select(maxfd+1, &tmpset, NULL, NULL, NULL)) < 0) break;
		if(num == 0) continue;
		for(int i=0; i<=maxfd; ++i) {
			if(FD_ISSET(i, &tmpset)) {
				if(i == serv_fd) {
					struct sockaddr_in clnt_addr = {0};
					socklen_t socklen = sizeof(clnt_addr);
					int clnt_fd = accept(i, (struct sockaddr *)&clnt_addr, &socklen);
					if(clnt_fd < 0) continue;
					FD_SET(clnt_fd, &readset);
					if(maxfd < clnt_fd) maxfd = clnt_fd;
					puts("New client.");
				} else {
					char buf[1024];
					int len = read(i, buf, sizeof(buf));
					if(len <= 0) {
						FD_CLR(i, &readset);
						close(i);
					} else {
						write(i, buf, len);
					}
				}
			}
		}
	}
	close(serv_fd);
	return 0;
}

