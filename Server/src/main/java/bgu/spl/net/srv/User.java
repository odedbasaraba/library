package bgu.spl.net.srv;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentHashMap;

public class User {
    private int conectionId;
    private String login;
    private String passcode;
    private ConnectionHandler myConnectionHandler;
    private ConcurrentHashMap <String, Integer> mySubscription;
    private ConcurrentHashMap <Integer, String> mySubscriptionIDs;
    boolean isLoggedIn=false;


    public User(int connctionId,ConnectionHandler connectionHandler)
    {
        this.conectionId=connctionId;
        this.login=null;
        this.passcode=null;
        this.myConnectionHandler=connectionHandler;
        mySubscription= new ConcurrentHashMap<>();
        mySubscriptionIDs= new ConcurrentHashMap<>();
    }

    public ConnectionHandler getMyConnectionHandler() {
        return myConnectionHandler;
    }

    public int getConectionId() {
        return conectionId;
    }

    public String getLogin() {
        return login;
    }

    public String getPasscode() {
        return passcode;
    }

    public ConcurrentHashMap<String, Integer> getMySubscription() {
        return mySubscription;
    }

    public ConcurrentHashMap<Integer, String> getMySubscriptionIDs() {
        return mySubscriptionIDs;
    }

    public void setLogin(String login) {
        this.login = login;
    }

    public void setPasscode(String passcode) {
        this.passcode = passcode;
    }

    public void Subscribe(String channelName, int channelId)
    {
        this.mySubscription.put(channelName,channelId);
        this.mySubscriptionIDs.put(channelId,channelName);
    }

    public void UnSubscribe(int channelId, String channelName)
    {
        this.mySubscription.remove(channelName);
        this.mySubscriptionIDs.remove(channelId);
    }

    public boolean checkIfLoggedIn()
    {
        return this.isLoggedIn;
    }

    public void setLoggedIn(boolean loggedIn) {
        isLoggedIn = loggedIn;
    }

    public void setMyConnectionHandler(ConnectionHandler myConnectionHandler) {
        this.myConnectionHandler = myConnectionHandler;
    }
}
