package bgu.spl.net.srv;

public class Frame {
    protected int connectionID;
    protected boolean isLoggedIn;
    protected StompProtocol stompProtocol;
    protected Connections connections;
    protected String[] message;

    public Frame(int connectionID, boolean isLoggedIn, StompProtocol stompProtocol, Connections connections, String[] message)
    {
        this.connectionID=connectionID;
        this.isLoggedIn=isLoggedIn;
        this.stompProtocol=stompProtocol;
        this.connections=connections;
        this.message=message;
    }


}
