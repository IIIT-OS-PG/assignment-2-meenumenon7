#include<unistd.h>
#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include<iostream>
#include<cstring>
#include<openssl/sha.h>
#include<bits/stdc++.h>
using namespace std; 
int listen_port;
char *listen_ip;
void * start_serv(void *);
void * start_client(void *);
void * start_fn(void *);
void * get_data_fn(void *);
unordered_map<string,vector<int>> chunk_info;//filename ,chunk info
std::vector<string> mygroups; 
unordered_map<int ,vector<int>> owners_chunk;//owners number list of hunks
deque<int> owner;
unordered_map<string,string> fpath;//filename,path

struct file_fd
{
	FILE *fp;
	char *ip;
	int port;
	string filenm;
	std::vector<int> chunk_to_download;
};
struct threads_info
{
	int client_fd;
	int cport;
	char *cip;
	// struct sockaddr ad;
// 	ad.sin_port;
// 	ad.sin_addr.s_addr;
// };
};
void get_chunks(vector<pair<string,int>>&owners_details,int i,string filename)
{
	string cip=owners_details[i].first;
	int cport=owners_details[i].second;
	cout<<"to contact "<<cip<<" "<<cport<<endl;
	int client_fd,status;
	struct sockaddr_in addr;


	client_fd=socket(AF_INET,SOCK_STREAM,0);
	if(client_fd==0)
	{
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}
	addr.sin_family=AF_INET;
	addr.sin_port=htons(cport);
	char clientip[100];
	strcpy(clientip,cip.c_str());
	addr.sin_addr.s_addr=inet_addr(clientip);
	
	
	status=connect(client_fd,(struct sockaddr*)&addr,sizeof(addr));
	if(status<0)
	{
		perror("connection failed");
		exit(EXIT_FAILURE);
	}	
	int x=send(client_fd,"chunkinfo",sizeof("chunkinfo"),0);
	int no_of_chunks;
	char buffer[512];
	int rcvbytes=recv(client_fd,buffer,512,0);
	memset(buffer,'\0',512);
	strcpy(buffer,filename.c_str());//sent filename
	x=send(client_fd,buffer,512,0);
	rcvbytes=recv(client_fd,&no_of_chunks,sizeof(no_of_chunks),0);
	x=send(client_fd,"ok",sizeof("ok"),0);
	int j=0;
	vector<int> owner_wise_chunk;
	while(j<no_of_chunks)
	{
		int chunk;	
		int rcvbytes=recv(client_fd,&chunk,sizeof(chunk),0);
		owner_wise_chunk.push_back(chunk);
		x=send(client_fd,"ok",sizeof("ok"),0);
		j++;
	}

	cout<<"the owner wise info"<<endl;
	for(int i=0;i<owner_wise_chunk.size();i++)
	{
		cout<<"h"<<owner_wise_chunk[i];
	}
	owners_chunk[i]=owner_wise_chunk;
}

int main(int arc,char *argv[])
{
	
	 char **args_token=(char **)malloc(1024*sizeof(char *));
	 int i=0;
   char* str=strtok(argv[1],":");
  	while(str!=NULL)
  	{
   		args_token[i]=str;
   	//cout<<spaced_token[i]<<"hhh"<<endl;
   		i++;
   		str=strtok(NULL,":");
  	}
  	args_token[i]=NULL;

//  	cout<<args_token[0]<<args_token[1]<<endl;
  	listen_ip=new char[strlen(args_token[0])];
  	listen_ip=args_token[0];
	listen_port=atoi(args_token[1]);
	//cout<<listen_ip<<endl<<listen_port<<endl;
	pthread_t serv_thread,client_thread;
	pthread_create(&serv_thread,NULL,start_serv,NULL);
	pthread_create(&client_thread,NULL,start_client,NULL);

	int client_fd,status;
	struct sockaddr_in addr;

	pthread_join(client_thread,NULL);

		return 0;
}

void * start_serv(void *cs)
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
		addr.sin_port=htons(listen_port);
		addr.sin_addr.s_addr=inet_addr(listen_ip);


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
		
