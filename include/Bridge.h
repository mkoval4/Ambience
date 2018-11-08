#ifndef Bridge_H
#define Bridge_H

#include <Wt/WResource>
#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WBreak>
#include <Wt/WLengthValidator>
#include <Wt/WRegExpValidator>
#include <iostream>
#include <regex>
#include <Wt/Http/Response>
#include <Wt/Http/Client>
#include <sstream>
#include <fstream>
#include <string>
#include <Wt/Json/Object>
//#include "Light.h" //todo: implement

using namespace std;
using namespace Wt;

using namespace std;

class Bridge {

public:
    Bridge(string name, string location, string ip,
           string port, string username = "newdeveloper");

    virtual ~Bridge();

    //GETTER METHODS
    string getName() {return bridgename_;}
    string getLocation() {return location_;}
    string getIP() {return ip_;}
    string getPort() {return port_;}
    string getUsername() {return username_;}
    string getJson() {return json_;}
    
    /*vector<Light> getLights() {return lights;} //todo: implement
    int getNumLights() {return lights.size();} //todo: implement
    Light* getLightAt(int index); //todo: implement*/

    //SETTER METHODS
    void setName(string name) {bridgename_ = name;}
    void setLocation(string location) {location_ = location;}
    void setIP(string ip) {ip_ = ip;}
    void setPort(string port) {port_ = port;}
    void setUsername(string username) {username_ = username;}
    
    void setJson(string json) {json_ = json;}
    
    /*void addLight(Light li); //todo: implement
    void addLight(string type, string name, string modelid,
                  string swversion, string uniqueid, struct state_t state); //todo: implement
    void removeLightAt(int index); //todo: implement*/

private:
    string bridgename_;
    string location_;
    string ip_;
    string port_;
    string username_;
    string json_;
    //vector<Light> lights; //todo: implement
};

#endif
