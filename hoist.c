#include <sys/socket.h>
#include <netinet/in.h>
#include "hoistlib.h"

int main(int argc, char *argv[])
{
	const char *NAME = "HOIST";
	int fd_in, fd_out;
	char in=0, ret;
	int height=0;

	FILE *log;
	int fd_log;

	if (argc<4){
		fprintf(stderr,"ERROR too few arguments passed\n");
		exit(1);
	}

	fd_log = atoi(argv[1]);
	if((log = fdopen(fd_log, "w"))==(FILE*)NULL){
		perror("HOIST: Log file open from filedes");
	}
	fd_in = atoi(argv[2]);
	fd_out = atoi(argv[3]);
	fprintf(log, "%s: starting\n", NAME); fflush(log);

	while(in!= EXIT){
		in = 0;	// to avoid it retaining previous values and potentially keep moving in case of red errors
		if((ret = read(fd_in, &in, 1))<0) error("ERROR reading from Server - Hoist pipe", ret);

		switch (in){
			case '+':	 // go UP
				height_tryup(&height);
				break;
			case '-': 	// go DOWN
				height_trydown(&height);
				break;
			// default: // will simply do nothing
		}
		if((ret = write(fd_out, &height, sizeof(int)))<0) error("ERROR writing to Hoist - Server pipe", ret);
	}
	fprintf(log, "%s: exiting\n", NAME); fflush(log);
	close(fd_log);
	close(fd_in);
	close(fd_out);

	return 0;
}
