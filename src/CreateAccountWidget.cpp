/**
 *  @file       CreateAccountWidget.cpp
 *  @author     CS 3307 - Team 13
 *  @date       11/7/2017
 *  @version    1.0
 *
 *  @brief      CS 3307, Hue Light Application screen for user account creation
 *
 *  @section    DESCRIPTION
 *
 *              This class represents the user account creation screen, before login
 *              the user can come to this screen or the login screen. This screen accepts
 *              a username, password, confirms password. And then stores the user's credentials
 *              to a file.
 */

#include <Wt/WText>
#include <Wt/WTable>
#include <fstream> // writing new accounts to a file
#include <string>
#include <iostream>
#include <unistd.h>
#include "CreateAccountWidget.h"
#include "Hash.h" // for password encryption
#include "Account.h"

using namespace Wt;
using namespace std;

/**
 *   @brief  Create Account Widget constructor
 *
 *   @param  *parent is a pointer the the containerwidget that stores this widget
 *   @param  *account is a pointer to the user account object
 *   @param  *main is a pointer to the app's welcome screen
 */
CreateAccountWidget::CreateAccountWidget(WContainerWidget *parent, Account *account, WelcomeScreen *main):
WContainerWidget(parent)
{
    setContentAlignment(AlignCenter);
    parent_ = main;
    account_ = account;
    setStyleClass("w3-animate-opacity");
}


/**
 *   @brief  Update function, clears the widget and re-populates with elements of the account
 *           creation screen
 *
 *   @return  void
 *
 */
void CreateAccountWidget::update()
{
    clear(); // everytime you come back to page, reset the widgets

    // username validator - must be a valid email address
    usernameValidator_ = new WRegExpValidator("[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,4}", this);
    usernameValidator_->setInvalidNoMatchText("Username must be valid email address");
    usernameValidator_->setMandatory(true);
    //non-empty validator
    inputNotEmpty_ = new WValidator(this);
    inputNotEmpty_->setMandatory(true);
    // password must be 6-20 characters in length
    passwordLengthValidator_ = new WLengthValidator(6, 20, this);
    passwordLengthValidator_->setInvalidTooShortText("Password must be at least 6 characters");
    passwordLengthValidator_->setInvalidTooLongText("Password must be max 20 characters");
    passwordLengthValidator_->setMandatory(true);

    // Page title
    WText *title = new WText("Create an account", this);
    title->setStyleClass("title");

    new WBreak(this);

    new WBreak(this);


    // Username box: create a username that is a valid email address
    new WText("User ID: ", this);
    username_ = new WLineEdit();
    username_->setTextSize(18); // to hold placeholder text
    username_->setPlaceholderText("person@xyz.com"); // placeholder text to increase intuitive-ness of application
    username_->setValidator(usernameValidator_);
    addWidget(username_);
    new WBreak(this);

    new WText("First Name: ", this);
    firstName_ = new WLineEdit();
    firstName_->setTextSize(18);
    firstName_->setValidator(inputNotEmpty_);
    addWidget(firstName_);
    new WBreak(this);


    new WText("Last Name: ", this);
    lastName_ = new WLineEdit();
    lastName_->setTextSize(18);
    lastName_->setValidator(inputNotEmpty_);
    addWidget(lastName_);
    new WBreak(this);


    // Password box: create a valid password
    new WText("Password: ", this);
    password_ = new WLineEdit();
    password_->setEchoMode(WLineEdit::EchoMode::Password); // hide password as you type and replace with *****
    password_->setValidator(passwordLengthValidator_);
    addWidget(password_);
    new WBreak(this);

    // user must confirm their password, and they must equal eachother
    new WText("Confirm Password: ", this);
    confirmPassword_ = new WLineEdit();
    confirmPassword_->setEchoMode(WLineEdit::EchoMode::Password);
    addWidget(confirmPassword_);
    new WBreak(this);


    // submit user details to create account
    createAccountButton_ = new WPushButton("Create Account");
    addWidget(createAccountButton_);
    new WBreak(this);

    // if passwords dont match show a warning text
    unsuccessfulPassword_ = new WText("Sorry, passwords do not match");
    unsuccessfulPassword_->setStyleClass("error");
    addWidget(unsuccessfulPassword_);
    unsuccessfulPassword_->setHidden(true); // password match warning text hidden by default

    unsuccessfulInput_ = new WText("Please fill all boxes with valid input."); // if password not 6-20 characters or username does not follow name@domain.tld
    unsuccessfulInput_->setStyleClass("error");
    addWidget(unsuccessfulInput_);

    unsuccessfulInput_->setHidden(true);
    accountExists_ = new WText("Sorry, username already in use"); // if password not 6-20 characters or username does not follow name@domain.tld
    accountExists_->setStyleClass("error");
    addWidget(accountExists_);
    accountExists_->setHidden(true);

    // user create account button connects with helper function
    createAccountButton_->clicked().connect(this, &CreateAccountWidget::submit);
}


