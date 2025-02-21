package bgu.spl.net.impl.stomp;

import java.util.function.Supplier;

import bgu.spl.net.Implimentation.EncoderDecoder;
import bgu.spl.net.Implimentation.StompMessagingProtocolImpl;
import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {
        // Ensure the correct number of arguments is provided
        if (args.length < 2) {
            System.err.println("Usage: <port> <tpc/reactor>");
            return;
        }

        int port;
        try {
            port = Integer.parseInt(args[0]); // Parse the port number
        } catch (NumberFormatException e) {
            System.err.println("Invalid port number. Please provide a valid integer.");
            return;
        }

        Supplier<StompMessagingProtocolImpl> protocolFactory = () -> new StompMessagingProtocolImpl();
        Supplier<EncoderDecoder> encoderDecoderFactory = EncoderDecoder::new;


        String serverType = args[1]; // Get the server type: "tpc" or "reactor"

        switch (serverType.toLowerCase()) {
            case "tpc":
                runThreadPerClientServer(port);
                break;

            case "reactor":
                runReactorServer(port);
                break;

            default:
                System.err.println("Unknown server type: " + serverType);
                System.err.println("Please specify 'tpc' for Thread-Per-Client or 'reactor' for Reactor.");
        }
    }

    private static void runThreadPerClientServer(int port) {
        Server.threadPerClient(
                port, // The port to listen on
                () -> new StompMessagingProtocolImpl(), //protocol factory
            EncoderDecoder::new //encdec factory
            ).serve(); // Start the server
    }

    private static void runReactorServer(int port) {
        Server.reactor(
                Runtime.getRuntime().availableProcessors(), // Number of threads
                port, // The port to listen on
                StompMessagingProtocolImpl::new, // Protocol factory
                EncoderDecoder::new  // Encoder/Decoder factory
        ).serve(); // Start the server
    }
}
