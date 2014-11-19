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

void format_msg(_msg &m, const char* cl_id, const int c, std::string &s)
{
   std::stringstream sInput(s);
   char t[4];
   std::string stemp;
   m.seq_no = c;
   sInput >> t;
   m.msg_t = read_type(t);
   strcpy(m.msg_src, cl_id);
   sInput >> m.msg_dest;
   sInput.ignore();
   getline(sInput, stemp);
   strcpy(m.msg_pl, stemp.c_str());
}

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

   /*
     Missing code here:
     The client needs to make a CONN message to the server here so the 
     servers can identify the client and add it to the client table
    */
   
   while (1)
   {
      
      std::cout << "\nSend a message.\nFormat: <type> <dest-ID> <message>\n(q or Q to quit): ";
      getline(std::cin, s);
      //If input is q, quit the client, otherwise format and send data
      if ((s.at(0) == 'q') || (s.at(0) == 'Q')) {
	/*
	  Missing code here:
	  The client needs to make a DCON message to the server here so the
	  servers can remove the client from the table.
	 */
	 break;
      } else {
	 format_msg(send_data, client_id, ++msg_counter, s);
	 if(send_data.msg_t == (enum msg_type)1) {
	    sendto(sock,(const char *)&send_data, sizeof(_msg), 0,
		   (struct sockaddr *)&server_addr,
		   sizeof(struct sockaddr));
	    recvfrom(sock,&recv_data,sizeof(_msg),0,
		     (struct sockaddr *)&server_addr,&sin_size);
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
	 else sendto(sock,(const char *)&send_data, sizeof(_msg), 0,
		     (struct sockaddr *)&server_addr,
		     sizeof(struct sockaddr));
	 
      }
   }
}
