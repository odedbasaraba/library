package bgu.spl.net.srv.OutFrames;

import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.Frame;
import bgu.spl.net.srv.StompProtocol;

public class MessageFrame extends Frame {
    public MessageFrame(int connectionID, boolean isLoggedIn, StompProtocol stompProtocol, Connections connections, String[] message) {
        super(connectionID, isLoggedIn, stompProtocol, connections, message);
    }

    public void process(String message, String destination)
    {
        String out="MESSAGE\nsubscription:\n"+"Message-id:"+connections.getGlobalMessageId()+"\ndestination:"+destination+"\n\n"+message+"\n\u0000";
        connections.send(destination,out);
    }
}
