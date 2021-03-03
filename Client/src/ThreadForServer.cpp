#include "../include/ThreadForServer.h"
#include "../include/connectionHandler.h"
#include "../include/encoderDecoder.h"

ThreadForServer::ThreadForServer (ConnectionHandler* h,bool* t,bool* logoutBool): handler(h) ,terminated(t), logoutBool(logoutBool){};
using namespace std;
void ThreadForServer::run() {


    while (!*terminated)
    {
        string newFrameLines="";
        bool flag=handler->readFrameFromSrv();
        if(!flag)
        {
            cout<<"Error with server"<<endl;
            break;
        }
    }

    //cout<<"finished ServerThread run"<<endl;
};