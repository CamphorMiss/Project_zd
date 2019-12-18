#pragma once
#include<iostream>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/sendfile.h>
#include<sys/stat.h>
#include<unordered_map>
#include<vector>
#include <fcntl.h>
#include<strings.h>
#include<string>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/wait.h>
#include"Tool.hpp"

//using namespace std;
#define WWW "./wwwroot"
class Request{
    private:
    string Requestline;
    string Requesthead;
    string Requestkong;
    string Requestbody;
 public:
    string method;
    string uri;
    string vesion;
    unordered_map<string,string>  head_kv;

    string path;
    string Queue_str;
    bool cgi;
    int recource_size;
    string suffix;
    public:
    Request():Requestkong("\n"),path(WWW),cgi(false)
    {
    }
    string & GetRequestline()
    {
     return Requestline;
    }
    string & GetRequesthead()
    {
     return Requesthead;
    }
    string & GetRequestbody()
    {
    return Requestbody;
    }
    string &Getvesion()
    {
    return vesion;
    }
    string &Getpath()
    {
    return path;
    }
    void RequestlineParse()
    {//分析请求行，分离成方法，资源路径，协议版本。
    // Function::Separate(GetRequestline(),method,uri,vesion);
    stringstream ss(Requestline);
    ss>>method>>uri>>vesion;
   //  cout<<"method: "<<method<<endl;
   //  cout<<"uri: "<<uri<<endl;
   //  cout<<"vesion: "<<vesion<<endl;
     
    }
    bool MethodIslegal()
    {
        //GET   POST toupper transform...
         Function::mytoupper(method);
     cout<<"method: "<<method<<endl;
     cout<<"uri: "<<uri<<endl;
     cout<<"vesion: "<<vesion<<endl;
     
        if(method=="POST"||method=="GET")
            return true;
        return false;
    }
    void RequestheadParse()
    {
          //怎么分析呢  要把头分离成 k_v 
          vector<string> v;
        Function::HeadtoVector(GetRequesthead(),v);//先分离出一格格的元素  放入vector中去
          for(auto it=v.begin();it!=v.end();it++)
          {
             string k;
             string v;
             Function::Makekv(*it,k,v);
             head_kv.insert(make_pair(k,v));
          }
    }
    bool IsneedRecv()
    {
      return (method=="POST")?true:false;
    }
    int GetContentLength()
    {
      auto it=head_kv.find("Content-Length");
      if(it==head_kv.end())
          {
            //找不到
            return -1;
          }
      
      return Function::StrtoInt(it->second);
    }
    bool Uriparse()
    {
        //POST 一定带参
        //GET  有？则带参   没有就不带参
      if(method=="POST")
      {
        cgi=true;
      }
      else
      {
        //GET
        auto pos=uri.find('?');
        if(string ::npos==pos)
        {
         path+=uri;
        }
        else
        {
            cout<<"cgi have ?!"<<endl;
            cgi=true;
            path=uri.substr(0,pos);
            Queue_str=uri.substr(pos+1);
            int pos1=path.rfind('.');
            suffix=path.substr(pos1);
        }
      }
      if(path[path.size()-1]=='/')
          path+="index.html";
    }

   bool Ispathlegal()
   {
       struct stat s;
       if(stat(path.c_str(),&s)==0)
       {
           //存在
           if(S_ISDIR(s.st_mode))
           {
           path+="/index.html";
           }
           else{
            if(s.st_mode&S_IXUSR||s.st_mode&S_IXGRP||s.st_mode&S_IXOTH){//exe
                cout<<"cgi is .exe!"<<endl;
                cgi=true;
            }
           }
       recource_size=s.st_size;
           return true;
       }
       else 
           return false;
   }
   string &Getpara()
   {
    if(method=="Get")
        return Queue_str;
    return Requestbody;
   }
   int GetrecourceSize()
   {
   return recource_size;
   }
   string &Getsuffix()
   {
     return suffix;
   }
   bool Iscgi()
   {//dir
   return cgi;
   }
    bool Getcgi()
    {
    return Iscgi();
    }
    ~Request()
    {
    }
};
class Respond{

    private:
    string Respondline;
    string Respondhead;
    string Respondkong;
    string Respondbody;
    string suffix;
    string path;

