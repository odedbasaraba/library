//
// Created by dory on 13/01/2020.
//

#include "StompProtocol.h"
#include "string"
#include "connectionHandler.h"
#include <vector>
#include <boost/asio/ip/tcp.hpp>
#include <boost/algorithm/string.hpp>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Receipt.h"
#include "Book.h"
#include <stdlib.h>

using namespace std;
StompProtocol::StompProtocol(bool* logout, bool *terminated, bool * bye,
        string &host, short &port): receiptIDs(0),receiptIDtoMessage(0),subscriptionIds(0),bookInventoryByTopic(),booksToLender(),
                                    booksThatIgaveAndAreNotMineToLender(),isLoggedIn(false),terminated(terminated), loggedOut(logout),bye(bye),host(host),port(port),myLoginName(""),
        passcode(""),destinationCounter(0),borrowBookName()
{
}

StompProtocol::~StompProtocol()
{
    for(auto & it: receiptIDtoMessage)
    {
        delete (it.second);
        //it.second= nullptr;
        //receiptIDtoMessage.erase(it.first);
    }
    receiptIDtoMessage.clear();

    subscriptionIds.clear();

    for(auto & it: bookInventoryByTopic)
    {
        vector<Book*>* vecPTR=it.second;
        for (unsigned int j = 0; j < vecPTR->size(); ++j) {
            delete(vecPTR->at(j));

        }

        vecPTR->clear();
        delete vecPTR;

    }
    bookInventoryByTopic.clear();

    for(auto & it: booksToLender)
    {
        delete(it.second);

    }
    booksToLender.clear();

    for(auto & it: booksThatIgaveAndAreNotMineToLender)
    {
        delete(it.second);

    }
    booksThatIgaveAndAreNotMineToLender.clear();

}

vector<string>* StompProtocol::processLineFromUser(string &lineCmd) {
    string firstWord=lineCmd.substr(0,lineCmd.find_first_of(" "));
    if(firstWord.compare("login")==0)
    {
        return loginS(lineCmd);
    }
    if(firstWord.compare("join")==0)
    {
        return join(lineCmd);
    }
    if(firstWord.compare("exit")==0)
    {
        return exitS(lineCmd);
    }
    if(firstWord.compare("add")==0) // send
    {
        return add(lineCmd);
    }
    if(firstWord.compare("borrow")==0) // send
    {
        return borrow(lineCmd);
    }
    if(firstWord.compare("return")==0) // send
    {
        return returnS(lineCmd);
    }
    if(firstWord.compare("status")==0) // send
    {
        return status(lineCmd);
    }
    if(firstWord.compare("logout")==0)
    {
        return logoutS(lineCmd);
    }
    if(firstWord.compare("bye")==0)
    {
        byeS(lineCmd);
        vector<string>* vPtr= new vector<string>();
        return vPtr;
    } else
    {
        throw std::exception();
        vector<string>* vPtr= new vector<string>();
        return vPtr;
    }
}

vector<string>* StompProtocol::join(string &basicString) {
    vector<string>* frameLinesOutput = new vector<string>();

    std::istringstream iss(basicString);
    std::vector<std::string> resultsInput((std::istream_iterator<std::string>(iss)),
                                          std::istream_iterator<std::string>());
    string nameOfTopic=resultsInput.at(1);
    int receiptIdForNew= this->receiptIDs;
    receiptIDs++;
    int topicDestIdNew=destinationCounter;
    destinationCounter++;
    frameLinesOutput->push_back("SUBSCRIBE");
    frameLinesOutput->push_back("destination:"+nameOfTopic);
    frameLinesOutput->push_back("id:"+to_string(receiptIdForNew));
    frameLinesOutput->push_back("receipt:"+to_string(receiptIdForNew)+"\n");
    //frameLinesOutput->push_back("\n");
    //TODO: remember to delete
    Receipt *receipt= new Receipt("SUBSCRIBE",nameOfTopic,receiptIdForNew);
    this->receiptIDtoMessage.insert(pair<int,Receipt*>(receiptIdForNew,receipt));
    frameLinesOutput->push_back("\0");
    this->subscriptionIds.insert(pair<string,int>(nameOfTopic,topicDestIdNew));
    return frameLinesOutput;
}

