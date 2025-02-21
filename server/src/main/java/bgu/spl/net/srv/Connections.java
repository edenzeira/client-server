package bgu.spl.net.srv;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentHashMap;

import bgu.spl.net.Implimentation.User;

public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(Integer connectionId);

    //we added
    void connectUser(int connectionId, String userName, String passcode);

    public User getUser(String userName);

    public boolean isSubscribed(int connectionId, String topic);

    public ConcurrentHashMap<Integer,String> getConnectedClients();
    
    public void subscribeToTopic(int id, String topic);

    public boolean isSubscriberExists(String topic, int id);
    
    public boolean hasChannel(String topic);

    public BlockingQueue<Integer> getSubscribers(String topic);

    public void unsubscribe(Integer id, String topic);
}
