/**
 *  @file       Group.cpp
 *  @author     CS 3307 - Team 13
 *  @date       10/7/2017
 *  @version    1.0
 *
 *  @brief      CS 3307, Hue Light Application Bridge class to store a Group object
 *
 *  @section    DESCRIPTION
 *
 *              This class stores a Group object that is generated when viewing a Bridge object
 */

#include "Group.h"

/**
 *   @brief  Group constructor
 *
 *   @param  groupNum the number key for the Group in the Hue API
 *   @param  groupData the Json object of a Group from the Hue API
 *
 */
Group::Group(WString groupNum, Json::Object groupData) {
    groupnum_ = groupNum;
    if(groupData.type("name") != 0) name_ = groupData.get("name");
    else name_ = "null";
    
    if(groupData.type("action") != 0) {
        Json::Object action = groupData.get("action");
        if(action.type("alert") != 0) alert_ = action.get("alert");
        else alert_ = "null";
        
        if(action.type("bri") != 0) bri_ = action.get("bri");
        else bri_ = -1;
        
        if(action.type("colormode") != 0) colormode_ = action.get("colormode");
        else colormode_ = "null";
        
        if(action.type("ct") != 0) ct_ = action.get("ct");
        else ct_ = -1;
        
        if(action.type("effect") != 0) effect_ = action.get("effect");
        else effect_ = "null";
        
        if(action.type("hue") != 0) hue_ = action.get("hue");
        else hue_ = -1;
        
        if(action.type("on") != 0) on_ = action.get("on");
        else on_ = 0;
        
        if(action.type("reachable") != 0) reachable_ = action.get("reachable");
        else reachable_ = 0;
        
        if(action.type("sat") != 0) sat_ = action.get("sat");
        else sat_ = -1;
        
        if(action.type("xy") != 0) {
            Json::Array xy = action.get("xy");
            xy_[0] = xy[0];
            xy_[1] = xy[1];
        }
        else {
            xy_[0] = -1.0;
            xy_[1] = -1.0;
        }
    }
    
    if(groupData.type("lights") != 0) {
        Json::Array lights = groupData.get("lights");
        lights_.reserve(5);
        if(groupData.type("lights") != 0) {
            for(Json::Value lightNum : lights) {
                addLight(lightNum);
            }
        }
    }
    
    transitiontime_ = 4; //default value from Hue is 0.4s (400ms)
}

/**
 *   @brief  Group destructor
 *
 */
Group::~Group() {
    
}
