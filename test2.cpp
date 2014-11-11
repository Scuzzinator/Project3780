#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "message.h"
using namespace std;
int main() {
   string s;
   _msg temp_msg;
   int count = 0;
   char t_char[4];
   //system("dialog --calendar hello! 0 0 11 11 2014");
   cout << "Message type: ";
   temp_msg.seq_no = count++;
   cin >> t_char;
   temp_msg.msg_t = read_type(t_char);
   cout << "source: ";
   cin >> temp_msg.msg_src;
   cout << "destination: ";
   cin >> temp_msg.msg_dest;
   cout << "message (max 70 char): ";
   cin.ignore();
   getline(cin, s);
   strcpy(temp_msg.msg_pl, s.c_str());
   cout << "stored information:\n";
   cout << "Sequence Number: " << temp_msg.seq_no << endl;
   cout << "Message Type: " << temp_msg.msg_t << endl;
   cout << "Source: " << temp_msg.msg_src << endl;
   cout << "Destination: " << temp_msg.msg_dest << endl;
   cout << "Payload: " << temp_msg.msg_pl << endl;
   cout << "string: " << s << endl;
   return 0;
}
