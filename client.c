#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <pthread.h> 

int PORT = 8000;
#define MAXLINE 1024
int ch=1;
void *reading(void * input)
{
	char buffer[MAXLINE];
	int newfd=*(int *) input;
	while(1)
	{
		if(ch==0)
			break;
		
		int n = read(newfd, buffer, MAXLINE);
		buffer[n] = '\0';
		printf("data from server :%s\n", buffer);
	}
}
void *writing(void *input)
{
	int newfd= *(int*)input;
	char buffer[MAXLINE];
	while(1)
	{		
		fgets(buffer, MAXLINE, stdin);
		write(newfd, buffer, strlen(buffer));
		//If the user enters END thent the loop breaks and turns ch to 0 which will break the reading loop aswell
		if(strcmp(buffer,"END\n")==0)
		{
			ch=0;
			break;
		}

		for(int i=0;i<MAXLINE;i++){
			buffer[i]= '\0';
		}
		printf("message send..\n");
	}
	close(newfd);

}
// Driver code 
int main(){ 

	int sockfd; 
	
	struct sockaddr_in	 servaddr; 

	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{ 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 

	printf("%d\n",sockfd);

	memset(&servaddr, 0, sizeof(servaddr)); 
	
	// Filling server information 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	

	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))!= 0)
	{
		printf("connection with the server failed...\n"); 
		exit(0); 
	}
	printf("connected to the server..\n");

	int port = ntohs(servaddr.sin_port);


	
	char check[MAXLINE];
	pthread_t pid[2];
	pthread_create(&pid[0], NULL, writing, (void *)&sockfd);
	pthread_create(&pid[1], NULL, reading, (void *)&sockfd);
	
	pthread_exit(NULL);

	
	return 0; 
}
