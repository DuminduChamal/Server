#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<errno.h>
#include<arpa/inet.h>
#include<string.h> 
#include <unistd.h>

#define html "Content-Type: text/html\r\n\r\n"
#define mp3	"Content-Type: audio/mpeg\r\n\r\n"
#define mp4	"Content-Type: video/mp4\r\n\r\n"
#define jpg "Content-Type: image/jpeg\r\n\r\n"
#define txt "Content-Type: text/plain\r\n\r\n"

int port = 9300;
char *mimeType;

void responce(int client_fd, char *header, char *body, struct sockaddr_in client_address)
{
	int len = strlen(body);
	//printf("%d\n",len);
	char responce[len+100];
	int lengthOfResponce = sprintf(responce, "%s\n" "connection : close\n" "content length : %d\n" "content type : %s\n" "\n", header, len, mimeType);
	//printf("%s y\n\n ",responce);
	//memcpy(responce+lengthOfResponce, body, strlen(body));
	int index = 0;
	int i,j;
	char responceArray[len+lengthOfResponce];
   	for(i = 0; i < strlen(responce); i++) 
		{
      responceArray[index] = responce[i];
      index++;
   	}
 
   	for(j = 0; j < strlen(body); j++) {
      responceArray[index] = body[j];
      index++;
   	}
	//printf("%s\n", responceArray);
	//printf("%s\n", responce);
	int sent_size = send(client_fd, responceArray, lengthOfResponce+len,0);
	printf("Sent %d bytes to client %s\n",sent_size, inet_ntoa(client_address.sin_addr));
}

void sendFile(int client_fd, char *fileName, struct sockaddr_in client_address, char *mime)
{
	char source[1024];
	FILE *file = fopen(fileName,"r");
	size_t bufferSiz, readVal;
	char status[] = "HTTP/1.1 200 OK\r\n";
	
	if(file!=NULL)
	{
		write(client_fd, status, strlen(status));
		if(strcmp(mime, "html") == 0) 
		{
			mimeType = html;	
		} 
		else if(strcmp(mime, "jpg") == 0 || strcmp(mime, "jpeg") == 0 || strcmp(mime, "png") == 0) 
		{
			mimeType = jpg;
		}
		else if(strcmp(mime, "mp3") == 0) 
		{
			mimeType = mp3;
		}
		else if(strcmp(mime, "mp4") == 0) 
		{
			mimeType = mp4;
		}
		else if(strcmp(mime, "webm") == 0) 
		{
			mimeType = mp4;
		}
		else if(strcmp(mime, "txt") == 0) 
		{
			mimeType = txt;
		}
		int sent_size = write(client_fd, mimeType, strlen(mimeType));
		//int sent_size = send(client_fd, responceArray, lengthOfResponce+len,0);
		printf("Sent %d bytes to client %s\n",sent_size, inet_ntoa(client_address.sin_addr));		
		while((readVal = fread(source, 1, sizeof(source), file))>0) 
		{
			write(client_fd, source, readVal);
		}
		
	}
	else
	{
		char *errorMSG = "<h1>FILE NOT FOUND!!!<h1>";
		mimeType=html;
		int lengthOfError = strlen(errorMSG);
		responce(client_fd,"HTTP/1.1 404 NOT FOUND", errorMSG, client_address);
	}
}



int main()
{
	//char msg[] = "Hello to the world!!!\n";
	//int sent_size;
	char buffer[1024] = {0}; 
	char requestType[4];       
	char requestPath[1024];
	
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
	printf("server listen on port %d...\n\n", port);
	
	int client_socket;
	
	while(1)
	{
		client_socket=accept(server_socket,(struct sockaddr*)&client_address, &length);
		if(client_socket==-1)
		{
			perror("Something wrong in the accept!!!\n");
			continue;
		}
		
		read(client_socket,buffer,1024);
		sscanf(buffer,"%s %s", requestType, requestPath);
		printf("Request Type : %s\n",buffer);
		
		char *mime = strrchr(requestPath, '.')+1;
		char *fileName = strtok(requestPath, "/");

		printf("file name : %s\n", fileName);
		
		
		if(mime)
		{
			mimeType = mime;
		}
		else
		{
			mimeType = NULL;
		}
		
		
		if(!strcmp(requestType,"GET")&& !strcmp(requestPath,"/"))
		{
			char *msg = "<h1>It's a GET request to base URL</h1>";
			mimeType = html;
			int lengthOfMsg = strlen(msg);
			responce(client_socket,"HTTP/1.1 200 OK", msg, client_address);
		}
		else if(strcmp(requestType,"GET")==0)
		{
			//printf("here\n");
			char *msg = "<h1>It's a GET request to base URL</h1>";
			mimeType = html;
			int lengthOfMsg = strlen(msg);
			//responce(client_socket,"HTTP/1.1 200 OK", msg, client_address);
			sendFile(client_socket, fileName, client_address, mime);
		}
		else if(!strcmp(requestType,"POST")&& !strcmp(requestPath,"/"))
		{
			char *msg = "It's a POST request to the base URL";
			mimeType = html;
			int lengthOfMsg = strlen(msg);
			responce(client_socket,"HTTP/1.1 200 OK", msg, client_address);
		}
		// else
		// {
		// 	sendFile(client_socket,fileName,client_address);
		// }
	
		//int sent_size = send(client_socket, msg, strlen(msg),0);
		//printf("Sent %d bytes to client %s\n",sent_size, inet_ntoa(client_address.sin_addr));
		close(client_socket);
		//close(server_socket); 
	}


	 
	return 0;
	
}
