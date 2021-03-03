package bgu.spl.net.srv;

import java.io.IOException;

public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(int connectionId);

    public void addClient(int connectionID, ConnectionHandler connectionHandler);


    public void SubscribeClient(int connectionID, int clientsChannelID, String channelName);
    public void UnSubscribeClient(int connectionID, int clientsChannelID);


    public boolean checkIfLoggedIn(int connectionID);

    public int getIdConnectionAndIncrement();
    public User getUser(String userNameLogin);

    public int getGlobalMessageId();

    public String checkIfLoginIsValid(String userName, String passcode);
    public User getPendingUser(Integer userID);
    public void addClientToBase(Integer connectionID);
    public void removePendingUser(Integer connectionId);
}
