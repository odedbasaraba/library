package bgu.spl.net.srv.InFrames;

import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.Frame;
import bgu.spl.net.srv.OutFrames.ReceiptFrame;
import bgu.spl.net.srv.StompProtocol;

public class DisconnectFrame extends Frame {
    private String receiptID;
    public DisconnectFrame(int connectionID, boolean isLoggedIn, StompProtocol stompProtocol, Connections connections, String[] message) {
        super(connectionID, isLoggedIn, stompProtocol, connections, message);
        receiptID=message[1].substring("receipt:".length());
    }

    public void process() {



        ReceiptFrame receiptFrame = new ReceiptFrame(this.connectionID,this.isLoggedIn,this.stompProtocol,this.connections,this.message);
        receiptFrame.process(receiptID);


        connections.disconnect(connectionID);

        this.stompProtocol.setLoggedIn(false);
        this.stompProtocol.setShoudlTerminate(true);



    }
}
