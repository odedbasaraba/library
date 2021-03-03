package bgu.spl.net.srv;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.InFrames.*;

import java.util.concurrent.atomic.AtomicInteger;

public class StompProtocol implements StompMessagingProtocol<String> {

    private boolean shoudlTerminate;
    private int connectionID;
    public static AtomicInteger nextID= new AtomicInteger(0);
    private Connections connections;
    private boolean isLoggedIn;


    @Override
    public void start(int connectionId, Connections<String> connections) {
        shoudlTerminate =false;
        this.connectionID=connectionId;
        this.connections=connections;
        isLoggedIn=false;
    }

    @Override
    public void process(String message) {
        String[] messageSplit= message.split("\n");

        if (messageSplit[0].equals("CONNECT"))
        {

                ConnectFrame connectFrame= new ConnectFrame(connectionID,isLoggedIn,this,this.connections,messageSplit);
                connectFrame.proccess();
        }
        else if (messageSplit[0].equals("DISCONNECT"))
        {
                DisconnectFrame disconnectFrame = new DisconnectFrame(connectionID,isLoggedIn,this,this.connections,messageSplit);
                disconnectFrame.process();
        }
        else if (messageSplit[0].equals("SEND"))
            {
            SendFrame sendFrame= new SendFrame(connectionID,isLoggedIn,this,this.connections,messageSplit);
            sendFrame.process();
        }
        else if (messageSplit[0].equals("SUBSCRIBE"))
        {
            SubscribeFrame subscribeFrame= new SubscribeFrame(connectionID,isLoggedIn,this,this.connections,messageSplit);
            subscribeFrame.process();
        }
        else if (messageSplit[0].equals("UNSUBSCRIBE"))
        {
            UnsubscribeFrame unsubscribeFrame= new UnsubscribeFrame(connectionID,isLoggedIn,this,this.connections,messageSplit);
            unsubscribeFrame.process();
        }
        else
        {
                //Error

        }

    }

    @Override
    public boolean shouldTerminate() {
        return shoudlTerminate;
    }

    public void setShoudlTerminate(boolean shoudlTerminate) {
        this.shoudlTerminate = shoudlTerminate;
    }// leads to termination

    public void setLoggedIn(boolean loggedIn) {
        isLoggedIn = loggedIn;
    }

    public boolean isLoggedIn() {
        return isLoggedIn;
    }
}
