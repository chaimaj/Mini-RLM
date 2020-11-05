/* 
 * File:   main.cpp
 * Author: PC-Z510
 *
 * Created on 18 avril 2015, 00:37
 */

#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <pthread.h>
#include <unistd.h>


#include "Server.h"
#include "Sprite.h"

using namespace std;

int main(int argc, char** argv) {

    Server s;

    // read file and save the sprite
    Sprite * sprite = s.ReadFile("..\\starwars.mov");
    
    //set up the 3 sockets
    int sock1 = s.CreateSocket();
    sockaddr_in addr1 = s.SetUpSocket(16000, "239.0.0.1");

    int sock2 = s.CreateSocket();
    sockaddr_in addr2 = s.SetUpSocket(16001, "239.0.0.2");

    int sock3 = s.CreateSocket();
    sockaddr_in addr3 = s.SetUpSocket(16002, "239.0.0.3");

    // send each frame of the sprite on a different socket "channel"
    for (int i = 0; i < sprite->getFrameCount() - 2; i += 3) {
        s.SendFrame(addr1, sprite->getFrame(i).getString(), sock1);
        s.SendFrame(addr2, sprite->getFrame(i + 1).getString(), sock2);
        s.SendFrame(addr3, sprite->getFrame(i + 2).getString(), sock3);
    }

    return 0;
}




// Reading .mov file : https://www.daniweb.com/software-development/cpp/threads/249884/how-to-process-mov-and-audio-files-using-c-