    int fd;
    int size;
    public:
    Respond():Respondkong("\n"),fd(-1)
    {
    }
    string &GetRespondline()
    {
    return Respondline;
    }
    string &GetRespondhead()
    {
     return Respondhead;
    }
    string &GetRespondkong()
    {
    return Respondkong;
    }
    string &GetRespondbody()
    {
    return Respondbody;
    }
    void Make_404()
    {
    suffix=".html";
    path="wwwroot/404.html";
    struct stat s;
    stat(path.c_str(),&s);
    size=s.st_size;
    }
    void Remake(int code)
    {
    switch(code)
    {
     case 404:
         Make_404();
         break;
     default:
         break;
    }
    }
    void MakeRespondline(int code,Request *rqs)
    {
     Respondline="HTTP/1.1 ";    
     Respondline+=Function::InttoStr(code);
     Respondline+=" ";
     switch(code)
     {
         case 200:
             Respondline+="OK";
             break;
         case 404:
             Respondline+="Not Found";
             break;
         default:
             Respondline+="Unkown error";
            break;
     }
    Respondline+="\r\n";
    }
    void MakeRespondhead(vector<string>& v)
    {
    auto it=v.begin();
    for(;it!=v.end();it++)
    {
      Respondhead+=*it;
      Respondhead+="\r\n";
    }
    }
     void MakeRespond(Request *rqs,int code,bool cgi)
    {
   //different code
   //
   cout<<"Makeresponse"<<endl;
    MakeRespondline(code,rqs);
    vector<string> v;
    if(cgi)
    {
        cout<<"is cgi"<<endl;
    //cgi
     suffix=Function::SuffixtoType("");
     v.push_back(suffix);
     string len="Content-Length: ";
     len+=Function::InttoStr(Respondbody.size());
     v.push_back(len);
     MakeRespondhead(v);
    }
    else{
    if(code==200)
    {
        suffix=rqs->Getsuffix();
        path=rqs->Getpath();
        cout<<"debug: "<<path<<endl;
       // fd=open(path.c_str(),O_RDONLY);
        size=rqs->GetrecourceSize();
    }
       cout<<"suffix"<<suffix<<endl;
       cout<<"size"<<size<<endl;
       cout<<"path: "<<path<<endl;
        fd=open(path.c_str(),O_RDONLY);
        string str=Function::SuffixtoType(suffix);
        string len="Content-Length: ";
        //  int GetrecourceSize()
        len+=Function::InttoStr(size);
        v.push_back(str);
        v.push_back(len);
        MakeRespondhead(v);
    cout<<"Makerespond sucess!"<<endl;
    cout<<"Respondline: "<<Respondline<<endl;
    cout<<"Respondhead: "<<Respondhead<<endl;
    cout<<"Respondkong: "<<Respondkong<<endl;
    cout<<"Respondbody: "<<Respondbody<<endl;
    }
    }
     int Getfd()
     {
     return fd;
     }
     int Getsize()
     {
     return size;
     }
    ~Respond()
    {
    if(fd<0)
    close(fd);

    }
};
class EnPoint
{
    private:
    int sock;
    public:
    EnPoint(int sock_):sock(sock_)
    {
    }
    int Recvline(string &r)
    {//按行把数据接收到请求行中
        char ch='a';
        while(ch!='\n')//但是它的换行符不确定是\n \r \r\n????
        {
          int s=recv(sock,&ch,1,0);
          if(s<0)
          {
            ch='\n';
            r.push_back(ch);
          }

        else{
           if(ch!='\n'&&ch!='\r')
           {//此时就是正常字符，就直接把他放在请求行中
            r.push_back(ch);
           }
           else
            {//此时遇见了换行符，不论是哪个换行符，都让它成为\n
                if(ch=='\r')
                {//此时有可能是\r  \r\n
                    recv(sock,&ch,1,MSG_PEEK);//MSG_PEEK 读取拷贝数据，数据还在缓冲区
                    if(ch=='\n')
                        //说明是\r\n
                      recv(sock,&ch,1,0);//才读走，不然就粘包了
                    else//\r
                        ch='\n';
                }
                //此时ch一定是\n
                r.push_back(ch);
            }
        }
        }
        return r.size();
        cout<<"recv sucuss!"<<endl;
    }
    void RecvRequestline(Request *rq)
    {
       Recvline(rq->GetRequestline());
    }
    void RecvRequesthead(Request * rq)
    {
        string &h=rq->GetRequesthead();
     do {
            string ret="";
            Recvline(ret);
            if(ret=="\n")
               break;
             h+=ret;
    }while(1);
    }
    void RecvRequestbody(Request *rq)
    {
      //读多少是由Content Lenth决定 
     int len=rq->GetContentLength();
     string &body=rq->GetRequestbody();
     char c;
     while(len--)
     {
       if(recv(sock,&c,1,0)>0)
       {
         body.push_back(c);
       }
     }
    }
    void SendRespond(Respond *rps,int sock,bool cgi)
    {
        cout<<"send1..."<<endl;
        string &respondline=rps->GetRespondline();
        string &respondhead=rps->GetRespondhead();
        string &respondkong=rps->GetRespondkong();
        send(sock,respondline.c_str(),respondline.size(),0);
        send(sock,respondhead.c_str(),respondhead.size(),0);
        send(sock,respondkong.c_str(),respondkong.size(),0);
        if(cgi)
        {
        //cgi idea
        string body=rps->GetRespondbody(); 
        send(sock,body.c_str(),body.size(),0);
        }
        else
        {
        //no cgi idea
        int fd=rps->Getfd();
        int size=rps->Getsize();
        sendfile(sock,fd,NULL,size);        
        }
    }   
    void ClearRequest(Request *rqs)
    {
        if(rqs->GetRequesthead().empty())
        {//path error exit!
           RecvRequesthead(rqs);         

        }
        if(rqs->IsneedRecv())
        {
            if((rqs->GetRequestbody().empty()))
             RecvRequestbody(rqs);
        }
    }
    ~EnPoint()
    {
    close(sock);
    cout<<"quit..."<<endl;
    }
};
class Entry{
    private:
       
