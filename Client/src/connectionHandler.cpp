#include <connectionHandler.h>


using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
 
ConnectionHandler::ConnectionHandler(string host, short port, bool * logout, bool *terminated, bool *bye): host_(host), port_(port), io_service_(), socket_(io_service_),_mutex() ,myStompProtocol(logout,terminated,bye,host,port){

}
    
ConnectionHandler::~ConnectionHandler() {

    close();
}
 
bool ConnectionHandler::connect() {
    std::cout << "Starting connect to " 
        << host_ << ":" << port_ << std::endl;
    try {
		tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
		boost::system::error_code error;
		socket_.connect(endpoint, error);
        //string line;
//
        //getLine(line);
		//cout<<line<<endl;
		if (error)
			throw boost::system::system_error(error);
    }
    catch (std::exception& e) {
        std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 
bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
    size_t tmp = 0;
	boost::system::error_code error;
    try {
        while (!error && bytesToRead > tmp ) {
			tmp += socket_.read_some(boost::asio::buffer(bytes+tmp, bytesToRead-tmp), error);
			//cout<<"got some"<<endl;
        }
		if(error) {
            //cout<<error<<endl;
            throw boost::system::system_error(error);

        }
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
    int tmp = 0;
	boost::system::error_code error;
    try {
        while (!error && bytesToWrite > tmp ) {
			tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
        }
		if(error)
			throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "send failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 
bool ConnectionHandler::getLine(std::string& line) {
    return getFrameAscii(line, '\0');
}

bool ConnectionHandler::sendLine(std::string& line) {
    return sendFrameAscii(line, '\n');
}
 

bool ConnectionHandler::getFrameAscii(std::string& frame, char delimiter) {
    char ch;
    // Stop when we encounter the null character.
    // Notice that the null character is not appended to the frame string.
    try {
        do{
            if(!getBytes(&ch, 1))
            {
                return false;
            }
            if(ch!='\0')
                frame.append(1, ch);

        }while (delimiter != ch);
    } catch (std::exception& e) {
        std::cerr << "recv failed2 (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 
 
bool ConnectionHandler::sendFrameAscii(const std::string& frame, char delimiter) {
	bool result=sendBytes(frame.c_str(),frame.length());
	if(!result) return false;
	return sendBytes(&delimiter,1);
	return result;
}
 
// Close down the connection properly.
void ConnectionHandler::close() {
    try{

        socket_.close();
    } catch (...) {
        std::cout << "closing failed: connection already closed" << std::endl;
    }
}

bool ConnectionHandler::sendFrameFromCin(string line)
{
    std::lock_guard<std::mutex> lock(_mutex);
    //cout<<"b"<<endl;
    vector<string>* linesToSend= this->myStompProtocol.processLineFromUser(line);
    //cout<<"c"<<endl;
    //mutex lock
    stringstream str;
    bool ans=true;
    for(string line:*linesToSend)
    {
        if(line.compare("\0")==0)
        {
            ans = ans & sendFrameAscii("",'\0');
        }
        else{
            ans = ans & this->sendLine(line);
        }
    }
    string string1=str.str();
    //string bla="hello\0";
    //this->sendLine(bla);
    //bool ans=this->sendLine(string1);
    //cout<<string1<<endl;
    //cout<<"d"<<endl;

    string feedback="";
    string nextStr="";

    linesToSend->clear();
    delete(linesToSend);
    //bool ans=this->getLine(feedback);
    //cout<<feedback<<endl;
    //while(!nextStr.compare("\0")==0 && this->getLine(nextStr)) {
    //    feedback.append(nextStr);
    //    cout<<"receiving: "<<feedback<<endl;
    //}
    //cout<<"e"<<endl;
    return ans;
}

string ConnectionHandler::sendLogin(string host, short port, string userName, string passcode) {
    std::lock_guard<std::mutex> lock(_mutex);


    vector<string>* linesToSend=this->myStompProtocol.loginToSrv(host,userName,passcode);

    //mutex lock
    stringstream str;
    for(string line:*linesToSend)
    {
        if(line.compare("\0")==0)
        {
            sendFrameAscii("",'\0');
        } else {
            this->sendLine(line);
        }

        str<<line<<"\n"<<endl;
    }
    string string1=str.str();
    //string bla="hello\0";
    //this->sendLine(bla);
    //this->sendLine(string1);


    string feedback="";
    string nextStr="";

    this->getLine(feedback);
    if(feedback.length()>0) {
        cout << feedback << endl;
    }
    linesToSend->clear();
    delete(linesToSend);

    return feedback;

}

bool ConnectionHandler::readFrameFromSrv() {

    string frame;
    bool ans=this->getLine(frame);
    vector<string> *linesToSend=this->myStompProtocol.processFromSrv(frame);
    string empty="";
    //vector<string>* dummCheck= this->myStompProtocol.processFromSrv(empty);

    if(linesToSend->size()>0) {

        std::lock_guard<std::mutex> lock(_mutex);
        //cout<<"i"<<endl;
        //mutex lock
        stringstream str;
        for (string line:*linesToSend) {
            //this->sendLine(line);
            if(line.compare("\0")==0)
            {
                ans = ans & sendFrameAscii("",'\0');
            } else {
                ans = ans & this->sendLine(line);
            }
        }
        string string1=str.str();
        //string bla="hello\0";
        //this->sendLine(bla);
        //cout<<string1<<endl;
        //this->sendLine(string1);
    }

    linesToSend->clear();
    delete(linesToSend);
    if(frame.length()>0) {
        cout << frame << endl;
    }
    return ans;
}

//void ConnectionHandler::send
