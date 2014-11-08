#include <string.h>
#include <stdio.h>
#include <iostream>
#include "message.h"
using namespace std;
int main() {
   string s;
   char c[100];
   // struct _msg *m = new _msg;
   cout << "enter a string: ";
   cin >> s;
   strcpy(c,s.c_str());
   //m->msg_pl = s;
   cout << "s size : " << sizeof(s) << endl;
   cout << "size of strings: " << sizeof(string) << endl;
   cout << "using the char: " << c << endl;
   // delete m;
   return 0;
}