    public:

    static int ProcessCgi(EnPoint *ep,Request *rqs,Respond *rps)
    {
       pid_t id=fork();
       int input[2]={0};//child
       int output[2]={0};
       pipe(input);
       pipe(output);
       string path=rqs->Getpath();//wwwroot/a/b
       string para=rqs->Getpara();//参数
        //   int GetrecourceSize()
       int size=rqs->GetrecourceSize();
       string contentlength="Content-Length=";
       contentlength+=Function::InttoStr(size);
       string text;

       if(id<0)
       {//失败
       cout<<"fork erreo!"<<endl; 
       return 502;
       }
       else if(id==0)
       {//子进程
       close(input[1]);//最先是要接收数据
       close(output[0]);//最后是要往管道输出结果
       putenv((char *)contentlength.c_str());
       dup2(input[0],0);//重定向，文件描述符不会被替换，但是这里使用数组保存的，数组是数据，会被替换，故此
       dup2(output[1],1);//理由同上，现在标准输出就是outputp[1],写
       execl(path.c_str(),path.c_str(),nullptr);
       cout<<"execl false!"<<endl;//替换函数失败才会走到这！
       exit(1);
       }
       else{//父进程
       close(input[0]);//关闭读端，往管道中输入数据
       close(output[1]);//关闭写端，最后要从管道获得数据
       for(auto it=para.begin();it!=para.end();it++)
       {
        char ret=*it;
        write(input[1],&ret,1);
       
       }
       waitpid(id,NULL,0);
       char c;
       while(read(output[0],&c,1)>0)
       {
         text.push_back(c);
       }
       }
       return 200;
    }
    static void *HandleRequest(int sock)
    {
        cout<<"enter Handler"<<endl;
        int code=200;
        EnPoint *ep=new EnPoint(sock);
        Request *rqs=new Request();
        Respond *rps=new Respond();
        ep->RecvRequestline(rqs);//
        rqs->RequestlineParse();
        if(!rqs->MethodIslegal())
        {
            //not is legal
            code=404;
            goto end;
        }
        //legal
        //
        ep->RecvRequesthead(rqs);
        cout<<rqs->GetRequesthead()<<endl;
        rqs->RequestheadParse();//分析结束爆头，接下来判断是否要继续读，
        //有没有数据？？Content Length==??
        if(rqs->IsneedRecv())
        {//如果需要再读
          ep->RecvRequestbody(rqs);
        }
        rqs->Uriparse();
        if(!rqs->Ispathlegal())
        {//这里判断path是否合法
          code=404;
          cout<<"path no legal!"<<endl;
          goto end;
        }
        cout<<"path legal!"<<endl;
        if(rqs->Getcgi())
        {
        //do cgi
        cout<<"do cgi!"<<endl;
        code=ProcessCgi(ep,rqs,rps);
        if(code==200)
        {
        rps->MakeRespond(rqs,code,rqs->Getcgi());
        ep->SendRespond(rps,sock,true);
        }
        }
        else{
            cout<<"no do cgi!"<<endl;
        rps->MakeRespond(rqs,code,rqs->Getcgi());
        ep->SendRespond(rps,sock,false);
        }
end:
        if(code!=200)
        {
              //     if(rqs->GetRequesthead()=="")
              //     {//path error exit!
              //        ep->RecvRequesthead(rqs);           
              //     if(rqs->IsneedRecv())
              //     {
              //          ep->RecvRequestbody(rqs);
              //     }
              //     }
        ep->ClearRequest(rqs);
        rps->Remake(code);
        rps->MakeRespond(rqs,code,false);
        ep->SendRespond(rps,sock,false);
        }

        delete ep;
        ep=NULL;
        delete rqs;
        rqs=NULL;
        delete rps;
        rps=NULL;
    }
    ~Entry()
    {
    
    }
};
