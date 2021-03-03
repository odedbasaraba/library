#include <stdlib.h>
#include <thread>
#include "stdio.h"
#include "sstream"
#include "../include/ThreadForServer.h"
#include "../include/connectionHandler.h"
#include "../include/encoderDecoder.h"



/**
* This code assumes that the server replies the exact text the client sent it (as opposed to the practical session example)
*/
using namespace std;
int main (int argc, char *argv[]) {
    bool *terminated = new bool(false);
    bool *logoutBool = new bool(false);
    bool *bye = new bool(false);
    /*argc = 3;
    argv[0] = "dor";
    argv[1] = "127.0.0.1";
    argv[2] = "7777";

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " host port" << std::endl << std::endl;
        return -1;
    }*/

    while(!*bye)
    {
        *terminated=false;
        *logoutBool=false;
        const short bufsize = 1024;
        cout << "System is ready, please login:" << endl;

        bool logInSucceed = false;
        ConnectionHandler *connectionHandler= nullptr;
        while (!logInSucceed) {
            //cout << "in while" << endl;
            char first[bufsize];
            cin.getline(first, bufsize);
            std::istringstream iss(first);
            std::vector<std::string> resultsConnectLine((std::istream_iterator<std::string>(iss)),
                                                        std::istream_iterator<std::string>());
            if(resultsConnectLine.at(0).compare("bye")==0)
            {
                *bye=true;
                logInSucceed=false;
                cout<<"byebye"<<endl;
                break;
            } else {
                auto wantToFind=resultsConnectLine.at(1).find(":");
                string host = resultsConnectLine.at(1).substr(0, wantToFind);
                short port = stoi(resultsConnectLine.at(1).substr(resultsConnectLine.at(1).find(":") + 1));
                string userName = resultsConnectLine.at(2);
                string passcode = resultsConnectLine.at(3);
                resultsConnectLine.clear();

                if(connectionHandler!= nullptr) {
                    delete (connectionHandler);
                }
                connectionHandler = new ConnectionHandler(host, port, logoutBool, terminated,
                                                                                bye);


                if (!connectionHandler->connect()) {

                    std::cerr << "Cannot connect to " << host << ":" << port << std::endl;


                } else {
                    string feedback = connectionHandler->sendLogin(host, port, userName, passcode);
                    vector<string> results;
                    boost::split(results, feedback, [](char c) { return c == '\n'; });
                    if (results.at(0).compare("CONNECTED") == 0) {

                        logInSucceed = true;
                        //cout<<feedback<<endl;
                        continue;
                    }

                }
            }

        }
        //cout << "AAA" << endl;
        if(logInSucceed) {
            ThreadForServer threadForServer(connectionHandler, terminated, logoutBool);
            thread srvThread(&ThreadForServer::run, &threadForServer);

            /*string str = "hello\0";
            connectionHandler->sendLine(str);
            cout << "BBB" << endl;*/
            //From here we will see the rest of the ehco client implementation:
            while (!*terminated && !*bye) {
                //std::cout << "in while" << endl;
                char buf[bufsize];
                std::cin.getline(buf, bufsize);
                std::string line(buf);
                //cout<<"a"<<endl;
                //int len = line.length();
                try {
                    if (!*terminated && !connectionHandler->sendFrameFromCin(line)) {
                        std::cout << "Disconnected. Exiting...\n" << std::endl;

                        break;
                    }
                    if (line.compare("logout") == 0) {
                        while (!*logoutBool) {//
                            //cout<<"fgh"<<endl;
                            sleep(1);
                            //} //TODO: check about this
                            //cout<<"finish connection handler"<<endl;
                            //delete (connectionHandler);
                        }
                        //connectionHandler->close();

                        *terminated = true;
                    }
                }catch(std::exception& e)
                {
                    std::cerr << "Wrong line input: " << e.what() << std::endl;
                    std::cout<<"Try again:"<<endl;
                }
                catch (const char* msg)
                {
                    std::cerr<<msg<<endl;
                }
            }
            //cout << "finish cin while" << endl;
            srvThread.join();
        }
        delete (connectionHandler);
        connectionHandler= nullptr;
    }
    delete (logoutBool);
    delete (terminated);
    delete (bye);



//		// connectionHandler.sendLine(line) appends '\n' to the message. Therefor we send len+1 bytes.
//        std::cout << "Sent " << len+1 << " bytes to server" << std::endl;
//        // We can use one of three options to read data from the server:
//        // 1. Read a fixed number of characters
//        // 2. Read a line (up to the newline character using the getline() buffered reader
//        // 3. Read up to the null character
//        std::string answer;
//        // Get back an answer: by using the expected number of bytes (len bytes + newline delimiter)
//        // We could also use: connectionHandler.getline(answer) and then get the answer without the newline char at the end
//        if (!connectionHandler.getLine(answer)) {
//            std::cout << "Disconnected. Exiting...\n" << std::endl;
//            break;
//        }
//
//		len=answer.length();
//		// A C string must end with a 0 char delimiter.  When we filled the answer buffer from the socket
//		// we filled up to the \n char - we must make sure now that a 0 char is also present. So we truncate last character.
//        answer.resize(len-1);
//        std::cout << "Reply: " << answer << " " << len << " bytes " << std::endl << std::endl;
//        if (answer == "bye") {
//            std::cout << "Exiting...\n" << std::endl;
//            break;
//        }

        return 0;
    }


