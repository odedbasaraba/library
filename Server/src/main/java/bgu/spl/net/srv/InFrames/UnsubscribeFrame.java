package bgu.spl.net.srv.InFrames;

import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.Frame;
import bgu.spl.net.srv.OutFrames.ReceiptFrame;
import bgu.spl.net.srv.StompProtocol;

public class UnsubscribeFrame extends Frame {
    private String SubscriptionID;
    private String receiptID;
    public UnsubscribeFrame(int connectionID, boolean isLoggedIn, StompProtocol stompProtocol, Connections connections, String[] message) {
        super(connectionID, isLoggedIn, stompProtocol, connections, message);
        SubscriptionID=message[1].substring("id:".length());
        receiptID=message[2].substring("receipt:".length());
    }

    public void process() {
        int subInt=Integer.parseInt(SubscriptionID);
        connections.UnSubscribeClient(connectionID, Integer.parseInt(SubscriptionID));
        ReceiptFrame receiptFrame = new ReceiptFrame(this.connectionID,this.isLoggedIn,this.stompProtocol,this.connections,this.message);
        receiptFrame.process(receiptID);
    }
}
