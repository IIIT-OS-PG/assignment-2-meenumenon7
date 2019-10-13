#include <stdio.h>
#include<iostream>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include<cstring>
#include<bits/stdc++.h>
#include<unordered_map>
#include<unistd.h>
#include"register.h"
using namespace std;
struct user_details
{
	string uname;
	string pwd;
	char * ip;
	int port;
};
//CAUTION CAUTION CAUTION!
//The ip and port details must be updated at every login time.coz the port might change every time a client connects to the tracker!!
struct group_info
{
	string gid;//group id which user enters
	string owner;//owners username
	char * ip;
	int port;
	list<pair<string,pair<string,int>>> members;//username,client ip,port
	list<pair<string,pair<string,int>>>filenames;//hashoftotal filename,name of file,size
	//vector<>
	unordered_map<string,list<string>> fileowners;//UPLOAD/DOWNLOAD
	//filenames,username for the files which the username people have
	//THis is used for download. When a user asks for download ,list all fileowners which are currently active by finding the port from the 
	//user_details and checking for the same in the active_user map.
	//list<pair<string,list<string>>> hashinfo;//hasoftotal ,list of chunk wise info
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

unordered_map<string,user_details> user_map;
unordered_map<string,group_info> group_map;//groupid and info
unordered_map<int,string>active_user;//port number ,username

// void create_user(int client_fd,string ip,int port_no)
// {
// 	client_fd=
// }
int list_files(threads_info *p)
{
	int from_client=p->client_fd;
	int port_no=p->cport;
	char buffer[512];
	int sentbytes=send(from_client,"hi",sizeof("hi"),0);
	int readbytes=recv(from_client,buffer,512,0);
	cout<<"buffer "<<buffer<<endl;
	string grpid=buffer;
	group_info gi;
	if(group_map.find(grpid)==group_map.end())
		return -1;
	gi=group_map[grpid];
	 for (auto x : gi.filenames) 
      cout << x.second.first<<endl;
  sentbytes=send(p->client_fd,"ok",sizeof("ok"),0);

}

int login_user(struct threads_info *p)
{	//UPDATE THE IP HERE AS WELL
	int from_client=p->client_fd;
	int port_no=p->cport;
	char buffer[512];
	int sentbytes=send(from_client,"hi",sizeof("hi"),0);
	int readbytes=recv(from_client,buffer,512,0);
	string user=buffer;
	memset(buffer,'\0',512);
	sentbytes=send(from_client,"ok",sizeof("ok"),0);
	readbytes=recv(from_client,buffer,512,0);
	string pwd=buffer;
	user_details ud;
	ud=user_map[user];
	string pass=ud.pwd;
	//cout<<"pass rcvd "<<pwd<<"in struct ="<<pass<<endl;
	if(pass==pwd)
	{	//cout<<"Authenticated\n";
		ud.port=port_no;//done
		active_user[port_no]=user;//all active users are maintained here.Once loggedout, remove from here
		string s="Welcome "+user;
		cout<<s<<endl;
		return 1;		
		//send a message to client that authenticated/not
		//sentbytes=send(from_client,"Welcome ",sizeof("ok"),0);
	}
	else
	{	cout<<"Not authenticated";
		return 0;

	}


}
void create_user(struct threads_info *p)
{
	int from_client=p->client_fd;
	char buffer[512];
	user_details ud;
	ud.port=p->cport;
	ud.ip=p->cip;
	int sentbytes=send(from_client,"hi",sizeof("hi"),0);
	int readbytes=recv(from_client,buffer,512,0);
	ud.uname=buffer;
	memset(buffer,'\0',512);
	sentbytes=send(from_client,"ok",sizeof("ok"),0);
	readbytes=recv(from_client,buffer,512,0);
	ud.pwd=buffer;
	sentbytes=send(from_client,"ok",sizeof("ok"),0);
	user_map[ud.uname]=ud;	

	return;
}
int create_group(struct threads_info *p)
{
	
	int from_client=p->client_fd;
	int port_no=p->cport;
	string my_ip=p->cip;
	cout<<"I am insidecreate_group func"<<endl;
	char buffer[512];
	group_info gi;
	int sentbytes=send(from_client,"hi",sizeof("hi"),0);
	
	//check if active
	if(active_user.find(port_no)==active_user.end())
		return -1;


	int readbytes=recv(from_client,buffer,512,0);
	gi.gid=buffer;
	cout<<"the group id is "<<buffer<<endl;
	string username=active_user[port_no];
	gi.owner=username;
	gi.port=port_no;
	gi.ip=p->cip;

	pair<string,int> p1(my_ip,port_no);
	pair<string,pair<string,int>> p2(username,p1);
	gi.members.push_back(p2);

	group_map[buffer]=gi;//added to map
	if(group_map.find(buffer)!=group_map.end())
		cout<<"inserted into map";
	 sentbytes=send(from_client,"Group created successfully ",sizeof("Group created successfully "),0);//ACK sent
	 return 0;
}


int list_groups(threads_info *p)
{
	cout<<endl;

	int port=p->cport;
	if(active_user.find(port)==active_user.end())
		return -1;

	int size_of_list=group_map.size();
	char buffer[512];
  	int sentbytes=send(p->client_fd,&size_of_list,sizeof(size_of_list),0);
  	memset(buffer,'\0',512);
  	int readbytes=recv(p->client_fd,buffer,512,0);
  	for (auto x : group_map) 
    {//   cout << x.first<<endl;
    	string s=x.first;
    	char grp[512];
    	strcpy(grp,s.c_str());
    	cout<<"sending "<<grp<<endl;
    	sentbytes=send(p->client_fd,grp,512,0);
    	readbytes=recv(p->client_fd,buffer,512,0);
    	memset(buffer,'\0',512);
    	memset(grp,'\0',512);

    }
    return 0;
	// unordered_map<string,group_info>::iterator itr;
	// while(itr!=group_map.end())
	// {	
	// 	cout<<*itr.first<<endl;
	// 	itr++;
	// }
}


int upload_file(threads_info *p)
{



	int from_client=p->client_fd;
	int port_no=p->cport;
	string my_ip=p->cip;
	
	int sentbytes=send(from_client,"hi",sizeof("hi"),0);
	char buffer[512];
	int readbytes=recv(from_client,buffer,512,0);
	
	string filename=buffer;
	cout<<"filename "<<filename<<endl;

	memset(buffer,'\0',512);
	sentbytes=send(from_client,"ack",sizeof("ack"),0);
	int filesize=99;
	
	readbytes=recv(from_client,&filesize,sizeof(filesize),0);
	sentbytes=send(from_client,"ack",sizeof("ack"),0);	
	cout<<"filesize "<<filesize<<endl;

	memset(buffer,'\0',512);
	readbytes=recv(from_client,buffer,512,0);
	string groupid=buffer;
	sentbytes=send(from_client,"ack",sizeof("ack"),0);	
	cout<<"gid "<<groupid<<endl;

	// int flag=0;
	 string username=active_user[port_no];
	group_info gi=group_map[groupid];
	// //CHECK IF THE UPLOADER IS A GROUP MEMBER OR NOT
	// for (auto it = gi.members.begin(); it != gi.members.end(); ++it) 
 //     {
 //     	if((*it).first==username)
 //     		flag=1;
 //     }

 //     if(flag==0)
 //     	return -1;

	long long int hashfilesize;
	readbytes=recv(from_client,&hashfilesize,sizeof(hashfilesize),0);
	sentbytes=send(from_client,"ack",sizeof("ack"),0);	
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
			
			
			readbytes=recv(from_client,buffer,512,0);
			cout<<"received "<<readbytes<<endl;
			string t=buffer;
			cout<<"t ="<<t<<endl;
			sentbytes=send(from_client,"ack",sizeof("ack"),0);
			hashfilename=hashfilename+t;
			cout<<"hashfilename "<<hashfilename<<endl;
			//cout<<readbytes<<"=readbytes"<<endl;
			temp=temp-20;
			cout<<"temp"<<temp<<endl;
			memset(buffer,'\0',512);

			



	}
	cout<<"hashfilename"<<hashfilename<<endl;
	//fclose(fp);
	//Entering filenames
	pair<string,int> p1(filename,filesize);
	pair<string,pair<string,int>> p2(hashfilename,p1);
	gi.filenames.push_back(p2);
	
