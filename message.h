#include <string.h>
enum msg_type {SEND, GET, ACK};
struct _msg {
   int seq_no;
   msg_type msg_t;
   char msg_src[11], msg_dest[11];
   char msg_pl[71];
};
msg_type read_type(const char *s) {
   if(strcmp(s,"SEND") == 0)
      return SEND;
   else if(strcmp(s,"GET") == 0)
      return GET;
   else if(strcmp(s,"ACK") == 0)
      return ACK;
}
char *etochar(msg_type &m) {
   switch(m) {
      case SEND:
	 return (char*)"SEND";
      case GET:
	 return (char*)"GET";
      case ACK:
	 return (char*)"ACK";
   }
   return (char*)"NONE";
}
void print_msg(_msg &m) {
   printf("%i %s %s %s %s\n", m.seq_no, etochar(m.msg_t), m.msg_src,
	  m.msg_dest, m.msg_pl);
}
