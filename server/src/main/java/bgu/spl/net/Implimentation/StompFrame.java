package bgu.spl.net.Implimentation;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class StompFrame {
    private String command;
    private ConcurrentHashMap<String, String> headers;
    private String body;

    public StompFrame(){}

    public StompFrame(String command, ConcurrentHashMap<String, String> headers, String body) {
        this.command = command;
        this.headers = headers;
        this.body = body;
    }
    
    //construct a string into a stomp frame
    public StompFrame(String msg){
        // Split the message into lines
        String[] lines = msg.split("\n");
        int currentIndex = 0;

        // Extract the command
        this.command = lines[currentIndex].trim();
        currentIndex++;

        // Extract headers
        this.headers = new ConcurrentHashMap();
        while (currentIndex < lines.length && !lines[currentIndex].trim().isEmpty()) {
            String[] keyValue = lines[currentIndex].split(":", 2); // Split into key and value
            if (keyValue.length == 2) {
                headers.put(keyValue[0].trim(), keyValue[1].trim());
            }
            currentIndex++;
        }

        // Extract body, skip blank line
        currentIndex++;
        StringBuilder bodyBuilder = new StringBuilder();
        while (currentIndex < lines.length && !lines[currentIndex].equals("\0")) {
            bodyBuilder.append(lines[currentIndex]).append("\n");
            currentIndex++;
        }

        // Remove trailing newline from the body
        this.body = bodyBuilder.toString().trim();
    }
    
    public String getCommand() {
        return command;
    }

    public Map<String, String> getHeaders() {
        return headers;
    }

    public String getBody() {
        return body;
    }

    public void setCommand(String command) {
        this.command = command;
    }

    public void setHeaders(ConcurrentHashMap<String, String> headers) {
        this.headers = headers;
    }
    
    public void setBody(String body) {
        this.body = body;
    }
    public String toString(){
        String ret = command + "\n";
        for (String s : headers.keySet())
            ret = ret + s + ":" + headers.get(s) + "\n";
        return ret + "\n" + body;
    }

    public StompFrame error(String receiptId , String error , String description){
        String errorString;
        if(receiptId == null) //without receipt id
            errorString = "ERROR" + "\n" + "message:" + error + "\n" + "\n" + "The message:" + "\n" + "-----" + "\n" + this.toString() + "-----";
        else //with receipt id
            errorString = "ERROR" + "\n" + "receipt-id" + receiptId + "\n" + "message:" + error + "\n" + "\n" + "The message:" + "\n" + "-----" + "\n" + this.toString() + "-----";
        
        command = errorString;
        headers.clear();
        body = description;
        
        return this;
    
    }

    public StompFrame receipt(String receipt){
        ConcurrentHashMap<String, String> respondHeaders = new ConcurrentHashMap<>();
        respondHeaders.put("receipt-id", receipt);
        return new StompFrame("RECEIPT", respondHeaders, ""); 
    }
}
