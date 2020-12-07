#include "hoistlib.h"

int main(int argc, char * argv[])
{
	const char *NAME = "READER";
	int ret;
	int sockfd;
	char buffer[25]; //will be used for printing hoist status
	msg_t msg = {0,0};

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

	fprintf(log, "%s: starting\n", NAME); fflush(log);
  while (1)
  {
    bzero(buffer, 25);
    if ((ret = read(sockfd, &msg, sizeof(msg))) < 0)
      error("READER: reading from socket", ret);
		switch (msg.status) {
			case 'U':
				sprintf(buffer, "Hoist going up");
				break;
			case 'D':
				sprintf(buffer, "Hoist going down");
				break;
			case 'S':
				sprintf(buffer, "Hoist stopped");
				break;
			case 'T':
				sprintf(buffer, "Maximal height reached");
				break;
			case 'B':
				sprintf(buffer, "Minimal height reached");
				break;
			case 'E':
				sprintf(buffer, "Exiting");
				break;
		}
    printf("height: %d cm\tstatus: %s\n", msg.height, buffer);
    if (msg.status == 'E')
      break;
	}


	fprintf(log, "%s: exiting\n", NAME); fflush(log);
	close(fd_log);
	close(sockfd);

  exit (0);
}
