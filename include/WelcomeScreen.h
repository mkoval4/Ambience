#ifndef WELCOME_SCREEN_H
#define WELCOME_SCREEN_H

#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WText>
#include <Wt/WMenu>
#include <Wt/WImage>
#include <Wt/WMenuItem>
#include <Wt/WNavigationBar>
#include <Wt/WPushButton>
#include <string>
#include <vector>
#include "Account.h"

namespace Wt {
  class WStackedWidget;
  class WAnchor;
}

class LoginWidget;
class CreateAccountWidget;
class BridgeScreenWidget;
class ProfileWidget;
class LightManagementWidget;

class WelcomeScreen : public Wt::WContainerWidget
{
public:
    WelcomeScreen(Wt::WContainerWidget *parent = 0);
    void handleInternalPath(const std::string &internalPath);

    Account getAccount() {return account_;};
    void setAccount(Account account) {account_ = account;};

    void connectBridge();
    void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message &response);

    void updateProfileName();
    void loginSuccess();

private:
    Wt::WNavigationBar *navBar_;
    Wt::WMenuItem *loginMenuItem_;
    Wt::WMenuItem *createMenuItem_;
    Wt::WMenuItem *bridgesMenuItem_;
    Wt::WMenuItem *profileMenuItem_;
    Wt::WMenuItem *logoutMenuItem_;
    Wt::WText *serverMessage_;
    Wt::WAnchor *profileAnchor_;
    Wt::WAnchor *homeAnchor_;
    Wt::WStackedWidget *mainStack_; // main stack of the application
    Wt::WContainerWidget *links_; // links container that stores anchors
    Wt::WImage *ppic_;
    Wt::WContainerWidget *picContainer_;

    CreateAccountWidget *createScreen_; // create widget
    LoginWidget *loginScreen_; // login widget
    BridgeScreenWidget *bridgeScreen_; // bridge widget
    ProfileWidget *profileScreen_; // profile widget
    LightManagementWidget *lightManage_; // light management widget

    void loginScreen();
    void createAccountScreen();
    void bridgeScreen();
    void profileScreen();
    void lightManagementScreen(int index);

    Account account_;
};

#endif //WELCOME_SCREEN_H
