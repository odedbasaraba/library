//
// Created by dory on 13/01/2020.
//
#include "string"
#ifndef BOOST_ECHO_CLIENT_PROTOCOLCINTRANSMITER_H
#define BOOST_ECHO_CLIENT_PROTOCOLCINTRANSMITER_H

using namespace std;
class protocolCinTransmiter {
public:
    int receiptIDs;
    void procees(string &LineCmd);
    protocolCinTransmiter(ConnectionHandler &connectionHandler,bool* logout, bool *terminated, string &host, int &port, protocolSrvReceiver &myProtocolSrvReceiver);

private:


    void join(string &basicString);
    void exitS(string &basicString);
    void add(string &basicString);
    void borrow(string &basicString);
    void returnS(string &basicString);
    void status(string &basicString);
    void logoutS(string &basicString);
    void bye(string &basicString);
    void loginS(string &basicString);
    ConnectionHandler &connectionHandler;
    bool *loggedOut;
    bool *terminated;
    string &host;
    int &port;
    string login;
    string passcode;
    int destinationCounter;
    protocolSrvReceiver &myProtocolSrvReceiver;
    void sendLinesVector(vector<string> &strlines);
};


#endif //BOOST_ECHO_CLIENT_PROTOCOLCINTRANSMITER_H
