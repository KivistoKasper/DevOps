#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 9191
#define BUFFER_SIZE 256

void error(const char *msg){
  perror(msg);
  exit(1);
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
    bzero(buffer,256);

    n = read(client_socket,buffer,255);
    if (n < 0) error("ERROR reading from socket");
    printf("Here is the message: %s\n",buffer);

    send(client_socket, "OK", 2, 0);
    close(client_socket);

  }
  close(server_socket);
  return 0;
}