	//Entering fileowners
	list<string> li;
	li.push_back(username);
	gi.fileowners[filename]=li;

	return 1;		





}
int  join_group(threads_info *p)
{
	int from_client=p->client_fd;
	int port_no=p->cport;
	string my_ip=p->cip;

//get the user id of the user calling create group
	string username=active_user[port_no];

	char buffer[512];
	group_info gi;
	int sentbytes=send(from_client,"hi",sizeof("hi"),0);
	int readbytes=recv(from_client,buffer,512,0);
	string groupid=buffer;
	if(group_map.find(groupid)==group_map.end())
	{		cout<<"No such group exits";
			return 0;
	}	
	else
	{
		gi=group_map[groupid];
		int port=gi.port;
		if(active_user.find(port)==active_user.end())
		{
			cout<<"Owner not active now. Try later";//if owner not active
			return 1;
		}
		else
		{
			int client_fd,status;
			struct sockaddr_in addr;
			int to_port=gi.port;
			const char* to_ip=gi.ip;

			client_fd=socket(AF_INET,SOCK_STREAM,0);
			if(client_fd==0)
			{
			perror("Socket creation failed");
			exit(EXIT_FAILURE);
			}
			addr.sin_family=AF_INET;
			addr.sin_port=htons(to_port);
			addr.sin_addr.s_addr=inet_addr(to_ip);

			status=connect(client_fd,(struct sockaddr*)&addr,sizeof(addr));
			if(status<0)
			{
				perror("connection failed");
				exit(EXIT_FAILURE);
			}
			char buffer[512];
			//cout<<status<<endl;
			char *cu="join_request";
			int x=send(client_fd,cu,sizeof(string(cu)),0);
			int readbytes=recv(client_fd,buffer,512,0);//owners response
			string response=buffer;
			if(response=="yes")
			{
				//gi.members.first=username;//enter the details of memebers here
				
				pair<string,int> p1(my_ip,port_no);
				pair<string,pair<string,int>> p2(username,p1);
				gi.members.push_back(p2);


				cout<<"You have joined the group"<<endl;
			}
			else
			{
				cout<<"Your request has been declined by the owner "<<endl;
			}
			x=send(client_fd,"ok",sizeof("ok"),0);
			close(client_fd);
			return 2;//all done

			}
		}

}
void *start_fn(void *cs)

