#ifndef HASH_H
#define HASH_H

#include <string>

// XY color structure
struct xy {
    float x;
    float y;
};

// RGB color structure
struct rgb {
    float r;
    float g;
    float b;
};

class Hash
{
    // static public methods
    public:
        static std::string sha256_hash(const std::string str);

};

#endif // HASH_H
