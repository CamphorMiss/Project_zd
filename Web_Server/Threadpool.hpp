#pragma once
#include<queue>
#include<pthread.h>
#include<iostream>
using namespace std;
typedef void *(*handler_t )(int);
class Task{
    private:
    handler_t handler;
    int sock;
    public:
    Task(int sock_,handler_t handler_):sock(sock_),handler(handler_)
    {
        //cout<<"Task sussecss!"<<endl;
    }
    void Run()
    {
        //cout<<"Task Run!"<<endl;
        handler(sock);
    }
    ~Task()
    {
    }

};
class Threadpool
{
 private:
     int num;//thread num
     int nowsize;
     queue<Task> task_queue;
     pthread_mutex_t lock;//huchisuo
     pthread_cond_t  cond;//tiaojianbainliang
 public:
     Threadpool(int _num):num(_num),nowsize(0)
     {
     pthread_mutex_init(&lock,NULL);
     pthread_cond_init(&cond,NULL);
     }
     void InitThreadpool()
     {
         pthread_t p;
         for(int i=0;i<num;i++)
         {
          pthread_create(&p,NULL,ThreadRoutine,this);    
         }
     }
      static void *ThreadRoutine(void * arg)
     {
      pthread_detach(pthread_self());//detach
      Threadpool * tp=(Threadpool *)arg;
      while(true)
      {
        tp->Lockqueue();
        while(tp->Queueisempty())
        {
         tp->Pthreadwait();   
        }
        Task t=tp->PopTask();
        tp->Unlockqueue();
        t.Run();
      }
     }
    void Pthread_signal()
   {
    pthread_cond_signal(&cond);
    }

     void Pthreadwait()
     {
     pthread_cond_wait(&cond,&lock);
     }
     bool Queueisempty()
     {
       return nowsize==0?true:false;
     }
     Task PopTask()
     {
      Task ret= task_queue.front();
      task_queue.pop();
      nowsize--;
      return ret;
     }
     void PushTask(Task & t)
     {
      nowsize++;
      task_queue.push(t);
      Pthread_signal();
     }
     void Lockqueue()
     {
       pthread_mutex_lock(&lock);    
     }
     void Unlockqueue()
     {
       pthread_mutex_unlock(&lock);
     }
     ~Threadpool()
     {
     pthread_mutex_destroy(&lock);
     pthread_cond_destroy(&cond);
     }
};
