#include <string>
enum msg_type {SEND, GET, ACK};
struct _msg {
   int seq_no;
   msg_type msg_t;
   char msg_src[10], msg_dest[10];
   char msg_pl[72];
};
