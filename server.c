/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>


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
	int sockfd, newsockfd, portno, clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int ret;
	char in=0;
	// char buffer[BUFSIZE];
	msg_t msg = {0,0};


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
		if ((ret = read(newsockfd,&in,1)) < 0) error("ERROR reading from socket", ret);
		printf("Received request ");
		switch (in){
			case 'U':	 // go UP
				if (msg.status == 'T') break;
				msg.height += STEP*(1+rand()%(MAX_STEP-msg.height/STEP));
				msg.status = msg.height>=MAX_HEIGHT?'T':'U';
				printf("UP\n");
				break;
			case 'S': 	// STOP
				msg.status = 'S';
				printf("STOP\n");
				break;
			case 'D': 	// go DOWN
				if (msg.status == 'B') break;
				msg.height -= STEP*(1+rand()%(msg.height/STEP));
				msg.status = msg.height<=0?'B':'D';
				printf("DOWN\n");
				break;
			case 'E':	// END
				msg.status = 'E';
				printf("END\n");
				break;
			// default 
		}
		if ((ret = write(newsockfd,&msg,sizeof(msg)))<0) error("ERROR writing on socket", ret); 
	}
	close(newsockfd);
	return 0; 
}
