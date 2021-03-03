//
// Created by dory on 11/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_ENCODERDECODER_H
#define BOOST_ECHO_CLIENT_ENCODERDECODER_H


#include <string>
#include "protocolSrvReceiver.h"
using namespace std;
class encoderDecoder {
private:
    protocolSrvReceiver* myProtocol;

public:
    encoderDecoder();
    int encodeFromConsole(std::string line, char* outputArrayOfChatBytes);
    string decodeFromSrv();
};


#endif //BOOST_ECHO_CLIENT_ENCODERDECODER_H
