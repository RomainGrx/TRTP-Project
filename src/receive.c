#include "packet_interface.h"
#include "queue.h"
#include "receive.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memset
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in6
#include <netdb.h> // addrinfo
#include <errno.h>
#include <unistd.h>//read()
#include <fcntl.h> // for open

#include <arpa/inet.h>


//Tuto qui nous a aidé: https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/
int print = 0;

//All needed variables for socket_listening.
int max_clients;
struct sockaddr_in6 address;
struct sockaddr_in6* clients = NULL;
fd_set readfds;


void free_receive(){
  free(clients);//
}

int create_master_socket(int * master_socket, char * hostname, int port, int * addrlen){
    *master_socket = socket(AF_INET6, SOCK_DGRAM, 0);
    address.sin6_family = AF_INET6;
    if(hostname == NULL) address.sin6_addr=in6addr_any;
    else inet_pton(AF_INET6, hostname, (void *)&address.sin6_addr.s6_addr);
    address.sin6_port =htons(port);
    address.sin6_scope_id = 0;
    if (bind(*master_socket, (struct sockaddr*) &address, sizeof(address)) < 0)
    {
            fprintf(stderr, "Error binding master socket: %s", strerror(errno));
            return -1;
    }
    *addrlen = sizeof(address);
    return 0;
}


int treat_message_from(struct sockaddr_in6 address, char* buffer, int bufsize){
  if(clients == NULL){
    clients = malloc(sizeof(struct sockaddr_in6));
    memcpy(&clients[0], &address, sizeof(struct sockaddr_in6));
    printf("Added client 0\n");
      //
      // return 0;
  }

  for(int i = 0; i<max_clients; i++){
    char adr1[INET6_ADDRSTRLEN];
    char adr2[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6,&clients[i].sin6_addr, adr1, INET6_ADDRSTRLEN);
    inet_ntop(AF_INET6,&address.sin6_addr, adr2, INET6_ADDRSTRLEN);
    if(strcmp(adr1,adr2)==0){
      //Ici on a recu le message buffer du client indice i;
      printf("Received message from client %d:::%s\n",  i, buffer);

      treat_bytestream(buffer, 1024, i);

      return 0;
    }
    else{
      printf("Oupsy doupsy qui c'est qui n'ira pas au concert de SHANIA TWAIN!!!\n");
    }
  }

}

int socket_listening(char* hostname, int port, int n_connections){
  max_clients = 1;
  int master_socket;
  int addrlen;
  char buffer[1024];
  printf("Socket listening\n");


  if( (create_master_socket(&master_socket, hostname, port, &addrlen)) == -1)
  {printf("Create master socket failed"); return -1;}


  //WHILE LOOP
  while(1)
   {
       //clear the socket set
       FD_ZERO(&readfds);
       //add master socket to set
       FD_SET(master_socket, &readfds);
       int max_sd = master_socket;

       int activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

       if ((activity < 0) && (errno!=EINTR))
       {
           printf("select error");
       }


       //If something happened on the master socket ,
       //then its an incoming connection
       if (FD_ISSET(master_socket, &readfds))
       {
            struct sockaddr_in6 newaddress;
           int bytesread = recvfrom(master_socket, buffer, 1024,MSG_WAITALL, (struct sockaddr*) &newaddress, (socklen_t*)&addrlen);
           if(bytesread <0){
             printf("Error reading: %s\n", strerror(errno));
           }
           treat_message_from(address, buffer, 1024);
           printf("Received: %s\n", buffer);



           //ICI JE CONNECTE NEWSOCKET A LA SOURCE
           //int errconnect  = connect(master_socket,(struct sockaddr *) &newaddress, addrlen);
           // printf("Errconnect returns: %d and err: %s\n", errconnect,strerror(errno));
           // char* message = "Bill Gates te remercie\n";
           // if(send(master_socket, message, strlen(message), 0) <1)
           // {
           //     printf("Send error: %s\n",strerror(errno));
           // }


       }
 }
}


int IPV6_translater(const char* hostname, struct sockaddr_in6 *ipv6){
    int status;
    struct addrinfo hints, *ai;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET6; // IPv6
    hints.ai_socktype = 0; // Datagrams
    hints.ai_protocol = IPPROTO_UDP; // UDP
    hints.ai_flags    = 0; // Pas de spécificité
    if ((status = getaddrinfo(hostname , NULL, &hints, &ai )) != 0) {
        fprintf(stderr, "[IPV6] getaddrinfo: %s\n", gai_strerror(status));
        return -1;
    }
    ipv6 = (struct sockaddr_in6 *)ai->ai_addr;
    char *ipstr = malloc(INET6_ADDRSTRLEN);

    void *addr;
    addr = &(ipv6->sin6_addr);
    char ipver = '6';
    inet_ntop(ai->ai_family, addr, ipstr, sizeof ipstr);
    printf(" IPv%c: %s\n", ipver, ipstr);

    inet_pton(AF_INET6, ipstr, &(ipv6->sin6_addr));

    freeaddrinfo(ai);
    return 0;
}

int socket_init(struct sockaddr_in6 *src_addr, const int src_port,
                struct sockaddr_in6 *dest_addr, const int dest_port){


    int sockfd = socket(AF_INET6, SOCK_DGRAM, 0); // IPv6, datagrams, UDP
    if(sockfd == -1){fprintf(stderr, "[socket_init] Impossible de créer un socket.\n"); return -1;}

    // Bind l'adresse source avec le socket
    // if(src_addr != NULL && src_port > 0){
    //     src_addr->sin6_port = htons(src_port);
    //     if(bind(sockfd,(const struct sockaddr *)src_addr, sizeof(struct sockaddr_in6)) < 0){ printf("%s\n", strerror(errno));fprintf(stderr, "[socket_init] Impossible de bind la source \n"); return -1;}
    // }

    // Connect l'adresse de destination avec le socket
    if(dest_addr != NULL && dest_port > 0){
        dest_addr->sin6_port = htons(dest_port);
        if(connect(sockfd,(const struct sockaddr *)dest_addr, (socklen_t)sizeof(struct sockaddr_in6)) < 0){ fprintf(stderr, "[socket_init] Impossible de connect la destination \n"); return -1;}
    }

    return sockfd;
}

int receive(char* hostname, int port, char* filename){
    return EXIT_SUCCESS;
}
