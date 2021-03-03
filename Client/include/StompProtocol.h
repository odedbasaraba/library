//
// Created by dory on 13/01/2020.
//
#include "string"
#include "connectionHandler.h"
#include <vector>
#include <boost/asio/ip/tcp.hpp>
#include <boost/algorithm/string.hpp>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Borrow.h"

#include "Receipt.h"
#include "Book.h"
#include <stdlib.h>

#ifndef BOOST_ECHO_CLIENT_STOMPPROTOCOL_H
#define BOOST_ECHO_CLIENT_STOMPPROTOCOL_H

using namespace std;
class StompProtocol {
public:
   //Fields
    int receiptIDs;





    //Basic

    StompProtocol(bool* logout, bool *terminated,bool * bye, string &host, short &port);
    ~StompProtocol();
    StompProtocol(const StompProtocol &other);
    StompProtocol &operator =(const StompProtocol & a);

     //Transmitter
    vector<string>* processLineFromUser(string &LineCmd);
    vector<string>* loginToSrv(string host,string userName,string passcode);

    //Receiver
    //Maps

    unordered_map<int,Receipt*>  receiptIDtoMessage;
    unordered_map <string,int> subscriptionIds;
    unordered_map<string,vector<Book*>*> bookInventoryByTopic;
    unordered_map<string,Book*> booksToLender;// books that i got from a leander
    unordered_map<string, Book*> booksThatIgaveAndAreNotMineToLender;

    //Methods
     vector<string>* getBookInventory();
    //TODO: rule of 5
    string userInputToFrame(string userInput);
    vector<string>* processFromSrv(string& framelLines);


private:

    //SrvReceiverFields
    bool isLoggedIn;

    bool *terminated;
    bool *loggedOut;
    bool *bye;

    //SrvReceiverMethdos
    vector<string>* login(string &framelLines);
    vector<string>* Connected(string&frameLines, vector<string> &results);
    vector<string>* processCin(string &line);
    vector<string>* Message(string& frameLines, vector<string> &vector);
    vector<string>* Error(string &basicString, vector<string> &vector);
    vector<string>* receipt(string &basicString, vector<string> &vector);

    //CinTransmiterFields

    string &host;
    short &port;
    string myLoginName;
    string passcode;
    int destinationCounter;
    //CinTransmiter methods
    vector<string>* join(string &basicString);
    vector<string>* exitS(string &basicString);
    vector<string>* add(string &basicString);
    vector<string>* borrow(string &basicString);
    vector<string>* returnS(string &basicString);
    vector<string>* status(string &basicString);
    vector<string>* logoutS(string &basicString);
    void byeS(string &basicString);
    vector<string>* loginS(string &basicString);
    void sendLinesVector(vector<string> &strlines);
    Borrow borrowBookName;

    vector<string>* terminateUser();


};


#endif //BOOST_ECHO_CLIENT_STOMPPROTOCOL_H
