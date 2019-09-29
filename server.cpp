#include<unistd.h>
#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include <arpa/inet.h> 
#include<iostream>
#include<cstring>
using namespace std; 
int main()
{
	int server_fd,status,queue_size=10,to_client_socket;
	struct sockaddr_in addr;
	int addrl=sizeof(addr);

	server_fd=socket(AF_INET,SOCK_STREAM,0);
	if(server_fd==0)
	{
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}
	addr.sin_family=AF_INET;
	addr.sin_port=htons(8000);
	addr.sin_addr.s_addr=inet_addr("127.0.0.1");


	status=bind(server_fd,(struct sockaddr*)&addr,sizeof(addr));
	if(status<0)
	{
		perror("Failed to bind");
		exit(EXIT_FAILURE);
	}
	
	status=listen(server_fd,queue_size);
	if(status<0)
	{
		perror("Failed to listen");
		exit(EXIT_FAILURE);
	}


	to_client_socket=accept(server_fd,(struct sockaddr*)&addr,(socklen_t*)&addrl);
	if(to_client_socket<0)
	{
		perror("Accept failed");
		exit(EXIT_FAILURE);
	}
	int readbytes;
	char buffer[1024];
	readbytes=recv(to_client_socket,buffer,1024,0);
	cout<<buffer<<endl;
	int filesize;
	readbytes=recv(to_client_socket,&filesize,sizeof(filesize),0);
	cout<<filesize;
	FILE *fp=fopen("/home/meenu/Documents/newfile","wb");


	memset(buffer,'\0',1024);
	while((readbytes=recv(to_client_socket,buffer,1024,0))>0&&filesize>0)
	{
			fwrite(buffer,sizeof(char),readbytes,fp);
			memset(buffer,'\0',1024);
			filesize=filesize-readbytes;


	}
	
	return 0;


}