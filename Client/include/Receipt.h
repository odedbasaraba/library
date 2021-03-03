//
// Created by dory on 13/01/2020.
//

#include "string"
#ifndef BOOST_ECHO_CLIENT_RECEIPT_H
#define BOOST_ECHO_CLIENT_RECEIPT_H

using namespace std;
class Receipt {
public:

    string action;
    string genere_dest;
    int receiptId;
    Receipt(string action,string genere, int receiptId);
    ~Receipt();

};


#endif //BOOST_ECHO_CLIENT_RECEIPT_H
