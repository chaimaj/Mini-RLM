/* 
 * File:   Server.h
 * Author: PC-Z510
 *
 * Created on 18 avril 2015, 00:38
 */

#ifndef SERVER_H
#define	SERVER_H

#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Sprite.h"

using namespace std;



class Server {
  

public:
    Server();
    Server(const Server& orig);
    virtual ~Server();
    Sprite * ReadFile (string filename);  //read file and return a Sprite
    sockaddr_in SetUpSocket(int port, char* address);  //set up the address and port of the scoket
    int CreateSocket ();   // create the scoket
    void SendFrame(sockaddr_in addr, string frame,int sock);  // send frame using the sockaddr-in and the scoket created
    
 
};

#endif	/* SERVER_H */

