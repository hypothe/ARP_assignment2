
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#define MAX_HEIGHT 200
#define MIN_HEIGHT 0
#define STEP 5
#define MAX_STEP MAX_HEIGHT/STEP

void error(char *msg, int ret)
{
    perror(msg);
    exit(ret); // error propagation
}

int main(int argc, char *argv[])
{
	int fd_in, fd_out;
	char in=0, ret;
	int height=0;
	
	if (argc<3){
		fprintf(stderr,"ERROR no input and/or output pipe provided\n");
		exit(1);
	}
	fd_in = atoi(argv[1]);
	fd_out = atoi(argv[2]);
	
	while(in!= 'E'){
		in = 0;	// to avoid it retaining previous values and potentially keep moving in case of red errors
		if((ret = read(fd_in, &in, 1))<0) error("ERROR reading from Server - Hoist pipe", ret);
		
		switch (in){
			case '+':	 // go UP
				height = height>=MAX_HEIGHT? height:height + STEP;
				break;
			case '-': 	// go DOWN
				height = height<=MIN_HEIGHT? height: height - STEP;
				break;
			// default: // will simply do nothing
		}
		if((ret = write(fd_out, &height, sizeof(int)))<0) error("ERROR writing to Hoist - Server pipe", ret);
	}
	
	return 0;
}