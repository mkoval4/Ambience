/**
 *  @file       Bridge.cpp
 *  @author     CS 3307 - Team 13
 *  @date       10/7/2017
 *  @version    1.0
 *
 *  @brief      CS 3307, Hue Light Application Bridge class to store a Bridge object
 *
 *  @section    DESCRIPTION
 *
 *              This class stores the user-provided information of the Bridge they are
 *              connecting to, if the connection is successful. It allows reading and
 *              writing of data found in the user account file and references it with
 *              the serialized Bridge object to construct a Bridge and add it to the
 *              user account.
 */

#include "Bridge.h"

/**
 *   @brief  Bridge constructor
 *
 *   @param  name is the name of the bridge
 *   @param  location is the location of the bridge
 *   @param  ip is the ip of the bridge URL
 *   @param  port is the port of the bridge URL
 *   @param  username is the username of the bridge URL, default 'newdeveloper'
 *
 */
Bridge::Bridge(string name, string location,
               string ip, string port, string username)
{
    bridgename_ = name;
    ip_ = ip;
    location_ = location;
    port_ = port;
    username_ = username;
    //lights.reserve(15); //todo: implement
}

/**
 *   @brief  Bridge destructor
 *
 */
Bridge::~Bridge() {
    
}
