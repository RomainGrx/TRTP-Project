#include "packet.h"
#include "queue.h"
#include "receive.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in6
#include <netdb.h> // addrinfo

void test_decode_header();
void test_decode_all();
void test_encode();
void test_pointer_struct();


int main(int argc, char const *argv[]) {

  init_queue(1);
  socket_listening(NULL, 8555, 5);
  free_receive();
  free_queue();
  // pkt_t *pkt = pkt_new();
  // pkt->TYPE = 0b01;
  // pkt->TR = 0;
  // pkt->WINDOW = 17;
  // pkt->L = 0;
  // pkt->LENGTH = 30;
  // pkt->SEQNUM = 0;
  // pkt->TIMESTAMP = 66;
  // pkt->CRC1 = 37;
  // pkt->CRC2 = 666;
  // char* data = "GROSSE GROSSE BITE DE NOIR";
  // pkt_set_payload(pkt, data, strlen(data)+1);
  //
  //
  // char* donnees = malloc(sizeof(char)*1024);
  // size_t coucou = 1024;
  // pkt_encode(pkt, donnees,&coucou);
  //
  //
  //
  // treat_bytestream(donnees, coucou, 0);
  // data_req(pkt,0);

  free_queue();
return 0;
}

    // int err;
    // //struct sockaddr_in6 *ipv6 = malloc(sizeof(struct sockaddr_in6));
    // struct sockaddr_in6 ipv6;
    // memset(&ipv6, 0, sizeof(ipv6));
    //
    // // err = IPV6_translater(argv[1], &ipv6);
    // int sockfd = socket_init(&ipv6, atoi(argv[2]), NULL, -1);
    //
    //
    // ipv6.sin6_family = AF_INET6;
    // ipv6.sin6_port = htons(atoi(argv[2]));
    // char v6dst[INET6_ADDRSTRLEN];
    //
    // strcpy(v6dst, argv[1]);
    //
    // inet_pton(AF_INET6, v6dst, &(ipv6.sin6_addr));
    //
    // if (bind(sockfd, (struct sockaddr *) &ipv6, sizeof(ipv6)) == -1){
    //     printf("%s  \n", strerror(errno));
    // }
    //
    // char *buffer = malloc(1024);
    // err = recv(sockfd, buffer, 1024, 0);
    // printf("> %s\n", buffer);
    //
    // free(buffer);


void test_decode_header(){
  pkt_t *pkt = pkt_new();
  // uint32_t HEADER = 0b01000011000001110000001100000000; // TYPE = 1, TR = 0, WINDOW = 3, L = 0, LENGTH = 7, SEQNUM = 3
  uint32_t HEADER = 0b01000011100000000000011100000011; // TYPE = 1, TR = 0, WINDOW = 3, L = 1, LENGTH = 7, SEQNUM = 3
  char *data = malloc(4);
  // data = (char*)&HEADER;
  memcpy(data, &HEADER, 4);
  pkt_decode((char*)&data, 4, pkt);

  pkt_del(pkt);
}

void test_decode_all(){

}

void test_pointer_struct(){
  pkt_t *pkt = pkt_new();

  pkt_set_type(pkt, 1);
  pkt_set_tr(pkt, 0);
  pkt_set_window(pkt, 3);
  pkt->L = 1;
  pkt_set_length(pkt, 1);
  pkt_set_seqnum(pkt, 3);
  pkt_set_timestamp(pkt, 777);
  pkt_set_crc1(pkt, 666);
  char *a = "COUCOUL";
  pkt_set_payload(pkt, a, 8);
  pkt_set_crc2(pkt, 111);

  // printf("TYPE \t%p\n", &(pkt->TYPE));
  // printf("TR \t%p\n", &(pkt->TR));
  // printf("WINDOW \t%p\n", &(pkt->WINDOW));
  // printf("LENGTH \t%p\n", &(pkt->LENGTH));
  // printf("SEQNUM \t%p\n", &(pkt->SEQNUM));
  // printf("TIMESTAMP \t%u\n", pkt_get_timestamp(pktd));
  // printf("CRC1 \t%u\n", pkt_get_crc1(pktd));
  // printf("PAYLOAD \t%s\n", pkt_get_payload(pktd));
  // printf("CRC2 \t%u\n", pkt_get_crc2(pktd));

}




void test_encode(){
    pkt_t *pkt = pkt_new();
    pkt->TYPE = 0b01;
    pkt->TR = 0;
    pkt->WINDOW = 12;
    pkt->L = 0;
    pkt->LENGTH = 30;
    pkt->SEQNUM = 12;
    pkt->TIMESTAMP = 66;
    pkt->CRC1 = 37;
    pkt->CRC2 = 666;
    char* data = "GROSSE GROSSE BITE DE NOIR";
    pkt_set_payload(pkt, data, pkt->LENGTH);

    size_t len = 128;
    char *buf = malloc(len);
    pkt_status_code status = pkt_encode(pkt, buf, &len);
    printf("TYPE : \t%u \n",((uint8_t)buf[0] >> 6));
    printf("TR : \t%u \n",((uint8_t)buf[0] >> 5) & 0b00000001);
    printf("WINDOW : \t%u \n",((uint8_t)buf[0]) & 0b00011111);
    printf("L : \t%u\n", (uint8_t)buf[1]>>7);
    printf("LENGTH : \t%u\n", (uint8_t)buf[1] & 0b01111111);
    char *text = malloc(27);
    memcpy(text, &buf[11], 27);
    printf("DATA : %s\n", text);

    pkt_t *pktd = pkt_new();
    status = pkt_decode(buf, 128, pktd);
    printf("Type decode \t: %u\n",pkt_get_type(pktd));
    printf("TR decode \t: %u\n",pkt_get_tr(pktd));
    printf("Window decode \t: %u\n",pkt_get_window(pktd));
    printf("L decode \t: %u\n",pkt_get_l(pktd));
    printf("Length decode \t: %u\n",pkt_get_length(pktd));
    printf("Seqnum decode \t: %u\n",pkt_get_seqnum(pktd));
    printf("Timestamp decode \t: %u\n",pkt_get_timestamp(pktd));
    printf("Timestamp decode \t: %u\n",pkt_get_timestamp(pktd));
    printf("Timestamp decode \t: %u\n",pkt_get_timestamp(pktd));
    free(text);
    pkt_del(pkt);
    pkt_del(pktd);
}
