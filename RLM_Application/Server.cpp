/* 
 * File:   Server.cpp
 * Author: PC-Z510
 * 
 * Created on 18 avril 2015, 00:38
 */

#include "Server.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>



using namespace std;

void discardCR(string &str);

sockaddr_in Server::SetUpSocket(int port, char* address){
    struct sockaddr_in addr;
    int addrlen;

    bzero((char *) &addr, sizeof (addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    addrlen = sizeof (addr);
    
    printf("I am a sender on port %d \n",port);

    addr.sin_addr.s_addr = inet_addr(address);
    
    return addr;
}

int Server::CreateSocket(){
    /* set up socket */
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }
    return sock;
}

void Server::SendFrame(sockaddr_in addr, string frame,int sock){
     int cnt;
     int addrlen = sizeof (addr);
     char message[2048];
        sprintf(message, "%s", frame.c_str()); /* make readable */
        printf("sending: %s\n", message);
        cnt = sendto(sock, message, strlen (message)+1 , 0, (struct sockaddr *) &addr, addrlen);
        if (cnt < 0) {
            perror("sendto");
            exit(1);
        }
        sleep(1);  // sleep for 1 second after each packet sent
}


// read the file
Sprite * Server::ReadFile(string filename) {
    const char * c = filename.c_str();
    ifstream myfile(c);
    int frame;
    int width;
    int height;
    Sprite *spr;
    string line;

    //Open File
    if (myfile.is_open()) {
        int i = 1;
        //repeat until end of file

        while ((myfile.good())&& (i <= 3)) {
            //read header
            switch (i) {
                case 1: // get number of frames
                    getline(myfile, line);
                    discardCR(line);
                    frame = atoi(line.substr(strlen("frames") + 1).c_str());
                    //cout << "frames " << frame << "\n";
                    break;
                case 2: // get width
                    getline(myfile, line);
                    discardCR(line);
                    width = atoi(line.substr(strlen("width") + 1).c_str());
                    //cout << "width " << width << "\n";
                    break;
                case 3: // get height
                    getline(myfile, line);
                    discardCR(line);
                    height = atoi(line.substr(strlen("height") + 1).c_str());
                    //cout << "height " << height << "\n";
                    break;

            }
            i++;
        }
        //create Sprite with number of frames
        spr = new Sprite(frame);
        spr->setWidth(width);
        spr->setHeight(height);
        bool eof = false;

        while ((myfile.good()) && (!eof)) {
            // Check if not end of line
            string frame_str = "";

            for (int j = 0; j <= height; j++) {
                getline(myfile, line);
                discardCR(line);
                if ((line.size() != width)&&(line != "end")&&(line != "stop")) {

                    Frame empty;
                    spr->addFrame(empty);

                } else if (line == "stop") {
                    eof = true;
                    break;
                } else if (line != "end") {
                    frame_str += line;
                }
                i++;
            }
            if (frame_str != "") {
                //cout << frame_str << "\n";
                Frame frame(width, height, frame_str);
                spr->addFrame(frame);
            }
        }
        //        //close file when done
    } else {

        return NULL;
    }
    myfile.close();


    return spr;
}

void discardCR(string & str) {
    if ((str.size() > 0) && (str[str.size() - 1] == '\r')) {
        cout << "erase" << str[str.size() - 1];
        str.erase(str.size() - 1);
    }
}

Server::Server() {
   
}

Server::Server(const Server & orig) {
}

Server::~Server() {
}

