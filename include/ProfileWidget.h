#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/WInPlaceEdit>
#include <Wt/WValidator>
#include <Wt/WLengthValidator>
#include <Wt/WDialog>
#include "WelcomeScreen.h"


class ProfileWidget: public Wt::WContainerWidget
{
public:
    ProfileWidget(Wt::WContainerWidget *parent = 0,
                Account *account = 0,
                WelcomeScreen *main = 0);
    void update();

private:

    WelcomeScreen *parent_;
    Account *account_;
    Wt::WInPlaceEdit *editableFirstName_;
    Wt::WInPlaceEdit *editableLastName_;
    Wt::WPushButton *updatePassword_;
    Wt::WDialog *passwordDialog_;
    Wt::WLineEdit *currentPass_;
    Wt::WLineEdit *newPass_;
    Wt::WLineEdit *confirmNewPass_;
    Wt::WText *passwordError_;
    Wt::WText *passwordSuccess_;
    Wt::WFileUpload *picUpload_;
    Wt::WText *profilePicOutMessage_;
    bool fileTooLarge = false;

    Wt::WValidator *inputNotEmpty_;
    Wt::WLengthValidator *passwordLengthValidator_;

    void updateFirstName();
    void updateLastName();
    void showPasswordDialog();
    bool checkValidPassword(std::string pass);
    void updatePassword();
    void uploadProfilePicture(std::string file);

};

#endif //PROFILEWIDGET_H
