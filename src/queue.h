
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


// Prend un paquet et une connection en argument et les Renvoie au def
// dans un buffer qui devra en faire un fichier
void data_ind(pkt_t *pkt, int connection);


//Envoie un ack avec pour numéro de séquence n à la connection
//correspondante

//ATTENTION ELLE NE MARCHE PAS ENCORE HOHO
void send_ack(int n,int connection)


//Initialise cette interface, à appeler dans la main
//@n: nombre de connections entrantes
int init_queue(int n);

//Free la queue, à appeler en fin d'exécution
void free_queue();

//Definit la taille de window d'une des connections,
//et initialise toutes les autres variables internes de cette connection.
//@con_indice: le numéro de la connection à initialiser (0,1,2,..,n-1)
//@window_Size: la taille de la fenêtre de cette connection
int define_connection(int con_indice, int window_size);

//LA PERLE NOIRE de cette classe
//prend un **BYTESTREAM** en entrée, et un numéro de connection
//Verifie si le packet est valide en appelant packet_implem
//fait en sorte que le paquet sera data_ind dans le bon ordre.
//appelle send_ack lorsque le protocole selective repeat l'impose
//@pkt: le paquet, sous forme de char*
//@connection: l'indice de la connections
//REMARQUE: pour le moment on prend un pkt en entrée.
int data_req(pkt_t *pkt, int connection);



//Ajoute un pkt_t dans le buffer de packets.
// Cette fonction ajoute le packet tel que leurs SEQNUM soient ordonnés
//Ainsi, le premier pkt sera celui avec le plus petit indice.
//@pkt: le paquet en question
//@connection: l'indice de la connection en cours.
void buffer_add(pkt_t *pkt, int connection);


//Renvoie le premier packet sur le buffer.
//Null si head==Null
//@connection: indice//@connection: indice de la connection de la connection
pkt_t* buffer_peak(int connection);

//Retire le premier packet du buffer et le free.
//Appeler buffer_peak pour récuperer ce paquet avant de le supprimer
//@connection: indice de la connection
void buffer_remove(int connection);

//Incrémente la variable next d'une connections
//En tenant compte des limtes en indices (2¨n)
//@connection: indice de la connection
void next_inc(int connection);


//Si t'as lu cette ligne de commentaire je te chatouillerai
//le pied gauche hihi
void window_inc(int connection);