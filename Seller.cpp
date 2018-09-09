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
  int tcpfd, connfd; 
  pid_t pid1,pid2;
  struct sockaddr_in servaddr; 
  char buff[30],seller;
  buff[0]='\0';
  char name[]="nunki.usc.edu";
  int n, agent; 
  struct sockaddr_storage their_addr;
  socklen_t sin_size;
  string filename;
  
  pid1=fork();
  
  if(pid1>0) 
  {
  	agent=1;
	servaddr.sin_port = htons(3700+875);
  	pid2=fork();
	if (pid2>0) 
	  {
	  	filename="sellerA.txt";
		strcat(buff,"A");
		seller='A';
	  }
  	else 
	  {
	  	filename="sellerB.txt";
		strcat(buff,"B");
		seller='B';
	  }
  }
  else 
  {
  	agent=2;
	servaddr.sin_port = htons(3800+875);
  	pid2=fork();
  	if (pid2>0) 
	  {
	  	filename="sellerC.txt";
		strcat(buff,"C");
		seller='C';
	  }
  	else 
	  {
	  	filename="sellerD.txt";
		strcat(buff,"D");
		seller='D';
	  }
  }
  ifstream fin(filename.c_str());
  string line; 
  vector<string> fields;
  while (getline(fin, line,'\r')) 
  {
    istringstream sin(line); 
    string field; 
    while (getline(sin, field, ':')) 
      fields.push_back(field); 
    
  }
  
  tcpfd = socket(AF_INET, SOCK_STREAM, 0);
  struct hostent *he;
  struct in_addr **addr_list;
    
  he = gethostbyname(name);
  addr_list = (struct in_addr **) he->h_addr_list;
  servaddr.sin_family = AF_INET; 
  servaddr.sin_addr= **addr_list; 
  
  connect(tcpfd , (struct sockaddr *)&servaddr , sizeof(servaddr));
  
  char guest_ip[20];
  struct sockaddr_in guest; 
  socklen_t guest_len=sizeof(guest);
  getsockname(tcpfd, (struct sockaddr *)&guest, &guest_len);
  inet_ntop(AF_INET, &guest.sin_addr, guest_ip, sizeof(guest_ip));

  printf("<Seller%c> has TCP port %d and IP address %s for Phase 1 part 1\n", seller, ntohs(guest.sin_port), guest_ip);
  printf("<Seller%c> is now connected to the <agent%d>\n", seller, agent);
  
  strcat(buff,".");
  strcat(buff,fields[3].c_str());
  strcat(buff,".");
  strcat(buff,fields[1].c_str());
  
  send(tcpfd , buff , strlen(buff) , 0);
  buff[strlen(buff)-1]='\0';
  printf("<Seller%c> has sent < %s > to the agent\n", seller, buff);
  printf("End of Phase 1 part 1 for <Seller%c>\n", seller);
  close(tcpfd);
  
  tcpfd = socket(AF_INET, SOCK_STREAM, 0);
    int on=1;  
    if((setsockopt(tcpfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)  
    {  
        perror("setsockopt failed");  
        exit(EXIT_FAILURE);  
    } 
    char buffer[20];
    string fn;
  if(pid1>0 && pid2>0)
  {
	servaddr.sin_port = htons(3300+875); 
	fn="<SellerA>: ";
  }
  if(pid1>0 && pid2<=0)
  {
	servaddr.sin_port = htons(3400+875); 
	fn="<SellerB>: ";
  }
  if(pid1<=0 && pid2>0)
  {
	servaddr.sin_port = htons(3500+875);
	fn="<SellerC>: ";
  }
  if(pid1<=0 && pid2<=0)
  {
	servaddr.sin_port = htons(3600+875); 
	fn="<SellerD>: ";
  }
    bind(tcpfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    printf("<Seller%c> has TCP port %d and IP address %s for Phase 1 part 4\n", seller, ntohs(servaddr.sin_port), inet_ntoa(servaddr.sin_addr));
    listen(tcpfd, 10);
    connfd = accept(tcpfd, (struct sockaddr*)&their_addr, &sin_size);
    n = recv(connfd, buffer, sizeof(buffer), 0);
    buffer[n] = '\0';
    cout << fn;
    if(buffer[0]=='N') printf("%s\n",buffer);
    else printf("<Buyer%s> buy my house\n", buffer);
    close(tcpfd);
}