vector<string>* StompProtocol::exitS(string &basicString) {
    vector<string>* frameLinesOutput = new vector<string>();

    std::istringstream iss(basicString);
    std::vector<std::string> resultsInput((std::istream_iterator<std::string>(iss)),
                                          std::istream_iterator<std::string>());
    string nameOfTopic=resultsInput.at(1);
    int receiptIdForNew= this->receiptIDs;
    receiptIDs++;
    int genreId=this->subscriptionIds.at(nameOfTopic);

    frameLinesOutput->push_back("UNSUBSCRIBE");
    frameLinesOutput->push_back("id:"+to_string(genreId));
    frameLinesOutput->push_back("receipt:"+to_string(receiptIdForNew)+"\n");
    //frameLinesOutput->push_back("\n");

    Receipt *receipt=new Receipt("UNSUBSCRIBE",nameOfTopic,receiptIdForNew);
    this->receiptIDtoMessage.insert(pair<int,Receipt*>(receiptIdForNew,receipt));
    frameLinesOutput->push_back("\0");
    return frameLinesOutput;
}

vector<string>* StompProtocol::add(string &basicString) {
    vector<string>* frameLinesOutput = new vector<string>();

    std::istringstream iss(basicString);
    std::vector<std::string> resultsInput((std::istream_iterator<std::string>(iss)),
                                          std::istream_iterator<std::string>());
    string nameOfTopic=resultsInput.at(1);//genere
    string firtWordInBookName=resultsInput.at(2);
    string nameOfBook=basicString.substr(basicString.find(firtWordInBookName));

    //search if the book exists:
    for (auto& it: this->bookInventoryByTopic) {
        for (unsigned int j = 0; j < it.second->size(); ++j) {
            if(it.second->at(j)->name.compare(nameOfBook)==0)
            {
                throw "Book already exits in the system";
            }
        }
    }
    for(auto & it: this->booksToLender)
    {
        if(it.first.compare(nameOfBook)==0)
        {
            throw "Book already exits in the system";
        }
    }

    //int genreId=this->subscriptionIds.at(nameOfTopic);
    if(this->subscriptionIds.find(nameOfTopic)!=subscriptionIds.end()) {
        int subscriptionTopicID = this->subscriptionIds.at(nameOfTopic);
        Book *book = new Book(nameOfBook, nameOfTopic,subscriptionTopicID);
        if (this->bookInventoryByTopic.find(nameOfTopic) != bookInventoryByTopic.end()) {
            this->bookInventoryByTopic.at(nameOfTopic)->push_back(book);
        } else {
            vector<Book*>* newTopicBooks= new vector<Book*>();
            newTopicBooks->push_back(book);
            this->bookInventoryByTopic.insert(pair<string,vector<Book*>*>(nameOfTopic,newTopicBooks));
        }
    } else
    {
        string join_str="join "+nameOfTopic;
        vector<string>* subscriptionFrame= this->join(join_str);
        for (unsigned int j = 0; j < subscriptionFrame->size(); ++j) {
            frameLinesOutput->push_back(subscriptionFrame->at(j));
        }
        subscriptionFrame->clear();
        delete(subscriptionFrame);
        this->subscriptionIds.insert(pair<string, int>(nameOfTopic,this->destinationCounter));

        int subscriptionTopicID = this->subscriptionIds.at(nameOfTopic);
        Book *book = new Book(nameOfBook, nameOfTopic,subscriptionTopicID);
        destinationCounter++;
        if (this->bookInventoryByTopic.find(nameOfTopic) != bookInventoryByTopic.end()) {
            this->bookInventoryByTopic.at(nameOfTopic)->push_back(book);
        } else {
            vector<Book*>* newTopicBooks= new vector<Book*>();
            newTopicBooks->push_back(book);
            this->bookInventoryByTopic.insert(pair<string,vector<Book*>*>(nameOfTopic,newTopicBooks));
        }
    }
    frameLinesOutput->push_back("SEND");
    frameLinesOutput->push_back("destination:"+nameOfTopic+"\n");
    frameLinesOutput->push_back(myLoginName+" has added the book "+nameOfBook+"\n");
    //frameLinesOutput->push_back("\n");
    frameLinesOutput->push_back("\0");


    return frameLinesOutput;

}

