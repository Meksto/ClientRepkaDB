#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#define MAX_LEN_MSG 256

// spdlog init
auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/log.txt", true);
//auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>("client", spdlog::sinks_init_list({file_sink}));

bool connection_succsess(const int& client_fd);

int main(int argc, char* argv[]) {
    //log config
    logger->set_level(spdlog::level::debug);
    logger->flush_on(spdlog::level::debug);


    if (argc != 3) {
        spdlog::warn("Usage: {0} <server-address> <port>", argv[0]);
        return 1;
    }

    //get IP and port from argv[]   
    const char *server_address_str = argv[1];
    const int port = std::stoi(argv[2]);
    logger->info("===== CLIENT LAUNCH =====");
    logger->info("server-ip: {}", argv[1]);
    logger->info("server port: {}", argv[2]);

    //create socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        logger->critical("Failed to create socket");
        return 1;
    }

    //connect to server
    struct sockaddr_in server_address {};
    server_address.sin_addr.s_addr = inet_addr(server_address_str);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if (connect(client_fd, reinterpret_cast<sockaddr*>(&server_address),sizeof(server_address)) == -1 || !connection_succsess(client_fd)) {
        //std::cerr << "Failed to connect to server at ip:" << server_address_str << std::endl;
        logger->error("Failed to connect to server at ip: {}", server_address_str);
        spdlog::error("Failed to connect to server at ip: {}", server_address_str);
        close(client_fd);
        return 1;
    }


    //communicate with server until the client close quit
    while (true)
    {
        std::cout << "Enter the message to server (or 'q' to quit)" << std::endl << std::endl;

        std::string msg;
        std::getline(std::cin, msg);

        //chek if quit
        if (msg == "q") {
            break;
        }
        else if (msg.empty() || msg.size()>MAX_LEN_MSG) {
            //std::cerr << "Bad request. Message too long or emty" << std::endl;
            logger->warn("Bad request. Message too big or empty: {}", msg);
            continue;
        }
        //send message to server
        int bytes_sent = send(client_fd,msg.c_str(), msg.length(),0);
        if (bytes_sent == -1) {
            //std::cerr << "Failed to sent message to server" << std::endl;
            logger->critical("Failed to sent message to server");
            close(client_fd);
            return 1;
        }
        usleep(100000);


        //receive msg from server;
        char buf[MAX_LEN_MSG];
        int bytes_received = recv(client_fd, buf, sizeof(buf),0);
        if (bytes_received == -1) {
            //std::cerr << "Failed to receive message from server" << std::endl;
            logger->critical("Failed to receive message from server");
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
        //std::cerr << "Failed to receive message from server" << std::endl;
        logger->critical("Failed to receive message from server");
        close(client_fd);
        return 1;
    }

    const char errbuf[16] = "E:TMC";
    if (!strcmp(buf,errbuf)) {
        //std::cerr << "Too many users. Server closed the connection" << std::endl;
        logger->error("Too many users. Server closed the connection");
        spdlog::warn("Too many users. Server closed the connection");
        return false;
    }
    //std::cout << "Connection succsessful!" << std::endl;
    logger->info("Connection succsessful!");
    return true;
}