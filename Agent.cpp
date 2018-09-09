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
  pid_t pid;
  struct sockaddr_in servaddr; 
  char name[]="nunki.usc.edu";
  int n, agent, agent_port; 

  struct sockaddr_storage their_addr;
  socklen_t sin_size;
  pid=fork();
  if( (tcpfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
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
  
  if (pid>0)
  servaddr.sin_port = htons(3700+875), agent=1, agent_port=3700+875; 
  else servaddr.sin_port = htons(3800+875), agent=2, agent_port=3800+875; 
  
  int on=1;  
    if((setsockopt(tcpfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)  
    {  
        perror("setsockopt failed");  
        exit(EXIT_FAILURE);  
    }  
  
    printf("<Agent%d> has TCP port %d and IP address %s for Phase 1 part 1\n", agent, ntohs(servaddr.sin_port), inet_ntoa(servaddr.sin_addr));
  if( bind(tcpfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
  {
    printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno); 
    exit(0); 
  } 
  if( listen(tcpfd, 10) == -1)
  { 
    printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno); 
    exit(0); 
  } 
  
  int m=0;
  char buff1[2][20];
  while(1)
  {
	sin_size = sizeof their_addr;
	 if( (connfd = accept(tcpfd, (struct sockaddr*)&their_addr, &sin_size)) == -1)
     { 
       printf("accept socket error\n"); 
     } 
	 n = recv(connfd, buff1[m], sizeof(buff1[m]), 0);
	 buff1[m][n] = '\0';
	 if(buff1[m][n-1]=='\n') buff1[m][n-1] = '\0';
     printf("Received house information from <Seller%c>\n", buff1[m][0]);
	 close(connfd);
	 m++;
	 if(m==2)break;
  }
  printf("End of Phase 1 part 1 for <Agent%d>\n", agent);
  
  sleep(8);
  for(int j=0; j<2; j++)
  {
  	for (int i=0; i<5; i++)
	 {
	 	servaddr.sin_port = htons(21400+875+i*100); 
	 	printf("The <Agent%d> has UDP port %d and IP address %s for Phase 1 part 2\n", agent, (21400+875+i*100), inet_ntoa(servaddr.sin_addr));
	    udpfd = socket(AF_INET, SOCK_DGRAM, 0);
	    sendto(udpfd, buff1[j], strlen(buff1[j]), 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
	    printf("<Agent%d> has sent < %s > to <Buyer%d>\n", agent, buff1[j], i+1);
    	close(udpfd);
    	
	 }
  }
     printf("End of Phase 1 part 2 for <Agent%d>\n", agent);

	 int c=0;
	 int max_A=0, max_B=0, max_C=0, max_D=0;
	 char buyA[1], buyB[1], buyC[1], buyD[1];
	 buyA[0]='\0', buyB[0]='\0', buyC[0]='\0', buyD[0]='\0';
	 int price[6], rank[6];
	 char prefer[6][5];
	 printf("The <Agent%d> has TCP port %d and IP address %s for Phase 1 part 3\n", agent, agent_port, inet_ntoa(servaddr.sin_addr));
	 while(1)
	 {
	 	c++;
		connfd = accept(tcpfd, (struct sockaddr*)&their_addr, &sin_size);
	    char buff3[120];
	    n = recv(connfd, buff3, sizeof(buff3), 0);
		buff3[n] = '\0';
	    istringstream sin(buff3);  
	    string field;
	    printf("<Agent%d> receives the offer from <Buyer%c>\n", agent, buff3[0]);
	    rank[c]=c;
	    price[c]=0;
	    
	    for(int i=0; getline(sin, field, '.'); i++)
	    {
	    	if(field=="NAK") 
			{
				price[c]=0;
				for(int j=1; j<i; j++)
				{
					prefer[c][j]='\0';
				}
				break;
			}
	    	if(i==0) prefer[c][0]=field[0];
	    	else if(i==1) 
	    	{
	    		 stringstream ss;
                 ss << field;
                 ss >> price[c];
			}
			else prefer[c][i-1]=field[0];
		}
	    close(connfd);
	    if(c==5) break;
	 }
	 int res;
	 for(int i=1; i<=5; i++)
	 {
	 	for(int j=i+1; j<=5; j++)
	 	{
	 		if(price[rank[i]]<price[rank[j]])
	 		{
	 			res=rank[i];
	 			rank[i]=rank[j];
	 			rank[j]=res;
			}
		}
	 }
	 for(int i=1; i<=5; i++)
	 {
	 	for(int j=1; j<=4; j++)
	 	{
	 		if(prefer[rank[i]][j]=='A' && max_A==0) 
			{
				buyA[0]=prefer[rank[i]][0];
				max_A=price[rank[i]];
				break;
			}
			if(prefer[rank[i]][j]=='B' && max_B==0) 
			{
				buyB[0]=prefer[rank[i]][0];
				max_B=price[rank[i]];
				break;
			}
			if(prefer[rank[i]][j]=='C' && max_C==0) 
			{
				buyC[0]=prefer[rank[i]][0];
				max_C=price[rank[i]];
				break;
			}
			if(prefer[rank[i]][j]=='D' && max_D==0) 
			{
				buyD[0]=prefer[rank[i]][0];
				max_D=price[rank[i]];
				break;
			}
		}
		 
	 }
	     
	 close(tcpfd);
	 tcpfd = socket(AF_INET, SOCK_STREAM, 0);
	 printf("End of Phase 1 part 3 for <Agent%d>\n", agent);
	 
	 char guest_ip[20];
     struct sockaddr_in guest; 
     socklen_t guest_len=sizeof(guest);
     
	 if(pid>0)
	 {
	 	 servaddr.sin_port = htons(3300+875);
		 connect(tcpfd , (struct sockaddr *)&servaddr , sizeof(servaddr));
		 getsockname(tcpfd, (struct sockaddr *)&guest, &guest_len);
         inet_ntop(AF_INET, &guest.sin_addr, guest_ip, sizeof(guest_ip));
         printf("The <Agent%d> has TCP port %d and IP address %s for Phase 1 part 4\n", agent, ntohs(guest.sin_port), guest_ip);
		 if(max_A==0) send(tcpfd , "NAK", 3, 0);
		 else  send(tcpfd , buyA, 1, 0);
		 printf("The <Agent%d> has send the result to <SellerA>\n", agent);
		 close(tcpfd);
    	 tcpfd = socket(AF_INET, SOCK_STREAM, 0);
		 servaddr.sin_port = htons(3400+875);
		 connect(tcpfd , (struct sockaddr *)&servaddr , sizeof(servaddr));
		 getsockname(tcpfd, (struct sockaddr *)&guest, &guest_len);
         inet_ntop(AF_INET, &guest.sin_addr, guest_ip, sizeof(guest_ip));
         printf("The <Agent%d> has TCP port %d and IP address %s for Phase 1 part 4\n", agent, ntohs(guest.sin_port), guest_ip);
		 if(max_B==0) send(tcpfd , "NAK", 3, 0);
		 else  send(tcpfd , buyB, 1, 0);
		 printf("The <Agent%d> has send the result to <SellerB>\n", agent);
	 }
	 else
	 {
	 	 servaddr.sin_port = htons(3500+875);
		 connect(tcpfd , (struct sockaddr *)&servaddr , sizeof(servaddr));
		 getsockname(tcpfd, (struct sockaddr *)&guest, &guest_len);
         inet_ntop(AF_INET, &guest.sin_addr, guest_ip, sizeof(guest_ip));
         printf("The <Agent%d> has TCP port %d and IP address %s for Phase 1 part 4\n", agent, ntohs(guest.sin_port), guest_ip);
		 if(max_C==0) send(tcpfd , "NAK", 3, 0);
		 else  send(tcpfd , buyC, 1, 0);
		 printf("The <Agent%d> has send the result to <SellerC>\n", agent);
		 close(tcpfd);
    	 tcpfd = socket(AF_INET, SOCK_STREAM, 0);
		 servaddr.sin_port = htons(3600+875);
		 connect(tcpfd , (struct sockaddr *)&servaddr , sizeof(servaddr));
		 getsockname(tcpfd, (struct sockaddr *)&guest, &guest_len);
         inet_ntop(AF_INET, &guest.sin_addr, guest_ip, sizeof(guest_ip));
         printf("The <Agent%d> has TCP port %d and IP address %s for Phase 1 part 4\n", agent, ntohs(guest.sin_port), guest_ip);
		 if(max_D==0) send(tcpfd , "NAK", 3, 0);
		 else  send(tcpfd , buyD, 1, 0);
		 printf("The <Agent%d> has send the result to <SellerD>\n", agent);
	 }
	 
	 close(tcpfd);
	 for(int i=1; i<6; i++)
	 {
	 	tcpfd = socket(AF_INET, SOCK_STREAM, 0);
	    servaddr.sin_port = htons(3800+875+i*100);
    	connect(tcpfd , (struct sockaddr *)&servaddr , sizeof(servaddr));
    	stringstream sss;
    	char b;
        sss << i;
        sss >> b;
        sss.clear();
        sss.str("");
	    if(buyA[0]!=b && buyB[0]!=b && buyC[0]!=b && buyD[0]!=b) send(tcpfd , "NAK", 3, 0);
	    else 
    	{
	 	char buff[15];
	 	for(int j=0; j<15; j++) buff[j]='\0';
	 	if(buyA[0]==b) strcat(buff,"A.");
	    if(buyB[0]==b) strcat(buff,"B.");
	    if(buyC[0]==b) strcat(buff,"C.");
	    if(buyD[0]==b) strcat(buff,"D.");
	    strcat(buff,"end");
	    send(tcpfd , buff, strlen(buff), 0);
	    }
	    printf("The <Agent%d> has send the result to <Buyer%c>\n", agent, b);
	 
	    close(tcpfd);
	 }
	 printf("End of Phase 1 part 4 for <Agent%d>\n", agent);
}
