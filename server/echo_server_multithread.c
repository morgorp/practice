#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void *serve_routine(void *arg)
{
	int fd = *((int *)arg);
	char buf[1024];
	int len;
	while((len = read(fd, buf, sizeof(buf))) >0 ) {
		write(fd, buf, len);
	}
	close(fd);
	return NULL;
}

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
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	for(;;) {
		struct sockaddr_in clnt_addr = {0};
		socklen_t socklen = sizeof(clnt_addr);
		int clnt_fd = accept(serv_fd, (struct sockaddr *)&clnt_addr, &socklen);
		if(clnt_fd < 0) continue;

		puts("New client.");

		pthread_t tid;
		pthread_create(&tid, &attr, serve_routine, &clnt_fd);	
	}
	pthread_attr_destroy(&attr);
	close(serv_fd);
	return 0;
}

