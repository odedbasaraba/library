package bgu.spl.net.srv.OutFrames;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.Frame;
import bgu.spl.net.srv.StompProtocol;

public class ConnectedFrame extends Frame {
    public ConnectedFrame(int connectionID, boolean isLoggedIn, StompProtocol stompProtocol, Connections connections, String[] message) {
        super(connectionID, isLoggedIn, stompProtocol, connections, message);
    }

    private String version;

    public void process(String version)
    {
        this.version=version;
        String out="CONNECTED\nversion:"+version+"\n\u0000";
        this.connections.send(connectionID,out);
    }


}
