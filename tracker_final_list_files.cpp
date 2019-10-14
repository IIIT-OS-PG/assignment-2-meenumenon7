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
int gp_count=0;

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
	char  *ip;
	//undoyaha tak
	int port;//listen port
	list<pair<string,pair<string,int>>> members;//username,client ip,port(to which  it listens to)
	//list<pair<string,pair<string,int>>>filenames;//hashoftotal filename,name of file,size
	unordered_map<string,pair<string,int>> filenames;//filename ,hash value,filesize
	//vector<>
	unordered_map<string,vector<string>> fileowners;//UPLOAD/DOWNLOAD
	//filenames,username for the files which the username people have
	//THis is used for download. When a user asks for download ,list all fileowners which are currently active by finding the port from the 
	//user_details and checking for the same in the active_user map.
	//then use gid usernae client ip and port to check
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
struct pending_requests
{
	int lport;
	char *lip;
	string requester_uname;
};
unordered_map<string,vector<string>> pending_count_map;//so as to get all pending per group owner
unordered_map<string,vector<pending_requests>>pending_map;//<gid,username of requester>,< username,listen<ip,port>> of requester
//get the username from the activeuser
unordered_map<string,user_details> user_map;
unordered_map<string,group_info> group_map;//groupid and info
unordered_map<int,string>active_user;//port number ,username

