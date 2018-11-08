#ifndef CREATE_ACCOUNT_WIDGET_H
#define CREATE_ACCOUNT_WIDGET_H

#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WBreak>
#include <Wt/WLengthValidator>
#include <Wt/WRegExpValidator>
#include "WelcomeScreen.h"
class CreateAccountWidget: public Wt::WContainerWidget
{
public:
    CreateAccountWidget(Wt::WContainerWidget *parent = 0,
                        Account *account = 0,
                        WelcomeScreen *main = 0);

    void update();
private:
    void writeUserInfo(std::string username, std::string password, std::string first, std::string last);
    Wt::WLineEdit *username_; // account name text box
    Wt::WLineEdit *firstName_;
    Wt::WLineEdit *lastName_;
    Wt::WLineEdit *password_; // password text box
    Wt::WLineEdit *confirmPassword_; // confirm password text box
    Wt::WPushButton *createAccountButton_; // submit account creation
    Wt::WText *unsuccessfulPassword_; // password unsuccessful message
    Wt::WText *unsuccessfulInput_; // input unsuccessful message
    Wt::WText *accountExists_; // account already exists message

    Wt::WRegExpValidator *usernameValidator_; // username validator
    Wt::WLengthValidator *passwordLengthValidator_; // pw validator
    Wt::WValidator *inputNotEmpty_; // empty input validator

    WelcomeScreen *parent_;
    Account *account_;

    void submit();
    bool validatePassword();
    bool validateInputFields();
    bool checkNewUserid(std::string userid);
};

#endif //CREATE_ACCOUNT_WIDGET_H
