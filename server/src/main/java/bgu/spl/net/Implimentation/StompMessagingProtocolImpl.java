package bgu.spl.net.Implimentation;
import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

public class StompMessagingProtocolImpl implements StompMessagingProtocol<StompFrame> {
    private static AtomicInteger messageId = new AtomicInteger(0);
    private Integer connectionId;
    private Connections<StompFrame> connections;
    private boolean shouldTerminate = false;
    private String currentReceipt;
    
    public StompMessagingProtocolImpl() {} 

    @Override
    public void start(int connectionId, Connections<StompFrame> connections) {
        this.connectionId = connectionId;
        this.connections = connections;
        this.currentReceipt = "";
    }

    public int getConnectionId(){
        return this.connectionId;
    }

    @Override
    public StompFrame process(StompFrame frame) {
        switch (frame.getCommand()) {
            case "CONNECT":
                return handleConnect(frame);
            case "SEND":
                return handleSend(frame);
            case "SUBSCRIBE":
                return handleSubscribe(frame);
            case "UNSUBSCRIBE":
                return handleUnsubscribe(frame);
            case "DISCONNECT":
                return handleDisconnect(frame);
            default : return null;
        }
    }

    public StompFrame handleConnect(StompFrame frame){
        String userName = frame.getHeaders().get("login");
        String passcode = frame.getHeaders().get("passcode");
        
        //if new user
        if(connections.getUser(userName) == null){
            connections.connectUser(connectionId, userName, passcode); 
            //returns connected respond
            ConcurrentHashMap<String ,String> headers = new ConcurrentHashMap<>();
            headers.put("version", frame.getHeaders().get("accept-version"));
            return new StompFrame("CONNECTED", headers , "");
        }

        //user is already logged in
        if(connections.getUser(userName).isLoggedIn()){
            shouldTerminate = true;
            return frame.error(null, "log in error", "User is already logged in");
        }

        //the user exist with another password
        if(connections.getUser(userName) != null && !connections.getUser(userName).getPasscode().equals(passcode)){ 
                shouldTerminate = true;
                return frame.error(null, "log in error", "Wrong password");
        }
    
        //if there is no errors and user exists
        else{
            connections.connectUser(connectionId, userName, passcode); 
            //returns connected respond
            ConcurrentHashMap<String ,String> headers = new ConcurrentHashMap<>();
            headers.put("version", frame.getHeaders().get("accept-version"));
            return new StompFrame("CONNECTED", headers , "");
        }
        
    }

    public StompFrame handleSend(StompFrame frame){
        String topic = frame.getHeaders().get("destination");
        String body = frame.getBody();

        if (!connections.getConnectedClients().containsKey(connectionId)){
            connections.disconnect(connectionId);
            shouldTerminate = true;
            return frame.error(frame.getHeaders().get("receipt"), "login error", "client is not logged in");
        }


        //user not subscribed to the topic
        else if(connections.isSubscribed(connectionId, topic) == false){
            connections.disconnect(connectionId);
            shouldTerminate = true;
            return frame.error(frame.getHeaders().get("receipt"), "subscription error", "user does not subscribed to the channel. first subscribe and then send a message");
        }
        
        //if there is no errors
        else{
            //send the message to all subscribers
            for (Integer channelId : connections.getSubscribers(topic)){
                if (channelId != connectionId){
                    ConcurrentHashMap<String, String> headers = new ConcurrentHashMap<>();
                    String username = connections.getConnectedClients().get(channelId);
                    User user = connections.getUser(username);
                    for(Integer subscriptionId : user.getSubscriptions().keySet()){
                        String channel = user.getSubscriptions().get(subscriptionId);
                        if(channel.equals(topic)){
                            headers.put("subscription", String.valueOf(subscriptionId));
                            headers.put("message-id" , messageId.getAndIncrement() + "");
                            headers.put("destination", topic);
                            StompFrame message = new StompFrame("MESSAGE", headers, body);
                            connections.send(channelId, message);
                        }  
                    }
                }
            } 
        }  
            if(frame.getHeaders().containsKey("receipt")){
                currentReceipt = frame.getHeaders().get("receipt");
            }
            //send receipt
            return frame.receipt(currentReceipt); 
        }
    

    public StompFrame handleSubscribe(StompFrame frame){
        String topic = frame.getHeaders().get("destination");
        String id = frame.getHeaders().get("id");
        
        if (!connections.getConnectedClients().containsKey(connectionId)){
            connections.disconnect(connectionId);
            shouldTerminate = true;
            return frame.error(frame.getHeaders().get("receipt"), "login error", "User is not logged in");
        }

        if (connections.hasChannel(topic) && connections.isSubscribed(connectionId, topic)){
            connections.disconnect(connectionId);
            shouldTerminate = true;
            return frame.error(frame.getHeaders().get("receipt"), "subscription error", "User is already subscribe to this channel");
        }

        else {
            connections.subscribeToTopic(connectionId, topic);
            String username = connections.getConnectedClients().get(connectionId);
            User user = connections.getUser(username);
            user.getSubscriptions().put(Integer.parseInt(id), topic);
            //send receipt
            if(frame.getHeaders().containsKey("receipt")){
                currentReceipt = frame.getHeaders().get("receipt");
            }
            return frame.receipt(currentReceipt); 
        }
    }

    public StompFrame handleUnsubscribe(StompFrame frame){
        String id = frame.getHeaders().get("id");
        int subId = Integer.parseInt(id);
        String username = connections.getConnectedClients().get(connectionId);
        User user = connections.getUser(username);

        if (!connections.getConnectedClients().containsKey(connectionId)){
            shouldTerminate = true;
            return frame.error(frame.getHeaders().get("receipt"), "login error", "client is not logged in");
        }

        //user is not subscribed with the id
        if (!user.getSubscriptions().containsKey(subId)){
            connections.disconnect(connectionId);
            shouldTerminate = true;
            return frame.error(frame.getHeaders().get("receipt"), "subscription error", "user is not subscribed with this id to any topic");
        }

        else{
           String topic = user.getSubscriptions().get(subId);
           user.getSubscriptions().remove(subId);
           connections.unsubscribe(connectionId, topic);
           //send receipt
           if(frame.getHeaders().containsKey("receipt")){
            currentReceipt = frame.getHeaders().get("receipt");
        }
        
           return frame.receipt(currentReceipt);
        }
    }

    public StompFrame handleDisconnect(StompFrame frame){

        if (!connections.getConnectedClients().containsKey(connectionId)){
            shouldTerminate = true;
            return frame.error(frame.getHeaders().get("receipt"), "login error", "user is already logout");
        }

        else {
            shouldTerminate = true; 
            connections.disconnect(connectionId);
            //send receipt
            if(frame.getHeaders().containsKey("receipt")){
                currentReceipt = frame.getHeaders().get("receipt");
            }
            
           return frame.receipt(currentReceipt);
        }

    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }
}