package bgu.spl.net.srv.OutFrames;

import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.Frame;
import bgu.spl.net.srv.StompProtocol;

public class ErrorFrame extends Frame  {

    public ErrorFrame(int connectionID, boolean isLoggedIn, StompProtocol stompProtocol, Connections connections, String[] message) {
        super(connectionID, isLoggedIn, stompProtocol, connections, message);
    }

    public boolean proccess(String version, String body)
    {
        try {
            StringBuilder oldMessgae = new StringBuilder();
            for (int i = 0; i < message.length; i++) {
                oldMessgae.append(this.message[i] + "\n");
            }
            String out = "ERROR\nversion:" + version + "\n" + "message:login failure\n\nThe Message:\n----\n" +
                    oldMessgae.toString() + "\n----\n" + body + "\u0000";
            boolean ans = this.connections.send(connectionID, out);
        this.connections.disconnect(connectionID);
            return ans;
        }catch (Exception e)
        {
            return false;
        }
    }
}
