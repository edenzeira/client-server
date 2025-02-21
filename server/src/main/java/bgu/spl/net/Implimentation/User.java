package bgu.spl.net.Implimentation;
import java.util.concurrent.ConcurrentHashMap;

public class User {
    private String passcode;
    private boolean isLoggedIn = false;
    private ConcurrentHashMap <Integer, String> subscriptions; //subscriptionId per topic
    

    public User(String passcode){
        this.passcode = passcode;
        subscriptions = new ConcurrentHashMap<>();
    }

    public String getPasscode(){
        return this.passcode;
    }

    public ConcurrentHashMap<Integer, String> getSubscriptions() {
        return this.subscriptions;
    }

    public void subscribe(Integer id, String topic){
        subscriptions.put(id, topic);
    }

    public void logIn(){
        isLoggedIn = true;
    }

    public void logOut(){
        subscriptions.clear();
        isLoggedIn = false;
    }

    public boolean isLoggedIn(){
        return isLoggedIn;
    }

}
