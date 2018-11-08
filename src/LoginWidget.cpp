/**
 *  @file       LoginWidget.cpp
 *  @author     CS 3307 - Team 13
 *  @date       11/7/2017
 *  @version    1.0
 *
 *  @brief      CS 3307, Hue Light Application screen for login
 *
 *  @section    DESCRIPTION
 *
 *              This class represents the login screen. This screen accepts a username,
 *              password and confirms authentication.
 */

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WText>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <string>
#include <stdio.h>
#include <fstream>
#include <openssl/sha.h>

#include "LoginWidget.h"
#include "Hash.h"
#include "Account.h"
#include "Bridge.h"

using namespace Wt;
using namespace std;

/**
 *   @brief  Login Widget constructor
 *
 *   @param  *parent is a pointer the the containerwidget that stores this widget
 *   @param  *account is a pointer to the user account object
 *   @param  *main is a pointer to the app's welcome screen
 */
LoginWidget::LoginWidget(WContainerWidget *parent, Account *account, WelcomeScreen *main):
WContainerWidget(parent)
{
    setContentAlignment(AlignCenter);
    parent_ = main;
    account_ = account;
    setStyleClass("w3-animate-opacity");
}

/**
 *   @brief  Update function, clears the widget and re-populates with elements of the login
 *           screen
 *
 *   @return  void
 *
 */
void LoginWidget::update()
{
    clear(); // everytime you come back to page, reset the widgets

    // username validator - must be a valid email address
    usernameValidator_ = new WRegExpValidator("[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,4}", this);
    usernameValidator_->setInvalidNoMatchText("Username must be valid email address");
    usernameValidator_->setMandatory(true);
    // password must be 6-20 characters in length
    passwordLengthValidator_ = new WLengthValidator(6, 20, this);
    passwordLengthValidator_->setInvalidTooShortText("Password must be at least 6 characters");
    passwordLengthValidator_->setInvalidTooLongText("Password must be max 20 characters");
    passwordLengthValidator_->setMandatory(true);

    // Page title
    WText *title = new WText("Login to account", this);
    title->setStyleClass("title");

    new WBreak(this);
    new WBreak(this);

    // Username box: enter a username that is a valid email address
    new WText("User ID: ", this);
    idEdit_ = new WLineEdit();
    idEdit_->setValidator(usernameValidator_);
    addWidget(idEdit_);
    new WBreak(this);

    // Password box: enter a valid password
    new WText("Password: ", this);
    pwEdit_ = new WLineEdit();
    pwEdit_->setEchoMode(WLineEdit::EchoMode::Password); // hide password as you type and replace with *****
    pwEdit_->setValidator(passwordLengthValidator_);
    addWidget(pwEdit_);
    new WBreak(this);

    // login with provided user and password
    loginButton_ = new WPushButton("Login");
    addWidget(loginButton_);
    new WBreak(this);

    // submit user details to log in
    loginButton_->clicked().connect(this, &LoginWidget::submit);

    // message that warns user incorrect user or password when login fails
    statusMessage_ = new WText("Invalid!",this);
    statusMessage_->setStyleClass("error");
    statusMessage_->setHidden(true); // becomes visible if wrong credentials
}

/**
 *   @brief  submit() function, triggered when user presses login button, displays
 *           any applicable warning messages and/or checks user database for authentication.
 *           Finally, it redirects to the bridge screen after successful login.
 */
void LoginWidget::submit(){
    // checks if provided user account and password exist, shows error message if not found
    if(idEdit_->validate() != 2 || pwEdit_->validate() != 2) {
        statusMessage_->setText("Invalid format for username or password!");
        statusMessage_->setHidden(false);
    }
    else if(!LoginWidget::checkCredentials(idEdit_->text().toUTF8(),pwEdit_->text().toUTF8())){
        statusMessage_->setText("Invalid credentials!");
        statusMessage_->setHidden(false);
    }
    else { // if successful, redirects to bridge page
        parent_->loginSuccess();
    }
}

/**
 *   @brief  checkCredentials() function, checks for username.txt, then compares
 *           encrypted version of the user's password
 *   @param  username is a string representing the user's inputted username
 *   @param  password is a string representing the user's inputted password
 *   @return bool representing if login successful
 */
bool LoginWidget::checkCredentials(string username, string password) {
    ifstream inFile;
    string str;
    string filename = "credentials/" + username + ".txt";
    inFile.open(filename.c_str()); // opens username.txt
    string hashedPW = Hash::sha256_hash(password); // cryptographically hash password

    if (!inFile) {
        return(false); // file not found
    }

    getline(inFile, str); //read hashed password from username.txt
    if(str.compare(hashedPW) == 0) {
        account_->setPassword(hashedPW);
        account_->setEmail(username);
        getline(inFile, str);
        account_->setFirstName(str);
        getline(inFile, str);
        account_->setLastName(str);
        while(getline(inFile, str)) {
            //get bridge name
            int beginIndex = 0;
            int endIndex = str.find(",", beginIndex + 1);
            string bname = str.substr(beginIndex, endIndex - beginIndex);

            //get bridge location
            beginIndex = endIndex + 2;
            endIndex = str.find(",", beginIndex + 1);
            string bloc = str.substr(beginIndex, endIndex - beginIndex);

            //get bridge ip
            beginIndex = endIndex + 2;
            endIndex = str.find(",", beginIndex + 1);
            string bip = str.substr(beginIndex, endIndex - beginIndex);

            //get bridge port
            beginIndex = endIndex + 2;
            endIndex = str.find(",", beginIndex + 1);
            string bport = str.substr(beginIndex, endIndex - beginIndex);

            //get bridge username
            beginIndex = endIndex + 2;
            endIndex = str.find('\n', beginIndex + 1);
            string buser = str.substr(beginIndex, endIndex - beginIndex);

            account_->addBridge(bname, bloc, bip, bport, buser);
        }
        inFile.close();
        return true;
    }
    inFile.close();
    return false;
}
