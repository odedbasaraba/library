//
// Created by dory on 13/01/2020.
//

#include "Receipt.h"
using namespace std;
Receipt::Receipt(string action,string genere, int receiptId): action(action),genere_dest(genere),receiptId(receiptId){}

Receipt::~Receipt() {}