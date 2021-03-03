//
// Created by dory on 13/01/2020.
//
#include "string"
#ifndef BOOST_ECHO_CLIENT_BOOK_H
#define BOOST_ECHO_CLIENT_BOOK_H

using namespace std;
class Book {
public:
    string name;
    string lender;
    string topic;
    int topicID;
    Book(string name, string topic, int topicID);
    string status;

};


#endif //BOOST_ECHO_CLIENT_BOOK_H
