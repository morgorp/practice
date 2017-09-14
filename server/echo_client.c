#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>

extern int errno;

void read_routine(int fd)
{
	char buf[1024];
	for(;;) {
		int len = read(fd, buf, sizeof(buf));
		if(len < 0) {
			if(errno==EAGAIN || errno==EINTR) {
				continue;
			}
			perror("read");
			return;
		}
		if(len == 0) {
			return;
		}
		buf[len] = 0;
		printf("Echo: %s\n", buf);
	}
}

void write_routine(int fd)
{
	char buf[1024];
	for(;;) {
		fgets(buf, sizeof(buf), stdin);
		if(buf[0] == 'q') {
			shutdown(fd, SHUT_WR);
			return;
		}
		write(fd, buf, strlen(buf));
	}
}

int main(int argc, char *argv[])
{
	int port;
	sscanf(argv[2], "%d", &port);

	int fd = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serv_addr = {0};
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(port);

	if(connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect");
	}

	if(fork() == 0) {
		write_routine(fd);
	} else {
		read_routine(fd);
	}

	close(fd);
	return 0;
}

