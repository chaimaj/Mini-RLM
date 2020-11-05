/* 
 * File:   main.cpp
 * Author: PC-Z510
 *
 * Created on 18 avril 2015, 03:10
 */

#include <cstdlib>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <curses.h>
#include <string>
#include<sys/time.h>
#include <fcntl.h>
#include "utility.h"
#include "Frame.h"
#include "Position.h"
#include "Sprite.h"

using namespace std;

#define BUFSIZE 2048
#define NUMBER_OF_CONSECUTIVE_PKT 3 // number of consecutive packets received to add a channel
/*
 * 
 */
WINDOW *buffer1; ///< Allocated drawing buffer 1.
WINDOW *buffer2; ///< Allocated drawing buffer 2.
WINDOW *p_buffer; ///< Pointer to current drawing buffer.
int max_x; //terminal dimensions
int max_y; //terminal dimensions

int ok;  // number of consecutive frames received 


int startUp();
void swapBuffers();
int drawCh(Position world_pos, char ch, int color);
int drawFrame(Position world_pos, Frame frame, bool centered, int color);
int SetTimer(struct timeval &tv, time_t sec);
int CheckTimer(struct timeval &tv, time_t sec);

void * ReceiveUnicast(void * p) {
    fd_set master; // master file descriptor list
    long port = (long) p;
    struct sockaddr_in myaddr; /* our address */
    struct sockaddr_in remaddr; /* remote address */
    socklen_t addrlen = sizeof (remaddr); /* length of addresses */
    int recvlen; /* # bytes received */
    int fd; /* our socket */
    char buf[BUFSIZE]; /* receive buffer */


    /* create a UDP socket */

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("cannot create socket\n");
        exit(1);
    }
    /* bind the socket to any valid IP address and a specific port */

    memset((char *) &myaddr, 0, sizeof (myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *) &myaddr, sizeof (myaddr)) < 0) {
        perror("bind failed");
        exit(1);
    }
    startUp();
    printf("waiting\n");


    /* now loop, receiving data and printing what we received */

    
    for (;;) {
        FD_ZERO(&master); // clear the master and temp sets
        FD_SET(fd, &master);
        struct timeval tv;
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        int rv = select(fd + 1, &master, NULL, NULL, &tv);  //non blocking socket
        if (rv == 1) {
            recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *) &remaddr, &addrlen);
            if (recvlen > 0) {
                buf[recvlen] = 0;
                string cont = buf;
                if ((cont != "loss")&&(cont != "drop")) {  // if packet received is not loss or drop then draw it
                    Frame frame(21, 7, string(buf));
                    drawFrame(Position(20, 20), frame, true, 1);
                    ok++;
                }
            } else {
                printf("loss");
            }
            string content = buf;
            if (content == "loss") {  // if it's loss reset ok to 0 and send loss to the router
                ok = 0;
                sprintf(buf, "loss");
            }
            else if (content == "drop"){ // if drop also reset ok to 0 and send drop to router
                sprintf(buf, "drop");
                ok = 0;
            }
            else if (ok==NUMBER_OF_CONSECUTIVE_PKT){ // if we have enough consecutive packets we assume no more loss and send "ok to the router
                sprintf(buf,"ok");
            }
            else {
                sprintf(buf,"ack");
            }
            //        }
            swapBuffers();
            //send answer to the router
            if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *) &remaddr, addrlen) < 0)
                perror("sendto");
        }

    }

}

void swapBuffers() {
    wrefresh(p_buffer);

    if (p_buffer == buffer1) {
        p_buffer = buffer2;
    } else {

        p_buffer = buffer1;
    }
    clear();
}

int startUp() {
    int error = 0;
    //enable curses
    initscr();

    p_buffer = buffer1;

    //Make full size windows
    buffer1 = newwin(0, 0, 0, 0);
    buffer2 = newwin(0, 0, 0, 0);

    //Get terminal dimensions
    getmaxyx(stdscr, max_y, max_x);

    //Make charachters bold
    //(Bold characters make graphics more vivid)
    error += wattron(buffer1, A_BOLD);
    error += wattron(buffer2, A_BOLD);

    //Make next refresh redraw window completely
    error += clearok(buffer1, TRUE);
    error += clearok(buffer2, TRUE);

    //LEave cursor where it is after refresh
    error += leaveok(buffer1, TRUE);
    error += leaveok(buffer2, TRUE);

    //Check if the terminal supports color
    if (has_colors()) {
        //enable color
        start_color();
        //setup colors pairs prior to use: init_pair (num,fg,bg)
        init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
        init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
        init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
        init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
        init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
        init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);

    }
    if (error == 0) {
        return 0;
    } else {
        return 1;
    }

}

int drawCh(Position world_pos, char ch, int color) {
    utility u;
    Position view_pos = u.worldToView(world_pos);

    //if drawing in color, turn on colornpair 'color'
    wattron(p_buffer, COLOR_PAIR(color));
    //draw single character at (x,y)
    mvwaddch(p_buffer, view_pos.getY(), view_pos.getX(), ch);
    //if drew in color, turn off color pair 'color'
    wattroff(p_buffer, COLOR_PAIR(color));
}

int drawFrame(Position world_pos, Frame frame, bool centered, int color) {
    int x_offset = 0;
    int y_offset = 0;
    //check for empty frame
    if (frame.getString() == "") {
        return -1;
    }

    //Centered ? then offset (x,y) by 1/2 (width/height)
    if (centered) {
        x_offset = frame.getWidth() / 2;
        y_offset = frame.getHeight() / 2;
    } else {
        x_offset = 0;
        y_offset = 0;
    }

    //frame data stored in String
    string str = frame.getString();

    //draw character by character
    for (int y = 0; y < frame.getHeight(); y++) {
        for (int x = 0; x < frame.getWidth(); x++) {
            Position temp_pos(world_pos.getX() - x_offset + x, world_pos.getY() - y_offset + y);
            drawCh(temp_pos, str[y * frame.getWidth() + x], color);
        }
    }
}

int SetTimer(struct timeval &tv, time_t sec) {
    gettimeofday(&tv, NULL);
    tv.tv_sec += sec;

    return 1;
}

int CheckTimer(struct timeval &tv, time_t sec) {
    struct timeval ctv;
    gettimeofday(&ctv, NULL);

    if (ctv.tv_sec > tv.tv_sec) {
        gettimeofday(&tv, NULL);
        tv.tv_sec += sec;
        return 1;
    } else
        return 0;
}

int main(int argc, char** argv) {
    
    //create 3 threads one for each channel and call them 
    pthread_t p1, p2, p3;
    int port1 = 21234;
    int port2 = 21235;
    int port3 = 21236;

    pthread_create(&p1, NULL, &ReceiveUnicast, (void *) port1);
    pthread_create(&p2, NULL, &ReceiveUnicast, (void *) port2);
    pthread_create(&p3, NULL, &ReceiveUnicast, (void *) port3);

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);


    return 0;
}

// http://cs.ecs.baylor.edu/~donahoo/practical/CSockets/practical/
