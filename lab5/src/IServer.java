
import java.rmi.*;

public interface IServer extends Remote {

    String getMessage() throws RemoteException;
    void  setMessage(String s) throws RemoteException;
    Boolean connect() throws RemoteException;


}