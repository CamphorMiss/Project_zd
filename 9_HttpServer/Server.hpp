#pragma once
#include"Protocol.hpp"
#include<iostream>
#include<sys/types.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<unistd.h>
#include"Threadpool.hpp"
using namespace std;
class Server{
    private:
        int port;
        int sock;
        Threadpool *tp;
    public:
        Server(int _port=8080):port(_port),sock(-1),tp(NULL)
        {}
        void Init()
        {
        sock=socket(AF_INET,SOCK_STREAM,0);
        if(sock<0)
        {
        cerr<<"socket err!"<<endl;
        exit(2);
        }
        cout<<sock<<endl;
        struct sockaddr_in local;
        local.sin_family=AF_INET;
        local.sin_addr.s_addr=htonl(INADDR_ANY);
        local.sin_port=htons(port);
        if(bind(sock,(struct sockaddr *)&local,sizeof(local))<0)
        {
        cerr<<"bind err!"<<endl;
        exit(3);
        }
        if(listen(sock,8)<0)
        {
        cerr<<"listen err!"<<endl;
        exit(4);
        }
        }
        void Run()
        {
            struct sockaddr_in peer;
            socklen_t len=0;
               
            //Task t(nsock,Entry::HandleRequest);
            tp= new Threadpool(10);
            tp->InitThreadpool();
        while(1)
        {
            int nsock=accept(sock,(struct sockaddr *)&peer,&len);
            if(nsock<0)
            {
            cerr<<"accept err!"<<endl;
            exit(5);
            }
            cout<<"Get a new link..."<<endl;
            
         //   pthread_t pd;
         //   int *p=&nsock;
         //   pthread_create(&pd,NULL,Entry::HandleRequest,(void *)p);
            Task t(nsock,Entry::HandleRequest);
        cout<<"Ready to PushTask()"<<endl;    
            tp->PushTask(t);
          //  Threadpool pool1;
          //  pool.InitThreadpool();
            //pool.Gettask_queue().push_back(t);
           // pool.ThreadRoutine();
        }
        }
        ~Server()
        {
        if(sock>=0)
        {
        close(sock);
        }
        }
};
