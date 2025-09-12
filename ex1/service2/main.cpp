#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <chrono>
#include <sstream>
#include <iomanip>

// for measuring process uptime 
const auto program_start_time = std::chrono::steady_clock::now();

#define PORT 9191
#define BUFFER_SIZE 256

void error(const char *msg){
  perror(msg);
  exit(1);
}

std::string get_timestamp() {
  const auto now = std::chrono::system_clock::now();
  auto itt = std::chrono::system_clock::to_time_t(now); // for C time functions

  std::ostringstream ss; // output stream
  ss << std::put_time(gmtime(&itt), "%FT%TZ"); // convert to UTC and format
  return ss.str();
}

std::string get_uptime() {
  auto now = std::chrono::steady_clock::now();
  auto elapsed = now - program_start_time;
  double uptime =  std::chrono::duration<double>(elapsed).count() / 3600.0;

  std::ostringstream ss;
  ss << std::fixed << std::setprecision(1) << uptime;
  return ss.str();
}

std::string build_response() {
  std::ostringstream ss;

  ss << get_timestamp()
     << ": uptime " << get_uptime()
     << " hours. free disk in root: <X> MBytes\n";
  
  return ss.str();
}

int main() {
  std::cout << "Hello from C++ container! Service2!" << std::endl;

  
  int server_socket, client_socket, portno;
  socklen_t clilen;
  char buffer[BUFFER_SIZE];
  struct sockaddr_in serv_addr, cli_addr;
  int n;
  
  // Create socket
  server_socket = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
  if (server_socket < 0){
    error("Error: Can't open socket!");
  }

  // Prepare the server address
  
  bzero((char *) &serv_addr, sizeof(serv_addr)); // clear address struct
  serv_addr.sin_family = AF_INET; // server byte order
  serv_addr.sin_addr.s_addr = INADDR_ANY; // fill current host addr
  serv_addr.sin_port = htons(PORT); //  convert short int to network

  // Bind the socket to the address
  if (bind(server_socket, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
      error("Error: can't bind");
      return 1;
  }

  // Listen for incoming connections
  if (listen(server_socket, 5) == -1) {
      error("Error: can't listen");
      return 1;
  }

  std::cout << "Server is listening on port " << PORT << std::endl;

  clilen = sizeof(cli_addr);

  while(true){
    //accept incoming 
    client_socket = accept(server_socket, (sockaddr*)&cli_addr, &clilen);
    if (client_socket == -1) {
        error("Error: on accepting connection!");
        continue;
    }

    // HTTP message
    // clear buffer
    bzero(buffer,256);

    n = read(client_socket,buffer,255);
    if (n < 0) error("Error: can't read message!");
    //printf("Here is the message: %s\n",buffer);

    // parse message
    std::string request(buffer, n);
    std::size_t first_space = request.find(' ');
    std::string method = request.substr(0, first_space);
    std::size_t second_space = request.find(' ', first_space + 1);
    std::string path = request.substr(first_space + 1, second_space - first_space - 1);

    std::cout << "Request " << method << " to " << path << std::endl;

    //check path and reply
    if ( method == "GET" && path == "/status"){
      std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\n\n"; // basic response 
      
      // build response
      response += build_response();
      send(client_socket, response.c_str(), response.length(), 0);
    }

    close(client_socket);

  }
  close(server_socket);
  return 0;
}