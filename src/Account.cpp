/**
 *  @file       Account.cpp
 *  @author     CS 3307 - Team 13
 *  @date       10/7/2017
 *  @version    1.0
 *
 *  @brief      CS 3307, Hue Light Application Account class to store a user account
 *
 *  @section    DESCRIPTION
 *
 *              This class stores the profile of the user that has registered to the
 *              application. The hashed password is stored and not in plain text. The
 *              Account class manages writing to the serialized account file, editing
 *              user details, and adding and removing Bridge objects that the user
 *              has registered to their account.
 */

#include <string>
#include "Account.h"

/**
 *   @brief  Account constructor
 *
 *   @param  fn is the first name of the user
 *   @param  ln is the last name of the user
 *   @param  em is the email of the user
 *   @param  pw is the hashed password of the user
 *
 */
Account::Account(string fn, string ln, string em, string pw) :
firstName_(fn),
lastName_(ln),
email_(em),
password_(pw),
auth_(false){
    bridges.reserve(15); //reserve initial 15 spaces which dynamically increases once surpassed
}

/**
 *   @brief  Account destructor
 *
 */
Account::~Account() {
    
}


/**
 *   @brief  Write Account object to the user account data file stored in application
 *
 *   @return void
 *
 */
void Account::writeFile() {
    
    // creates credentials folder if one does not exist
    const int dir_err = system("mkdir -p credentials");
    if (-1 == dir_err)
    {
        cout << "ERROR - Could not create directory\n";
        exit(1);
    }
    
    ofstream writefile;
    string file = "credentials/" + getEmail() +".txt";
    
    writefile.open(file.c_str());
    
    writefile << getPassword() << endl; // cryptographically hashed password
    writefile << getFirstName() << endl;
    writefile << getLastName() << endl;
    
    for(auto &bridge : getBridges()) { //loop through all bridges
        writefile << bridge.getName() << ", "
        << bridge.getLocation() << ", "
        << bridge.getIP() << ", "
        << bridge.getPort() << ", "
        << bridge.getUsername() << "\n";
    }
    
    writefile.close();
}

/**
 *   @brief  Add a Bridge to the user account
 *
 *   @param  br an instantiated Bridge object to add to the account
 *
 *   @return void
 *
 */
void Account::addBridge(Bridge br) {
    bridges.push_back(br);
}

/**
 *   @brief  Add a Bridge to the user account
 *
 *   @param  bname name of bridge to add to user account
 *   @param  bloc location of bridge to add to user account
 *   @param  bip ip of bridge to add to user account
 *   @param  bport port of bridge to add to user account
 *   @param  buser username of bridge to add to user account
 *
 *   @return void
 *
 */
void Account::addBridge(string bname, string bloc, string bip,
                        string bport, string buser) {
    Bridge *br = new Bridge(bname, bloc, bip, bport, buser);
    bridges.push_back(*br);
}

/**
 *   @brief  Returns a pointer to a Bridge in the user account
 *
 *   @param  index the position of the Bridge in the vector to retrieve (0 is first position)
 *
 *   @return pointer to Bridge object in user account
 *
 */
Bridge* Account::getBridgeAt(int index) {
    return &bridges.at(index);
}

/**
 *   @brief  Remove a Bridge from the user account
 *
 *   @param  index the position of the Bridge in the vector to remove (0 is first position)
 *
 *   @return void
 *
 */
void Account::removeBridgeAt(int index) {
    bridges.erase(bridges.begin() + index);
}
