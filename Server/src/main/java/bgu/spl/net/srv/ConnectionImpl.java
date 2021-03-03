package bgu.spl.net.srv;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

public class ConnectionImpl<T> implements Connections<T> {
    //TODO: check if synch is needed everywhere
    private static ConnectionImpl instance;
    ConcurrentHashMap<Integer, User> ConnectionToUser;
    ConcurrentHashMap<String, User> loginUserNameToUserObject;
    ConcurrentHashMap<String,List<Integer>>ChannelNameToListOfSubscribers;
    private AtomicInteger idCounter;
    ConcurrentHashMap<Integer, User> pendingNewUsers;
    private AtomicInteger globalMessageId;


    private ConnectionImpl()
    {
        ConnectionToUser = new ConcurrentHashMap<>();
        ChannelNameToListOfSubscribers= new ConcurrentHashMap<>();
        idCounter= new AtomicInteger(0);
        globalMessageId= new AtomicInteger(0);
        loginUserNameToUserObject= new ConcurrentHashMap<>();
        pendingNewUsers= new ConcurrentHashMap<>();

    }

    public static Connections getInstance() {
        if(instance==null)
        {
            instance= new ConnectionImpl();
            return instance;
        }
        else
        {
            return instance;
        }
    }

    @java.lang.Override
    public synchronized boolean send(int connectionId, T msg) {
        if(ConnectionToUser.containsKey(connectionId)) {
            ConnectionToUser.get(connectionId).getMyConnectionHandler().send(msg);
            return true;
        }
        else if(this.pendingNewUsers.containsKey(connectionId))
        {
            pendingNewUsers.get(connectionId).getMyConnectionHandler().send(msg);
            return true;
        }
        else
        {
            return false;
        }
    }

    @java.lang.Override
    public synchronized void send(String channel, T msg) {
        //the only message that could be sent by this function is MESSAGE kind
        String msgStr=(String)msg;
        String newStr1= msgStr.substring(0,msgStr.indexOf("subscription:")+"subscription:".length());
        String newStr2= msgStr.substring(msgStr.indexOf("subscription:")+"subscription:".length()+1);
        for (Integer idConnetion: this.ChannelNameToListOfSubscribers.get(channel)) {
            if(ConnectionToUser.containsKey(idConnetion)) {
                String strToSend= newStr1+ConnectionToUser.get(idConnetion).getMySubscription().get(channel)+"\n"+newStr2;//The middle part returns the subscriptionID for this channel of each user
                this.ConnectionToUser.get(idConnetion).getMyConnectionHandler().send(strToSend);
            }
        }
    }

    @java.lang.Override
    public synchronized void disconnect(int connectionId) {
        User userToChange=this.ConnectionToUser.get(connectionId);

        for (String channel:userToChange.getMySubscription().keySet()) {
            this.ChannelNameToListOfSubscribers.get(channel).remove(userToChange.getConectionId());
        }
        userToChange.isLoggedIn=false;
        //this.loginUserNameToUserObject.remove(userToChange.getLogin());
        //ConnectionToUser.remove(connectionId);


    }

    public synchronized void addClient(int connectionID, ConnectionHandler connectionHandler)
    {
        User newUser= new User(connectionID,connectionHandler);
        pendingNewUsers.put(connectionID,newUser);
    }



    public synchronized void SubscribeClient(int connectionID, int clientsChannelID, String channelName)
    {
        if(this.ChannelNameToListOfSubscribers.containsKey(channelName))
        {
            this.ChannelNameToListOfSubscribers.get(channelName).add(connectionID);

        }
        else
        {
            List <Integer> newLst= new ArrayList<>();
            newLst.add(connectionID);
            this.ChannelNameToListOfSubscribers.put(channelName,newLst);
        }

        User userToSubscribe=this.ConnectionToUser.get(connectionID);
        if(!userToSubscribe.getMySubscription().containsKey(channelName)) {
            userToSubscribe.Subscribe(channelName, clientsChannelID);
        }

    }

    @Override
    public synchronized void UnSubscribeClient(int connectionID, int clientsChannelID) {
        User userToUnsubscribe=this.ConnectionToUser.get(connectionID);
        String channelName=userToUnsubscribe.getMySubscriptionIDs().get(clientsChannelID);
        int idx=this.ChannelNameToListOfSubscribers.get(channelName).indexOf(userToUnsubscribe.getConectionId());
        this.ChannelNameToListOfSubscribers.get(channelName).remove(idx);
        userToUnsubscribe.UnSubscribe(clientsChannelID,channelName);
    }

    public synchronized boolean checkIfLoggedIn(int connectionID)
    {
        return this.ConnectionToUser.containsKey(connectionID);
    }

    public synchronized String checkIfLoginIsValid(String userName, String passcode)
    {
        if(this.loginUserNameToUserObject.containsKey(userName))
        {
            if(loginUserNameToUserObject.get(userName).checkIfLoggedIn())
            {
                return "User already logged in";
            }
            else if(!loginUserNameToUserObject.get(userName).getPasscode().equals(passcode))
            {
                return "Wrong password";
            }
            else
            {
                return "contains user but it is not logged in and passcode is ok";
            }

        }
        else
        {
            return "doesn't conatin user- new user";
        }
    }

    @Override
    public synchronized int getIdConnectionAndIncrement() {
        return idCounter.getAndIncrement();
    }

    public synchronized int getGlobalMessageId() {
        return globalMessageId.getAndIncrement();
    }

    public synchronized User getUser(String userLoingName)
    {
        return this.loginUserNameToUserObject.get(userLoingName);
    }

    public synchronized User getPendingUser(Integer userID)
    {
        if(this.pendingNewUsers.containsKey(userID))
        {
            return this.pendingNewUsers.get(userID);
        }
        else
        {
            return null;
        }
    }

    public synchronized void addClientToBase(Integer connectionID)
    {
        User user=this.pendingNewUsers.get(connectionID);
        this.pendingNewUsers.remove(connectionID);
        this.ConnectionToUser.put(connectionID,user);
        this.loginUserNameToUserObject.put(user.getLogin(),user);
    }
    public synchronized void removePendingUser(Integer connectionId)
    {
        this.pendingNewUsers.remove(connectionId);
    }
}
