#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdlib> 
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <fstream>
#include <netinet/in.h>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <netdb.h>
#include <filesystem>

// for measuring process uptime 
const auto program_start_time = std::chrono::steady_clock::now();

const int DEBUG = 1; // for extra console output
#define BUFFER_SIZE 256
// setting env vars
const char* STR_PORT = std::getenv("PORT"); // convert this to int
const int PORT = STR_PORT ? std::atoi(STR_PORT) : 9191;
const char* STORAGE_URL = std::getenv("STORAGE_URL") ? std::getenv("STORAGE_URL") : "localhost";
const char* STORAGE_PORT = std::getenv("STORAGE_PORT") ? std::getenv("STORAGE_PORT") : "8080";

void error(const char *msg){
  perror(msg);
  exit(1);
}

void clg(const char *msg){
  std::cout << msg << std::endl;
  return;
}

/**
 * Function to create timestamp
 */
std::string get_timestamp() {
  const auto now = std::chrono::system_clock::now();
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000; // get millisecods for fun
  auto itt = std::chrono::system_clock::to_time_t(now); // for C time functions

  std::ostringstream ss; // output stream
  ss << std::put_time(gmtime(&itt), "%FT%T"); // convert to ISO
  ss << '.' << std::setw(3) << std::setfill('0') << ms.count() << 'Z'; // add milliseconds
  return ss.str();
}

/**
 * Function to get current uptime
 */
std::string get_uptime() {
  auto now = std::chrono::steady_clock::now();
  auto elapsed = now - program_start_time;
  double uptime =  std::chrono::duration<double>(elapsed).count() / 3600.0;

  std::ostringstream ss;
  ss << std::fixed << std::setprecision(1) << uptime;
  return ss.str();
}

/**
 * Function to get current free disk space
 */
std::string get_free_disk() {
  std::filesystem::space_info disk_info = std::filesystem::space("/");
  return std::to_string(disk_info.available / (1024  * 1024)); // return in MB
}

/**
 * Function to build the record message
 */
std::string build_response() {
  std::ostringstream ss;
  ss << "--SERVICE 2-- "
     << get_timestamp()
     << ": uptime " << get_uptime()
     << " hours, free disk in root: " << get_free_disk()
     << " MBytes";
  return ss.str();
}

void send_to_vStorage(const std::string &msg) {
  std::ofstream vStorage;
  clg("Opening file...");
  vStorage.open("/usr/src/vStorage", std::ios::app);
  if ( vStorage.is_open()){
    clg("Writing vStorage");
    clg(msg.c_str());
    vStorage << msg << "\n";
    vStorage.close();
    clg("done");
  }
  else {
    error("Error: error opening vStorage");
  }
  return;
}

/**
 * Function to send record message to storage service
 */
std::string send_to_storage(const std::string &data) {
  
  //const char* STORAGE_URL = "storage";
  //const char* STORAGE_PORT = "8080";
  const char* path = "/log";

  //std::string::size_type pos = url.find('/');
  //std::string base_url = url.substr(0, pos);
  //std::string path = url.substr(pos);
  if (DEBUG){
       std::cout << "STORAGE REQUEST DATA: \n" << data << std::endl;
  }
  if (data.empty()){
    error("Error: no data in request");
    return "Exiting post";
  }
  std::string hello = "This is data hello";
  std::string body = "{\"data\":\"" + data + "\"}";
  std::string request =
        "POST " + std::string(path) + " HTTP/1.1\r\n" +
        "Host: " + STORAGE_URL + ":8080\r\n" +
        "Content-Type: application/json\r\n" +
        "Content-Length: " + std::to_string(body.length()) + "\r\n" +
        "Connection: close\r\n\r\n" +
        body;

  if (DEBUG){
       std::cout << "STORAGE REQUEST: \n" << request << std::endl;
  }

  // Get address info
    struct addrinfo hints{}, *res;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
  /*
  if (getaddrinfo(host, port, &hints, &res) != 0) {
        error("Error: getaddrinfo");
        return "Exiting post";
  }
  */
  int err = getaddrinfo(STORAGE_URL, STORAGE_PORT, &hints, &res);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s\n", STORAGE_URL, gai_strerror(err));
        abort();
    }

  // Create socket
  int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (sockfd < 0) {
      error("Error: error creating socket");
      return "Exiting post";
  }

  // Connect
  if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
      error("Error: error connecting to socket ");
      close(sockfd);
      return "Exiting post";
  }

  // Send request
  send(sockfd, request.c_str(), request.length(), 0);

  // Read and print response
  char buffer[BUFFER_SIZE];
  ssize_t bytesReceived;
  while ((bytesReceived = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
      buffer[bytesReceived] = '\0';
      if(DEBUG){
        std::cout << "STORAGE RESPONSE: "  << std::endl;
        std::cout << buffer;
      }
  }

  // Clean up
  close(sockfd);
  freeaddrinfo(res);
  return "all done";
}

/**
 * Main function, listens to coming requests 
 */
int main() {
  if (DEBUG){
    std::cout << "VARIABLES: \n" << STORAGE_URL << std::endl;
    std::cout << STORAGE_PORT << std::endl;
  }

  // sockets and adresses
  int server_socket, client_socket;
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

  std::cout << "Service2 running on port: " << PORT << std::endl;
  clilen = sizeof(cli_addr);

  // handle requests
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

    // DEBUG REQUEST
    if ( DEBUG == 1){
      std::cout << "Buffer: " << buffer << std::endl;
    }
    // parse message
    std::string request(buffer, n);
    std::size_t first_space = request.find(' ');
    std::string method = request.substr(0, first_space);
    std::size_t second_space = request.find(' ', first_space + 1);
    std::string path = request.substr(first_space + 1, second_space - first_space - 1);

    // Console info
    if(!DEBUG){
      std::cout << "Request " << method << " to " << path << std::endl;
    }

    //check method and path 
    if ( method == "GET" && path == "/status"){
      // build response
      std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n";  // End of headers
      // 5. analyze status and create record
      std::string record = build_response();
      response += record + "\r\n";

      // 6. log the response with storage service
      std::string post_res = send_to_storage(record);

      // 7. log the record to vStorage
      send_to_vStorage(record);

      // 8. send the record as a response
      send(client_socket, response.c_str(), response.length(), 0);
    }
    else {
      std::string response =
        "HTTP/1.1 501 Not Implemented\r\n"
        "Content-Type: text/plain\r\n"
        "Connection: close\r\n"
        "\r\n"
        "Not Implemented\r\n";

      send(client_socket, response.c_str(), response.length(), 0);
    }
    close(client_socket);
  }
  close(server_socket);
  return 0;
}