#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>

typedef struct message{
	int height;
	char status;
} msg_t;

void error(char *msg, int ret)
{
    perror(msg);
    exit(ret); // error propagation
}

int main(int argc, char * argv[])
{
  int ret;
  int sockfd = atoi(argv[1]);
  char buffer[25]; //will be used for printing hoist status
  msg_t msg = {0,0};
  while (1)
  {
    bzero(buffer, 25);
    if ((ret = read(sockfd, &msg, sizeof(msg))) < 0)
      error("ERROR reading from socket", ret);
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
  exit (0);
}
