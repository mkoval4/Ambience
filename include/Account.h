#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <string>
#include "Bridge.h"
#include <vector>

using namespace std;

class Account
{
private:
    string firstName_;
    string lastName_;
    string email_;
    string password_;
    bool auth_;
    vector<Bridge> bridges;

public:
    Account(string fn, string ln, string em, string pw);
    virtual ~Account();

    //Name, email, and password getters/setters
    string getFirstName() {return firstName_;}
    string getLastName() {return lastName_;}
    string getEmail() {return email_;}
    string getPassword() {return password_;}
    bool isAuth() {return auth_;}
    vector<Bridge> getBridges() {return bridges;}
    int getNumBridges() {return bridges.size();}
    Bridge* getBridgeAt(int index);

    void setFirstName(string fn) {firstName_ = fn;}
    void setLastName(string ln) {lastName_ = ln;}
    void setEmail(string em) {email_ = em;}
    void setPassword(string pw) {password_ = pw;}
    void setAuth(bool val) {auth_ = val;}
    
    void addBridge(Bridge br);
    void addBridge(string bname, string bloc, string bip,
                   string bport, string buser);
    void removeBridgeAt(int index);
    
    void writeFile();

};

#endif // ACCOUNT_H