vector<string>* StompProtocol::borrow(string &basicString) {
    vector<string>* frameLinesOutput = new vector<string>();

    std::istringstream iss(basicString);
    std::vector<std::string> resultsInput((std::istream_iterator<std::string>(iss)),
                                          std::istream_iterator<std::string>());
    string nameOfTopic=resultsInput.at(1);//genere
    string firtWordInBookName=resultsInput.at(2);
    string nameOfBook=basicString.substr(basicString.find(firtWordInBookName));

    //int genreId=this->subscriptionIds.at(nameOfTopic);

    frameLinesOutput->push_back("SEND");
    frameLinesOutput->push_back("destination:"+nameOfTopic+"\n");
    //frameLinesOutput->push_back("\n");
    frameLinesOutput->push_back(myLoginName+" wish to borrow "+nameOfBook+"\n");
    //frameLinesOutput->push_back("\n");
    frameLinesOutput->push_back("\0");
    borrowBookName.bookName=nameOfBook;
    borrowBookName.status="waiting";
    return frameLinesOutput;
}

vector<string>* StompProtocol::returnS(string &basicString) {

    vector<string>* frameLinesOutput = new vector<string>();

    std::istringstream iss(basicString);
    std::vector<std::string> resultsInput((std::istream_iterator<std::string>(iss)),
                                          std::istream_iterator<std::string>());
    string nameOfTopic=resultsInput.at(1);//genere
    string firtWordInBookName=resultsInput.at(2);
    string nameOfBook=basicString.substr(basicString.find(firtWordInBookName));
    if(this->booksToLender.find(nameOfBook)!=booksToLender.end()) {
        Book *theBookToReturn=booksToLender.at(nameOfBook);
        //int genreId = this->subscriptionIds.at(nameOfTopic);

        frameLinesOutput->push_back("SEND");
        frameLinesOutput->push_back("destination:" + nameOfTopic+"\n");
        //frameLinesOutput->push_back("\n");
        frameLinesOutput->push_back("Returning " + nameOfBook + " to " + theBookToReturn->lender+"\n");
        //frameLinesOutput->push_back("\n");
        frameLinesOutput->push_back("\0");

        booksToLender.erase(nameOfBook);
        delete (theBookToReturn);
        theBookToReturn= nullptr;
        return frameLinesOutput;
    } else
    {
        vector<string>* vPtr= new vector<string>();
        return vPtr;
    }

}

vector<string>* StompProtocol::status(string &basicString) {
    vector<string>* frameLinesOutput = new vector<string>();

    std::istringstream iss(basicString);
    std::vector<std::string> resultsInput((std::istream_iterator<std::string>(iss)),
                                          std::istream_iterator<std::string>());
    string nameOfTopic=resultsInput.at(1);//genere
    //int genreId;
    if(!(subscriptionIds.find(nameOfTopic)!=subscriptionIds.end()) ){
        this->subscriptionIds.insert(pair<string,int>(nameOfTopic,this->destinationCounter));
        destinationCounter++;
        //genreId= this->subscriptionIds.at(nameOfTopic);
    }
        frameLinesOutput->push_back("SEND");
        frameLinesOutput->push_back("destination:" + nameOfTopic+"\n");
        //frameLinesOutput->push_back("\n");
        frameLinesOutput->push_back("book status");
        frameLinesOutput->push_back("\0");
        return frameLinesOutput;

}

vector<string>* StompProtocol::logoutS(string &basicString) {
    vector<string>* frameLinesOutput = new vector<string>();

    std::istringstream iss(basicString);
    std::vector<std::string> resultsInput((std::istream_iterator<std::string>(iss)),
                                          std::istream_iterator<std::string>());
    int receiptIdForNew= this->receiptIDs;
    receiptIDs++;

    frameLinesOutput->push_back("DISCONNECT");

    frameLinesOutput->push_back("receipt:"+to_string(receiptIdForNew)+"\n");
    Receipt *receipt=new Receipt("DISCONNECT","",receiptIdForNew);
    this->receiptIDtoMessage.insert(pair<int,Receipt*>(receiptIdForNew,receipt));
    //frameLinesOutput->push_back("\n");
    frameLinesOutput->push_back("\0");
    return frameLinesOutput;
}

