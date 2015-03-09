#include "unp.h"
#include <stdio.h>
#include <sys/time.h>
#include <sys/select.h>
#include <signal.h>
void sig_chld(int signo);

int main(int argc, char **argv)
{
	int sel, sockfd, echosockfd, timesockfd, n, ret, pfd[2], maxfdp1, flag, choice;
	char ch, recvline[MAXLINE + 1], ipaddr[20], sdbuff[MAXLINE], pbuf[MAXLINE], pfdbuf[MAXLINE], *choicechar;
	struct sockaddr_in servaddr, timeservaddr, echoservaddr;
	struct hostent *hptr;
	pid_t pid;
	fd_set selset;
	/* Checking for input arguments*/
	if (argc != 2)
		err_quit("usage: ./client <server-ipaddress>");

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_sys("socket error");
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	//servaddr.sin_port   = htons(4321);	/* daytime server */	
	ret = (inet_pton(AF_INET,argv[1],&servaddr.sin_addr));
	if (ret == -1) //error
		err_quit("inet_pton error for %s", argv[1]);
	else if (ret == 1) { //Ip address entered
		if (gethostbyaddr((char *)&servaddr.sin_addr,sizeof(servaddr.sin_addr),AF_INET) == NULL) /* Quit if invalid address */  
			err_quit("IP address entered %s is invalid\n", inet_ntoa(servaddr.sin_addr));
	} 
	else if (ret == 0) { //host name entered
		/* Get Ip address by hostname */
		if( (hptr = gethostbyname(argv[1])) == NULL)
			err_quit("gethostbyname error for host: %s: %s", argv[1],hstrerror(h_errno));
		else	
			printf("Connecting to host %s.cs.stonybrook.edu\n",hptr->h_name);
		if (inet_ntop(AF_INET, *(hptr->h_addr_list), ipaddr, sizeof(ipaddr)) <= 0)
			err_quit("inet_ntop error for %s", argv[1]);
		printf("IP address of %s is %s\n",argv[1],ipaddr);
		/* converting IP  back to Network format*/	
		if (inet_pton(AF_INET, ipaddr, &servaddr.sin_addr) <= 0)
			err_quit("inet_pton error for %s", argv[1]);
	}
	close(sockfd);
	
	/* SIGCHLD handliing */	
	struct sigaction sa;
	sa.sa_handler = &sig_chld;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	if (sigaction(SIGCHLD, &sa, 0) == -1) {
		perror(0);
		exit(1);
	}
	/* Ask for user input about selecting options*/
again:
	while (1) {
		printf ("\nPlease select an option among 1/2/3:\n");
		printf ("1. Echoclient\n");
		printf ("2. Timeclient\n");
		printf ("3. Quit\n\n");
		fscanf (stdin, "%s", choicechar);
		printf("Input read: %s\n", choicechar);
		choice = atoi(choicechar);
			//printf("number of chars read: %d\n", flag);
			/*if(flag != 1) {
				ch = getchar();
				printf("Invalid input: %c entered. Please enter an integer.\n");
				break;
			} 

			//printf ("Please enter correct integer value.\n");
			//exit(1);
		} */ 
		if (choice != 0)
			printf("choice selected: %d\n", choice);
		FD_ZERO(&selset);
		if (choice == '1') { //start of choice = 1
			printf("Echo client selected\n");
			if ( (echosockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
				err_sys("socket error");
			bzero(&echoservaddr, sizeof(echoservaddr));
			echoservaddr.sin_family = AF_INET;
			echoservaddr.sin_port   = htons(4441);	/* echo client */				
			ret = (inet_pton(AF_INET,ipaddr,&echoservaddr.sin_addr));
			if (ret == -1)
				err_quit("inet_pton error for %s", argv[1]);
			/* Fill socket address structure*/
			printf("Serv addr before connect: %s\n", inet_ntoa(echoservaddr.sin_addr));
			if (connect(sockfd, (SA *) &echoservaddr, sizeof(echoservaddr)) < 0)
				err_quit("Connect Error: %s", strerror(errno));
			/* Piping between parent and child. Close pfd[1] in parent and pfd[0] in child*/
			if (pipe(pfd) == -1)
			{
				perror("pipe failed");
				exit(1);
			}			
			sprintf(sdbuff,"%d", sockfd);
			sprintf(pfdbuf,"%d", pfd[1]); 
				
			if ((pid = fork()) < 0)
			{
				perror("fork failed");
				exit(2);
			}	
			if (pid == 0)  //child
			{
				close(pfd[0]);
				printf("Before execlp\n");
				printf("Sdbuff: %s\n", sdbuff);
				printf("pfdbuff: %s\n", pfdbuff);
				if ( (execlp("xterm", "xterm", "-e", "./echoclient", sdbuff, pfdbuf, (char *) 0)) < 0) 
					err_quit("Echo client: execlp error");
				close(pfd[1]);
				exit(0);
			}
			else {
				//parent
				close(pfd[1]);
				while (1) { //parent while loop for select	
					FD_SET(pfd[0], &selset);
					FD_SET(fileno(stdin), &selset);
					maxfdp1 = max(pfd[0], fileno(stdin)) + 1;
					//printf("before select: %d, %d\n",fileno(stdin), pfd[0]);
					if (select(maxfdp1, &selset, NULL, NULL, NULL) < 0) {
						if(errno == EINTR)
							break;
						printf("select error :%s\n",strerror(errno));
					}
					if (FD_ISSET(pfd[0], &selset)) {		  
						if ((n = read(pfd[0], pbuf, MAXLINE)) > 0)
							printf("Reading from child %d: %s\n", pid, pbuf);         	
						if(n <= 0)
							goto again;
					} //close of if
					if (FD_ISSET(fileno(stdin), &selset))	{	  
						if( (n=read(fileno(stdin),pbuf,MAXLINE)) > 0)		
							printf("Invalid Input. Please enter only in child terminal\n");
						if(n <= 0)
							goto again;
					} //close if
				} //close while loop for select
				close(pfd[0]);
			} //close of parent 
		close(echosockfd);
		} //end for choice = 1	
		/* timeclient starts*/	
		else if (choice == 2) {
			printf("Time client selected\n");
			/* Fill socket address stucture*/
			if ( (timesockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
				err_sys("socket error");
			bzero(&timeservaddr, sizeof(timeservaddr));
			timeservaddr.sin_family = AF_INET;
			timeservaddr.sin_port   = htons(4321);	/* daytime server */	
			ret = (inet_pton(AF_INET,ipaddr,&timeservaddr.sin_addr));
			if (ret == -1)
				err_quit("inet_pton error for %s", argv[1]);

			/* Fill socket address structure*/
			if (connect(timesockfd, (SA *) &timeservaddr, sizeof(timeservaddr)) < 0)
				err_sys("Connect Error");			
			/* Piping between parent and child. Close pfd[1] in parent and pfd[0] in child*/
			if (pipe(pfd) == -1)
			{
				perror("pipe failed");
				exit(1);
			}
			sprintf(sdbuff,"%d", sockfd);
			sprintf(pfdbuf,"%d", pfd[1]);		
			if ((pid = fork()) < 0)
			{
				perror("fork failed");
				exit(2);
			}	
			if  (pid == 0) //child
			{
				close(pfd[0]);
				if ( (execlp("xterm", "xterm", "-e", "./timeclient", sdbuff, pfdbuf, (char *) 0)) < 0)
					err_quit("Time client: execlp error");	
				close(pfd[1]);
				exit(0);
			}
			else {
				//parent
				close(pfd[1]);
				while (1) { //parent while loop for select
					FD_SET(pfd[0], &selset);
					FD_SET(fileno(stdin), &selset);
					maxfdp1 = max(pfd[0], fileno(stdin)) + 1;
					//printf("before select: %d, %d\n",fileno(stdin), pfd[0]);
					if (select(maxfdp1, &selset, NULL, NULL, NULL) < 0) {
						if(errno == EINTR)
							break;
						printf("select error :%s\n",strerror(errno));
					}
					if (FD_ISSET(pfd[0], &selset)) {		  
						if ((n = read(pfd[0], pbuf, MAXLINE)) > 0)
						{            		
							printf("Reading from Child %d: %s\n", pid, pbuf);
						}
						if(n <= 0)
							goto again;
					} //close of if
					if (FD_ISSET(fileno(stdin), &selset))	
					{	  
						if( (n=read(fileno(stdin),pbuf,MAXLINE)) > 0)		
							printf("Invalid Input. Please enter only in child terminal\n");
						if(n <= 0) {
							goto again;
						}
					}
				} //close while loop for select.
				close(pfd[0]);			
			} //close of parent
		close(timesockfd);	
		} //end for choice = 2
		else if (choice == 3) {
			goto end;
		} //end for choice = 3
		else { // check for invalid input
		if (choice != 0)			
			printf("\n%d is an invalid option. Please select a correct integer choice.\n\n", choice);
		else
			printf("\nInvalid input entered. Please select a correct integer choice.\n");	
		}	
	//close(sockfd);
	} // end of while loop

end:
	printf("Exiting the program, Goodbye!\n");	
	exit(0);
}

void sig_chld(int signo) {
	pid_t pid;
	int stat;
	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0 )
		printf("Child %d terminated \n", pid);
	return;
}
