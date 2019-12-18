#include<iostream>
#include<sstream>
#include<string>
using namespace std;
int main()
{
  string str="hello word !";
  stringstream s(str);
  string s2,s3,s4;

  s>>s2>>s3>>s4;
  cout<<s2<<endl;
  cout<<s3<<endl;
  cout<<s4<<endl;
  return 0;
}
