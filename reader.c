#include "hoistlib.h"

int main(int argc, char * argv[])
{
	const char *NAME = "READER";
	int ret;
	int sockfd;
	char buffer[25]; //will be used for printing hoist status
	msg_t msg;
	int msg_size;

	FILE *log;
	int fd_log;

	if (argc<3){
		fprintf(stderr,"READER: too few arguments passed\n");
		exit(1);
	}

	fd_log = atoi(argv[1]);
	if((log = fdopen(fd_log, "w"))==(FILE*)NULL){
		perror("READER: Log file open from filedes");
	}
	sockfd = atoi(argv[2]);
	
	msg_init(&msg); msg_size = msg_getsize(); // initialize message pointed to by msg

	fprintf(log, "%s: starting\n", NAME); fflush(log);
	while (msg_getstatus(msg) != EXIT)
	{
		bzero(buffer, 25);
		if ((ret = read(sockfd, msg, msg_size)) < 0)
			error("READER: reading from socket", ret);
		switch (msg_getstatus(msg)) {
			case UP:
				sprintf(buffer, "Hoist going up");
				break;
			case DOWN:
				sprintf(buffer, "Hoist going down");
				break;
			case STOP:
				sprintf(buffer, "Hoist stopped");
				break;
			case TOP:
				sprintf(buffer, "Maximal height reached");
				break;
			case BOTTOM:
				sprintf(buffer, "Minimal height reached");
				break;
			case EXIT:
				sprintf(buffer, "Exiting");
				break;
		}
		printf("height: %d cm\tstatus: %s\n", msg_getheight(msg), buffer); fflush(stdout);
	}


	fprintf(log, "%s: exiting\n", NAME); fflush(log);
	msg_free(msg);
	close(fd_log);
	close(sockfd);

  exit (0);
}
