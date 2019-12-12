#include<iostream>
#include<string>
#include<sstream>
#include<vector>
#include<algorithm>
using namespace std;
class Function
{
    public:
    static void Separate(string &str1,string &str2,string &str3,string &str4)
    {
      
      stringstream s(str1);
      s>>str2>>str3>>str4;
    //  cout<<str1<<endl;
    //  cout<<str2<<endl;
    //  cout<<str3<<endl;
    //  cout<<str4<<endl;
    }
    static void mytoupper(string &str){
        transform(str.begin(),str.end(),str.begin(),::toupper);
    }
    static void HeadtoVector(string &head,vector<string> &v)
    {//
          //Function::HeadtoVector(head_kv,v);//先分离出一格格的元素  放入vector中去
    size_t star=0;
    while(1)
    {
        size_t pos=head.find("\n",star);
        if(pos==string::npos)
        {
        break;
        }
        string ret=head.substr(star,pos-star);
        v.push_back(ret);
        star=pos+1;
      }
    }

    static void Makekv(string str,string &k,string &v)
    {
        size_t pos=str.find(": ",0);
        if(pos==string::npos)
        {//遍历结束
        return;
        }
        k=str.substr(0,pos);
        v=str.substr(pos+2);
    }

      static int StrtoInt(string str)
      {
      stringstream ss(str);
      int ret=0;
      ss>>ret;
      return ret;
      }

      static string InttoStr(int code)
      {
      string str;
      stringstream ss;
      ss<<code;
      str=ss.str();
      return str;
      }

     static string SuffixtoType(string str)
     {
       string ret="Content-Type: ";
       if(str=="html")
           ret+="text/html";
       else if(str==".css")
           ret+="text/css";
       else if(str==".js")
           ret+="application/x-javascript";
       else if(str==".jpg")
           ret+="image/jprg";
       else if(str==".png")
           ret+="image/png";
       else
           ret+="text/html";
     return ret;
     }
};

