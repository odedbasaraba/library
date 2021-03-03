package bgu.spl.net.srv.OutFrames;

import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.Frame;
import bgu.spl.net.srv.StompProtocol;

public class ReceiptFrame extends Frame {
    public ReceiptFrame(int connectionID, boolean isLoggedIn, StompProtocol stompProtocol, Connections connections, String[] message) {
        super(connectionID, isLoggedIn, stompProtocol, connections, message);
    }
    public void process(String receiptID)
    {
        String out="RECEIPT\n" +
                "receipt-id:"+receiptID+"\n" +
                "\u0000";
        this.connections.send(connectionID,out);
    }
}
