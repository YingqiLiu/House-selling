#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <ctime>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector> 
using namespace std; 

int main() 
{ 
  int udpfd, tcpfd, connfd; 
  struct sockaddr_in servaddr; 
  char name[]="nunki.usc.edu";
  int n, buyer; 
  pid_t pid=0, pid1=0, pid2=0, pid3=0, pid4=0, pid5=0;

  struct sockaddr_storage their_addr;
  socklen_t sin_size;
  string filename;
  
  pid=fork();
  if(pid==0)
  {
  	exit(0);
  }
  else
  {
  	pid1=fork();
  	if(pid1==0) 
	  {
	  	pid2=fork();
	  	if(pid2==0) 
		  {
		  	pid3=fork();
		  	if(pid3==0) 
			  {
			  	pid4=fork();
			  	if(pid4==0) 
				  {
				  	pid5=fork();
				  	if(pid5==0) exit(0);
				  }
			  }
		  }
	  }	
  }
  if( (udpfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1 )
  { 
    printf("create socket error: %s(errno: %d)\n",strerror(errno),errno); 
    exit(0); 
  } 
  
  struct hostent *he;
  struct in_addr **addr_list;
  struct sockaddr_in server;
    
  he = gethostbyname(name);
  addr_list = (struct in_addr **) he->h_addr_list;
  servaddr.sin_family = AF_INET; 
  servaddr.sin_addr= **addr_list; 
  if(pid1>0) servaddr.sin_port = htons(21400+875), filename="buyer1.txt", buyer=1; 
  else if(pid2>0) servaddr.sin_port = htons(21500+875), filename="buyer2.txt", buyer=2;
  else if(pid3>0) servaddr.sin_port = htons(21600+875), filename="buyer3.txt", buyer=3;
  else if(pid4>0) servaddr.sin_port = htons(21700+875), filename="buyer4.txt", buyer=4;
  else if(pid5>0) servaddr.sin_port = htons(21800+875), filename="buyer5.txt", buyer=5;
  
  int on=1;  
    if((setsockopt(udpfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)  
    {  
        perror("setsockopt failed");  
        exit(EXIT_FAILURE);  
    }  
  
    
  if( bind(udpfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
  {
    printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno); 
    exit(0); 
  } 
  
    printf("The <Buyer%d> has UDP port %d and IP address %s for Phase 1 part 2\n", buyer, ntohs(servaddr.sin_port), inet_ntoa(servaddr.sin_addr));
	string fields[4][3];
	char buff[4][120];
    for(int i=0; i<4; i++)
    {
    	sin_size = sizeof their_addr;
	    n = recvfrom(udpfd, buff[i], sizeof(buff[i]), 0, (struct sockaddr*)&their_addr, &sin_size);
	    buff[i][n] = '\0';
	    if(buff[i][0]=='A' || buff[i][0]=='B') printf("<Buyer%d> Received house information from <Agent1>\n", buyer);
	    else printf("<Buyer%d> Received house information from <Agent2>\n", buyer);
	    istringstream sin(buff[i]);  
	    string field;
	    for(int j=0; j<3; j++)
	    {
	    	getline(sin, field, '.');
	    	fields[i][j]=field;
		}
	}
    
	 close(udpfd);
	 printf("End of Phase 1 part 2 for <Buyer%d>\n", buyer);
	
  ifstream fin(filename.c_str());
  string line; 
  vector<string> fields1;
  while (getline(fin, line, '\r')) 
  {
    istringstream sin(line); 
    string field1; 
    while (getline(sin, field1, ':')) 
      fields1.push_back(field1); 
  }
  stringstream ss;
  int r_sq, r_bg;
  ss << fields1[1];
  ss >> r_sq;
  ss.clear();
  ss.str("");
  ss << fields1[3];
  ss >> r_bg;
  ss.clear();
  ss.str("");
  int num[4][2];
  int m=-1;
  int flag1=0;
  int flag2=0;
  for (int i=0; i<4; i++)
  {
  	int g_lp, g_sq;
    ss << fields[i][1];
    ss >> g_sq;
    ss.clear();
    ss.str("");
    ss << fields[i][2];
    ss >> g_lp;
    ss.clear();
    ss.str("");
	if(g_lp<=r_bg && g_sq>=r_sq)
	{
		m++;
		num[m][1]=i;
		num[m][2]=g_lp;
		if(fields[i][0]=="A" || fields[i][0]=="B") flag1=1;
		else flag2=1;
	}
  }
  int res;
  for(int i=0; i<=m; i++)
  {
  	for(int j=i+1; j<=m; j++)
  	{
  		if(num[i][2]>num[j][2])
  		{
  			res=num[i][2];
  			num[i][2]=num[j][2];
  			num[j][2]=res;
  			res=num[j][1];
  			num[j][1]=num[i][1];
  			num[i][1]=res;
		}
	}
  }
  char buffer[120];
  for(int i=0; i<120; i++) buffer[i]='\0';
  if(pid1>0) strcat(buffer,"1."); 
  else if(pid2>0) strcat(buffer,"2.");
  else if(pid3>0) strcat(buffer,"3.");
  else if(pid4>0) strcat(buffer,"4.");
  else if(pid5>0) strcat(buffer,"5.");
  strcat(buffer,fields1[3].c_str());
  for(int i=0; i<=m; i++)
  {
  	strcat(buffer,".");
	strcat(buffer,fields[num[i][1]][0].c_str());
  }
  
  close(tcpfd);
  tcpfd = socket(AF_INET, SOCK_STREAM, 0);
  
  char guest_ip[20];
  struct sockaddr_in guest; 
  socklen_t guest_len=sizeof(guest);
  
  if(flag1==1 && flag2==0)
  {
  	servaddr.sin_port = htons(3700+875);
    connect(tcpfd , (struct sockaddr *)&servaddr , sizeof(servaddr));
    getsockname(tcpfd, (struct sockaddr *)&guest, &guest_len);
    inet_ntop(AF_INET, &guest.sin_addr, guest_ip, sizeof(guest_ip));
    printf("The <Buyer%d> has TCP port %d and IP address %s for Phase 1 part 3\n", buyer, ntohs(guest.sin_port), guest_ip);
    send(tcpfd , buffer , strlen(buffer) , 0);
    printf("<Buyer%d> has sent< %s > to <Agent1>\n", buyer, buffer);
    close(tcpfd);
    
	
	tcpfd = socket(AF_INET, SOCK_STREAM, 0);
    servaddr.sin_port = htons(3800+875);
    connect(tcpfd , (struct sockaddr *)&servaddr , sizeof(servaddr));
    getsockname(tcpfd, (struct sockaddr *)&guest, &guest_len);
    inet_ntop(AF_INET, &guest.sin_addr, guest_ip, sizeof(guest_ip));
    printf("The <Buyer%d> has TCP port %d and IP address %s for Phase 1 part 3\n", buyer, ntohs(guest.sin_port), guest_ip);
    strcat(buffer,".NAK");
    send(tcpfd , buffer , strlen(buffer), 0);
    printf("<Buyer%d> has sent< %s > to <Agent2>\n", buyer, buffer);
  }
  else if(flag1==0 && flag2==1)
  {
  	servaddr.sin_port = htons(3800+875);
    connect(tcpfd , (struct sockaddr *)&servaddr , sizeof(servaddr));
    getsockname(tcpfd, (struct sockaddr *)&guest, &guest_len);
    inet_ntop(AF_INET, &guest.sin_addr, guest_ip, sizeof(guest_ip));
    printf("The <Buyer%d> has TCP port %d and IP address %s for Phase 1 part 3\n", buyer, ntohs(guest.sin_port), guest_ip);
    send(tcpfd , buffer , strlen(buffer) , 0);
    printf("<Buyer%d> has sent< %s > to <Agent2>\n", buyer, buffer);
    close(tcpfd);
    
	
	tcpfd = socket(AF_INET, SOCK_STREAM, 0);
	servaddr.sin_port = htons(3700+875);
    connect(tcpfd , (struct sockaddr *)&servaddr , sizeof(servaddr));
    getsockname(tcpfd, (struct sockaddr *)&guest, &guest_len);
    inet_ntop(AF_INET, &guest.sin_addr, guest_ip, sizeof(guest_ip));
    printf("The <Buyer%d> has TCP port %d and IP address %s for Phase 1 part 3\n", buyer, ntohs(guest.sin_port), guest_ip);
    strcat(buffer,".NAK");
    send(tcpfd , buffer , strlen(buffer), 0);
    printf("<Buyer%d> has sent< %s > to <Agent1>\n", buyer, buffer);
  }
  else if(flag1==1 && flag2==1)
  {
  	servaddr.sin_port = htons(3700+875);
    connect(tcpfd , (struct sockaddr *)&servaddr , sizeof(servaddr));
    getsockname(tcpfd, (struct sockaddr *)&guest, &guest_len);
    inet_ntop(AF_INET, &guest.sin_addr, guest_ip, sizeof(guest_ip));
    printf("The <Buyer%d> has TCP port %d and IP address %s for Phase 1 part 3\n", buyer, ntohs(guest.sin_port), guest_ip);
    send(tcpfd , buffer , strlen(buffer) , 0);
    printf("<Buyer%d> has sent< %s > to <Agent1>\n", buyer, buffer);
    close(tcpfd);
    
	
	tcpfd = socket(AF_INET, SOCK_STREAM, 0);
    servaddr.sin_port = htons(3800+875);
    connect(tcpfd , (struct sockaddr *)&servaddr , sizeof(servaddr));
    getsockname(tcpfd, (struct sockaddr *)&guest, &guest_len);
    inet_ntop(AF_INET, &guest.sin_addr, guest_ip, sizeof(guest_ip));
    printf("The <Buyer%d> has TCP port %d and IP address %s for Phase 1 part 3\n", buyer, ntohs(guest.sin_port), guest_ip);
    send(tcpfd , buffer , strlen(buffer) , 0);
    printf("<Buyer%d> has sent< %s > to <Agent2>\n", buyer, buffer);
  }
  else
  {
  	servaddr.sin_port = htons(3700+875);
    connect(tcpfd , (struct sockaddr *)&servaddr , sizeof(servaddr));
    getsockname(tcpfd, (struct sockaddr *)&guest, &guest_len);
    inet_ntop(AF_INET, &guest.sin_addr, guest_ip, sizeof(guest_ip));
    printf("The <Buyer%d> has TCP port %d and IP address %s for Phase 1 part 3\n", buyer, ntohs(guest.sin_port), guest_ip);
    strcat(buffer,".NAK");
    send(tcpfd , buffer , strlen(buffer), 0);
    printf("<Buyer%d> has sent< %s > to <Agent1>\n", buyer, buffer);
    close(tcpfd);
    
	
	tcpfd = socket(AF_INET, SOCK_STREAM, 0);
    servaddr.sin_port = htons(3800+875);
    connect(tcpfd , (struct sockaddr *)&servaddr , sizeof(servaddr));
    getsockname(tcpfd, (struct sockaddr *)&guest, &guest_len);
    inet_ntop(AF_INET, &guest.sin_addr, guest_ip, sizeof(guest_ip));
    printf("The <Buyer%d> has TCP port %d and IP address %s for Phase 1 part 3\n", buyer, ntohs(guest.sin_port), guest_ip);
    strcat(buffer,".NAK");
    send(tcpfd , buffer , strlen(buffer), 0);
    printf("<Buyer%d> has sent< %s > to <Agent2>\n", buyer, buffer);
  }

  close(tcpfd);
  printf("End of Phase 1 part 3 for <Buyer%d>\n", buyer);
  
  
  if(pid1>0) servaddr.sin_port = htons(3900+875); 
  else if(pid2>0) servaddr.sin_port = htons(4000+875);
  else if(pid3>0) servaddr.sin_port = htons(4100+875);
  else if(pid4>0) servaddr.sin_port = htons(4200+875);
  else if(pid5>0) servaddr.sin_port = htons(4300+875);
  tcpfd = socket(AF_INET, SOCK_STREAM, 0);
  bind(tcpfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
  listen(tcpfd, 10);
  printf("<Buyer%d> has TCP port %d and IP address %s for Phase 1 part 4\n", buyer, ntohs(servaddr.sin_port), inet_ntoa(servaddr.sin_addr));
  char buff1[3][15];
  printf("<Buyer%d>: ", buyer);
  for(int i=1; i<=2; i++)
  {
    connfd = accept(tcpfd, (struct sockaddr*)&their_addr, &sin_size);
    n = recv(connfd, buff1[i], sizeof(buff1[i]), 0);
    buff1[i][n] = '\0';
    if(buff1[i][0]!='N') printf("Will buy house from <Seller%c>\n", buff1[i][0]);
    close(connfd);
  }
    if(buff1[1][0]=='N' && buff1[2][0]=='N') printf("%s\n", buff1[1]);
	 close(tcpfd);
	 printf("End of Phase 1 part 4 for <Buyer%d>\n", buyer);
	 
}
