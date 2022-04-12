#include <pthread.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
#include <semaphore.h>

int count=0;
int PORT = 8000;
#define MAXLINE 1024

struct ipInfo
{
	int port;
	char* ip;
	int newfd;

};
struct ipInfo connected_clients[5];
void *myThread(void *arg) 
{
	int pos=count-1;
	char buffer[MAXLINE];
	char conn[MAXLINE]="Client Just joined the chat with port: ";
	char sesEnd[MAXLINE]="chat left by port";
	char portnum[5];
	char sending[MAXLINE];
	sprintf(portnum,"%d",connected_clients[count-1].port);
	strcat(conn,portnum);
	strcat(sesEnd,portnum);
	strcpy(sending,portnum);
	for(int i=0;i<count;i++){
		write(connected_clients[i].newfd,conn,strlen(conn));
	}
	
	while(1)
	{
		memset(buffer, '\0', sizeof(buffer));
		int n = read(connected_clients[pos].newfd, buffer, MAXLINE);
		buffer[n] = '\0';
		if(strcmp(buffer,"END\n")==0)
		{
			break;
		}	
		strcat(sending,buffer);
		for(int i=0;i<count;i++)
		{
			if(i==pos)
				continue;
			write(connected_clients[i].newfd,sending,strlen(sending));
		}
		printf("message send\n");

	}
	for(int i=0;i<count;i++)
	{
		if(i==pos)
			continue;
		write(connected_clients[i].newfd,sesEnd,strlen(sesEnd));
	}

	close (connected_clients[pos].newfd);
}

int main(int argc, char *argv[])
{
    int size = 0;
	int sockfd; 
	struct sockaddr_in	 servaddr, cliaddr;

	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{ 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 

	memset(&servaddr, 0, sizeof(servaddr)); 
	
	// Filling server information 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_addr.s_addr = INADDR_ANY; 

	if(bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))<0)
	{
		exit(-1);
	}

	listen(sockfd, 5);
	pthread_t tid[5];

	while(1)
	{
		int len = sizeof(cliaddr);
		int newfd = accept(sockfd, (struct sockaddr *) &cliaddr, &len);
		if (newfd < 0)
		{
			printf("server acccept failed...\n"); 
			exit(0); 
		}
		printf("server acccept the client...\n");

		int port = ntohs(cliaddr.sin_port);
		char* ip = inet_ntoa(cliaddr.sin_addr);

		connected_clients[count].ip = ip;
		connected_clients[count].port = port;
		connected_clients[count].newfd = newfd;
		count++;
		int error = pthread_create(&tid[count-1], NULL, myThread, NULL);
		if(error != 0)
		{
			printf("Thread not created\n");
			exit(0);
		}
	}
	close(sockfd);
	return 0;
}