// void create_user(int client_fd,string ip,int port_no)
// {
// 	client_fd=
// }
int list_files(threads_info *p)
{
	cout<<"inside list files";
	int from_client=p->client_fd;
	int port_no=p->cport;
	char buffer[512];
	int sentbytes=send(from_client,"hi",sizeof("hi"),0);
	int readbytes=recv(from_client,buffer,512,0);
	cout<<"The recv val is "<<buffer;
	
	sentbytes=send(from_client,"hi",sizeof("hi"),0);

	//getthe listen ip and port
	//######### GET the listen port n ip of the peer
    int listenport;
	readbytes=recv(from_client,&listenport,sizeof(listenport),0);
	sentbytes=send(from_client,"ok",sizeof("ok"),0);
	//get ip
	char listenip[512];
	readbytes=recv(from_client,listenip,512,0);
	//sentbytes=send(from_client,"ok",sizeof("ok"),0);
	//##################################################

	cout<<"listen ip"<<listenip<<listenport<<endl;
	if(active_user.find(port_no)==active_user.end())
	{	cout<<"incative\n;";
		return -1;

	}
	cout<<"active"<<endl;
	string username=active_user[port_no];
	cout<<"username is "<<username<<endl;
	cout<<"buffer "<<buffer<<endl;
	string grpid=buffer;
	group_info gi;
	if(group_map.find(grpid)==group_map.end())
		return -2;
	cout<<"well the group exists"<<endl;
	

	gi=group_map[grpid];

	pair<string,int>p1(listenip,listenport);
	pair<string,pair<string,int>>p2(username,p1);
	list<pair<string,pair<string,int>>>::iterator itr;
	itr=find(gi.members.begin(),gi.members.end(),p2);
	if(itr==gi.members.end())
		return -3;
	else
		cout<<"memeber hai\n";



	int no_of_files=gi.filenames.size();
	cout<<"no of files "<<no_of_files<<endl;

	 std::vector<string> temp;
	  for (auto x : gi.filenames) 
      {	temp.push_back(x.first);
       	cout << x.first<<endl;
       } 
  	sentbytes=send(p->client_fd,&no_of_files,sizeof(no_of_files),0);
	int var=0;
  	while(var<no_of_files)
  	{	memset(buffer,'\0',512);
  		readbytes=recv(from_client,buffer,512,0);

  		string tosend=temp[var];
  		memset(buffer,'\0',512);
  		strcpy(buffer,tosend.c_str());
  		cout<<"buffer is "<<buffer<<endl;
  		sentbytes=send(from_client,buffer,512,0);
  		var++;

  	}


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
	cout<<ud.ip<<endl;
	int sentbytes=send(from_client,"hi",sizeof("hi"),0);
	int readbytes=recv(from_client,buffer,512,0);
	ud.uname=buffer;
	//cout<<ud.uname;
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
	// if(active_user.find(port_no)==active_user.end())
	// {	cout<<"incative\n;";
	// 	return -1;

	// }
	int readbytes=recv(from_client,buffer,512,0);
	gi.gid=buffer;
	cout<<"the group id is "<<buffer<<endl;
	sentbytes=send(from_client,"ok",sizeof("ok"),0);
	//memset(buffer,'\0',512);
	//######### GET the listen port n ip of the peer
    int listenport;
	readbytes=recv(from_client,&listenport,sizeof(listenport),0);
	sentbytes=send(from_client,"ok",sizeof("ok"),0);
	//get ip
	char listenip[512];
	readbytes=recv(from_client,listenip,512,0);
	//sentbytes=send(from_client,"ok",sizeof("ok"),0);
	//##################################################
	if(active_user.find(port_no)==active_user.end())
	{	cout<<"incative\n;";
		return -1;

	}

	string username=active_user[port_no];
	gi.owner=username;
	gi.port=listenport;
	//cout<<gi.port<<endl;
	//strcpy(gi.ip,listenip);
	//cout<<gi.ip<<endl;
	gi.ip=listenip;
	gp_count++;
	pair<string,int> p1(listenip,listenport);
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
	{	cout<<"inactive user list_groups;"<<endl;
		return -1;
	}
	int siz=0;
	// for(auto x : group_map){
	// 	siz++;
	// }
	//int size_of_list=siz;
	int size_of_list=group_map.size();
	if(size_of_list==0)
		return -2;
	char buffer[512];
	int sentbytes=send(p->client_fd,&size_of_list,sizeof(size_of_list),0);
	// char intbuf[10];
	// string s=to_string(gp_count);
	// strcpy(intbuf,s.c_str());
 //  	int sentbytes=send(p->client_fd,intbuf,10,0);
 //  	cout<<"sending list size as "<<intbuf<<endl;
  	//int sentbytes=send(p->client_fd,&gp_count,sizeof(gp_count),0);
  	memset(buffer,'\0',512);
  	int readbytes=recv(p->client_fd,buffer,512,0);
 	//return 0;
  	cout<<"buffer"<<buffer<<endl;
  	for (auto x : group_map) 
    {//   cout << x.first<<endl;
    	string s=x.first;
    	char grp[512];
    	cout<<s<<"is the gid"<<endl;
    	strcpy(grp,s.c_str());
    	cout<<"sending "<<grp<<endl;
    	sentbytes=send(p->client_fd,grp,512,0);
    	readbytes=recv(p->client_fd,buffer,512,0);
    	cout<<"received inside  group_map"<<buffer<<endl;
    	memset(buffer,'\0',512);
    	memset(grp,'\0',512);

    }
    cout<<"exiting list_groups"<<endl;
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
	int port=p->cport;
	if(active_user.find(port)==active_user.end())
	{	cout<<"inactive user upload file;"<<endl;
		return -1;
	}


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
	//sentbytes=send(from_client,"ack",sizeof("ack"),0);	
	cout<<"gid "<<groupid<<endl;

	 int flag=0;
	 group_info gi;
	 string username=active_user[port_no];
	if(group_map.find(groupid)!=group_map.end())
	{	cout<<"found the group"<<endl;
		gi=group_map[groupid];
	// //CHECK IF THE UPLOADER IS A GROUP MEMBER OR NOT
	 for (auto it = gi.members.begin(); it != gi.members.end(); ++it) 
     {
      	if((*it).first==username)
      		flag=1;
      }
    }
      cout<<flag<<"=flag";
      if(flag==0)
      	return -2;
     sentbytes=send(from_client,"ack",sizeof("ack"),0);
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
	cout<<endl<<"filename is <<"<<filename<<endl;
	cout<<"the username is "<<username<<endl;
	//fclose(fp);
	//Entering filenames
	//pair<string,int> p1(filename,filesize);
	pair<string,int> p1(hashfilename,filesize);
	
	//pair<string,pair<string,int>> p2(hashfilename,p1);
	//pair<string,pair<string,int>> p2(filename,p1);
	
	//gi.filenames.push_back(p2);
	gi.filenames[filename]=p1;
	if(gi.filenames.find(filename)!=gi.filenames.end())
		cout<<"entered successfully"<<endl;
	else
		cout<<"map is empty"<<endl;
	cout<<"size of map "<<gi.filenames.size()<<endl;
	//Entering fileowners
	vector<string> li;
	li.push_back(username);
	gi.fileowners[filename]=li;

	group_map[groupid]=gi;

	return 1;		





}
bool MatchMember(pending_requests m1, pending_requests m2)
{
	return  (m1.requester_uname==m2.requester_uname);
}

