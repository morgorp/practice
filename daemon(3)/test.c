#include "daemon.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main()
{
	mydaemon(1, 0);

	int fd = open("log", O_WRONLY|O_CREAT, 0666);
	unsigned short n = 0;
	char str[11];
	for(;;) {
		int len = sprintf(str, "%hu\n", n);
		write(fd, str, len);
		++n;
		sleep(1);
	}
	return 0;
}
