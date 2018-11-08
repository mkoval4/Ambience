#ifndef SCHEDULE_H
#define SCHEDULE_H

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

class Schedule {

public:
    Schedule(WString scheduleNum, Json::Object scheduleData);

    virtual ~Schedule();

    //GETTERS
    WString getSchedulenum() {return schedulenum_;}
    WString getName() {return name_;}
    WString getDescription() {return description_;}
    WString getTime() {return time_;}
    WString getAddress() {return address_;}
    WString getMethod() {return method_;}
    int getBri() {return bri_;}
    int getTransition() {return transition_;}
    bool getOn() {return on_;}
    double getX() {return xy_[0];}
    double getY() {return xy_[1];}

private:
    WString schedulenum_; // schedule number
    WString name_; // name
    WString description_; // description
    WString time_; // time
    WString address_; // address
    WString method_; // method
    int bri_; // brightness value
    int transition_; // transition time
    bool on_; // on/off toggle
    vector<double> xy_; // X and Y color values
};

#endif //SCHEDULE_H