int accept_request(threads_info *p)
{
	int from_client=p->client_fd;
	int port_no=p->cport;
	string myip=p->cip;

//get the user id of the user calling create group
	
	char buffer[512];
	group_info gi;
	int sentbytes=send(from_client,"hi",sizeof("hi"),0);
	int readbytes=recv(from_client,buffer,512,0);
	string grpid=buffer;

	memset(buffer,'\0',512);
	sentbytes=send(from_client,"hi",sizeof("hi"),0);
	readbytes=recv(from_client,buffer,512,0);
	string username=buffer;
	memset(buffer,'\0',512);

	//remove from pending_count_map
	vector<string> user_from_map=pending_count_map[grpid];
	std::vector<string>::iterator it; 


	it = std::find (user_from_map.begin(), user_from_map.end(), username); 
    if (it != user_from_map.end()) 
    { 
       cout << "Element  found at position : "<<endl ; 
    	user_from_map.erase(it);
    	pending_count_map[grpid]=user_from_map;
    } 

    //remove from pending_map

	
	pending_requests pqstruct;
	pqstruct.requester_uname=username;

	vector<pending_requests> vpq;
	vpq=pending_map[grpid];

	vector<pending_requests> searchlist;
	searchlist.push_back(pqstruct);

	pending_requests to_be_inserted;
	vector<pending_requests>::iterator itr = std::search(vpq.begin(),vpq.end(),searchlist.begin(),searchlist.end(),MatchMember);

	if (itr != vpq.end()) 
    { 
    	 cout << "Element  found at position : "<<endl ; 
    	 to_be_inserted=(*itr);//copy it to another struct
    	 vpq.erase(itr);
    	 pending_map[grpid]=vpq;
    	 cout<<"now size of vpq is"<<vpq.size()<<endl;
       
    } 

    //Add it to the member list;
    group_info gi_temp;
    gi_temp=group_map[grpid];

    //get ip
    int port=to_be_inserted.lport;

    //get port
    string my_ip=to_be_inserted.lip;

    pair<string,int>p1(my_ip,port);
    
    pair<string,pair<string,int>>p2(username,p1);

    gi_temp.members.push_back(p2);

    group_map[grpid]=gi_temp;//added to map

    cout<<"now themembers are "<<gi_temp.members.size()<<endl;
	if(group_map.find(grpid)!=group_map.end())
		cout<<"inserted into map";
    
	sentbytes=send(from_client,"ok",sizeof("ok"),0);


}

