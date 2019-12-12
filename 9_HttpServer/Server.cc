#include"Server.hpp"
int main(int argc,char *argv[])
{
    if(argc!=2)
    {
    cout<<"Usage :"<<argv[0]<<" port"<<endl;
    exit(1);
    }
    Server *sp=new Server(atoi(argv[1]));
    cout<<argv[1]<<endl;
    sp->Init();
    sp->Run();
    delete sp;
    sp=NULL;
    return 0;
}
