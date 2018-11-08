#ifndef COLOURCONVERT_H
#define COLOURCONVERT_H

// structure with X and Y components of a colour and brightness value
struct xy {
    float x;
    float y;
    float brightness;
};

// structure with red, green, blue components of a colour and brightness value
struct rgb {
    float r;
    float g;
    float b;
    float brightness;
};


class ColourConvert
{
    //static public methods
    public:
        static struct xy *rgb2xy(float red, float green, float blue);
        static struct rgb *xy2rgb(float x, float y, float brightness);
        static struct rgb *hsv2rgb(float hue, float sat, float bri);

    protected:

    private:
};

#endif // COLOURCONVERT_H
