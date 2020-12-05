/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/select.h>


#define MAX_HEIGHT 200
#define STEP 5
#define MAX_STEP MAX_HEIGHT/STEP
#define BUFSIZE 20

typedef struct message{
	int height;
	char status;
} msg_t;

void error(char *msg, int ret)
{
    perror(msg);
    exit(ret); // error propagation
}

int main(int argc, char *argv[])
{
	char STATUS = 'B';
	int sockfd, newsockfd, portno, clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int ret;
	unsigned long sec = 1, usec = 0;
	char in=0;
	struct timeval tv;
	msg_t msg = {0,0};
	fd_set fd_in;


	if (argc < 2) {
	 fprintf(stderr,"ERROR, no port provided\n");
	 exit(1);
	}
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	error("ERROR opening socket", sockfd);

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if ((ret = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) < 0){ 
		error("ERROR on binding", ret);
		close(sockfd);
	}
	listen(sockfd,5);
	clilen = sizeof(cli_addr);

	if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) < 0) 
	  error("ERROR on accept", newsockfd);

	while(in!='E'){
		// --- preconfig ---
		FD_ZERO(&fd_in);
		FD_SET(newsockfd, &fd_in);
		tv.tv_sec = sec; tv.tv_usec = usec;
		in = 0;	// reset each time, in order to avoid problems if we somehow read 0Byte from the readable socket
		
		if((ret=select(newsockfd+1, &fd_in, NULL, NULL, &tv)) < 0){	error("Select on socket", ret); }
		/* Updates status only if a new one is submitted and it is compatible with current status, otherwise stop the hoist */
		if (FD_ISSET(newsockfd, &fd_in)){
			if ((ret = read(newsockfd,&in,1)) < 0) error("ERROR reading from socket", ret);
			printf("Received request: ");	// won't print nicely if we read 0 bit
			if ( (in == 'U' && STATUS != 'T') || (in == 'D' && STATUS != 'B') || (in == 'E'))
				STATUS = in;
		}
		
		switch (STATUS){
			case 'U':	 // go UP
				msg.height += STEP;
				STATUS = msg.height>=MAX_HEIGHT?'T':'U';
				msg.status = STATUS;
				printf("UP: %d\n", msg.height);
				break;
			
			case 'D': 	// go DOWN
				msg.height -= STEP;
				STATUS = msg.height<=0?'B':'D';
				msg.status = STATUS;
				printf("DOWN: %d\n", msg.height);
				break;
			case 'S': 	// STOP
				printf("STOP: %d\n", msg.height);
				msg.status = STATUS;
				break;
			case 'T':
				printf("TOP: %d\n", msg.height);
				msg.status = STATUS;
				break;
			case 'B':
				printf("BOTTOM: %d\n", msg.height);
				msg.status = STATUS;
				break;
			case 'E':	// END
				msg.status = 'E';
				printf("END: %d\n", msg.height);
				break;
			default: // will simply do nothing
				msg.status = STATUS;
		}
		if ((ret = write(newsockfd,&msg,sizeof(msg)))<0) error("ERROR writing on socket", ret);
		usleep(1000000*(sec-tv.tv_sec)+(usec-tv.tv_usec));	// tv will yield the amount of time passed before reading
	}														// with this usleep we approximate a period of 1s for the serve
															
	close(newsockfd);
	return 0; 
}
