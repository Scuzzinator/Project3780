#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "message.h"
#include <string>
#include <iostream>
#include <map>
#include <queue>
#include <sstream>

#define PORT "5000"

int main()
{
   //  int SOCKET_PORT = 5000;    //socket port number
   int sock;                  //socket
   int bytes_read;            //length of received message
   socklen_t addr_len;        //length of client address
   _msg recv_data, send_data; //message structures for sent/receive data
   //addresses for client and server
   struct sockaddr_in server_addr , client_addr;
   pid_t pid; //process ID for fork
   std::map<std::string,std::queue<_msg> > msgbuffer;
   
   //Create Socket
   if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
      perror("Socket");
      exit(1);
   }
   //Server address properties
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(5000);      //Socket number
   server_addr.sin_addr.s_addr = INADDR_ANY;
   bzero(&(server_addr.sin_zero),8);
   
   //Bind socket of server
   if (bind(sock,(struct sockaddr *)&server_addr,
            sizeof(struct sockaddr)) == -1)
   {
      perror("Bind");
      exit(1);
   }
   
   addr_len = sizeof(struct sockaddr);


   //Getting server hostname
   char _hostname[255];
   if(gethostname(_hostname, sizeof(_hostname)) == 0)
      printf("\nServer Hostname: %s\n", _hostname);
   else
      perror("gethostname");
	 

   
   printf("\nUDP Server Waiting for client on port %i\n", 5000);
   fflush(stdout);
   
   while (1)
   {
      //receive data from client
      bytes_read = recvfrom(sock,&recv_data,sizeof(_msg),0,
                            (struct sockaddr *)&client_addr, &addr_len);

      if(pid < 0)
	 pid = fork();
      
      if(bytes_read > 0) { //Fork process, and let child handle it
	 if(pid == 0) {
	    switch(recv_data.msg_t) {
	       case SEND:
		  msgbuffer[(std::string)recv_data.msg_dest].push(recv_data);
		  break;
	       case GET:
		  std::ostringstream tempchar;
		  tempchar << msgbuffer[(std::string)recv_data.msg_src].size();
		  strcpy(send_data.msg_pl, tempchar.str().c_str());
		  sendto(sock,(const char *)&send_data,sizeof(_msg),0,
			 (struct sockaddr *)&client_addr,
			 sizeof(struct sockaddr));
		  //We assume everything works properly
		  //loop send the data that exists in the queue
		  //while waiting for ack
		  bytes_read = recvfrom(sock,&recv_data,sizeof(_msg),0,
					(struct sockaddr *)&client_addr,
					&addr_len);
		  if(bytes_read > 0 && recv_data.msg_t == (enum msg_type)2) {
		     std::string recvsrc(recv_data.msg_src);
		     //strcpy(recvsrc, recv_data.msg_src);
		     while(msgbuffer[recvsrc].size() > 0) {
			//send front
			msg_copy(send_data,
				 msgbuffer[recvsrc].front());
			sendto(sock,(const char *)&send_data,sizeof(_msg),0,
			       (struct sockaddr *)&client_addr,
			       sizeof(struct sockaddr));
			//recv ack and pop queue
			bytes_read = recvfrom(sock,&recv_data,sizeof(_msg),0,
					      (struct sockaddr *)&client_addr,
					      &addr_len);
			if(bytes_read > 0
			   && recv_data.msg_t == (enum msg_type)2)
			   msgbuffer[recvsrc].pop();
		     }
		  }
	    }
	 }
      }
   
/*
      printf("(%s , %d) said :\n",inet_ntoa(client_addr.sin_addr),
	     ntohs(client_addr.sin_port));
      print_msg(recv_data);
      printf("\nSENDING : ");
      send_data->seq_no = 0;
      send_data->msg_t = (msg_type)0;
      strcpy(send_data->msg_src, "kdkdkdkdkd");
      strcpy(send_data->msg_dest, "sewseseses");
      strcpy(send_data.msg_pl,etochar(recv_data.msg_t));
      std::cout << send_data.msg_pl << "\n\n";
      //Send data to client
      sendto(sock,(const char *)&send_data,sizeof(_msg),0,(struct sockaddr *)&client_addr,sizeof(struct sockaddr));
*/
      fflush(stdout);
      
   }
   return 0;
}
