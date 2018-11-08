/**
 *  @file       ColourConvert.cpp
 *  @author     CS 3307 - Team 13
 *  @date       11/27/2017
 *  @version    1.0
 *
 *  @brief      CS 3307, Colour converter for Hue Light Application.
 *
 *  @section    DESCRIPTION
 *
 *              This is a helper class used to convert colour values from RGB format
 *              to the XY format, RGB to the Hue Sat Bri format, or from XY to RGB format.
 */

#include "ColourConvert.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>

using namespace std;

/**
 *   @brief  RGB to XY converter
 *
 *   @param  r is the red component of the colour
 *   @param  g is the green component of the colour
 *   @param  b is the blue component of the colour
 *
 *   @return struct xy containing XY value representation of parameters
 */
struct xy *ColourConvert::rgb2xy(float r, float g, float b)
{
    
    //convert values 0-255 to between 0 and 1
    float red = r/255.0f;
    float green = g/255.0f;
    float blue = b/255.0f;
    
    // gamma correction to rgb values
    
    red = (red > 0.04045f) ? pow((red + 0.055f) / (1.0f + 0.055f), 2.4f) : (red / 12.92f);
    green = (green > 0.04045f) ? pow((green + 0.055f) / (1.0f + 0.055f), 2.4f) : (green / 12.92f);
    blue = (blue > 0.04045f) ? pow((blue + 0.055f) / (1.0f + 0.055f), 2.4f) : (blue / 12.92f);
    
    // convert the RGB values to xyz using wide rgb d65 conversion formula
    
    float X = red * 0.649926f + green * 0.103455f + blue * 0.197109f;
    float Y = red * 0.234327f + green * 0.743075f + blue * 0.022598f;
    float Z = red * 0.000000f + green * 0.053077f + blue * 1.035763f;
    
    // calculate the xy values from the xyz values
    float x = 0.0, y = 0.0;
    if(X != 0.0 || Y != 0.0 || X != 0.0) {
        x = X / (X + Y + Z);
        y = Y / (X + Y + Z);
    }
    
    // TO DO : check for if the xy value lays in the color gamut triangle for the light
    // use the Y value of the XYZ as brightness
    
    // xy values cannot be less than zero
    if (x < 0)
        x = 0.00001f;
    if (y < 0)
        y = 0.00001f;
    if (Y < 0.0)
        Y = 0.0;
    if (Y > 1.0)
        Y = 1.0;
    
    struct xy *xyStruct = (struct xy*) malloc(sizeof(struct xy));
    
    xyStruct->brightness = Y * 254.0;
    xyStruct->x = x;
    xyStruct->y = y;
    
    return xyStruct;
}

/**
 *   @brief  XY to RGB converter
 *
 *   @param  inputX is the X component of the colour
 *   @param  inputY is the Y component of the colour
 *   @param  brightness is the brightness value of the light
 *
 *   @return struct rgb containing RGB value representation of parameters
 */
struct rgb *ColourConvert::xy2rgb(float inputX, float inputY, float brightness)
{
    
    // CONVERT TO XYZ
    float x = inputX;
    float y = inputY;
    
    float z = 1.0f - x - y;
    float Y = brightness / 254.0f; // the given brightness level
    
    float X = (Y / y) * x;
    float Z = (Y / y) * z;
    
    // COnvert to rgb using wide rgb d65 conversion
    
    float r = X * 3.2410f - Y * 1.5374f - Z * 0.4986f;
    float g = -X * 0.9692f + Y * 1.8760f + Z * 0.0416f;
    float b = X * 0.0556f - Y * 0.2040f + Z * 1.0570f;
    
    //apply reverse gamma correction
    r = r <= 0.0031308f ? 12.92f * r : (1.0f + 0.055f) * pow(r, (1.0f / 2.4f)) - 0.055f;
    g = g <= 0.0031308f ? 12.92f * g : (1.0f + 0.055f) * pow(g, (1.0f / 2.4f)) - 0.055f;
    b = b <= 0.0031308f ? 12.92f * b : (1.0f + 0.055f) * pow(b, (1.0f / 2.4f)) - 0.055f;
    
    //convert to 0-255
    if (r > 0)
        r = (r * 255);
    else
        r = 0;
    
    if (g > 0)
        g = g * 255;
    else
        g = 0;
    
    if (b > 0)
        b = b * 255;
    else
        b = 0;
    
    if ( r > 255 )
        r = 255;
    if (g > 255)
        g = 255;
    if (b > 255)
        b = 255;
    
    
    // create pointer of rgb structure
    struct rgb *rgbStruct = (struct rgb*) malloc(sizeof(struct rgb));
    rgbStruct->r = r;
    rgbStruct->b = b;
    rgbStruct->g = g;
    
    return rgbStruct;
}

/**
 *   @brief  Hue Sat Bri to RGB converter
 *
 *   @param  hue is the Hue component of the colour
 *   @param  sat is the Sat component of the colour
 *   @param  bri is the Brightness value of the light
 *
 *   @return struct rgb containing RGB value representation of parameters
 */
struct rgb *ColourConvert::hsv2rgb(float hue, float sat, float bri) {
    float r, g, b;
    double hh, p, q, t, ff;
    long i;
    
    double h = hue/ 65280.0; // hue in smartlights is 0-65280
    h = h * 360.0; // this formula uses an angle for hue
    
    struct rgb *rgbStruct = (struct rgb*) malloc(sizeof(struct rgb));
    
    bri = bri / 254.0; // expected brightness value 0-1
    sat = sat / 255.0; // expected saturation value 0-1
    
    if (sat <= 0.0) {
        rgbStruct->r = bri * 254.0;
        rgbStruct->b = bri * 254.0;
        rgbStruct->g = bri * 254.0;
        
        return rgbStruct;
        
    }
    
    hh = h;
    
    if (hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    
    i = (long)hh;
    
    ff = hh - i;
    
    p = bri * (1.0 - sat);
    q = bri * (1.0 - (sat * ff));
    t = bri * (1.0 - (sat * (1.0 - ff)));
    
    switch(i){
        case 0:
            r = bri;
            g = (float)t;
            b = (float)p;
            break;
        case 1:
            r = (float)q;
            g = bri;
            b = (float)p;
            break;
        case 2:
            r = (float)p;
            g = bri;
            b = (float)t;
            break;
        case 3:
            r = (float)p;
            g = (float)q;
            b = bri;
            break;
        case 4:
            r = (float)t;
            g = (float)p;
            b = bri;
            break;
        case 5:
        default:
            r = bri;
            g = (float)p;
            b = (float)q;
            break;
    }
    
    rgbStruct->r = r * 255.0;
    rgbStruct->g = g * 255.0;
    rgbStruct->b = b * 255.0;
    return rgbStruct;
    
}
