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
#include <limits.h>
#include "message.h"
#include <iostream>
#include <sstream>

#define PORT 5000

int main()
{
   int sock,bytes_recv, msg_counter = 0;
   socklen_t sin_size;
   struct sockaddr_in server_addr;
   struct hostent *host;
   _msg send_data, recv_data;
   std::string s, server_name;
   char client_id[11];

   std::cout << "Please enter the server hostname: ";
   std::cin >> server_name;
   host= (struct hostent *) gethostbyname(server_name.c_str());
   
   //create socket
   if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
   {
      perror("socket");
      exit(1);
   }
   
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(PORT);
   server_addr.sin_addr = *((struct in_addr *)host->h_addr);
   bzero(&(server_addr.sin_zero),8);
   sin_size = sizeof(struct sockaddr);

   std::cout << "Please enter a ten digit client ID:";
   std::cin >> client_id;
   std::cin.ignore(INT_MAX, '\n'); //Flush the input buffer

   //Send a CONN message to the server notifying you connect
   std::string conndcon = "CONN NULL Blank";
   format_msg(send_data, client_id, msg_counter, conndcon);
   sendto(sock,(const char *)&send_data, sizeof(_msg), 0,
	  (struct sockaddr *)&server_addr,
	  sizeof(struct sockaddr));
   
   while (1)
   {
      
      std::cout << "\nSend a message.\nFormat: <type> <dest-ID> <message>\n(q or Q to quit): ";
      getline(std::cin, s);
      //If input is q, quit the client, otherwise format and send data
      if ((s.at(0) == 'q') || (s.at(0) == 'Q')) {
	 //Send a DCON message to the server notifying you disconnect
	 conndcon = "DCON NULL Blank";
	 format_msg(send_data, client_id, msg_counter, conndcon);
	 sendto(sock,(const char *)&send_data, sizeof(_msg), 0,
		(struct sockaddr *)&server_addr,
		sizeof(struct sockaddr));
	 break;
      } else {
	 format_msg(send_data, client_id, ++msg_counter, s);
	 if(send_data.msg_t == (enum msg_type)1) {
	    sendto(sock,(const char *)&send_data, sizeof(_msg), 0,
		   (struct sockaddr *)&server_addr,
		   sizeof(struct sockaddr));
	    recvfrom(sock,&recv_data,sizeof(_msg),0,
		     (struct sockaddr *)&server_addr,&sin_size);
	    if(strcmp(recv_data.msg_pl, "NULL") == 0) {
	       std::cout << "Received: No new messages on server.\n";
	    } else {
	       int i;
	       sscanf(recv_data.msg_pl, "%d", &i);
	       send_data.msg_t = (enum msg_type)2;
	       sendto(sock,(const char *)&send_data, sizeof(_msg), 0,
		      (struct sockaddr *)&server_addr,
		      sizeof(struct sockaddr));
	       for(int j = 0; j < i; j++) {
		  recvfrom(sock,&recv_data,sizeof(_msg),0,
			   (struct sockaddr *)&server_addr,&sin_size);
		  std::cout << "Received: ";
		  print_msg(recv_data);
		  sendto(sock,(const char *)&send_data, sizeof(_msg), 0,
			 (struct sockaddr *)&server_addr,
			 sizeof(struct sockaddr));
	       }
	    }
	 }
	 else sendto(sock,(const char *)&send_data, sizeof(_msg), 0,
		     (struct sockaddr *)&server_addr,
		     sizeof(struct sockaddr));
	 
      }
   }
}
