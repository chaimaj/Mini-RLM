/* 
 * File:   utility.cpp
 * Author: PC-Z510
 * 
 * Created on 5 février 2015, 19:01
 */

#include <cstdlib>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <sstream>
#include <string>
#include "utility.h"


using namespace std;

utility::utility() {
}

utility::utility(const utility& orig) {
}

utility::~utility() {
}

char * utility::getTimeString() {
    //String to return, made 'static' so persists
    static char time_str[30];
    //System calls to get time
    time_t now;
    time(&now);
    struct tm *p_time = localtime(&now);

    //'02' gives two digits, '%d' for integers
    sprintf(time_str, "%02d:%02d:%02d", p_time -> tm_hour, p_time ->tm_min, p_time ->tm_sec);

    return time_str;
}

bool utility::positionIntersect(Position p1, Position p2) {
    if ((p1.getX() == p2.getX())&&(p1.getY() == p2.getY())) {
        return true;
    }
    return false;
}


//convert world position to view position

Position utility::worldToView(Position world_pos) {
    Position view_origin(0,0);
    int view_x = view_origin.getX();
    int view_y = view_origin.getY();
    Position view_pos(world_pos.getX() - view_x, world_pos.getY() - view_y);

    return view_pos;
}

Position utility::viewToWorld (Position view_pos){
    Position view_origin(0,0);
    int view_x = view_origin.getX();
    int view_y = view_origin.getY();
    Position world_pos(view_pos.getX() + view_x, view_pos.getY() + view_y);
    
    return world_pos;
}

string utility::toString (int i){
    stringstream ss;
    ss << i;
    return ss.str();
}

