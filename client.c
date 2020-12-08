#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include "hoistlib.h"

int main(int argc, char *argv[])
{
	const char *NAME = "CLIENT";
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;
    int ret;
    char out = 0;
	char err_msg[80];

	FILE *log;
	char fd_log_str[20];
	int  fd_log;
	char *log_file = "./client_log.txt";

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);

	if((log = fopen(log_file, "w"))==(FILE*)NULL){
		perror("Log file open from");
		exit(1);
	}
	fd_log = fileno(log);

	fprintf(log, "%s: starting\n", NAME); fflush(log);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error("ERROR opening socket", sockfd);
	fprintf(log, "%s: socket created\n", NAME); fflush(log);
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
	fprintf(log, "%s: connected succesfully\n", NAME); fflush(log);

	int tmp_socket[2] = {sockfd, -1};
    int child_pid = spawn("./reader", fd_log, tmp_socket, NULL, 1);
	fprintf(log, "%s: reader spawned\n", NAME); fflush(log);

    while (out!=EXIT) {
      printf("Enter the hoist command:\n");
	  printf("<U> UP\t\t<D> DOWN\t<S> STOP\t<E> EXIT\n");
      scanf(" %c", &out);
      if ((out != UP) && (out != STOP) && (out != DOWN) && (out != EXIT))
      {
        printf("This command does not exist. Please, try again.\n");
        continue;
      }
      if ((ret = write(sockfd, &out, 1)) < 0)
        error("ERROR writing to socket", ret);
    }
	fprintf(log, "%s: communication terminated\n", NAME); fflush(log);

    waitpid(child_pid, &ret, 0);
	if (!WIFEXITED(ret)){
		sprintf(err_msg, "Reader terminated with an error %d %d\n", WIFSIGNALED(ret), WTERMSIG(ret));
		perror(err_msg);
	}
	else
		printf("Reader exited with value %d\n", WEXITSTATUS(ret)); fflush(stdout);

	fprintf(log, "%s: exiting\n", NAME); fflush(log);
	close(fd_log);
	close(sockfd);
    return 0;
}
