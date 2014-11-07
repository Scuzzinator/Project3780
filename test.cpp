
#include <iostream>
#include "message.h"
using namespace std;
int main()
{
//   char a[10];
   cout << "hello" << endl;
   cout << sizeof(msg_type) << endl;
   cout << sizeof(int) << endl;
   cout << sizeof(char[10]) << endl;
   cout << sizeof(string) << endl;
   cout << sizeof(_msg) << endl;
   return 0;
}
