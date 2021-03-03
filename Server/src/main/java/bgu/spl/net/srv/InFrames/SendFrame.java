package bgu.spl.net.srv.InFrames;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.Frame;
import bgu.spl.net.srv.OutFrames.MessageFrame;
import bgu.spl.net.srv.StompProtocol;

public class SendFrame extends Frame {

    private String desteneation;
    private String body;
    public SendFrame(int connectionID, boolean isLoggedIn, StompProtocol stompProtocol, Connections connections, String[] message) {
        super(connectionID, isLoggedIn, stompProtocol, connections, message);
        body=message[message.length-1];
        desteneation=message[1].substring("destination:".length());
    }

    public void process() {

        MessageFrame messageFrame = new MessageFrame(connectionID,isLoggedIn,stompProtocol,connections,message);
        messageFrame.process(body,desteneation);


    }
}
