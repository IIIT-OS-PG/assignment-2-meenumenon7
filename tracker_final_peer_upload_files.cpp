#include<unistd.h>
#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include<iostream>
#include<cstring>
#include<openssl/sha.h>
using namespace std; 

void * start_serv(void *);
void * start_client(void *);

int main()
{
	
	pthread_t serv_thread,client_thread;
	//pthread_create(&serv_thread,NULL,start_serv,NULL);
	pthread_create(&client_thread,NULL,start_client,NULL);

	int client_fd,status;
	struct sockaddr_in addr;

	pthread_join(client_thread,NULL);
// 	client_fd=socket(AF_INET,SOCK_STREAM,0);
// 	if(client_fd==0)
// 	{
// 		perror("Socket creation failed");
// 		exit(EXIT_FAILURE);
// 	}
// 	addr.sin_family=AF_INET;
// 	addr.sin_port=htons(8203);
// 	addr.sin_addr.s_addr=inet_addr("127.0.0.1");

// 	status=connect(client_fd,(struct sockaddr*)&addr,sizeof(addr));
// 	if(status<0)
// 	{
// 		perror("connection failed");
// 		exit(EXIT_FAILURE);
// 	}
// 	char buffer[512];
// 	cout<<status<<endl;
// 	char *cu="create_user";
// 	int x=send(client_fd,cu,sizeof(string(cu)),0);
// 	cout<<"sent uname"<<x;
// 	int readbytes=recv(client_fd,buffer,512,0);

// 	x=send(client_fd,"uname2",sizeof("uname2"),0);
// 	memset(buffer,'\0',512);
// 	readbytes=recv(client_fd,buffer,512,0);
// 	x=send(client_fd,"passw",sizeof("passw"),0);
// 	memset(buffer,'\0',512);
// 	readbytes=recv(client_fd,buffer,512,0);


// 	x=send(client_fd,"login",sizeof("login"),0);
// 	cout<<"sent uname"<<x;
// 	readbytes=recv(client_fd,buffer,512,0);

// 	x=send(client_fd,"uname2",sizeof("uname2"),0);
// 	memset(buffer,'\0',512);
// 	readbytes=recv(client_fd,buffer,512,0);
// 	x=send(client_fd,"passw",sizeof("passw"),0);
// 	memset(buffer,'\0',512);
// 	int ret_val;
// 	readbytes=recv(client_fd,&ret_val,sizeof(ret_val),0);
// 	cout<<"tHE RE VAL IS"<<ret_val<<endl;

// string s;
// 	getline(cin,s);
// 	char abc[s.size()+1];
// 	strcpy(abc,s.c_str());
// 	 char **spaced_token=(char **)malloc(1024*sizeof(char *));
// 	 int i=0;
//    char* str=strtok(abc," ");
//   	while(str!=NULL)
//   	{
//    	spaced_token[i]=str;
//    	//cout<<spaced_token[i]<<"hhh"<<endl;
//    	i++;
//    	str=strtok(NULL," ");
//   	}
//   	spaced_token[i]=NULL;
//   //	cout<<"the piped spaced_token is "<<spaced_token[0]<<spaced_token[1]<<endl;
  	
  
// 	char buffer1[512];
// //	cout<<status<<endl;

//   	 x=send(client_fd,spaced_token[0],sizeof(string(spaced_token[0])),0);
//   	cout<<"sent uname"<<x<<"hu"<<endl;
//  readbytes=recv(client_fd,buffer1,512,0);
//  cout<<readbytes<<"in recv "<<endl;
// 	x=send(client_fd,spaced_token[1],sizeof(string(spaced_token[1])),0);
// 	 cout<<readbytes<<"in recv gid"<<endl;
// 	readbytes=recv(client_fd,buffer1,512,0);

// 	x=send(client_fd,"list_groups",sizeof("list_groups"),0);
// 	readbytes=recv(client_fd,buffer1,512,0);


// 	//cout<<buffer;
// 	// x=send(client_fd,"password",sizeof("password"),0);
// 	// cout<<"sent pass"<<x;
// 	// x=send(client_fd,"password",sizeof("password"),0);
// 	// cout<<"sent pass"<<x;
// 	// FILE *fp=fopen("/home/meenu/Downloads/adhaar.pdf","rb");
// 	// fseek(fp,0,SEEK_END);
// 	// int filesize=ftell(fp);
// 	// rewind(fp);
// 	// send(client_fd,&filesize,sizeof(filesize),0);
	
// 	//fclose(fp);
// 	close(client_fd);
		return 0;
}

