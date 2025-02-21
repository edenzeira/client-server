package bgu.spl.net.Implimentation;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;

import bgu.spl.net.api.MessageEncoderDecoder;

public class EncoderDecoder implements MessageEncoderDecoder<StompFrame> {
     private byte[] bytes = new byte[1 << 10]; //start with 1k
    private int len = 0;

    @Override
    public StompFrame decodeNextByte(byte nextByte) {
        if (nextByte == '\0') {
            return new StompFrame(popString());
        }

        pushByte(nextByte);
        return null; //not a line yet
    }

    @Override
    public byte[] encode(StompFrame message) {
        return (message.toString() + '\u0000').getBytes(); //uses utf8 by default
    }

    private void pushByte(byte nextByte) {
        if (len >= bytes.length) {
            bytes = Arrays.copyOf(bytes, len * 2);
        }

        bytes[len++] = nextByte;
    }

    private String popString() {
        String result = new String(bytes, 0, len, StandardCharsets.UTF_8);
        len = 0;
        return result;
    }
}