void StompProtocol::byeS(string &basicString) {
//TODO: to complete
    *terminated=true;
    *loggedOut=true;
    *bye=true;
}

vector<string>* StompProtocol::loginS(string &basicString) {
    vector<string>* frameLinesOutput = new vector<string>();

    std::istringstream iss(basicString);
    std::vector<std::string> resultsInput((std::istream_iterator<std::string>(iss)),
                                          std::istream_iterator<std::string>());
    string login=resultsInput.at(2);
    string passcode=resultsInput.at(3);
    frameLinesOutput->push_back("CONNECT");
    frameLinesOutput->push_back("accept-version:1.2");
    frameLinesOutput->push_back("host:"+host);
    frameLinesOutput->push_back("login:"+login);
    frameLinesOutput->push_back("passcode:"+passcode+"\n");
    //frameLinesOutput->push_back("\n");
    frameLinesOutput->push_back("\0");

    this->myLoginName=login;
    this->passcode=passcode;
    return frameLinesOutput;

}

vector<string>* StompProtocol::loginToSrv(string host,string userName,string passcode)
{
    vector<string>* frameLinesOutput = new vector<string>();


    frameLinesOutput->push_back("CONNECT");
    frameLinesOutput->push_back("accept-version:1.2");
    frameLinesOutput->push_back("host:"+host);
    frameLinesOutput->push_back("login:"+userName);
    frameLinesOutput->push_back("passcode:"+passcode+"\n");
    //frameLinesOutput->push_back("\n");
    frameLinesOutput->push_back("\0");

    this->myLoginName=userName;
    this->passcode=passcode;
    return frameLinesOutput;
}




vector<string>* StompProtocol::processFromSrv(string &framelLines) {
    //cout<<"g"<<endl;
    vector<string> results;
    boost::split(results,framelLines,[](char c){return c == '\n';});
    if(results.at(0).compare("CONNECTED")==0)
    {
        return Connected(framelLines,results);
    }
    if(results.at(0).compare("MESSAGE")==0)
    {
        return Message(framelLines,results);
    }
    if(results.at(0).compare("ERROR")==0)
    {
        return Error(framelLines,results);
    }
    if(results.at(0).compare("RECEIPT")==0)
    {
        return receipt(framelLines,results);
    } else
    {
        vector<string>* vecPtr= new vector<string>();
        return vecPtr;
    }

}


vector<string>* StompProtocol:: Connected(string &frameLines, vector<string> &results)
{
    *loggedOut= false; //the user got approved that he is logged in
    std::cout<<"Login successful"<<endl;
    vector<string>* vecPtr= new vector<string>();
    return vecPtr;

}

