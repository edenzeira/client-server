package bgu.spl.net.Implimentation;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.LinkedBlockingQueue;

import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;

public class ConnectionsImpl<T> implements Connections<T> {
    private ConcurrentHashMap<String, User> users; //username per User
    private ConcurrentHashMap<Integer, String> connectedClients; //connectionId per username
    private ConcurrentHashMap<Integer, ConnectionHandler<T>> handlers; //connectionId per handler
    private ConcurrentHashMap<String, BlockingQueue<Integer>> topics; //topic per list of subscribers by connectionId
    private static class ConnectionsImplHolder{
        private static ConnectionsImpl instance = new ConnectionsImpl<>();
    }

    private ConnectionsImpl() {
        connectedClients = new ConcurrentHashMap<>();
        users = new ConcurrentHashMap<>();
        topics = new ConcurrentHashMap<>();
        handlers = new ConcurrentHashMap<>();
    }

    public static ConnectionsImpl getInstance(){
        return ConnectionsImplHolder.instance;
    }

    @Override
    public boolean send(int connectionId, T msg) {
        ConnectionHandler<T> handler = handlers.get(connectionId);
        if (handler != null) {
            handler.send(msg);
            return true;
        }
        return false; // connectionID does not exist
    }

    @Override
    public void send(String channel, T msg) {
        BlockingQueue<Integer> subscribers = topics.get(channel);
        if (subscribers != null && !subscribers.isEmpty()) {
            for (int connectionId : subscribers) {
                send(connectionId, msg);
            }
        }
    }

    @Override
    public void disconnect(Integer connectionId) {
        // Remove the client from all topics
        ConcurrentHashMap <Integer, String> subscriptions = users.get(connectedClients.get(connectionId)).getSubscriptions();
        for (String topic : subscriptions.values()){
            topics.get(topic).remove((Integer) connectionId);
        }
        //logout the user
        users.get(connectedClients.get(connectionId)).logOut();
        connectedClients.remove(connectionId);

        handlers.remove(connectionId);
    }

    public void addConnection(int connectionId, ConnectionHandler<T> handler){ //used by the server
        handlers.put(connectionId, handler);
    }

    public User getUser(String userName){
        return users.get(userName);
    }

    public void connectUser(int connectionId, String userName, String passcode){
        users.putIfAbsent(userName, new User(passcode));
        users.get(userName).logIn();
        connectedClients.putIfAbsent(connectionId, userName);

    }

    public boolean isSubscribed(int connectionId, String topic){
        if(topics.containsKey(topic))
            return topics.get(topic).contains(connectionId);
        return false;
    }

    public ConcurrentHashMap<String,User> getUsers() {
        return this.users;
    }


    public ConcurrentHashMap<Integer,String> getConnectedClients() {
        return this.connectedClients;
    }


    //Subscribe a client to a topic
    public void subscribeToTopic(int id, String topic) {
        topics.computeIfAbsent(topic, k -> new LinkedBlockingQueue<>()).add(id);
    }

    public boolean isSubscriberExists(String topic, int id){
        return topics.get(topic).contains(id);
    }

    @Override
    public boolean hasChannel(String topic) {
        return topics.containsKey(topic);
    }
    public BlockingQueue<Integer> getSubscribers(String topic){
        return topics.get(topic);
    }

    public void unsubscribe(Integer id, String topic){   
        topics.get(topic).remove(id);
    }
}
