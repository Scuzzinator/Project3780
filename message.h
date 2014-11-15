#include <string.h>
enum msg_type {SEND, GET, ACK};
struct _msg {
   int seq_no;
   msg_type msg_t;
   char msg_src[11], msg_dest[11];
   char msg_pl[71];
};
void msg_copy(_msg &_dest, _msg &_src) {
   _dest.seq_no = _src.seq_no;
   _dest.msg_t = _src.msg_t;
   strcpy(_dest.msg_src, _src.msg_src);
   strcpy(_dest.msg_dest, _src.msg_dest);
   strcpy(_dest.msg_pl, _src.msg_pl);
}
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
