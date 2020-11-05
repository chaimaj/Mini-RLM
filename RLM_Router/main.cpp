/* 
 * File:   main.cpp
 * Author: PC-Z510
 *
 * Created on 18 avril 2015, 01:51
 */

#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <netdb.h>
#include <pthread.h>


using namespace std;

#define BUFLEN 2048
#define LOSS 0  //percentage of loss
#define NUMBER_OF_LOSS 2 //variable that determines after how many lost packets should we drop a channel

struct arg_struct { // structure of arguments for the function ReceiveMulticast 
    int port;  // port on which we receive the multicast data from server
    char * address;  // address on which we receive the multicast data from server
    char * addressSend; // address of the client we will send the channel tp
    int portSend;   // port of the client
};

int numberOfTotalDrop = 0; // number of lost packets for all channels
/*
 * 
 */
int SendUnicast(char* ip, int port, char* message);  // method that sends unicast to the client

//method that receives multicast from server

void * ReceiveMulticast(void * arguments) {
    // getting the arguments from the struct
    struct arg_struct *args = (arg_struct*) arguments;
    int port = args->port;
    char * address = args->address;
    int portSend = args->portSend;
    char * addressSend = args->addressSend;
    bool dropped = false;  // boolean that checks if this channel is dropped or not
    int numberOfDrop = 0;  // number of lost Packets for this channel
   
    struct sockaddr_in addr;
    int addrlen, sock, cnt;
    struct ip_mreq mreq;
    char message[2048];

    /* set up socket */
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }
    bzero((char *) &addr, sizeof (addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    addrlen = sizeof (addr);

    printf("I am a receiver.  Waiting for messages...\n");

    /* receiver plays out messages */
    if (bind(sock, (struct sockaddr *) &addr, sizeof (addr)) < 0) {
        perror("bind");
        exit(1);
    }
    mreq.imr_multiaddr.s_addr = inet_addr(address);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
            &mreq, sizeof (mreq)) < 0) {
        perror("setsockopt mreq");
        exit(1);
    }
    while (1) { // receive packet from server
        cnt = recvfrom(sock, message, sizeof (message), 0, (struct sockaddr *) &addr, &addrlen);
        if (cnt < 0) {
            perror("recvfrom");
            exit(1);
        } else if (cnt == 0) {
            break;
        }
        //setting up loss
        srand(time(NULL));
        int send = rand() % 100;
        printf ("random number %d \n", send);
        printf("numOf Total drop %d \n", numberOfTotalDrop);
        printf("numOf drop %d port %d \n", numberOfDrop, port);
        if (numberOfTotalDrop<NUMBER_OF_LOSS){ // if the total number of drops is reset (means we received "ok" from the client)
            dropped =false;                    // this channel is no longer dropped
            numberOfDrop =0;                   // his number of drops are reset
        }
        //numberOfTotalDrop is increased and reset according to the return of SendUnicast function
        if (!dropped) {  // if channel is not dropped
            if ((send > LOSS)&& (numberOfDrop < NUMBER_OF_LOSS)) { // if there is no loss and number of drops for this channel < max we send the packet
                numberOfTotalDrop += SendUnicast(addressSend, portSend, message);
                if (numberOfTotalDrop==0){  // if the total is reset, we reset the number of loss for this channel
                    numberOfDrop=0;
                }
                printf("sent %s \n", message);
            } else if ((numberOfTotalDrop >= NUMBER_OF_LOSS)&&(numberOfDrop>=NUMBER_OF_LOSS)){  // if number of loss >= max we send "drop" to the client that means we drop the channel
                numberOfTotalDrop += SendUnicast(addressSend, portSend, "drop");
                dropped = true;   // dropped is true for thsi channel
                printf("drop %d \n", portSend);
            } else {  // if we don't send and we don't drop that means the packet is lost and we send "loss"
                numberOfTotalDrop += SendUnicast(addressSend, portSend, "loss");
                numberOfDrop++; // we increase the number of lost packets
                printf("loss %s \n", message);
            }
        }
    }
}

// send unicast packet to the client return 0 if received, 1 if lost and -NumberOfTotal loss if "ok" received from client
int SendUnicast(char* ip, int port, char* message) {
    struct sockaddr_in myaddr, remaddr;
    int fd, slen = sizeof (remaddr);
    char buf[BUFLEN]; /* message buffer */
    int recvlen; /* # bytes in acknowledgement message */
    char *server = ip; /* change this to use a different server */

    /* create a socket */

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("socket created\n");

    /* bind it to all local addresses and pick any port number */

    memset((char *) &myaddr, 0, sizeof (myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(0);

    if (bind(fd, (struct sockaddr *) &myaddr, sizeof (myaddr)) < 0) {
        perror("bind failed");
        exit(1);
    }

    memset((char *) &remaddr, 0, sizeof (remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(port);
    if (inet_aton(server, &remaddr.sin_addr) == 0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    /* now let's send the messages */


    printf("Sending packet to %s port %d\n", server, port);
    sprintf(buf, "%s", message);
    if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *) &remaddr, slen) == -1) {
        perror("sendto");
        exit(1);
    }
    /* now receive an acknowledgement from the server */
    recvlen = recvfrom(fd, buf, BUFLEN, 0, (struct sockaddr *) &remaddr, &slen);
    if (recvlen >= 0) {
        buf[recvlen] = 0; /* expect a printable string - terminate it */
        printf("received message: \"%s\"\n", buf);
        string content = buf;
        if (content == "loss") { // return 1 if we receive "loss" from client
            return 1;
        } else if (content == "ok") {  // return -numberOfTotalDrop if receive "ok" from client
            return -numberOfTotalDrop;
        } else if (content == "drop") { //return 0 if "drop" received
            return 0;
        }
    }

    close(fd);
    return 0; // return 0 otherwise (packet sent)
}

int main(int argc, char** argv) {
    
    // set up 3 threads and their arguments fro our 3 channels
    pthread_t p1, p2, p3;
    struct arg_struct args1;
    args1.port = 16000;
    args1.address = "239.0.0.1";
    args1.addressSend = "127.0.0.1";
    args1.portSend = 21234;

    struct arg_struct args2;
    args2.port = 16001;
    args2.address = "239.0.0.2";
    args2.addressSend = "127.0.0.1";
    args2.portSend = 21235;

    struct arg_struct args3;
    args3.port = 16002;
    args3.address = "239.0.0.3";
    args3.addressSend = "127.0.0.1";
    args3.portSend = 21236;
    
    //call each thread
    pthread_create(&p1, NULL, &ReceiveMulticast, (void *) &args1);
    pthread_create(&p2, NULL, &ReceiveMulticast, (void *) &args2);
    pthread_create(&p3, NULL, &ReceiveMulticast, (void *) &args3);
    
    //thread will terminate only if all threads terminate
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);

    return 0;
}

