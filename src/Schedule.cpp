/**
 *  @file       Schedule.cpp
 *  @author     CS 3307 - Team 13
 *  @date       10/7/2017
 *  @version    1.0
 *
 *  @brief      CS 3307, Hue Light Application Bridge class to store a Schedule object
 *
 *  @section    DESCRIPTION
 *
 *              This class stores a Schedule object that is generated when viewing a Bridge object
 */

#include "Schedule.h"

/**
 *   @brief  Schedule constructor
 *
 *   @param  scheduleNum the number key for the Schedule in the Hue API
 *   @param  scheduleData the Json object of a Schedule from the Hue API
 *
 */
Schedule::Schedule(WString scheduleNum, Json::Object scheduleData) {
    schedulenum_ = scheduleNum;
    if(scheduleData.type("name") != 0) name_ = scheduleData.get("name");
    else name_ = "";
    
    if(scheduleData.type("description") != 0) description_ = scheduleData.get("description");
    else description_ = "";
    
    if(scheduleData.type("time") != 0) time_ = scheduleData.get("time");
    else time_ = "";
    
    Json::Object command = scheduleData.get("command");
    if(command.type("address") != 0) address_ = command.get("address");
    else address_ = "";
    
    if(command.type("method") != 0) method_ = command.get("method");
    else method_ = "";
    
    Json::Object body = command.get("body");
    if(body.type("transition") != 0) transition_ = body.get("transition");
    else transition_ = -1;
    
    if(body.type("bri") != 0) bri_ = body.get("bri");
    else bri_ = -1;
    
    if(body.type("transition") != 0) transition_ = body.get("transition");
    else transition_ = 4; //default value from Hue is 0.4s (400ms)
    
    if(body.type("on") != 0) on_ = body.get("on");
    else on_ = 0;
    
    if(body.type("xy") != 0) {
        Json::Array xy = body.get("xy");
        xy_.push_back(xy[0]);
        xy_.push_back(xy[1]);
    }
    else {
        xy_.push_back(-1.0);
        xy_.push_back(-1.0);
    }
}

/**
 *   @brief  Schedule destructor
 *
 */
Schedule::~Schedule() {
    
}
