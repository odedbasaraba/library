package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.Server;
import bgu.spl.net.srv.StompMessageEncoderDecoder;
import bgu.spl.net.srv.StompProtocol;

public class StompServer {

    public static void main(String[] args) {
        int numerOfThreadsForReactor=2;

        int port = Integer.parseInt(args[0]);
        String srvName=args[1];
        if(srvName.equals("tpc")) {
            Server.threadPerClient(
                    port, //port
                    () -> new StompProtocol(), //protocol factory
                    StompMessageEncoderDecoder::new //message encoder decoder factory
            ).serve();
        }

        if(srvName.equals("reactor"))
        {
            Server.reactor(
                    numerOfThreadsForReactor,
                    port, //port
                    () -> new StompProtocol(), //protocol factory
                    StompMessageEncoderDecoder::new //message encoder decoder factory
            ).serve();
        }


    }

}
