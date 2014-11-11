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
#include <sstream>

void format_msg(_msg &m, const int c, std::string &s)
{
   std::stringstream sInput(s);
   char t[4];
   std::string stemp;
   m.seq_no = c;
   sInput >> t;
   m.msg_t = read_type(t);
   sInput >> m.msg_src;
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
   std::string s;
   
   host= (struct hostent *) gethostbyname((char *)"17prius");
   
   //create socket
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
      
      std::cout << "\nSend a message.\nFormat: <type> <source> <destination> <message>\n(q or Q to quit): ";
      getline(std::cin, s);
      //If input is q, quit the client, otherwise format and send data
      if ((s.at(0) == 'q') || (s.at(0) == 'Q'))
	 break;
      else {
	 format_msg(send_data, ++msg_counter, s);
	 std::cout << "Sending: ";
	 print_msg(send_data);
	 sendto(sock,(const char *)&send_data, sizeof(_msg), 0,
		(struct sockaddr *)&server_addr,
		sizeof(struct sockaddr));
	 
      }
      bytes_recv = recvfrom(sock,&recv_data,sizeof(_msg),0,(struct sockaddr *)&server_addr,&sin_size);
      std::cout << "Received: " << recv_data.msg_pl << std::endl;
   }
}
