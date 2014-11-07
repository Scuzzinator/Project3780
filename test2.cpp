#include <iostream>
#include "message.h"
using namespace std;
int main() {
   string s;
   // struct _msg *m = new _msg;
   cout << "enter a string: ";
   cin >> s;
   //m->msg_pl = s;
   cout << "s size : " << sizeof(s) << endl;
   cout << "size of strings: " << sizeof(string) << endl;
   // delete m;
   return 0;
}
