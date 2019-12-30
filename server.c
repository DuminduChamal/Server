#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<errno.h>
#include<arpa/inet.h>
#include<string.h> 
#include <unistd.h>

int port = 9000;

int main()
{
	char msg[] = "Hello to the world!!!\n";
	int sent_size;
	
	//create a socket
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket==-1)
	{
		perror("Couldnot create the server socket\n");
		exit(-1);
	} 
	
	struct sockaddr_in server_address, client_address;
	
	//define the server address
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = INADDR_ANY;
	
	unsigned int length = sizeof(server_address);
	
	//bind the socket to our specified IP and port
	int server_bind;
	server_bind = bind(server_socket,(struct sockaddr*)&server_address, length);
	if(server_bind==-1)
	{
		perror("Couldnot bind the IP and port!!!\n");
		exit(-1);
	}
	
	//listen to the client requests
	if((listen(server_socket,5))==-1)
	{
		perror("Something wrong with listen!!!\n");
		exit(-1);
	}
	printf("serve listen on port %d...\n\n", port);
	
	int client_socket;
	
	while(1)
	{
		client_socket=accept(server_socket,(struct sockaddr*)&client_address, &length);
		if(client_socket==-1)
		{
			perror("Something wrong in the accept!!!\n");
			continue;
		}
		sent_size = send(client_socket, msg, strlen(msg),0);
		printf("Sent %d bytes to client %s\n",sent_size, inet_ntoa(client_address.sin_addr));
		close(client_socket);
		//close(server_socket);
		 
	}
	 
	return 0;
	
}