void * start_serv(void *t)
	{   cout<<"Inside t_p2S"<<endl; 
		int server_fd,status,queue_size=5;
		struct sockaddr_in addr,c_addr;
		cout<<"INside t_p2s_2";
		int addrl=sizeof(addr);
		int c_addrl=sizeof(c_addr);
		server_fd=socket(AF_INET,SOCK_STREAM,0);
		if(server_fd==0)
		{
			perror("Socket creation failed");
			exit(EXIT_FAILURE);
		}
		addr.sin_family=AF_INET;
		addr.sin_port=htons(8090);
		addr.sin_addr.s_addr=inet_addr("127.0.0.1");


		status=bind(server_fd,(struct sockaddr*)&addr,sizeof(addr));
		if(status<0)
		{
			perror("Failed to bind");
			exit(EXIT_FAILURE);
		}
		cout<<"Bind successful"<<endl;
	
		int lstatus=listen(server_fd,queue_size);
		if(lstatus<0)
		{
			perror("Failed to listen");
			exit(EXIT_FAILURE);
		}
		cout<<"lsattus"<<lstatus<<endl;
		cout<<"Listen successful"<<endl;
		int num_threads=8;
	pthread_t th[4];
	int i=0,j=0;
//	while(1)
	//{ Commented while (1) here t check if multiple clients are able to connect
	int to_client_fd;
	to_client_fd=accept(server_fd,(struct sockaddr*)&c_addr,(socklen_t*)&c_addrl);
	if(to_client_fd<0)
	{
		perror("Accept failed");
		exit(EXIT_FAILURE);
	}
	int filesize;
		char buffer[512];
		int readbytes=recv(to_client_fd,&filesize,sizeof(filesize),0);
		cout<<filesize<<endl;
		//mkdir("/home/meenu/Documents/client_x");
		j++;
		string str="/home/meenu/Documents/newfile"+to_string(j);
		const char* fname=str.c_str();
		//strcpy(fname,str.c_str());
		FILE *fp=fopen(fname,"wb");

		//int to_client_socket;
		memset(buffer,'\0',512);
		while((readbytes=recv(to_client_fd,buffer,512,0))>0&&filesize>0)
		{
				fwrite(buffer,sizeof(char),readbytes,fp);
				memset(buffer,'\0',512);
				filesize=filesize-readbytes;



		}
		close(to_client_fd);
		
	//}
	close(server_fd);
	pthread_exit(NULL);
	/*while(i<num_threads)
	{
		to_client_socket=accept(server_fd,(struct sockaddr*)&c_addr,(socklen_t*)&caddrl);
		if(to_client_socket<0)
		{
			perror("Accept failed");
			exit(EXIT_FAILURE);
		}
		pthread_create(&th[i],NULL,start_fn,(void*)&to_client_socket);
		i++;
	}
	//pthread_join(th[3],NULL);
	for(int j=0;j<num_threads;j++)
	{   cout<<"join "<<j<<"  ";
		pthread_join(th[j],NULL);
	}*/




	}
	
