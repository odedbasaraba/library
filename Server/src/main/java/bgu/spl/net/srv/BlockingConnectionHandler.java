package bgu.spl.net.srv;

import bgu.spl.net.api.MessageEncoderDecoder;
import bgu.spl.net.api.MessagingProtocol;
import bgu.spl.net.api.StompMessagingProtocol;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.sql.Connection;

public class BlockingConnectionHandler<T> implements Runnable, ConnectionHandler<T> {

    private final StompMessagingProtocol<T> protocol;
    private final MessageEncoderDecoder<T> encdec;
    private final Socket sock;
    private BufferedInputStream in;
    private BufferedOutputStream out;
    private volatile boolean connected = true;
    private Connections connections;
    private int conncetionID;

    public BlockingConnectionHandler(Socket sock, MessageEncoderDecoder<T> reader, StompMessagingProtocol<T> protocol,
                                     Connections connection, int connectionID) {
        this.sock = sock;
        this.encdec = reader;
        this.protocol = protocol;
        this.connections=connection;
        this.conncetionID=connectionID;

    }

    @Override
    public void run() {
        protocol.start(conncetionID,connections);
        try (Socket sock = this.sock) { //just for automatic closing
            int read;

            in = new BufferedInputStream(sock.getInputStream());
            out = new BufferedOutputStream(sock.getOutputStream());
            //System.out.println("waiting for messages");

            //read=in.read();
            while (!protocol.shouldTerminate() && connected && (read = in.read()) >= 0) {
                //System.out.println("waiting for messages");
                String nextMessage =(String) encdec.decodeNextByte((byte) read);

                if (nextMessage != null) {
                    System.out.println("-----------Income-Frame-----------");
                    System.out.println(nextMessage);
                    System.out.println("-----------Income-Frame-End-----------");
//                    if(nextMessage.contains("DISCONNECT"))
//                    {
//                        System.out.println("disconnect");
//                    }
                    protocol.process(nextMessage);
                    //if (response != null) {
                    //    out.write(encdec.encode(response));
                    //    out.flush();
                    //}
                }
            }

        } catch (IOException ex) {
            ex.printStackTrace();
        }
//do not forgrt to log out after finishing your work ssession...
//someone less nice then me could doo a lot of bad things... lol
    }

    @Override
    public void close() throws IOException {
        connected = false;
        sock.close();
    }

    @Override
    public void send(T msg) {
        try {
            out.write(encdec.encode(msg));
            out.flush();
            System.out.println("$$$$$  Sent message:   $$$$$\n"+msg+"\n$$$$$$$$$");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    public void send(String msg) {
        try {
            out.write(encdec.encode((T)msg));
            out.flush();
            System.out.println("$$$$$  Sent message:\n"+msg+"\n$$$$$");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
