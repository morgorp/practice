#include "daemon.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/resource.h>

int mydaemon(int nochdir, int noclose)
{
	pid_t pid;

	umask(0);

	if( ( pid = fork() ) < 0 ) {
		return -1;
	}
	if(pid !=0 ) {
		_exit(0);
	}

	if(setsid() < 0) {
		return -1;
	}

	if(nochdir == 0) {
		if(chdir("/")<0) {
			return -1;
		}
	}

	if(noclose == 0) {
		struct rlimit rl;
		getrlimit(RLIMIT_NOFILE, &rl);
		if(rl.rlim_max == RLIM_INFINITY) {
			rl.rlim_max = 1024;
		}
		for(int i=0; i<rl.rlim_max; ++i) {
			close(i);
		}
		if( open("/dev/null", O_RDWR)<0 || dup(0)<0 || dup(0)<0 ) {
			return -1;
		}
	}

	return 0;
}
