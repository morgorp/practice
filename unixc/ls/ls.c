#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <sys/time.h>
#include <time.h>


/**
 * get the string of mode
 *	r w x s S
**/
char *get_file_mode(mode_t st_mode) {
	char init[]="----------.";
	char *file_mode = malloc(sizeof(init)*sizeof(char));
	strcpy(file_mode, init);
	
	if( S_ISREG(st_mode) ) {
		file_mode[0] = '-';
	} else if( S_ISDIR(st_mode) ) {
		file_mode[0] = 'd';
	} else if( S_ISCHR(st_mode) ) {
		file_mode[0] = 'c';
	} else if( S_ISBLK(st_mode) ) {
		file_mode[0] = 'b';
	} else if( S_ISFIFO(st_mode) ) {
		file_mode[0] = 'f';
	} else if( S_ISLNK(st_mode) ) {
		file_mode[0] = 'l';
	} else if( S_ISSOCK(st_mode)) {
		file_mode[0] = 's';
	}
	
	int mode_type[]={
		S_IRUSR, S_IWUSR, S_IXUSR,
		S_IRGRP, S_IWGRP, S_IXGRP,
		S_IROTH, S_IWOTH, S_IXOTH
	};
	char mode_name[]={
		'r', 'w', 'x',
		'r', 'w', 'x',
		'r', 'w', 'x'
	};
	for(int i=0; i<9; ++i) {
		if(mode_type[i]&st_mode) {
			file_mode[i+1] = mode_name[i];
		}
	}

	if( S_ISUID&st_mode ) {
		if( file_mode[3]=='-' )	{
			file_mode[3] = 'S';
		} else {
			file_mode[3] = 's';
		}
	}
	if( S_ISGID&st_mode ) {
		if( file_mode[6]=='-' )	{
			file_mode[6] = 'S';
		} else {
			file_mode[6] = 's';
		}
	}

	file_mode[11] = '\0';

	return file_mode;
}


/**
 * list the infomation of a file
**/
void list_file_info(int parfd, char *filename)
{
	struct stat sta;
	if( fstatat(parfd, filename, &sta, AT_SYMLINK_NOFOLLOW)==-1 ) {
		perror(filename);
		return;
	}

	char *file_mode = get_file_mode(sta.st_mode); // mode
	nlink_t hlnk_num = sta.st_nlink; // the number of hard links
	char *pw_name = getpwuid(sta.st_uid)->pw_name; // passwd name
	char *gr_name = getgrgid(sta.st_gid)->gr_name; // group name
	off_t size = sta.st_size; // size

	struct tm *tm = localtime(&sta.st_mtime);
	char time[50]; // formated time
	strftime(time, 50, "%h %e %H:%M", tm);

	char lnkto[300]="\0-> "; // symbol link
	if(file_mode[0]=='l') {
		ssize_t len = readlinkat(parfd, filename, lnkto+4, 256);
		if(len!=-1) {
			lnkto[len+4] = 0;
			lnkto[0] = ' ';
		}
	}

	printf("%s\t%d\t%s\t%s\t%d\t%s\t%s%s\n",
			file_mode,hlnk_num,pw_name,gr_name,size,time,filename,lnkto);

	free(file_mode);
}


/**
 * list all files belong to the directory
**/
void list_file(char *path)
{
	int fd=open(path, O_RDONLY);
	if( fd==-1 ) {
		perror("open");
		return;
	}
	DIR *dp = fdopendir(fd);
	if( dp==NULL ) {
		list_file_info(AT_FDCWD, path);
		return;
	}

	struct dirent *dirp = NULL;
	while( (dirp=readdir(dp))!=NULL ) {
		char *filename = dirp->d_name;
		list_file_info(fd, filename);
	}

	if( closedir(dp)==-1 ) {
		perror("closedir");
	}
}


int main(int argc, char *argv[])
{

	if(argc<=1) {
		// list the information of all files belong to current working directory
		list_file(".");
	} else {
		// list the information of the specific files and directories
		for(int i=1; i<argc; ++i) {
			if(i>1) {
				putchar('\n');
			}
			printf("%s:\n",argv[i]);
			list_file(argv[i]);
		}
	}

	return 0;
}

