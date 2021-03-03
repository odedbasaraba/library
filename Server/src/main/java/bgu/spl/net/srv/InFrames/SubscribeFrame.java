package bgu.spl.net.srv.InFrames;

import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.Frame;
import bgu.spl.net.srv.OutFrames.ReceiptFrame;
import bgu.spl.net.srv.StompProtocol;


public class SubscribeFrame extends Frame {
    private String destChannel;
    private String SubscriptionID;
    private String receiptID;
    public SubscribeFrame(int connectionID, boolean isLoggedIn, StompProtocol stompProtocol, Connections connections, String[] message) {
        super(connectionID, isLoggedIn, stompProtocol, connections, message);
        destChannel=message[1].substring("destination:".length());
        SubscriptionID=message[2].substring("id:".length());
        receiptID=message[3].substring("receipt:".length());
    }

    public void process() {
        int subInt=Integer.parseInt(SubscriptionID);
        connections.SubscribeClient(connectionID, Integer.parseInt(SubscriptionID),destChannel);
        ReceiptFrame receiptFrame = new ReceiptFrame(this.connectionID,this.isLoggedIn,this.stompProtocol,this.connections,this.message);
        receiptFrame.process(receiptID);
    }
}
