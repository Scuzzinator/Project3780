#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "message.h"
#include <iostream>

int main()
{
   int sock,bytes_recv;
   socklen_t sin_size;
   struct sockaddr_in server_addr;
   struct hostent *host;
   //_msg *send_data = new _msg, *recv_data = new _msg;
   _msg send_data, recv_data;
   void *pVoid;
   char temp[1024];
   std::string s;
   
   host= (struct hostent *) gethostbyname((char *)"127.0.0.1");
   

   if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
   {
      perror("socket");
      exit(1);
   }
   
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(5000);
   server_addr.sin_addr = *((struct in_addr *)host->h_addr);
   bzero(&(server_addr.sin_zero),8);
   sin_size = sizeof(struct sockaddr);
   
   while (1)
   {
      
      std::cout << "Type Something (q or Q to quit): ";
      std::cin >> s;
      
            
      if ((s == "q") || (s == "Q"))
	 break;
      
      else {
	 strcpy(send_data.msg_pl,s.c_str());
	 //pVoid = &send_data;
	 std::cout << sendto(sock,(const char *)&send_data, sizeof(_msg), 0,
			     (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) << std::endl;
      }
      //pVoid = &recv_data;
      bytes_recv = recvfrom(sock,&recv_data,sizeof(_msg),0,(struct sockaddr *)&server_addr,&sin_size);
      std::cout << "blaaah " << bytes_recv << std::endl;
      //recv_data[bytes_recv]= '\0';
      std::cout << "Received: " << recv_data.msg_pl << std::endl;
   }
   //delete recv_data;
   //delete send_data;
}

/* CURRENT PROBLEMS

   The program apparently "sends" and "receives" data, but when we try to access the message section of our struct, the program segfaults with valgrind whining that there's an "invalid read of size 8" as well as "Syscall param socketcall.sendto(msg) points to uninitialized bytes"

We've tried numerous methods of fixing the problem, including switching our recv_data and send_data back and forth between pointers and references, but nothing fixes the problem of not being able to access any of the message section of the structure. */
