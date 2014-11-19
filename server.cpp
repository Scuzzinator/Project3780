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
#include <math.h>
#include "message.h"
#include <string>
#include <iostream>
#include <map>
#include <queue>
#include <sstream>
#include <vector>

#define PORT 5000

std::string servers[] = {"17jetta","17civic","17camaro","17beetle","17vfr"};

struct sockaddr_in servAddr[5];

int main()
{
   int sock;                  //socket
   int bytes_read;            //length of received message
   socklen_t addr_len;        //length of client address
   _msg recv_data, send_data; //message structures for sent/receive data
   //addresses for client and server
   struct sockaddr_in server_addr , client_addr;
   pid_t pid; //process ID for fork
   std::map<std::string,std::queue<_msg> > msgbuffer;
   std::map<std::string,int> clientTable;
   int loopindex, serv_index;
   char hostname[128];
   struct hostent *host;

   //Determine server and index value in servers
   if(gethostname(hostname, sizeof(hostname)) == 0)
      printf("\nServer Hostname: %s\n", hostname);
   else
      perror("gethostname");
   serv_index = -1;
   for(int i = 0; i < 5; i++)
      if(strcmp(hostname, servers[i].c_str()) == 0) {
	 serv_index = i;
	 break;
      }
   if(serv_index == -1) {
      std::cout << "Hostname is not in list\n";
      perror("Hostname Not in List");
      exit(1);
   }
   
   //Initialize servAddr list
   for(int i = -2; i < 3; i++) {
      loopindex = (serv_index + i) % 5;
      if(abs(i) == 2){
	 host = (struct hostent *) gethostbyname(
	    servers[(loopindex + 1) % 5].c_str());
      } else {
	 host = (struct hostent *) gethostbyname(servers[loopindex].c_str());
      }
      servAddr[loopindex].sin_family = AF_INET;
      servAddr[loopindex].sin_port = htons(PORT);
      servAddr[loopindex].sin_addr = *((struct in_addr *)host->h_addr);
      bzero(&(servAddr[loopindex].sin_zero),8);
   }
   
   //Create Socket
   if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
      perror("Socket");
      exit(1);
   }
   
   //Server address properties
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(PORT);      //Socket number
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


   printf("\nUDP Server Waiting for client on port %i\n", 5000);
   fflush(stdout);
   
   while (1)
   {
      //receive data from client
      bytes_read = recvfrom(sock,&recv_data,sizeof(_msg),0,
                            (struct sockaddr *)&client_addr, &addr_len);

      //if(pid < 0)
      // pid = fork();
      pid = 0;
      
      if(bytes_read > 0) { //Fork process, and let child handle it
	 //pid = fork();
	 if(pid == 0) {
	    //Deal with message based on enum type
	    std::ostringstream tempchar;
	    int tempindex;
	    switch(recv_data.msg_t) {
	      /*
		Send:
		Server should check the client table to see where to send
		the message. If the client table says the current server
		is directly connected to the client, it should keep the 
		message in the map.

		Status: Unfinished
		Currently only saves to itself, and does not check the 
		client table if it should be sending on the message.
	       */
	       case SEND:
		  msgbuffer[(std::string)recv_data.msg_dest].push(recv_data);
		  break;
	      /*
		Get:
		Server checks its message buffer to see if there are any
		messages for the client that issued the get request. Sends
		a message to the client with the number of available messages
		in the payload of our _msg structure. The client acknowledges,
		and sets the server off in a for loop of stop-and-wait to
		get the messages to the client. After each ACK of the message,
		the server deletes the message.
	       */
	       case GET:
		  tempchar.flush();
		  tempchar << msgbuffer[(std::string)recv_data.msg_src].size();
		  strcpy(send_data.msg_pl, tempchar.str().c_str());
		  sendto(sock,(const char *)&send_data,sizeof(_msg),0,
			 (struct sockaddr *)&client_addr,
			 sizeof (struct sockaddr));
		  //We assume everything works properly
		  //loop send the data that exists in the queue
		  //while waiting for ack
		  bytes_read = recvfrom(sock,&recv_data,sizeof(_msg),0,
					(struct sockaddr *)&client_addr,
					&addr_len);
		  if(bytes_read > 0 && recv_data.msg_t == (enum msg_type)2) {
		     std::string recvsrc(recv_data.msg_src);
		     while(msgbuffer[recvsrc].size() > 0) {
			//send front
			msg_copy(send_data,
				 msgbuffer[recvsrc].front());
			sendto(sock,(const char *)&send_data,sizeof(_msg),0,
			       (struct sockaddr *)&client_addr,
			       sizeof (struct sockaddr));
			//recv ack and pop queue
			bytes_read = recvfrom(sock,&recv_data,sizeof(_msg),0,
					      (struct sockaddr *)&client_addr,
					      &addr_len);
			if(bytes_read > 0
			   && recv_data.msg_t == (enum msg_type)2)
			   msgbuffer[recvsrc].pop();
		     }
		  }
		  break;
	       /*
		 Connect:
		 If message is straight from client, add client to table
		 using the current server's index, then format message
		 to send to server as seq_no = TTL = 1, type = CONN,
		 src = recv_data.msg_src, dest = serv_index
		 
		 else add client to table using recv_data.msg_dest; if
		 --TTL (seq_no) == 0, don't send the message
	       */
	       case CONN:
		  msg_copy(send_data, recv_data);
		  if(strcmp(recv_data.msg_dest, "NULL") == 0) {
		     clientTable[(std::string)recv_data.msg_src] = serv_index;
		     send_data.seq_no = 1; //TTL
		     tempchar.flush();
		     tempchar << serv_index;
		     strcpy(send_data.msg_pl, tempchar.str().c_str());
		     sendto(sock,(const char *)&send_data,sizeof(_msg),0,
			    (struct sockaddr *)&servAddr[(serv_index + 1) % 5],
			    sizeof (struct sockaddr));
		     sendto(sock,(const char *)&send_data,sizeof(_msg),0,
			    (struct sockaddr *)&servAddr[(serv_index - 1) % 5],
			    sizeof (struct sockaddr));
		  } else {
		     sscanf(recv_data.msg_src, "%d", &tempindex);
		     clientTable[(std::string)recv_data.msg_src] = tempindex;
		     if(recv_data.seq_no) {
			--send_data.seq_no;
			//Only got to here, need moar code!
			//send the message on to the node that has yet to
			//receive it in this "line" of nodes
		     }
		  }
		  break;
	      /*
		Disconnect:
		Very similar to the connect case, but each node removes
		the client from the client list, and then passes the message
		on to remove the client.

		One extra thing here different then the connection case,
		we need to decide what to do with any messages that were
		meant for the client before they logged off. Should we just
		keep them? Either we can remove all available messages from 
		the buffer of the server that was connected with the client,
		or we can keep the messages and then make an extra check on
		any CONN messages to have the server check its buffer for a
		matching ID of the new connected client, and transfer the 
		messages to the server the client is connected to for the
		next time they make a get request.
	       */
	       case DCON:
		  clientTable.erase((std::string)recv_data.msg_src);
		  if(strcmp(recv_data.msg_dest, "NULL") == 0) {
		    //missing code
		  } else {
		    //more missing code
		  }
		  break;
	    }
	 }
      }
      fflush(stdout);
   }
   return 0;
}