{		
		// int to_client_socket=*(int*)cs;
		 threads_info *sti=(threads_info*)cs;
		// cout<<"print"<<sti->client_fd<<" next "<<sti->cip<<" and next is "<<sti->cport;
		//{
		cout<<"entering start fn"<<endl<<endl;	
		 int from_client=(sti->client_fd);
		// cout<<"from client"<<from_client<<sti->cip<<" "<<sti->cport<<endl;
		 char buffer[512];
		// int readbytes=recv(from_client,buffer,512,0);
		// int sentbytes=send(from_client,"hi",sizeof("hi"),0);
		// string uname=buffer;
		// memset(buffer,'\0',512);
		// cout<<"after clearing buffer="<<buffer<<endl;
		//  readbytes=recv(from_client,buffer,512,0);
		//  //fflush();
		//  cout<<"readbytes for pwd "<<readbytes<<buffer<<endl;
		// string passwor=buffer;
		
		// cout<<"username and password are "<<uname<<passwor<<endl;
		// readbytes=recv(from_client,buffer,512,0);
		// cout<<"pwd "<<buffer<<endl;
		// if(readbytes<=0)
		//  	cout<<"error receiving"<<endl;
		while(1)
		{	cout<<"while 1 inside thread "<<endl;
			int readbytes=recv(from_client,buffer,512,0);
			char *user_q=buffer;
			cout<<"The received value is "<<user_q<<endl;
			string u=buffer;
			int ret_val;
			if(strcmp(user_q,"create_user")==0)
			{	cout<<"calling create user"<<endl;
				create_user(sti);
			
			}

			else if(strcmp(user_q,"login")==0)
			{
				ret_val=login_user(sti);//cout<<"going to login"<<endl;
				cout<<"after return from login"<<endl;
				send(sti->client_fd,&ret_val,sizeof(ret_val),0);
				// if(ret_val==0)
				// {	
				// 	pthread_exit(NULL);	

				// }	
			}
			else if(strcmp(user_q,"create_group")==0)
			{
				create_group(sti);cout<<"back from create group"<<endl;			
			}
			else if(strcmp(user_q,"join_group")==0)
			{
				ret_val=join_group(sti);
				if(ret_val==0||ret_val==1)
					pthread_exit(NULL);

			}
		// 	else if(strcmp(user_q,"leave_group")==0)
		// 	{
		// 		leave_group(sti);
		// 	}
		 	else if(strcmp(user_q,"list_groups")==0)
		 	{
		 		cout<<"calinf leave grpup"<<endl;
		 		ret_val=list_groups(sti);
		 		send(sti->client_fd,&ret_val,sizeof(ret_val),0);
		 	
		 	}
		// 	else if(strcmp(user_q,"accept_request")==0)
		// 	{
		// 		leave_group(sti);
		// 	}
		// 	else if(strcmp(user_q,"list_groups")==0)
		// 	{
		// 		leave_group(sti);
		// 	}
		 	else if(strcmp(user_q,"list_files")==0)
		 	{
		 		list_files(sti);
		 	}
			else if(strcmp(user_q,"upload_file")==0)
		 	{ cout<<"callingupload file"<<endl;
		 		ret_val=upload_file(sti);
		 		if(ret_val==-1)
		 			pthread_exit(NULL);
		 		
			}
		// 	else if(strcmp(user_q,"download_file")==0)
		// 	{
		// 		leave_group(sti);
		// 	}
		// 	else if(strcmp(user_q,"logout")==0)
		// 	{
		// 		logout(sti);
		// 	}
		// 	else if(strcmp(user_q,"stop_share")==0)
		// 	{
		// 		stop_share(sti);
		// 	}

	// 	unordered_map<string,user_details>::iterator itr;
	// itr=user_map.find("uname");
	// while(1)
	// {if(itr!=user_map.end())
	// {	cout<<"itr.second.first";
	// 	cout<<(*itr).second.uname<<endl<<(*itr).second.pwd<<endl;
	// 	cout<<"port and ip "<<(*itr).second.port<<" "<<(*itr).second.ip<<endl;
		
	// }
	// else cout<<"no";
	

	}
		cout<<"going back to main"<<endl;
			//cout<<"thread "; 
		//}
			pthread_exit(NULL);
}
	


