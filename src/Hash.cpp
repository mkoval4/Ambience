/**
 *  @file       Hash.cpp
 *  @author     CS 3307 - Team 13
 *  @date       10/7/2017
 *  @version    1.0
 *
 *  @brief      CS 3307, Hue Light Application password encryption hash function
 *
 *  @section    DESCRIPTION
 *
 *              This class represents the Hash function. A SHA256 cryptographic hashing function which the
 *              password can be run through before storage.
 *              Two of the same passwords run through the same function would return the same hash,
 *              however, it is near impossible to reverse-engineer this function to take the hash
 *              and return the plain-text password
 */

#include "Hash.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <openssl/sha.h>

using namespace std;

/**
*   @brief  sha256_hash function, a static function which takes a plaintext string and outputs an encrypted string
*
*   @param  string of plain-text
*   @return string (encrypted)
*
*/
string Hash::sha256_hash(const string str)
{

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;

    SHA256_Init(&sha256);

    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);

    stringstream ss;

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];

    }

    return ss.str();
}
