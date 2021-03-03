#ifndef THREADFORSERVER_H
#define THREADFORSERVER_H

#include "connectionHandler.h"
using namespace std;
class ThreadForServer {
public:
    ThreadForServer (ConnectionHandler* h,bool* b,bool* logoutBool);
    void run();

private:
    ConnectionHandler *handler;
    bool * terminated;
    bool* logoutBool;



};

#endif //THREADFORSERVER_H