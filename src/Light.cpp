/**
 *  @file       Light.cpp
 *  @author     CS 3307 - Team 13
 *  @date       10/7/2017
 *  @version    1.0
 *
 *  @brief      CS 3307, Hue Light Application Bridge class to store a Light object
 *
 *  @section    DESCRIPTION
 *
 *              This class stores a Light object that is generated when viewing a Bridge object
 */

#include "Light.h"

/**
 *   @brief  Light constructor
 *
 *   @param  lightNum the number key for the Light in the Hue API
 *   @param  lightData the Json object of a Light from the Hue API
 *
 */
Light::Light(WString lightNum, Json::Object lightData) {
    lightnum_ = lightNum;
    if(lightData.type("name") != 0) name_ = lightData.get("name");
    else name_ = "null";
    if(lightData.type("type") != 0) type_ = lightData.get("type");
    else type_ = "null";
    if(lightData.type("modelid") != 0) modelid_ = lightData.get("modelid");
    else modelid_ = "null";
    
    Json::Object state = lightData.get("state");
    if(state.type("alert") != 0) alert_ = state.get("alert");
    else alert_ = "null";
    
    if(state.type("bri") != 0) bri_ = state.get("bri");
    else bri_ = -1;
    
    if(state.type("colormode") != 0) colormode_ = state.get("colormode");
    else colormode_ = "null";
    
    if(state.type("ct") != 0) ct_ = state.get("ct");
    else ct_ = -1;
    
    if(state.type("effect") != 0) effect_ = state.get("effect");
    else effect_ = "null";
    
    if(state.type("hue") != 0) hue_ = state.get("hue");
    else hue_ = -1;
    
    if(state.type("on") != 0) on_ = state.get("on");
    else on_ = 0;
    
    if(state.type("reachable") != 0) reachable_ = state.get("reachable");
    else reachable_ = 0;
    
    if(state.type("sat") != 0) sat_ = state.get("sat");
    else sat_ = -1;
    
    if(state.type("xy") != 0) {
        Json::Array xy = state.get("xy");
        xy_[0] = xy[0];
        xy_[1] = xy[1];
    }
    else {
        xy_[0] = -1.0;
        xy_[1] = -1.0;
    }
    
    transitiontime_ = 4; //default value from Hue is 0.4s (400ms)
}

/**
 *   @brief  Light destructor
 *
 */
Light::~Light() {
    
}