void * start_client(void *y)
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
	addr.sin_port=htons(8203);
	addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	
	
	status=connect(client_fd,(struct sockaddr*)&addr,sizeof(addr));
	if(status<0)
	{
		perror("connection failed");
		exit(EXIT_FAILURE);
	}	

	string data;
	while(1)
	{	
	cout<<"Please enter what do you want to do."<<endl;
	getline(cin,data);
	
	char input[data.size()+1];
	//getinput
	strcpy(input,data.c_str());
	 char **spaced_token=(char **)malloc(1024*sizeof(char *));
	 int i=0;
   char* str=strtok(input," ");
  	while(str!=NULL)
  	{
   	spaced_token[i]=str;
   	//cout<<spaced_token[i]<<"hhh"<<endl;
   	i++;
   	str=strtok(NULL," ");
  	}
  	spaced_token[i]=NULL;
  //	cout<<"the piped spaced_token is "<<spaced_token[0]<<spaced_token[1]<<endl;
  	
  
	char buffer[512];

	cout<<spaced_token[0]<<"is the first token"<<endl;
	if(strcmp(spaced_token[0],"create_user")==0)
	{

		int x=send(client_fd,spaced_token[0],sizeof(string(spaced_token[0])),0);
		//cout<<"sent uname"<<x;
		memset(buffer,'\0',512);
		int readbytes=recv(client_fd,buffer,512,0);

		x=send(client_fd,spaced_token[1],sizeof(string(spaced_token[1])),0);
		memset(buffer,'\0',512);
		readbytes=recv(client_fd,buffer,512,0);
		x=send(client_fd,spaced_token[2],sizeof(string(spaced_token[2])),0);
		memset(buffer,'\0',512);
		readbytes=recv(client_fd,buffer,512,0);

	}
	else if(strcmp(spaced_token[0],"login")==0)
	{
		int x=send(client_fd,"login",sizeof(string("login")),0);
		//cout<<"sent uname"<<x;
		int readbytes=recv(client_fd,buffer,512,0);

		
		memset(buffer,'\0',512);
		x=send(client_fd,spaced_token[1],sizeof(string(spaced_token[1])),0);
		memset(buffer,'\0',512);
		readbytes=recv(client_fd,buffer,512,0);
		x=send(client_fd,spaced_token[2],sizeof(string(spaced_token[2])),0);
		memset(buffer,'\0',512);
		int ret;
		readbytes=recv(client_fd,&ret,sizeof(ret),0);
		if(ret==1)
			cout<<"Welcome "<<spaced_token[1]<<endl;
		else
			cout<<"Invalid username/password.Try again\n";
		//cout<<buffer<<endl;
		
		memset(buffer,'\0',512);

	}


	else if(strcmp(spaced_token[0],"create_group")==0)
	{
		int x=send(client_fd,"create_group",sizeof(string("create_group")),0);
		//cout<<"sent uname"<<x;
		int readbytes=recv(client_fd,buffer,512,0);

		
		memset(buffer,'\0',512);
		x=send(client_fd,spaced_token[1],sizeof(string(spaced_token[1])),0);
		memset(buffer,'\0',512);
		int ret;
		readbytes=recv(client_fd,&ret,sizeof(ret),0);
		//cout<<buffer<<endl;
		if(ret==-1)
			cout<<"PLease login first"<<endl;
		else
			cout<<"Group created ."<<endl;
		memset(buffer,'\0',512);


	}
	else if(strcmp(spaced_token[0],"join_group")==0)
	{
		int x=send(client_fd,"join_group",sizeof(string("join_group")),0);
		//cout<<"sent uname"<<x;
		int readbytes=recv(client_fd,buffer,512,0);

		
		memset(buffer,'\0',512);
		x=send(client_fd,spaced_token[1],sizeof(string(spaced_token[1])),0);
		memset(buffer,'\0',512);
		readbytes=recv(client_fd,buffer,512,0);

	}
	else if(strcmp(spaced_token[0],"leave_group")==0)
	{

		int x=send(client_fd,"leave_group",sizeof(string("leave_group")),0);
		//cout<<"sent uname"<<x;
		int readbytes=recv(client_fd,buffer,512,0);

		
		memset(buffer,'\0',512);
		x=send(client_fd,spaced_token[1],sizeof(string(spaced_token[1])),0);
		memset(buffer,'\0',512);
		readbytes=recv(client_fd,buffer,512,0);

		

	}
	else if(strcmp(spaced_token[0],"list_requests")==0)
	{
		
		int x=send(client_fd,"list_requests",sizeof(string("list_requests")),0);
		//cout<<"sent uname"<<x;
		int readbytes=recv(client_fd,buffer,512,0);

		
		memset(buffer,'\0',512);
		x=send(client_fd,spaced_token[1],sizeof(string(spaced_token[1])),0);
		memset(buffer,'\0',512);
		readbytes=recv(client_fd,buffer,512,0);
	}
	else if(strcmp(spaced_token[0],"accept_requests")==0)
	{
		int x=send(client_fd,"accept_requests",sizeof(string("accept_requests")),0);
		//cout<<"sent uname"<<x;
		int readbytes=recv(client_fd,buffer,512,0);

		
		memset(buffer,'\0',512);
		x=send(client_fd,spaced_token[1],sizeof(string(spaced_token[1])),0);
		memset(buffer,'\0',512);
		readbytes=recv(client_fd,buffer,512,0);

	}
	else if(strcmp(spaced_token[0],"list_groups")==0)
	{
		int x=send(client_fd,"list_groups",sizeof(string("list_groups")),0);
		//cout<<"sent uname"<<x;
		int size_of_list;
		int readbytes=recv(client_fd,&size_of_list,sizeof(size_of_list),0);
		cout<<"list size is "<<size_of_list<<endl;
		x=send(client_fd,"ok",sizeof("ok"),0);
		memset(buffer,'\0',512);
		int val=0;
		while(val<size_of_list)
		{
			readbytes=recv(client_fd,buffer,512,0);
			cout<<val+1<<")"<<buffer<<endl;
			memset(buffer,'\0',512);
			x=send(client_fd,"ok",sizeof("ok"),0);
			val++;



		}

		int ret;
		readbytes=recv(client_fd,&ret,sizeof(ret),0);
		//cout<<buffer<<endl;
		if(ret==-1)
			cout<<"PLease login first"<<endl;
		
		memset(buffer,'\0',512);
		// x=send(client_fd,spaced_token[1],sizeof(string(spaced_token[1])),0);
		// memset(buffer,'\0',512);
		// readbytes=recv(client_fd,buffer,512,0);

		
		

	}
	else if(strcmp(spaced_token[0],"list_files")==0)
	{
		int x=send(client_fd,"list_files",sizeof(string("list_files")),0);
		//cout<<"sent uname"<<x;
		int readbytes=recv(client_fd,buffer,512,0);

		
		memset(buffer,'\0',512);
		x=send(client_fd,spaced_token[1],sizeof(string(spaced_token[1])),0);
		memset(buffer,'\0',512);
		readbytes=recv(client_fd,buffer,512,0);

	}
	else if(strcmp(spaced_token[0],"upload_file")==0)
	{

		int x=send(client_fd,"upload_file",sizeof(string("upload_file")),0);
		//cout<<"sent uname"<<x;
		int readbytes=recv(client_fd,buffer,512,0);
		memset(buffer,'\0',512);
		//change the hash thing here
		//spaced_token[1] contains the entire path.So extract the filename from the pathname		
		
		cout<<"the file path i s "<<spaced_token[1]<<endl;
			
		cout<<"going to read "<<spaced_token[1]<<endl;
		FILE *fp=fopen(spaced_token[1],"rb");
		fseek(fp,0,SEEK_END);
		int filesize=ftell(fp);
		rewind(fp);
		cout<<"the file sze is"<<filesize<<endl;


		char *filepath=spaced_token[1];
		char **file_token=(char **)malloc(1024*sizeof(char *));
	 	 int i=0;
   		char* filename=strtok(spaced_token[1],"/");
  		while(filename!=NULL)
  		{
   			file_token[i]=filename;
   	//cout<<spaced_token[i]<<"hhh"<<endl;
   			i++;
   			filename=strtok(NULL,"/");
  		}
  		file_token[i]=NULL;
  		cout<<"The filetoken is "<<file_token[i-1]<<endl;

  		//filetoken[i-1] contains the filename to be sent 




//##############################getting the  filesize and getting the hash###################################

  	//filename
  	x=send(client_fd,file_token[i-1],sizeof(string(file_token[i-1])),0);
	memset(buffer,'\0',512);
	readbytes=recv(client_fd,buffer,512,0);
	// cout<<"going to read "<<filepath<<endl;
	// FILE *fp=fopen(filepath,"rb");
	// fseek(fp,0,SEEK_END);
	// int filesize=ftell(fp);
	// rewind(fp);
	send(client_fd,&filesize,sizeof(filesize),0);
	//cout<<"filesize sent";
	readbytes=recv(client_fd,buffer,512,0);
	cout<<readbytes<<"readbytes"<<endl;
	memset(buffer,'\0',512);

	//groupid
	cout<<"now sending gid "<<spaced_token[2]<<endl;
	x=send(client_fd,spaced_token[2],sizeof(string(spaced_token[2])),0);
	memset(buffer,'\0',512);
	readbytes=recv(client_fd,buffer,512,0);

	memset(buffer,'\0',512);


	char buffer_sha[512*1024];
	int sent_bytes;
	string  final;
	string final_sha;
	unsigned char digest[SHA_DIGEST_LENGTH];
	int no_of_times=0;

//calculate sha1
	cout<<"going to calculate sha1"<<endl;
	while((sent_bytes=fread(buffer_sha,sizeof(char),512*1024,fp))>0&&filesize>0)
	{ 	no_of_times++;
		//cout<<"senntbytes "<<sent_bytes<<endl;
    	SHA1((unsigned char*)&buffer_sha, strlen(buffer_sha), (unsigned char*)&digest);    
 	
//cout<<digest;
    char mdString[SHA_DIGEST_LENGTH*2+1];
 	
    for(int i = 0; i < SHA_DIGEST_LENGTH; i++)
         sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
 
    //cout<<mdString;
     final=mdString;
 	final_sha=final_sha+final.substr(0,20);
		
		//send(client_fd,buffer,sizeof(buffer),0);
		memset(buffer_sha,'\0',512*1024);
		filesize=filesize-sent_bytes;
	}
	cout<<final_sha<<endl;

	long long int shastring=final_sha.length();
	//sending the string size of sha1
	send(client_fd,&shastring,sizeof(shastring),0);
	//cout<<"filesize sent";
	readbytes=recv(client_fd,buffer,512,0);
	memset(buffer,'\0',512);

	//start sending sha string
	char send_sha[512];
	long long int start_index=0;
	while(shastring>0)
	{ 	//no_of_times++;
		memset(buffer,'\0',512);
		memset(send_sha,'\0',512);
		string temp=final_sha.substr(start_index,20);
		strcpy(send_sha,temp.c_str());
		cout<<"sending "<<send_sha<<endl;
		start_index=start_index+20;
		send(client_fd,send_sha,sizeof(string(send_sha)),0);
		memset(send_sha,'\0',512);
		shastring=shastring-20;
		readbytes=recv(client_fd,buffer,512,0);


	}



//#####################################################done###################################################




		


	}
	else if(strcmp(spaced_token[0],"download_file")==0)
	{
		int x=send(client_fd,"upload_file",sizeof(string("upload_file")),0);
		//cout<<"sent uname"<<x;
		int readbytes=recv(client_fd,buffer,512,0);

		//change the hash thing here

		
		memset(buffer,'\0',512);
		x=send(client_fd,spaced_token[1],sizeof(string(spaced_token[1])),0);
		memset(buffer,'\0',512);
		readbytes=recv(client_fd,buffer,512,0);
		x=send(client_fd,spaced_token[2],sizeof(string(spaced_token[2])),0);
		memset(buffer,'\0',512);
		readbytes=recv(client_fd,buffer,512,0);
		//there is a destination path as well here
		
	}
	else if(strcmp(spaced_token[0],"logout")==0)
	{
		int x=send(client_fd,"logout",sizeof(string("logout")),0);
		//cout<<"sent uname"<<x;
		int readbytes=recv(client_fd,buffer,512,0);
	}
	// else if(strcmp(spaced_token[0],"show_downloads")==0)
	// {
	// 	//locally process this.

	// }
	// else if(strcmp(spaced_token[1],"stop_share")==0)
	// {
	// 	//not yet implemnted in tracker!
	// }
	else
	{
		cout<<"Wrong command"<<endl;
	}
}
}
