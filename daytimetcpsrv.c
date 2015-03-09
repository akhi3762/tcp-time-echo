#include	"unp.h"
#include 	"unpthread.h"
#include	<time.h>
#include 	<arpa/inet.h>
#include 	<sys/select.h>
#include 	<fcntl.h>
#include 	<sys/file.h>

static void *timeserver(void *);
static void *echoserver(void *);
static FILE *fp;
static char buff[MAXLINE];
static time_t ticks;
static fd_set selset;
static 	struct timeval timeout;
static int maxfdp1;

int main(int argc, char **argv)
{
	int addrlen, *connfd, *connfd1, listenfd, listenfd1, maxfdp1, fileflags, fileflags1;
	struct sockaddr_in	timeservaddr, echoservaddr;
	int optval = 1;
	fd_set selset;
	pthread_t  tid;
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	listenfd1 = Socket(AF_INET, SOCK_STREAM, 0);
	
	if ( setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1 )
    		err_quit ("Timeserver: setsockopt error\n");
	if ( setsockopt(listenfd1, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1 )
    		err_quit ("Echoserver: setsockopt error\n");
	/* Fill socket address structure for echoserver*/
	bzero(&echoservaddr, sizeof(echoservaddr));
	echoservaddr.sin_family      = AF_INET;
	echoservaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//printf("server address: %s\n", inet_ntoa (echoservaddr.sin_addr.s_addr));
	echoservaddr.sin_port        = htons(4441);	/* daytime server */
	/* fcntl() function */	
	if (fileflags1 = fcntl(listenfd1, F_GETFL, 0) == -1)  {
   		perror("fcntl F_GETFL");
   		exit(1);
  	}
	if (fcntl(listenfd1, F_SETFL, fileflags1 | FNDELAY) == -1)  {
  		perror("fcntl F_SETFL, FNDELAY");
   		exit(1);
  	}
	if(bind(listenfd1, (SA *) &echoservaddr, sizeof(echoservaddr)) < 0)
          err_sys("Echo server: Bind Error");
	Listen(listenfd1, LISTENQ);
	
	/* Fill socket address structure for timeserver*/
	bzero(&timeservaddr, sizeof(timeservaddr));
	timeservaddr.sin_family      = AF_INET;
	timeservaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//printf("server address: %s\n", inet_ntoa (timeservaddr.sin_addr.s_addr));
	timeservaddr.sin_port        = htons(4321);	/* daytime server */
	/* fcntl() function */	
	if (fileflags = fcntl(listenfd, F_GETFL, 0) == -1)  {
   		perror("1.fcntl F_GETFL");
   		exit(1);
  	}
	if (fcntl(listenfd, F_SETFL, fileflags | FNDELAY) == -1)  {
  		perror("1.fcntl F_SETFL, FNDELAY");
   		exit(1);
  	}	
	if(bind(listenfd, (SA *) &timeservaddr, sizeof(timeservaddr)) < 0)
          err_sys("Time server: Bind Error");
	Listen(listenfd, LISTENQ);
	printf("starting server...\n");
	printf("waiting for incoming connections:\n");
	for ( ; ; ) {	
		connfd = malloc(sizeof(int));
		connfd1 = malloc(sizeof(int));
		FD_SET(listenfd, &selset);
		FD_SET(listenfd1, &selset);
		maxfdp1 = max(listenfd, listenfd1) + 1;
		//printf("before select: %d, %d\n",fileno(stdin), pfd[0]);
		if (select(maxfdp1, &selset, NULL, NULL, NULL) < 0) {
			if(errno == EINTR)
				break;
		printf("select error :%s\n",strerror(errno));
		}
		if (FD_ISSET(listenfd, &selset)) { //timeserver		  
		*connfd = Accept(listenfd, (SA *) NULL, NULL);
		//printf("connfd: %d\n", &connfd);
		//if ((fileflags = fcntl(&connfd, F_GETFL, 0)) < 0) 
   		// 	err_sys ("2.fcntl F_GETFL error");
		fileflags &= ~FNDELAY;		
		if (fcntl(*connfd, F_SETFL, fileflags ) < 0) 
    		 	err_sys ("fcntl F_SETFL error"); 
		Pthread_create(&tid, NULL, timeserver, (void *) connfd);
		printf("New thread ID created for timeserver: %d\n", tid);
		}
		if (FD_ISSET(listenfd1, &selset)) { //echo server
		*connfd1 = Accept(listenfd1, (SA *) NULL, NULL);
		//if ((fileflags1 = fcntl(&connfd, F_GETFL, 0)) < 0) 
   		// 	err_sys ("3.fcntl F_GETFL error");
		fileflags1 &= ~FNDELAY;	
		if (fcntl(*connfd1, F_SETFL, fileflags1 ) < 0) 
    		 	err_sys ("3.fcntl F_SETFL error"); 
		Pthread_create(&tid, NULL, echoserver, (void *) connfd1);
		printf("New thread ID created for echoserver: %d\n", tid);
		}
	//printf("connfd after accept is: %d\n", connfd);
	} //close for loop
} //main


static void *timeserver (void *arg) {
	int connfd;
	connfd = *((int *) arg);
	free(arg);
	Pthread_detach(pthread_self());
	//printf ("In time server, connfd: %d\n", connfd);
	/* First instance of time printing without waiting for 5 seconds timeout */
	printf("Time server: Client connection accepted.\n");	
	strcpy(buff, "\0");
	printf("Time server: writing time to client socket.\n");
	if (!strcmp(buff,"\0")) {
		ticks = time(NULL);
	        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
	        Write(connfd, buff, strlen(buff));
	}
	while (1) {
	FD_ZERO(&selset);
	FD_SET(connfd,&selset);
	maxfdp1 = connfd + 1;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	if (select(maxfdp1, &selset, NULL, NULL, &timeout) < 0) {
		printf("Timeserver: select error :%s\n", strerror(errno));
		exit(1);
	}
	//printf("connfd after select is: %d\n", connfd);
	if (FD_ISSET(connfd, &selset)) {		 
		printf ("Time server: client terminated: FIN received from client\n");
		printf("Continue to run server...\n");
		break;      	
	}
	else {
		//printf("connfd before timeclient starting is: %d\n", connfd);
		ticks = time(NULL);
	        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
		printf("Time server: writing time to client socket.\n");
	        Write(connfd, buff, strlen(buff));
      	     } //else
	} //while
	close(connfd); 	
return (NULL);
} //end fucntion


static void *echoserver (void *arg) {
	int connfd, echobuf[MAXLINE];
	connfd = *((int *) arg);
	free(arg);
	Pthread_detach(pthread_self());
	//printf ("In echo server, connfd: %d\n", connfd);
	ssize_t	n;
	char	buf[MAXLINE];
	printf("Echo server: client connection accepted\n");
	/* FD_ZERO(&selset);
	FD_SET(connfd,&selset);
	maxfdp1 = connfd + 1;
	if (FD_ISSET(connfd, &selset)) {		 
		printf ("Echo server: client terminated: FIN received from client\n");
		printf("Continue to run server...\n");  	
	}
	else { */
again:
		while ( (n = read(connfd, echobuf, MAXLINE)) > 0) {
			printf("Echo server recieved %d bytes and writing to client socket.\n", n);
			Writen(connfd, echobuf, n);
		}
		if (n < 0 && errno == EINTR) 
			goto again;
		if (n == 0) {		
			printf("Echo server: client terminated: socket read returned 0.\n");
			printf("continue to run server...\n");
		}
		else if (n < 0) {
			printf("Echo server: client terminated: socket read returned value -1, errno = %s\n", strerror(errno));
			exit (1);
		}
      	//}
}