vector<string>* StompProtocol::Message(string &frameLines, vector<string> &results) {
    //string body=fra
    vector<string>* ansFrame = new vector<string>();
    //TODO: print income message
    string maindestinationLine= results.at(3);
    string maindestinationStr="destination:";
    string maintheDestination=maindestinationLine.substr(maindestinationStr.length());
    string mainBody= results.at(results.size()-2);
    //std::cout<<"{"<<maintheDestination<<"}:{"<<mainBody<<"}"<<endl;
    if(results.at(results.size()-2).find("book status")!=string::npos)
    {
        string destinationLine= results.at(3);
        string destinationStr="destination:";
        string theDestination=destinationLine.substr(destinationStr.length());

        //stringstream ans();
        string ans;
        int number=0;

        if(this->bookInventoryByTopic.find(theDestination)!=bookInventoryByTopic.end()&&
        this->bookInventoryByTopic.at(theDestination)->size()>0) {
            for (Book *nextBook: *bookInventoryByTopic.at(theDestination)) {
                if (number == 0) {
                    ans.append(nextBook->name);

                    number++;
                } else
                    ans.append(","+nextBook->name);

            }
        }

        for(auto &it: this->booksToLender)
        {
            if(it.second->topic.compare(theDestination)==0)
            {
                if(number==0)
                {
                    ans.append(it.second->name);

                    number++;
                } else
                {
                    ans.append(","+it.second->name);

                }
            }
        }

        if(number>0) {
            string str = "";
            ansFrame->push_back("SEND");
            ansFrame->push_back(destinationLine);
            ansFrame->push_back("\n");
            string body = myLoginName + ":" + ans;
            ansFrame->push_back(body);
            ansFrame->push_back("\0");
        }


    }
    if(results.at(results.size()-2).find("borrow")!=string::npos)
    {

        //bookInventoryByTopic.i
        //in borrow status
        //TODO: understand how to go threw the map of vectors
        //TODO: go threw all books also from lenders: for transitive borrow
        int loactionOfBorrow= results.at(results.size()-2).find("borrow ");
        string theStrBorrow= "borrow ";
        string bookName= results.at(results.size()-2).substr(loactionOfBorrow+theStrBorrow.length());
        string destinationLine= results.at(3);

        bool answer=false;

        for(auto & it : bookInventoryByTopic)
        {
            for (unsigned int j = 0; j < it.second->size(); ++j) {
                if(it.second->at(j)->name.compare(bookName)==0)
                {
                    answer=true;
                    break;
                }
            }
            if(answer)
            {
                break;
            }
        }


        if(!answer && booksToLender.find(bookName)!=booksToLender.end())
        {
            answer=true;
        }

        if(answer)
        {

            string str="";
            ansFrame->push_back("SEND");
            ansFrame->push_back(destinationLine);
            ansFrame->push_back("\n");
            string body=myLoginName+ " has "+bookName;
            ansFrame->push_back(body);
            ansFrame->push_back("\0");

        }
    }

    if(results.at(results.size()-2).find("has")!=string::npos)
    {
        if(results.at(results.size()-2).find("added")==string::npos)
        {

            int loactionOfHas= results.at(results.size()-2).find("has ");
            string theStrHas= "has ";
            string bookName= results.at(results.size()-2).substr(loactionOfHas+theStrHas.length());
            string lenderName=results.at(results.size()-2).substr(0,loactionOfHas-1);

            //string destinationLine3= results.at(3);
            //cout<<results.at(3)<<endl;

            if(this->borrowBookName.bookName.compare(bookName)==0 && borrowBookName.status.compare("waiting")==0)
            {
                string str="";
                ansFrame->push_back("SEND");
                ansFrame->push_back(maindestinationLine);
                ansFrame->push_back("\n");
                string body="Taking "+bookName+" from " +lenderName;
                ansFrame->push_back(body);
                ansFrame->push_back("\0");
                borrowBookName.status="taking sent";
            }



        }
    }


    if(results.at(results.size()-2).find("Taking")!=string::npos)
    {
        //bookInventoryByTopic.i
        //in borrow status
        //TODO: understand how to go threw the map of vectors
        //TODO: go threw all books also from lenders: for transitive borrow
        int loactionOfTaking= results.at(results.size()-2).find("Taking ");
        string theStrTaking= "Taking ";
        string fromStr="from ";
        int locationOfFrom=results.at(results.size()-2).find("from ");
        int delta=locationOfFrom-1-(loactionOfTaking+theStrTaking.length());
        string nameToTakeFrom= results.at(results.size()-2).substr(locationOfFrom+fromStr.length());
        string bookName= results.at(results.size()-2).substr(loactionOfTaking+theStrTaking.length(),delta);

        string destinationLine= results.at(3);
        string destinationStr="destination:";
        string theDestination=destinationLine.substr(destinationStr.length());



        bool answer=false;
        if(this->myLoginName.compare(nameToTakeFrom)==0) {
            //If i am the lender
            for (auto &it : bookInventoryByTopic) {
                for (unsigned int j = 0; j < it.second->size(); ++j) {
                    if (it.second->at(j)->name.compare(bookName) == 0) {
                        answer = true;
                        //Book book=it.second.at(j);
                        //book.lender=nameToTakeFrom;
                        it.second->erase(it.second->begin() + j);
                        //this->booksToLender.insert(pair<string,Book>(bookName,book));
                        break;
                    }
                }
                if (answer) {
                    break;
                }
            }

            if (!answer && booksToLender.find(bookName) != booksToLender.end()) {

                booksThatIgaveAndAreNotMineToLender.insert(pair<string, Book*>(bookName, (booksToLender[bookName])));
                booksThatIgaveAndAreNotMineToLender.at(bookName)->status="borrowd";
                booksToLender.erase(bookName);
                answer = true;
            }

        }

        if(this->borrowBookName.status.compare("taking sent")==0 &&
        this->borrowBookName.bookName.compare(bookName)==0)
        {
            //I am the one borrowing the book
            int subscriptionId=0;
            if(this->subscriptionIds.find(theDestination)!=this->subscriptionIds.end())
            {
                subscriptionId=subscriptionIds.at(theDestination);
            } else
            {
                subscriptionIds.insert(pair<string,int>(theDestination,this->destinationCounter));
                destinationCounter++;
            }
            Book* lendedBook = new Book(bookName,theDestination,subscriptionId);
            lendedBook->lender=nameToTakeFrom;
            this->booksToLender.insert(pair<string,Book*>(lendedBook->name,lendedBook));
            this->borrowBookName.bookName="";
            this->borrowBookName.status="";
        }
    }

    if(results.at(results.size()-2).find("Returning")!=string::npos)
    {

        int loactionOfReturning= results.at(results.size()-2).find("Returning ");
        int loactionOfTo=results.at(results.size()-2).find("to");
        string theStrReturning= "Returning ";
        int delta=loactionOfTo-1-(loactionOfReturning+theStrReturning.length());
        string bookName= results.at(results.size()-2).substr(loactionOfReturning+theStrReturning.length(),delta);
        string theStrTo="to ";
        string clientName=results.at(results.size()-2).substr(loactionOfTo+theStrTo.length());
        string destinationLine= results.at(3);
        string destinationStr="destination:";
        string theDestination=destinationLine.substr(destinationStr.length());


        if(this->myLoginName.compare(clientName)==0)
        {
            if(this->booksThatIgaveAndAreNotMineToLender.find(bookName)!=booksThatIgaveAndAreNotMineToLender.end())
            {
                string lender=booksThatIgaveAndAreNotMineToLender.at(bookName)->lender;
                Book* book=booksThatIgaveAndAreNotMineToLender.at(bookName);

                this->booksThatIgaveAndAreNotMineToLender.erase(bookName);
                this->booksToLender.insert(pair<string,Book*> (bookName,book));
            }
            else{
                if(int id= this->subscriptionIds.find(theDestination)!=this->subscriptionIds.end() &&
                this->bookInventoryByTopic.find(theDestination)!=bookInventoryByTopic.end())
                {
                    Book* book = new Book(bookName,theDestination,id);
                    this->bookInventoryByTopic.at(theDestination)->push_back(book);
                }
                else
                {
                    vector<Book*>* books = new vector<Book*>();
                    Book* book = new Book(bookName,theDestination,id);
                    books->push_back(book);
                    this->bookInventoryByTopic.insert(pair<string, vector<Book*>*>(theDestination,books));
                }
            }
        }
    }
    return ansFrame;

}

