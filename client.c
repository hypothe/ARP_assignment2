#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>

int spawn(const char * program, char ** arg_list)
{
    pid_t child_pid = fork();
    if (child_pid != 0)
        return child_pid;
    else
    {
        execvp (program, arg_list);
        perror("exec failed");
        return 1;
    }
}

void error(char *msg, int ret)
{
    perror(msg);
    exit(ret); // error propagation
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;
    int ret;
    char out = 0;
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error("ERROR opening socket", sockfd);
    if ((server = gethostbyname(argv[1])) == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if ((ret = connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))) < 0)
        error("ERROR connecting", ret);
    printf("Connected successfully\n");

    char sock_string[16];
    sprintf(sock_string, "%d", sockfd);
    char * arg_list[] = { "/usr/bin/konsole",  "-e", "./reader", sock_string, (char*)NULL };
    int child_pid = spawn(arg_list[0], arg_list);

    while (1) {
      printf("Enter the hoist command:\n");
      scanf(" %c", &out);
      if ((out != 'U') && (out != 'S') && (out != 'D') && (out != 'E'))
      {
        printf("This command does not exist. Please, try again.\n");
        continue;
      }
      if ((ret = write(sockfd, &out, 1)) < 0)
        error("ERROR writing to socket", ret);
      if (out == 'E')
        break;
    }

    waitpid(child_pid, &ret, 0);
    if (!WIFEXITED(ret))
  		perror("Reader terminated with an error.\n");
		printf("Reader exited with value %d\n", WEXITSTATUS(ret));

    return 0;
}
