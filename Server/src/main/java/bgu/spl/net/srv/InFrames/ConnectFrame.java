package bgu.spl.net.srv.InFrames;

import bgu.spl.net.srv.*;
import bgu.spl.net.srv.OutFrames.ConnectedFrame;
import bgu.spl.net.srv.OutFrames.ErrorFrame;

public class ConnectFrame extends Frame {

    private String acceptVersion;
    private String host;
    private String loginName;
    private String passcode;


    StompMessageEncoderDecoder<String> stompMessageEncoderDecoder;


    public ConnectFrame(int connectionID, boolean isLoggedIn, StompProtocol stompProtocol, Connections connections, String[] message) {
        super(connectionID, isLoggedIn, stompProtocol, connections, message);
    }

    public void proccess() {
        //If already signed in to the system

            int ofLastHeader = -1;
            int ofBody = -1;
            int ofNull = -1;

            int i = 0;
            //detecting indexes
            while (i < message.length) {
                if (message[i].equals("") && ofLastHeader == -1) {
                    ofLastHeader = i - 1;
                }
                if (ofLastHeader != -1 && !message[i].equals("") && ofBody == -1) {
                    ofBody = i;
                }
                if (ofBody != -1 && message[i].equals(('\u0000'))) {
                    ofNull = i;
                }
                i++;
            }

                acceptVersion = message[1].substring("accept-version:".length());
                host = message[2].substring("host:".length());
                loginName = message[3].substring("login:".length());
                passcode = message[4].substring("passcode:".length());
            if(this.stompProtocol.isLoggedIn())
            {
                ErrorFrame errorFrame = new ErrorFrame(this.connectionID, this.isLoggedIn, this.stompProtocol, this.connections, this.message);
                errorFrame.proccess(this.acceptVersion, "You are already connected to a user");
            }
            else {


                String ans = connections.checkIfLoginIsValid(loginName, passcode);

                if (ans.equals("contains user but it is not logged in and passcode is ok")) {

                    User a = connections.getUser(loginName);
                    a.setLoggedIn(true);
                    this.stompProtocol.setLoggedIn(true);
                    ConnectionHandler newPendingUserToRemvoeCH = connections.getPendingUser(connectionID).getMyConnectionHandler();
                    a.setMyConnectionHandler(newPendingUserToRemvoeCH);
                    this.connections.removePendingUser(connectionID);//removing connectionIdOfNewUser
                    this.connectionID = a.getConectionId();
                    ConnectedFrame connectedFrame = new ConnectedFrame(this.connectionID, this.isLoggedIn, this.stompProtocol, this.connections, this.message);
                    connectedFrame.process(this.acceptVersion);

                } else if (ans.equals("doesn't conatin user- new user")) {

                    User a = connections.getPendingUser(connectionID);
                    a.setLogin(loginName);
                    a.setPasscode(passcode);
                    a.setLoggedIn(true);
                    this.stompProtocol.setLoggedIn(true);
                    connections.addClientToBase(connectionID);

                    this.stompProtocol.setLoggedIn(true); //TODO: check if needed and delete
                    ConnectedFrame connectedFrame = new ConnectedFrame(this.connectionID, this.isLoggedIn, this.stompProtocol, this.connections, this.message);
                    connectedFrame.process(this.acceptVersion);
                } else {
                    //Already logged in or bad password

                    ErrorFrame errorFrame = new ErrorFrame(this.connectionID, this.isLoggedIn, this.stompProtocol, this.connections, this.message);
                    if (errorFrame.proccess(this.acceptVersion, ans)) {
                        this.stompProtocol.setShoudlTerminate(true);
                    }
                    connections.removePendingUser(connectionID);
                }
            }


    }

    private void dontUse()
    {
        if(!isLoggedIn) {

            int ofLastHeader = -1;
            int ofBody = -1;
            int ofNull = -1;

            int i = 0;
            //detecting indexes
            while (i < message.length) {
                if (message[i].equals("") && ofLastHeader == -1) {
                    ofLastHeader = i - 1;
                }
                if (ofLastHeader != -1 && !message[i].equals("") && ofBody == -1) {
                    ofBody = i;
                }
                if (ofBody != -1 && message[i].equals(('\u0000'))) {
                    ofNull = i;
                }
                i++;
            }

            if (ofBody == -1) {
                //TODO: send error frame
                //return "No body in frame";
            }
            if (ofLastHeader != 4) {
                //TODO: send error frame
                //return "Not enough headers in frame";
            }
            if (message[1].contains("accept-version:")) {
                acceptVersion = message[1].substring("accept-version:".length());
            } else {
                //TODO: send error frame
                //return "Not enough headers in frame";
            }
            if (message[2].contains("host:")) {
                host = message[2].substring("host:".length());
            } else {
                //TODO: send error frame
                ////error no host
            }
            if (message[3].contains("login:")) {
                loginName = message[3].substring("login:".length());
            } else {
                //TODO: send error frame
                ////error no host
            }
            if (message[4].contains("passcode:")) {
                passcode = message[4].substring("passcode:".length());
            } else {
                //TODO: send error frame
                ////error no host
            }

            //IF reached here all conditions are ok
            //TODO: return Connected frame


            //String ans=connections.checkIfLoggedIn(login, passcode);
            User a= connections.getUser("");
            a.setLogin(loginName);
            a.setPasscode(passcode);
            ConnectedFrame connectedFrame=new ConnectedFrame(this.connectionID,this.isLoggedIn,this.stompProtocol,this.connections,this.message);
            connectedFrame.process(this.acceptVersion);
            this.stompProtocol.setLoggedIn(true);
        }
        else
        {
            //TODO: return not logged in
            //error is alreadyConnected
            ErrorFrame errorFrame = new ErrorFrame(this.connectionID,this.isLoggedIn,this.stompProtocol,this.connections,this.message);
            if(errorFrame.proccess(this.acceptVersion,"")) {
                this.stompProtocol.setLoggedIn(false);
                this.stompProtocol.setShoudlTerminate(true);
            }

        }
    }



}
