#ifndef LIGHT_H
#define LIGHT_H

#include <iostream>
#include <regex>
#include <sstream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <Wt/Json/Value>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
#include <Wt/Json/Array>

using namespace std;
using namespace Wt;

class Light {

public:
    Light(WString lightNum, Json::Object lightData);

    virtual ~Light();

    //GETTERS
    WString getLightnum() {return lightnum_;}
    WString getName() {return name_;}
    WString getType() {return type_;}
    WString getModelid() {return modelid_;}
    WString getAlert() {return alert_;}
    int getBri() {return bri_;}
    WString getColormode() {return colormode_;}
    int getCt() {return ct_;}
    WString getEffect() {return effect_;}
    int getHue() {return hue_;}
    bool getOn() {return on_;}
    bool getReachable() {return reachable_;}
    int getSat() {return sat_;}
    double getX() {return xy_[0];}
    double getY() {return xy_[1];}
    int getTransition() {return transitiontime_;}

    //SETTERS
    void setTransition(int transitiontime) {transitiontime_ = transitiontime;}

private:
    WString lightnum_; // light number
    WString name_; // name of light
    WString type_; // type of light
    WString modelid_; // id
    WString alert_; // alert mode
    int bri_; // brightness value
    WString colormode_; // color mode
    int ct_; // ct
    WString effect_; // effect
    int hue_; // hue value
    bool on_; // on/off toggle
    bool reachable_; // reachable
    int sat_;// saturation value
    double xy_[2]; // XY color value
    int transitiontime_; // light transition time
};

#endif //LIGHT_H
