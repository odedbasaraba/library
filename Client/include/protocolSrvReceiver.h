//
// Created by dory on 11/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_PROTOCOLSRVRECEIVER_H
#define BOOST_ECHO_CLIENT_PROTOCOLSRVRECEIVER_H

#include <string>
#include <map>

#include <vector>
#include <boost/asio/ip/tcp.hpp>
#include <boost/algorithm/string.hpp>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include "connectionHandler.h"
#include "Receipt.h"
#include "Book.h"
#include <stdlib.h>

using namespace std;
class protocolSrvReceiver {
public:
    protocolSrvReceiver(string userName, ConnectionHandler &connectionHandler, bool &terminated, bool &loggedOut);
    //TODO: rule of 5
    string userInputToFrame(string userInput);

    void process(string& framelLines);
    vector<string>* getBookInventory();
    unordered_map<int,Receipt>  receiptIDtoMessage;
    unordered_map <string,int> subscriptionIds;

    unordered_map<string,vector<Book>> bookInventoryByTopic;
    unordered_map<string,Book> booksToLender;// books that i got from a leander
    unordered_map<string, Book> booksThatIgaveAndAreNotMineToLender;
    string userName;
    string passcode;
private:


    bool isLoggedIn;
    ConnectionHandler &connectionHandler;
    bool &terminated;
    bool loggedOut;
    void login(string &framelLines);

    void Connected(string&frameLines, vector<string> &results);

    void processCin(string &line);

    void Message(string& frameLines, vector<string> &vector);

    void Error(string &basicString, vector<string> &vector);

    void receipt(string &basicString, vector<string> &vector);
};


#endif //BOOST_ECHO_CLIENT_PROTOCOLSRVRECEIVER_H