vector<string>* StompProtocol::Error(string &basicString, vector<string> &vector) {
    //std::cout<<vector.at(vector.size()-2)<<endl; //TODO: fix print
    //std::cout<<basicString<<endl;
    //return terminateUser();
    *terminated=true;
    std::vector<string>* vecPtr= new std::vector<string>();
    return vecPtr;

}

vector<string>* StompProtocol::receipt(string &frameLines, vector<string> &results) {

    string theStrReceipt= "receipt-id:";
    //TODO: check if stoi works
    string receiptID= results.at(1).substr(theStrReceipt.length());
    int receiptIDint = stoi(receiptID);
    if(this->receiptIDtoMessage.find(receiptIDint)!=receiptIDtoMessage.end())
    {
        string action=receiptIDtoMessage.at(receiptIDint)->action;
        string genere=receiptIDtoMessage.at(receiptIDint)->genere_dest;
        if(action.compare("SUBSCRIBE")==0)
        {
            cout<<"Joined club " << genere<<endl;
        }
        if(action.compare("UNSUBSCRIBE")==0)
        {
            cout<<"Exited club " << genere<<endl;
        }
        if(action.compare("DISCONNECT")==0)
        {
            //TODO: send logged out both to socekt object and consoleProtocol
            *loggedOut=true;
            *terminated=true;
            //cout<<"h"<<endl;
        }

    }
    std::vector<string>* vecPtr= new std::vector<string>();
    return vecPtr;
}

vector<string>* StompProtocol::terminateUser()
{
    string logout="logout";
    return logoutS(logout);
}