/**
 *   @brief  submit() function, triggered when user presses create account button, displays
 *           any applicable warning messages and/or triggers the account credentials write-to-file.
 *           Finally, it redirects to the login screen.
 */
void CreateAccountWidget::submit(){


    // If currently showing the unsuccessful password/user text or the password not matching text, erase it
    unsuccessfulPassword_->setHidden(true);
    unsuccessfulInput_->setHidden(true);
    accountExists_->setHidden(true);

    if (!CreateAccountWidget::validatePassword()) { // if the password != confirmed password
        unsuccessfulPassword_->setHidden(false); // show user that passwords don't match
    }
    else if (!CreateAccountWidget::validateInputFields()) {
        unsuccessfulInput_->setHidden(false);
    }
    else if (!CreateAccountWidget::checkNewUserid(username_->text().toUTF8())){

        accountExists_->setHidden(false);
    }
    else { // if password and confirmed password match
        CreateAccountWidget::writeUserInfo(username_->text().toUTF8(), password_->text().toUTF8(), firstName_->text().toUTF8(), lastName_->text().toUTF8()); // write the user's credentials to file
        WApplication::instance()->setInternalPath("/login", true);
    }

}

/**
 *   @brief  checkNewUserId() function, checks to see if the user's inputted id already exists in the system
 *
 *   @param  userid is a string representing the user's inputted username
 *   @return  bool: true if the username is "new" false if already exists
 */
bool CreateAccountWidget::checkNewUserid(string userid) {
    string file = "credentials/" + userid + ".txt";
    ifstream f(file.c_str());
    return !f.good();
}

/**
 *   @brief  writeCredentials() function, writes an encrypted version of the user's password
 *           into username.txt
 *   @param  username is a string representing the user's inputted username
 *   @param  password is a string representing the user's inputted password
 */
void CreateAccountWidget::writeUserInfo(string username, string password, string firstName, string lastName) {

    // creates credentials folder if one does not exist
    const int dir_err = system("mkdir -p credentials");
    if (-1 == dir_err)
    {
        cout << "ERROR - Could not create directory\n";
        exit(1);
    }

    ofstream writefile;
    string file = "credentials/" + username + ".txt";

    writefile.open(file.c_str());

    writefile << Hash::sha256_hash(password) <<endl; // cryptographically hash password

    writefile<< firstName << endl;
    writefile<< lastName << endl;

    writefile.close();
    //TODO: Error handling in the file write

    // creates profile pictures folder if one does not exist
    const int prof_pic_dir_err = system("mkdir -p Wt/images/ppics");
    if (-1 == prof_pic_dir_err)
    {
        cout << "ERROR - Could not create directory\n";
        exit(1);
    }


    string profPicFile = "Wt/images/ppics/" + username; // file extension?
    string defaultProfPicFileLocation = "Wt/images/default_ppic.png";


    ifstream ppic_in(defaultProfPicFileLocation.c_str());//ios::binary);
    ofstream ppic_writefile(profPicFile.c_str());
    ppic_writefile << ppic_in.rdbuf();
    ppic_writefile.close();
    ppic_in.close();

}

/**
 *   @brief  validatePassword() function, confirms the the user's inputted password == inputted confirmed password
 *   @return bool representing if the passwords are the same or not
 */
bool CreateAccountWidget::validatePassword() {
    return password_->text() == confirmPassword_->text(); // password must equal password in the confirm password box
}

/**
 *   @brief  validateInputFields(): ensures that username is valid email address and pass is between 6-20 chars
 *   @return bool true if the input fields are valid, false otherwise
 */
bool CreateAccountWidget::validateInputFields() {
    return username_->validate() == 2 &&
    password_->validate() == 2 &&
    firstName_->validate() == 2 &&
    lastName_->validate() == 2;
}