while(1)
	{ //Commented while (1) here t check if multiple clients are able to connect
		int to_client_fd;
		cout<<"came vack to while 1 serv thread"<<endl;
		to_client_fd=accept(server_fd,(struct sockaddr*)&c_addr,(socklen_t*)&c_addrl);
		if(to_client_fd<0)
		{
			perror("Accept failed");
			exit(EXIT_FAILURE);
		}
		
		 threads_info *tinf=(threads_info*)cs;
		pthread_t th;
		cout<<"creating thread"<<endl;
		tinf=(threads_info*)malloc(sizeof(struct threads_info));
		
		char *str1=inet_ntoa(c_addr.sin_addr);
		
		int port_v=ntohs(c_addr.sin_port);
	
		tinf->client_fd=to_client_fd;
		tinf->cport=port_v;






		tinf->cip=str1;
	
		pthread_create(&th,NULL,start_fn,(void*)tinf);//removed &
	



	}
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
		 readbytes=recv(client_fd,buffer,512,0);
		 memset(buffer,'\0',512);
		//#############SEND LISTEN IP &PORT
		//send port
		 int lport=listen_port;
		 x=send(client_fd,&lport,sizeof(lport),0);
		 readbytes=recv(client_fd,buffer,512,0);
		 memset(buffer,'\0',512);
		//send ip
		 x=send(client_fd,listen_ip,sizeof(string(listen_ip)),0);
		// readbytes=recv(client_fd,buffer,512,0);
		//#######################################3
		readbytes=recv(client_fd,buffer,512,0);
		//readbytes=recv(client_fd,&ret,sizeof(ret),0);
		//cout<<buffer<<endl;
		if(strcmp(buffer,"notlogged")==0)
			cout<<"PLease login first"<<endl;
		else
			cout<<"created the group ."<<endl;
		memset(buffer,'\0',512);
		mygroups.push_back(spaced_token[1]);


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
		if(strcmp(buffer,"nogroup")==0)
			cout<<"No such group exists"<<endl;
		else if(strcmp(buffer,"notlogged")==0)
			cout<<"Please login first."<<endl;
		else
		{	memset(buffer,'\0',512);
			//#############SEND LISTEN IP &PORT
			//send port
			 int lport=listen_port;
			 x=send(client_fd,&lport,sizeof(lport),0);
			 readbytes=recv(client_fd,buffer,512,0);
			 memset(buffer,'\0',512);
			//send ip
			 x=send(client_fd,listen_ip,sizeof(string(listen_ip)),0);
			 readbytes=recv(client_fd,buffer,512,0);
			 cout<<"Your request has been submiited"<<endl;
			//#######################################3
		}

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
		char buffer[512];
		int x=send(client_fd,"list_requests",sizeof(string("list_requests")),0);
		//cout<<"sent uname"<<x;
		int readbytes=recv(client_fd,buffer,512,0);
		cout<<"recd "<<buffer<<endl;
		
		memset(buffer,'\0',512);
		x=send(client_fd,spaced_token[1],sizeof(string(spaced_token[1])),0);
		memset(buffer,'\0',512);
		int unames_size;
		readbytes=recv(client_fd,&unames_size,sizeof(unames_size),0);
		if(unames_size==0)
		{
			cout<<"No pending requests"<<endl;
		}
		else
		{
			char buff[512];
			int start=0;
			while(start<unames_size)
			{
				int sentbytes=send(client_fd,"ok",sizeof("ok"),0);
				readbytes=recv(client_fd,buff,512,0);
				cout<<start+1<<")"<<buff<<endl;
				memset(buff,'\0',512);
				start++;

			}


		}
		
	}
	else if(strcmp(spaced_token[0],"accept_request")==0)
	{
		int x=send(client_fd,"accept_request",sizeof(string("accept_request")),0);
		//cout<<"sent uname"<<x;
		int readbytes=recv(client_fd,buffer,512,0);

		
		memset(buffer,'\0',512);
		x=send(client_fd,spaced_token[1],sizeof(string(spaced_token[1])),0);
		
		readbytes=recv(client_fd,buffer,512,0);
		memset(buffer,'\0',512);

		//send username
		x=send(client_fd,spaced_token[2],sizeof(string(spaced_token[2])),0);
		
		
		readbytes=recv(client_fd,buffer,512,0);
		memset(buffer,'\0',512);





	}
	else if(strcmp(spaced_token[0],"list_groups")==0)
	{
		cout<<"sending ist"<<endl;
		int x=send(client_fd,"list_groups",sizeof(string("list_groups")),0);
		//cout<<"sent uname"<<x;
		int size_of_list=0;
		// char nbuffer[10];
		// memset(nbuffer,'\0',10);
		// cout<<"waiting to receive size"<<endl;
		// int readbytes=recv(client_fd,nbuffer,10,0);
		// cout<<"nbuffer ="<<nbuffer<<endl;
		//string test=to_string(nbuffer);
		// size_of_list=atoi(nbuffer);
		cout<<"sizeof list "<<size_of_list<<endl;
		int readbytes=recv(client_fd,&size_of_list,sizeof(size_of_list),0);
		//int readbytes=read(client_fd,nbuffer,10);
		//cout<<nbuffer<<"with read value"<<endl;
		//size_of_list=atoi(nbuffer);
		int value=size_of_list;
		cout<<"sizeof"<<size_of_list<<endl;
		if(size_of_list==-1)//CHECK HERE!!!!
		   cout<<"PLease login first"<<endl;
		else if(size_of_list==-2)
			cout<<"No groups yet."<<endl;
		else
		{
			//cout<<"list size is "<<size_of_list<<endl;
			x=send(client_fd,"ok",sizeof("ok"),0);
			//break;
			memset(buffer,'\0',512);
			//cout<<"before while "<<buffer<<endl;
			int val=0;
			while(val<value)
			{	
				//cout<<"inside while "<<value<<endl;
				readbytes=recv(client_fd,buffer,512,0);
				//cout<<"rcvd "<<buffer<<endl;
				cout<<val+1<<")"<<buffer<<endl;
				memset(buffer,'\0',512);
				x=send(client_fd,"ok",sizeof("ok"),0);
				val++;
				//cout<<val<<endl;


			}

			// int ret;
			// readbytes=recv(client_fd,&ret,sizeof(ret),0);
			//cout<<buffer<<endl;
			
		
				memset(buffer,'\0',512);
		}// x=send(client_fd,spaced_token[1],sizeof(string(spaced_token[1])),0);
		// memset(buffer,'\0',512);
		// readbytes=recv(client_fd,buffer,512,0);

		
		

		}
	 else if(strcmp(spaced_token[0],"list_files")==0)
	 {
		cout<<"inside list fles"<<endl;
		int x=send(client_fd,"list_files",sizeof(string("list_files")),0);
		cout<<"sent uname"<<x<<endl;
		memset(buffer,'\0',512);
		cout<<"receving "<<endl;
		int readbytes=recv(client_fd,buffer,512,0);
		cout<<readbytes<<endl;
		cout<<"recvd "<<buffer<<endl;
		//send(client_fd,"list_f",sizeof(string("list_f")),0);
		
		memset(buffer,'\0',512);
		x=send(client_fd,spaced_token[1],sizeof(string(spaced_token[1])),0);
		memset(buffer,'\0',512);
		readbytes=recv(client_fd,buffer,512,0);


	//#############SEND LISTEN IP &PORT
			//send port
			 int lport=listen_port;
			 x=send(client_fd,&lport,sizeof(lport),0);
			 readbytes=recv(client_fd,buffer,512,0);
			 memset(buffer,'\0',512);
			//send ip
			 x=send(client_fd,listen_ip,sizeof(string(listen_ip)),0);
	//#####################################done
			 int no_of_files;
			 readbytes=recv(client_fd,&no_of_files,sizeof(no_of_files),0);
			 cout<<"no of files "<<no_of_files<<endl;
			 if(no_of_files==-1)
			 	cout<<"Please login first"<<endl;
			 else if(no_of_files==-2)
			 	cout<<"No such group exists"<<endl;
			 else if(no_of_files==-3)
			 	cout<<"You are not a member of the group"<<endl;
			 else
			 { cout<<"no_of_files"<<no_of_files<<endl;
			 	int start=0;
			 	while(start<no_of_files)
			 	{	memset(buffer,'\0',512);
			 		 x=send(client_fd,buffer,512,0);
			 		 memset(buffer,'\0',512);
			 		 readbytes=recv(client_fd,buffer,512,0);
			 		 cout<<start+1<<")"<<buffer<<endl;
			 		 start++;

			 	}



			 }

	}
	else if(strcmp(spaced_token[0],"upload_file")==0)
	{

		int x=send(client_fd,"upload_file",sizeof(string("upload_file")),0);
		//cout<<"sent uname"<<x;
		
		int readbytes=recv(client_fd,buffer,512,0);
		if(strcmp(buffer,"notlogged")==0)
		{	cout<<"PLease log in first"<<endl;

			memset(buffer,'\0',512);
		}
		else
		{

			//change the hash thing here
		//spaced_token[1] contains the entire path.So extract the filename from the pathname		
		
			cout<<"the file path i s "<<spaced_token[1]<<endl;
			
			cout<<"going to read "<<spaced_token[1]<<endl;
			FILE *fp=fopen(spaced_token[1],"rb");
			fseek(fp,0,SEEK_END);
			int filesize=ftell(fp);
			rewind(fp);
			cout<<"the file sze is"<<filesize<<endl;

			string pth=spaced_token[1];
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
  			fpath[file_token[i-1]]=pth;
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
			//check if member of group
			if(strcmp(buffer,"notmember")==0)
			{
				cout<<"You are not a member of this group"<<endl;
			}
			
			else
			{	
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

				std::vector<int> v;
				cout<<"no of times"<<no_of_times<<endl;
				for(int i=1;i<=no_of_times;i++)
					v.push_back(i);
				chunk_info[file_token[i-1]]=v;
				cout<<"contents "<<endl;
				for(int i=0;i<no_of_times;i++)
					cout<<v[i]<<endl;



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


			}
//#####################################################done###################################################




		}	


	}
	else if(strcmp(spaced_token[0],"download_file")==0)
	{
		string pathname=spaced_token[3];
		int x=send(client_fd,"download_file",sizeof(string("download_file")),0);
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
		

		//#############SEND LISTEN IP &PORT
		//send port
		 int lport=listen_port;
		 x=send(client_fd,&lport,sizeof(lport),0);
		 readbytes=recv(client_fd,buffer,512,0);
		 memset(buffer,'\0',512);
		//send ip
		 x=send(client_fd,listen_ip,sizeof(string(listen_ip)),0);
		// readbytes=recv(client_fd,buffer,512,0);
		//#######################################3

		 //receive the no_of_owners
		 int no_of_owners;
		 readbytes=recv(client_fd,&no_of_owners,sizeof(no_of_owners),0);
		 cout<<no_of_owners<<"are owners "<<endl;
		  if(no_of_owners==-1)
			 	cout<<"Please login first"<<endl;
			 else if(no_of_owners==-2)
			 	cout<<"No such group exists"<<endl;
			 else if(no_of_owners==-3)
			 	cout<<"You are not a member of the group"<<endl;
			 else

			 {	

			 int i=0;
			 vector<pair<string,int>> owners_details;
			 while(i<no_of_owners)
			 {  memset(buffer,'\0',512);
			 	x=send(client_fd,"hi",sizeof("hi"),0);
			 	readbytes=recv(client_fd,buffer,512,0);
			 	string ip=buffer;
			 	cout<<buffer<<"ip"<<endl;
			 	x=send(client_fd,"hi",sizeof("hi"),0);
			 	int port;

			 	readbytes=recv(client_fd,&port,sizeof(port),0);
			 	pair<string,int>pr(ip,port);
			 	cout<<port<<"is port"<<endl;
			 	owners_details.push_back(pr);


			 	i++;
			 }
			 x=send(client_fd,"hi",sizeof("hi"),0);
			 int filesize;
			 readbytes=recv(client_fd,&filesize,sizeof(filesize),0);
			 x=send(client_fd,"ack",sizeof("ack"),0);	



			long long int hashfilesize;
			readbytes=recv(client_fd,&hashfilesize,sizeof(hashfilesize),0);
			x=send(client_fd,"ack",sizeof("ack"),0);	
			long long int temp=hashfilesize;
			cout<<"the string"<<hashfilesize<<endl;
			
			// string unames=username+filename;
			// char hashfilename[unames.size()+1];
			// strcpy(hashfilename,unames.c_str());
			// cout<<"hashfilename "<<hashfilename<<endl;
			// FILE *fp=fopen(hashfilename,"wb");

				//int to_client_socket;
				//memset(buffer,'\0',512);
			string hashfilename;
			cout<<"temp is now "<<temp<<endl;
			while(temp>0)
			{
					
					
					readbytes=recv(client_fd,buffer,512,0);
					cout<<"received "<<readbytes<<endl;
					string t=buffer;
					cout<<"t ="<<t<<endl;
					x=send(client_fd,"ack",sizeof("ack"),0);
					hashfilename=hashfilename+t;
					cout<<"hashfilename "<<hashfilename<<endl;
					//cout<<readbytes<<"=readbytes"<<endl;
					temp=temp-20;
					cout<<"temp"<<temp<<endl;
					memset(buffer,'\0',512);

					
				}

				//cout<<hashfilename;
				string file=spaced_token[2];
			
				for(int i=0;i<owners_details.size();i++)
				{
					get_chunks(owners_details,i,file);
				}

				for(int j=0;j<owners_details.size();j++)
				{
					owner.push_back(j);
				}

				std::vector<int> chunk_to_be_downloaded[owners_details.size()];
				int mod=filesize%(512*1024);
				int no_of_chunks=filesize/(512*1024);
				if(mod!=0)
					no_of_chunks++;
				int count=1;
				while(count<=no_of_chunks)
				{
					int peer=owner.front();
					chunk_to_be_downloaded[peer].push_back(count);
					owner.pop_front();
					owner.push_back(peer);

					count++;

				}
				file_fd obj[owners_details.size()];
				FILE *fptr[owners_details.size()];
				string fname=spaced_token[2];
				string file_path=pathname+'/'+fname;
				char final_path[file_path.length()+1];
				strcpy(final_path,file_path.c_str());
				cout<<"final path is "<<final_path;
				cout<<"size of owners "<<owners_details.size()<<endl;
				for(int z=0;z<owners_details.size();z++)
				{
					fptr[z]= fopen(final_path,"w+t");
				}
				FILE *fpt=fopen(final_path,"w+t");
				char blank[1]={'0'};
				for(int k=0;k<filesize;k++)
				{
					fwrite(blank,sizeof(blank),1,fpt);
				}

				fclose(fpt);
				for(int y=0;y<owners_details.size();y++)
				{	
					obj[y].fp=fptr[y];
					obj[y].filenm=fname;
					cout<<"the filename is"<<fname;
					string cip=owners_details[y].first;
					
  					obj[y].ip=new char[cip.length()+1];
  					strcpy(obj[y].ip,cip.c_str());

  					cout<<obj[y].ip;
  					obj[y].port=owners_details[y].second;
  					obj[y].chunk_to_download=chunk_to_be_downloaded[y];
  					cout<<obj[y].port;
					
				}
				cout<<"creating pthread"<<endl;
				 pthread_t th[owners_details.size()];

				 for(int peers=0;peers<owners_details.size();peers++)
				 {	cout<<"calling pthread create<<"<<endl;
				 	pthread_create(&th[peers],NULL,get_data_fn,(void*)&obj[peers]);
				 }					
				 for(int d=0;d<owners_details.size();d++)
				 {
				 	pthread_join(th[d],NULL);
				 }
			}
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


void * get_data_fn(void *obj_info)
{
	file_fd *fd_obj=(file_fd*) obj_info;
	cout<<"this "<<fd_obj->ip<<" is ip"<<endl;
	cout<<"this"<<fd_obj->port<<"  is the port"<<endl;
	int client_fd,status;
	struct sockaddr_in addr;
	std::vector<int> ch=fd_obj->chunk_to_download;
	FILE *fp=fd_obj->fp;
	int chunk=512*1024;
	client_fd=socket(AF_INET,SOCK_STREAM,0);
	if(client_fd==0)
	{
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}
	addr.sin_family=AF_INET;
	addr.sin_port=htons(fd_obj->port);
	addr.sin_addr.s_addr=inet_addr(fd_obj->ip);
	char buffer[1024];
	
	status=connect(client_fd,(struct sockaddr*)&addr,sizeof(addr));
	if(status<0)
	{
		perror("connection failed");
		exit(EXIT_FAILURE);
	}	
	
	int x=send(client_fd,"getdata",sizeof(string("getdata")),0);
	int readbytes=recv(client_fd,buffer,sizeof(buffer),0);
	memset(buffer,'\0',1024);

	string name=fd_obj->filenm;
	strcpy(buffer,name.c_str());
	x=send(client_fd,buffer,512,0);
	memset(buffer,'\0',1024);
	readbytes=recv(client_fd,buffer,sizeof(buffer),0);
	memset(buffer,'\0',1024);
	int num=ch.size();

	x=send(client_fd,&num,sizeof(num),0);
	readbytes=recv(client_fd,buffer,sizeof(buffer),0);
	memset(buffer,'\0',1024);


	for(int i=0;i<ch.size();i++)
	{	int bytes_done=0;
		char buff[1024];
		  fseek ( fp , (ch[i]-1)*chunk , SEEK_SET);
		 int seek_pos=(ch[i]-1)*chunk;
		 cout<<"seek pos"<<seek_pos<<endl;
		 x=send(client_fd,&seek_pos,sizeof(seek_pos),0);
		 int n;
		 while((n=recv(client_fd,buffer,chunk,0))>0  && bytes_done <=512*1024)
	    {
	        fwrite(buff,sizeof(char),n,fp);
	        memset(buff,'\0',chunk);
	       //file_size=file_size-n;
	        x=send(client_fd,"ok",sizeof("ok"),0);
	        bytes_done=bytes_done+n;
    	}

	}
}



void * start_fn(void *cs)
{
	threads_info *sti=(threads_info*)cs;
		// cout<<"print"<<sti->client_fd<<" next "<<sti->cip<<" and next is "<<sti->cport;
		//{
		cout<<"entering start fn"<<endl<<endl;	
		 int from_client=(sti->client_fd);
		// cout<<"from client"<<from_client<<sti->cip<<" "<<sti->cport<<endl;
		 char buffer[512];
		 string filename_d;
		 	
			int readbytes=recv(from_client,buffer,512,0);
			char *user_q=buffer;
			cout<<"The received value is "<<user_q<<endl;
			string u=buffer;
			int ret_val;
			if(strcmp(user_q,"quit")==0)
				cout<<"yes"<<endl;
			else if(strcmp(user_q,"chunkinfo")==0)
			{
				
				int x=send(from_client,"ok",sizeof("ok"),0);
				memset(buffer,'\0',512);
				
				readbytes=recv(from_client,buffer,512,0);
				filename_d=buffer;
				memset(buffer,'\0',512);
				
				vector<int>temp_info=chunk_info[filename_d];
				
				int size=temp_info.size();
				x=send(from_client,&size,sizeof(size),0);
				readbytes=recv(from_client,buffer,512,0);
				int index=0;
				while(index<size)
				{
					int to_send;
					to_send=temp_info[index];
					x=send(from_client,&to_send,sizeof(to_send),0);
					memset(buffer,'\0',512);
					readbytes=recv(from_client,buffer,512,0);
					index++;
				}

			}
			else if(strcmp(user_q,"getdata")==0)
			{
				int chunk_no;

				char buffer[512];
				cout<<"inside get data"<<endl;
				int sentbytes=send(from_client,"hi",sizeof("hi"),0);
				char filename[200];
				int readbytes=recv(from_client,filename,sizeof(string(filename)),0);
				string path=fpath[filename];cout<<"the path "<<path<<endl;

				sentbytes=send(from_client,"hi",sizeof("hi"),0);
				int num;
				readbytes=recv(from_client,&num,sizeof(num),0);
				sentbytes=send(from_client,"hi",sizeof("hi"),0);
				cout<<"num "<<num<<endl;
				char fpath[512];
				strcpy(fpath,path.c_str());
				FILE *fp=fopen(fpath,"rb");
				for(int i=0;i<num;i++)
				{	int bytes_done=0;
					int seek_p;
					char buff[1024];
					readbytes=recv(from_client,&seek_p,sizeof(seek_p),0);

					 fseek ( fp , seek_p , SEEK_SET);
					 //int seek_pos=(ch[i]-1)*chunk;
					 cout<<"seek pos"<<seek_p<<endl;
					int n;
					 while((n=fread(buff,sizeof(char),1024,fp))>0 && bytes_done<=1024*512)
				    {
				       send(from_client,buff,n,0);

				        memset(buff,'\0',1024);
				       //file_size=file_size-n;
				      int  x=recv(from_client,buffer,1024,0);
				        bytes_done=bytes_done+n;
			    	}

				}	
				
				


				
				memset(buffer,'\0',512);

			}
		cout<<"done leaving thread"<<endl;
}