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
	int client_fd,status;
	struct sockaddr_in addr;


	client_fd=socket(AF_INET,SOCK_STREAM,0);
	if(client_fd==0)
	{
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}
	addr.sin_family=AF_INET;
	addr.sin_port=htons(8000);
	addr.sin_addr.s_addr=inet_addr("127.0.0.1");

	status=connect(client_fd,(struct sockaddr*)&addr,sizeof(addr));
	if(status<0)
	{
		perror("connection failed");
		exit(EXIT_FAILURE);
	}
	send(client_fd,"hello.cpp",sizeof("hello.cpp"),0);
	FILE *fp=fopen("/home/meenu/Downloads/adhaar.pdf","rb");
	fseek(fp,0,SEEK_END);
	int filesize=ftell(fp);
	rewind(fp);
	send(client_fd,&filesize,sizeof(filesize),0);
	char buffer[100];
	int sent_bytes;
	while((sent_bytes=fread(buffer,sizeof(char),100,fp))>0&&filesize>0)
	{
		send(client_fd,buffer,sizeof(buffer),0);
		memset(buffer,'\0',100);
		filesize=filesize-sent_bytes;
	}
	
		return 0;
}