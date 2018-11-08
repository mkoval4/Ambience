#ifndef LOGIN_WIDGET_H
#define LOGIN_WIDGET_H

#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include "WelcomeScreen.h"

class LoginWidget: public Wt::WContainerWidget
{
public:
    LoginWidget(Wt::WContainerWidget *parent = 0,
                Account *account = 0,
                WelcomeScreen *main = 0);
    void update();

private:
    Wt::WLineEdit *idEdit_; // id text box
    Wt::WLineEdit *pwEdit_; // password texrt box
    Wt::WPushButton *loginButton_; // login button
    Wt::WText *statusMessage_; // message to warn user if user or password incorrect
    
    Wt::WRegExpValidator *usernameValidator_;
    Wt::WLengthValidator *passwordLengthValidator_;
    
    WelcomeScreen *parent_;
    Account *account_;

    void submit();
    bool checkCredentials(std::string username, std::string password);
};

#endif //LOGIN_WIDGET_H
