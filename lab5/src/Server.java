
import java.io.File;
import java.rmi.*;
import java.rmi.registry.*;
import java.rmi.server.*;
import java.util.*;

public class Server implements IServer {
    public static final String BINDING_NAME = "chat";
    public static final int PORT = 3456;

    private static Hashtable<String, Integer> clients;
    private  static List<String> messages;

    public String getMessage() {
        try {
            if (messages.size() - 1 > clients.get(UnicastRemoteObject.getClientHost())) {
            clients.put(UnicastRemoteObject.getClientHost(), clients.get(UnicastRemoteObject.getClientHost()) + 1);
            return messages.get(clients.get(UnicastRemoteObject.getClientHost()));
        }
        } catch (ServerNotActiveException e) {
            e.printStackTrace();
        }
        return null;
    }

    public void setMessage(String s) {
        try {
            System.out.println(UnicastRemoteObject.getClientHost()+ ": " + s);
            messages.add(UnicastRemoteObject.getClientHost()+ ": " + s);
        } catch (ServerNotActiveException e) {
            e.printStackTrace();
        }
    }

    public Boolean connect() {
        try {
                clients.put(UnicastRemoteObject.getClientHost(), messages.size()-1);

                return true;
        } catch (ServerNotActiveException e) {
            e.printStackTrace();
        }
        return false;
    }

    public static void main(String... args) throws Exception {
        System.out.print("1...");
        final Registry registry = LocateRegistry.createRegistry(PORT);
        final IServer service = new Server();
        Remote stub = UnicastRemoteObject.exportObject(service, 0);
        System.out.print("2...");
        registry.bind(BINDING_NAME, stub);
        System.out.print("3...");
        clients = new Hashtable<String, Integer>();
        messages = new ArrayList<String>();
        System.out.println(" OK");
        while (true) {
            Thread.sleep(Integer.MAX_VALUE);
        }
    }
}