int  join_group(threads_info *p)
{
	int from_client=p->client_fd;
	int port_no=p->cport;
	string my_ip=p->cip;

//get the user id of the user calling create group
	
	char buffer[512];
	group_info gi;
	int sentbytes=send(from_client,"hi",sizeof("hi"),0);
	int readbytes=recv(from_client,buffer,512,0);
	string groupid=buffer;
	string username;
	if(active_user.find(port_no)!=active_user.end())
		 username=active_user[port_no];
	else
		return -1;
	if(group_map.find(groupid)==group_map.end())
	{		//cout<<"No such group exits";
			return -2;
	}	
	else
	{	sentbytes=send(from_client,"ok",sizeof("ok"),0);
		gi=group_map[groupid];
		//int port=gi.port;
		//get the listen ip and port details of requster
		//######### GET the listen port n ip of the peer
	    int listenport;
		readbytes=recv(from_client,&listenport,sizeof(listenport),0);
		sentbytes=send(from_client,"ok",sizeof("ok"),0);
		//get ip
		char listenip[512];
		readbytes=recv(from_client,listenip,512,0);
		sentbytes=send(from_client,"ok",sizeof("ok"),0);
		//##################################################
		//store in pending requests
		pending_requests pq;
		pq.lport=listenport;
		pq.lip=listenip;
		pq.requester_uname=username;
		vector<string> req;
		if(pending_count_map.find(groupid)!=pending_count_map.end())
			req=pending_count_map[groupid];

		cout<<"current pending are "<<req.size()<<endl;
		req.push_back(username);
		cout<<"now it is "<<req.size()<<endl;
		pending_count_map[groupid]=req;
		//pair<string,string> p1(group_id,username);
		vector<pending_requests> vpq;
		if(pending_map.find(groupid)!=pending_map.end())
			vpq=pending_map[groupid];
		vpq.push_back(pq);
		pending_map[groupid]=vpq;
		 if(pending_map.find(groupid)!=pending_map.end())
		 	cout<<"inserted into pq"<<endl;


		 return 0;
		
	}
		// }

}

 int download_file(threads_info *p)
 {

 	int from_client=p->client_fd;
// 	int port_no=p->cport;
// 	string my_ip=p->cip;

// //get the user id of the user calling create group
	
// 	char buffer[512];
// 	group_info gi;
// 	int sentbytes=send(from_client,"hi",sizeof("hi"),0);
// 	int readbytes=recv(from_client,buffer,512,0);
// 	string grpid=buffer;
// 	memset(buffer,'\0',512);
	
// 	int sentbytes=send(from_client,"hi",sizeof("hi"),0);
// 	int readbytes=recv(from_client,buffer,512,0);
// 	string filename=buffer;
// 	memset(buffer,'\0',512);

// 	//getthe listen ip and port
// 	//######### GET the listen port n ip of the peer
//     int listenport;
// 	readbytes=recv(from_client,&listenport,sizeof(listenport),0);
// 	sentbytes=send(from_client,"ok",sizeof("ok"),0);
// 	//get ip
// 	char listenip[512];
// 	readbytes=recv(from_client,listenip,512,0);
// 	//sentbytes=send(from_client,"ok",sizeof("ok"),0);
// 	//##################################################



// 	int port=p->cport;
// 	if(active_user.find(port)==active_user.end())
// 	{	cout<<"inactive download file"<<endl;
// 		return -1;
// 	}

// 	if(group_map.find(grpid)==group_map.end())
// 		return -2;
// 	cout<<"well the group exists"<<endl;
	


// 	gi=group_map[grpid];

// 	pair<string,int>p1(listenip,listenport);
// 	pair<string,pair<string,int>>p2(username,p1);
// 	list<pair<string,pair<string,int>>>::iterator itr;
// 	itr=find(gi.members.begin(),gi.members.end(),p2);
// 	if(itr==gi.members.end())
// 		return -3;
	
// 	vector<string> temp_owners;
// 	temp_owners=gi.fileowners[filename];

// 	int no_of_owners=temp_owners.size();

// 	vector<pair<string,int>>
// 	//vector<int> port_nos;
// 	//vector<string> owner_ips;





// 
}
int  list_requests(threads_info *p)
{
	int from_client=p->client_fd;
	int port_no=p->cport;
	string my_ip=p->cip;

//get the user id of the user calling create group
	
	char buffer[512];
	group_info gi;
	int sentbytes=send(from_client,"hi",sizeof("hi"),0);
	int readbytes=recv(from_client,buffer,512,0);
	// //get no of groups
	// //then getthe gids
	// int no_of_groups;
	// int readbytes=recv(from_client,&no_of_groups,sizeof(no_of_groups),0);
	// sentbytes=send(from_client,"ok",sizeof("ok"),0);

	// int x=0;
	// std::vector<string> temp;
	// while(x<no_of_groups)
	// {
	// 	readbytes=recv(from_client,buffer,512,0);
	// 	temp.push_back(buffer);
	// 	memset(buffer,'\0',512);
	// 	sentbytes=send(from_client,"ok",sizeof("ok"),0);

	// }

	//CaLcUlAtE the coount of the vecor then send it
	//sentbytes=send(from_client,"ok",sizeof("ok"),0);
	string grpid=buffer;
	cout<<"the grp id recd is "<<grpid<<endl;
		if(pending_count_map.find(grpid)!=pending_count_map.end())
		{
			std::vector<string> unames=pending_count_map[grpid];
			int unames_size;
			unames_size=unames.size();
			sentbytes=send(from_client,&unames_size,sizeof(unames_size),0);
			for(int k=0;k<unames.size();k++)
			{
				memset(buffer,'\0',512);
				readbytes=recv(from_client,buffer,512,0);
				char buff[512];
				strcpy(buff,unames[k].c_str());
				sentbytes=send(from_client,buff,sizeof(string(buff)),0);
				memset(buff,'\0',512);


			}
		}
		else
		{   int unames_size=0;
			sentbytes=send(from_client,&unames_size,sizeof(unames_size),0);
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
				ret_val=create_group(sti);cout<<"back from create group"<<endl;			
				if(ret_val==-1)
					send(sti->client_fd,"notlogged",sizeof("notlogged"),0);
			}
			else if(strcmp(user_q,"join_group")==0)
			{
				ret_val=join_group(sti);
				if(ret_val==-1)
					send(sti->client_fd,"notlogged",sizeof("notlogged"),0);
				if(ret_val==-2)
					send(sti->client_fd,"nogroup",sizeof("nogroup"),0);

			}
		// 	else if(strcmp(user_q,"leave_group")==0)
		// 	{
		// 		leave_group(sti);
		// 	}
		 	else if(strcmp(user_q,"list_groups")==0)
		 	{
		 		cout<<"calinf list grpup"<<endl;
		 		ret_val=list_groups(sti);
		 		cout<<"the return value is "<<ret_val<<endl;
		 		 if(ret_val==-1)
		 			  send(sti->client_fd,&ret_val,sizeof(ret_val),0);
		 		if(ret_val==-2)
		 			 send(sti->client_fd,&ret_val,sizeof(ret_val),0);
		 	}
		 	else if(strcmp(user_q,"accept_request")==0)
		 	{
		 		ret_val=accept_request(sti);
		 	}
			else if(strcmp(user_q,"list_requests")==0)
		 	{
		 		cout<<"callinglist req"<<endl;
		 		ret_val=list_requests(sti);
			}
		 	else if(strcmp(user_q,"list_files")==0)
		 	{ cout<<"calling list_files";
		 		// char ni[10]="bye";
		 		// cout<<ni<<endl;
		 		// cout<<sti->client_fd<<endl;
		 		// int x= send(sti->client_fd,ni,10,0);
		 		//  cout<<"send bye"<<x<<endl;
		 		//  memset(buffer,'\0',512);
		 		// recv(sti->client_fd,buffer,512,0);

		 		ret_val=list_files(sti);
		 		cout<<"the ret val is "<<ret_val<<endl;
		 		if(ret_val==-1)

		 		{	//ret_val=10000;
		 			int g= send(sti->client_fd,&ret_val,sizeof(ret_val),0);
		 			cout<<"g "<<g<<endl;
		 		}
		 		if(ret_val==-2)
		 		{	//ret_val=20000;
		 			send(sti->client_fd,&ret_val,sizeof(ret_val),0);
		 		}	 
		 		if(ret_val==-2)
		 		{	//ret_val=30000;
		 			send(sti->client_fd,&ret_val,sizeof(ret_val),0);

		 		}	 

		 	}
			else if(strcmp(user_q,"upload_file")==0)
		 	{ cout<<"callingupload file"<<endl;
		 		ret_val=upload_file(sti);
		 		if(ret_val==-1)
		 			int x=send(sti->client_fd,"notlogged",sizeof("notlogged"),0);
		 		if(ret_val==-2)
		 			int sentbytes=send(sti->client_fd,"notmember",sizeof("notmember"),0);
			}
		 	else if(strcmp(user_q,"download_file")==0)
		 	{
		 		//leave_group(sti);
		 		ret_val=download_file(sti);
		 	}
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