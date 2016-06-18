
import java.rmi.registry.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

public class Client {

    public static void main(String... args) throws Exception {
        Registry registry = LocateRegistry.getRegistry("localhost", 3456);
        IServer service = (IServer) registry.lookup("chat");
        System.out.print("Подключение...");
        service.connect();
        Scanner sc = new Scanner(System.in);
        System.out.println(" OK");
        while (true) {
            System.out.println("Выберите действие: ");
            System.out.println("1. Проверить сообщения");
            System.out.println("2. Написать сообщение");
            Integer s = sc.nextInt();
            String message;
            switch (s) {
                case 1:
                message = service.getMessage();
                    while (message != null) {
                        System.out.println(message);
                        message = service.getMessage();
                    }
                    break;
                case 2:
                    System.out.println("Введите сообщение: ");
                    sc.nextLine();
                    message = sc.nextLine();
                    service.setMessage(message);
                    break;
            }
        }

    }

}