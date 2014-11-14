#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <sys/select.h>
#include <unistd.h>

void observe_inotify_event(int fd)
{
	char ieventbuf[200];
	struct inotify_event* event = NULL;
	int len = 0;
	event = (struct inotify_event*)&(ieventbuf[0]);
	printf("something happened in signcfg/.\n");
	len = read(fd, &ieventbuf[0], 200 * sizeof(char));
	if (len == -1 && errno != EAGAIN){
		perror("read() start of inotify struct");
		exit(EXIT_FAILURE);
	}
	if (event->mask & IN_CREATE)
		printf("CREATED: ");
	else if(event->mask & IN_DELETE)
		printf("DELETED: ");	

	if(event->len)
		printf("%s", event->name);

	if (event->mask & IN_ISDIR)
		printf(" [directory]\n");
	else
		printf(" [file]\n");

}

int main(int argc, char* argv[])
{
	int fd;	
	int signcfg_fd;
	fd = inotify_init();
	if (fd == -1) {
		perror("inotify_init1");
		exit(EXIT_FAILURE);
	}
	signcfg_fd = inotify_add_watch(fd, "./signcfg/", IN_CREATE | IN_DELETE);
	if (signcfg_fd == -1) {
		perror("inotify_add_watch");
		exit(EXIT_FAILURE);
	}
	//setup select for inotify events
	printf("fd=%d\n", fd);
	observe_inotify_event(fd);
	fd_set ifs;
	int fd_set_size = fd + 1;
	while(true){
		int num_fds = 0;
		printf("fd_set_size=%d signcfg_fd=%d\n", fd_set_size, signcfg_fd);
		printf("waiting for events to happen in signcfg/ directory\n");
		FD_ZERO(&ifs);
		FD_SET(fd, &ifs);
		FD_SET(0, &ifs);
		num_fds = select(fd_set_size, &ifs, NULL, NULL, NULL);
		printf("got num_fds=%d\n", num_fds);
		if (num_fds == -1){
			perror("select()");
			exit(EXIT_FAILURE);
		}else if (num_fds){
			printf("directory something happened.\n");
			if (FD_ISSET(fd, &ifs)){
				observe_inotify_event(fd);
			}else if(FD_ISSET(0, &ifs)){
				int len;
				char buf[200];
				printf("got something from stdin\n");
				len = read(0, (void*)buf, 200 * sizeof(char));
				printf("read %d\n", len);
				if (len == -1 && errno != EAGAIN){
					perror("read() start of inotify struct");
					exit(EXIT_FAILURE);
				}
				buf[len] = '\0';
				printf("%s", buf);
			}
		}
	}	
	return 0;
}
