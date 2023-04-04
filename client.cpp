#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_LEN_MSG 256

bool connection_succsess(const int& client_fd);

int main(int argc, char* argv[]) {
    setlocale(LC_ALL,"ru");

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server_address> <port>" << std::endl;
        return 1;
    }

    //get IP and port from argv[]   
    const char *server_address_str = argv[1];
    const int port = std::stoi(argv[2]);

    //create socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    //connect to server
    struct sockaddr_in server_address {};
    server_address.sin_addr.s_addr = inet_addr(server_address_str);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if (connect(client_fd, reinterpret_cast<sockaddr*>(&server_address),sizeof(server_address)) == -1 || !connection_succsess(client_fd)) {
        std::cerr << "Failed to connect to server at ip:" << server_address_str << std::endl;
        close(client_fd);
        return 1;
    }



    //communicate with server until the client close quit
    while (true)
    {
        std::cout << "Enter the message to server (or 'q' to quit)" << std::endl;

        std::string msg;
        std::getline(std::cin, msg);

        //chek if quit
        if (msg == "q") {
            break;
        }
        else if (msg.empty() || msg.size()>MAX_LEN_MSG) {
            std::cerr << "Bad request. Message too long or emty" << std::endl;
            continue;
        }
        //send message to server
        int bytes_sent = send(client_fd,msg.c_str(), msg.length(),0);
        if (bytes_sent == -1) {
            std::cerr << "Failed to sent message to server" << std::endl;
            close(client_fd);
            return 1;
        }
        usleep(100000);


        //receive msg from server;
        char buf[MAX_LEN_MSG];
        int bytes_received = recv(client_fd, buf, sizeof(buf),0);
        if (bytes_received == -1) {
            std::cerr << "Failed to receive message from server" << std::endl;
            close(client_fd);
            return 1;
        }
        //print msg from server
        buf[bytes_received] = '\0';
        std::cout << "Server: " << buf << std::endl;

    }
    


close(client_fd);
return 0;
}


bool connection_succsess(const int& client_fd) {
    char buf[16];
    int bytes_received = recv(client_fd, buf, sizeof(buf),0);
    if (bytes_received == -1) {
        std::cerr << "Failed to receive message from server" << std::endl;
        close(client_fd);
        return 1;
    }

    const char errbuf[16] = "E:TMC";
    if (!strcmp(buf,errbuf)) {
        std::cerr << "Too many users. Server closed the connection" << std::endl;
        return false;
    }
    std::cout << "Connection succsessful!" << std::endl;
    return true;
}