int main()
{
	
	int tracker_fd,to_client_fd,choice,status,queue_size=10;

	struct sockaddr_in addr,c_addr;

	int addrl=sizeof(addr);
	int c_addrl=sizeof(c_addr);

	tracker_fd=socket(AF_INET,SOCK_STREAM,0);
	if(tracker_fd==0)
	{
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}
	addr.sin_family=AF_INET;
	addr.sin_port=htons(8203);
	addr.sin_addr.s_addr=inet_addr("127.0.0.1");


	status=bind(tracker_fd,(struct sockaddr*)&addr,sizeof(addr));
	if(status<0)
	{
		perror("Failed to bind");
		exit(EXIT_FAILURE);
	}
	

	status=listen(tracker_fd,queue_size);
	if(status<0)
	{
		perror("Failed to listen");
		exit(EXIT_FAILURE);
	}

	 cout<<"outside while";
	threads_info *tinf;
	while(1)
	{
		 cout<<"inside while"<<endl;

		to_client_fd=accept(tracker_fd,(struct sockaddr*)&c_addr,(socklen_t*)&c_addrl);
		if(to_client_fd<0)
		{
			perror("Accept failed");
			exit(EXIT_FAILURE);
		}
		
		pthread_t th;
		cout<<"creating thread"<<endl;
		tinf=(threads_info*)malloc(sizeof(struct threads_info));
		//tinf=new threads_info;
		 // send_c_addr.sin_port=c_addr.sin_port;
		// // send_c_addr.sin_addr.s_addr=c_addr.sin_addr;
		char *str1=inet_ntoa(c_addr.sin_addr);
		//cout<<endl<<str1<<endl;
		int port_v=ntohs(c_addr.sin_port);
		//cout<<port_v;
		tinf->client_fd=to_client_fd;
		tinf->cport=port_v;






		tinf->cip=str1;
		//cout<<"from client"<<to_client_fd;
		// // tinf->ad=send_c_addr;
		pthread_create(&th,NULL,start_fn,(void*)tinf);//removed &
	//	pthread_create(&th,NULL,start_fn,NULL);


		//pthread_join(th,NULL);
	//cout<<"Select your choice\n";
	//cout<<"1.Register\n2.Login\n";
		cout<<"Bye "<<endl;
		int readbytes;
		char usr[512], pwd[512];
		int flag=0;
	}
}