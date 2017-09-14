#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void childproc_handle(int sig)
{
	while(waitpid(-1, NULL, 0) > 0);
}

void serve_routine(int fd)
{
	char buf[1024];
	int len;
	while((len = read(fd, buf, sizeof(buf))) >0 ) {
		write(fd, buf, len);
	}
	close(fd);
}

int main(int argc, char *argv[])
{
	int port;
	sscanf(argv[1], "%d", &port);
	
	struct sigaction act;
	act.sa_handler = childproc_handle;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGCHLD, &act, 0);

	int serv_fd = socket(PF_INET, SOCK_STREAM, 0);
	
	struct sockaddr_in serv_addr = {0};
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);
	bind(serv_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	listen(serv_fd, 50);
	
	for(;;) {
		struct sockaddr_in clnt_addr = {0};
		socklen_t socklen = sizeof(clnt_addr);
		int clnt_fd = accept(serv_fd, (struct sockaddr *)&clnt_addr, &socklen);
		if(clnt_fd < 0) continue;

		puts("New client.");

		int pid = fork();
		if(pid == -1) {
			close(clnt_fd);
			continue;
		}

		if(pid == 0) {
			close(serv_fd);
			serve_routine(clnt_fd);
			return 0;
		} else {
			close(clnt_fd);
		}
	}
	close(serv_fd);
	return 0;
}

