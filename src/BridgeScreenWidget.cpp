/**
 *  @file       BridgeScreenWidget.cpp
 *  @author     CS 3307 - Team 13
 *  @date       10/7/2017
 *  @version    1.0
 *
 *  @brief      CS 3307, Hue Light Application screen for viewing, adding, and removing bridges
 *
 *  @section    DESCRIPTION
 *
 *              This class represents the bridge view screen, the user can view the bridges
 *              already connected to their account, register a new bridge, or remove an existing
 *              bridge.
 */

#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WTable>
#include <Wt/WImage>
#include <Wt/WTime>
#include <Wt/WDialog>
#include <Wt/WBorderLayout>
#include <fstream> // writing new accounts to a file
#include "BridgeScreenWidget.h"
#include "Bridge.h"
#include "Light.h"
#include "Hash.h" // for password encryption
#include <string>
#include <vector>
#include "Account.h"
#include <Wt/Json/Value>
#include <Wt/WSplitButton>
#include <Wt/WPopupMenu>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
#include <Wt/Json/Array>

using namespace Wt;
using namespace std;

/**
 *   @brief  Bridge Screen Widget constructor
 *
 *   @param  *parent is a pointer the the containerwidget that stores this widget
 *   @param  *account is a pointer to the user account object
 *   @param  *main is a pointer to the app's welcome screen
 */
BridgeScreenWidget::BridgeScreenWidget(WContainerWidget *parent, Account *account, WelcomeScreen *main):
WContainerWidget(parent)
{
    setContentAlignment(AlignCenter);
    parent_ = main;
    account_ = account;

    setStyleClass("w3-animate-opacity");

}

/**
 *   @brief  Update function, clears the widget and re-populates with elements of the bridge screen
 *
 *   @return  void
 *
 */
void BridgeScreenWidget::update()
{
    clear(); // everytime you come back to page, reset the widgets


    //ip address validator - ensure non-empty proper IP format
    ipValidator_ = new WRegExpValidator("(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])", this);
    ipValidator_->setInvalidNoMatchText("Invalid IP Address");
    ipValidator_->setMandatory(true);
    //string validator - ensure non-empty string without commas
    stringValidator_ = new WRegExpValidator("[^,]*",this);
    stringValidator_->setInvalidNoMatchText("Invalid text");
    stringValidator_->setMandatory(true);
    //port validator - ensure non-empty valid port
    portValidator_ = new WIntValidator(0, 65535, this);
    portValidator_->setMandatory(true);


    useridValidator_ = new WRegExpValidator("[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,4}", this);

    // Page title
    WText *title = new WText("Register a new Hue Bridge", this);
    title->setStyleClass("title");

    new WBreak(this);
    new WBreak(this);

    //Bridge Name
    new WText("Bridge Name: ", this);
    bridgename_ = new WLineEdit();
    bridgename_->setTextSize(10); // to hold placeholder text
    bridgename_->setPlaceholderText("Mr. Bridge"); // placeholder text
    addWidget(bridgename_);
    bridgename_->setValidator(stringValidator_);

    new WBreak(this);

    //Bridge Location
    new WText("Location: ", this);
    location_ = new WLineEdit();
    location_->setTextSize(10); // to hold placeholder text
    location_->setPlaceholderText("Bedroom"); // placeholder text
    addWidget(location_);
    location_->setValidator(stringValidator_);

    new WBreak(this);

    //ip address
    new WText("IP Address: ", this);
    ip_ = new WLineEdit();
    ip_->setTextSize(18); // to hold placeholder text
    ip_->setPlaceholderText("127.0.0.1"); // placeholder text
    addWidget(ip_);
    ip_->setValidator(ipValidator_);

    new WBreak(this);

    //port number
    new WText("Port Number: ", this);
    port_ = new WLineEdit();
    port_->setTextSize(6); // to hold placeholder text
    port_->setPlaceholderText("80"); // placeholder text
    addWidget(port_);
    port_->setValidator(portValidator_);

    new WBreak(this);

    //username
    new WText("Username: ", this);
    username_ = new WLineEdit();
    username_->setTextSize(18); // to hold placeholder text
    username_->setValueText("newdeveloper"); // placeholder text
    addWidget(username_);
    username_->setValidator(stringValidator_);

    new WBreak(this);

    //button for registering a bridge
    registerBridgeButton_ = new WPushButton("Register Bridge");
    addWidget(registerBridgeButton_);
    registerBridgeButton_->clicked().connect(this, &BridgeScreenWidget::registerBridge);



    //WText to handle any status messaging from user actions
    statusMessage_ = new WText("", this);
    statusMessage_->setStyleClass("error");
    statusMessage_->setHidden(true);

    new WBreak(this);
    new WBreak(this);
    new WBreak(this);

    // Page title
    WText *title2 = new WText( account_->getFirstName()+ "'s Bridges", this);
    title2->setStyleClass("title");

    new WBreak(this);
    new WBreak(this);

    //create WTable and call updateBridgeTable
    bridgeTable_ = new WTable(this);
    bridgeTable_->setHeaderCount(1); //set first row as header
    BridgeScreenWidget::updateBridgeTable();

}

/**
 *   @brief  Function called when Register Bridge button is pressed to test a connection to the provided URL
 *
 *   @return  void
 *
 */

void BridgeScreenWidget::registerBridge() {
    if(bridgename_->validate() == 2 &&
       location_->validate() == 2 &&
       ip_->validate() == 2 &&
       port_->validate() == 2 &&
       username_->validate() == 2) {

        string url = "http://" + ip_->text().toUTF8() + ":" + port_->text().toUTF8() + "/api/" + username_->text().toUTF8();

        cout << "BRIDGE: Registering at URL " << url << "\n";
        Wt::Http::Client *client = new Wt::Http::Client(this);
        client->setTimeout(2); //2 second timeout of request
        client->setMaximumResponseSize(1000000);
        client->done().connect(boost::bind(&BridgeScreenWidget::registerBridgeHttp, this, _1, _2));

        if(client->get(url)) {
            WApplication::instance()->deferRendering();
        }
        else {
            cerr << "BRIDGE: Error in client->get(url) call\n";
            statusMessage_->setText("Unable to register bridge.");
            statusMessage_->setHidden(false);
        }
    }
    else {
        string errmsg = "Invalid input for: ";
        if(bridgename_->validate() != 2) errmsg += "Name ";
        if(location_->validate() != 2) errmsg += "Location ";
        if(ip_->validate() != 2) errmsg += "IP ";
        if(port_->validate() != 2) errmsg += "Port ";
        if(username_->validate() != 2) errmsg += "Username ";

        statusMessage_->setText(errmsg);
        statusMessage_->setHidden(false);
    }
}

/**
 *   @brief  Function to handle the Http response generated by the Wt Http Client object in the registerBridge() function.
 *
 *   @param  *err stores the error code generated by an Http request, null if request was successful
 *   @param  &response stores the response message generated by the Http request
 *
 *   @return  void
 *
 */
void BridgeScreenWidget::registerBridgeHttp(boost::system::error_code err, const Wt::Http::Message &response)
{
    WApplication::instance()->resumeRendering();
    if (!err && response.status() == 200) {
        statusMessage_->setText("Successfully registered!");
        statusMessage_->setHidden(false);

        //add Bridge to user account
        account_->addBridge(bridgename_->text().toUTF8(), location_->text().toUTF8(), ip_->text().toUTF8(), port_->text().toUTF8(), username_->text().toUTF8());
        account_->writeFile(); //update credentials file

        BridgeScreenWidget::updateBridgeTable();
    }
    else {
        cerr << "Error: " << err.message() << ", " << response.status() << "\n";
        // message that warns user of failed connection
        statusMessage_->setText("Unable to connect to Bridge");
        statusMessage_->setHidden(false);
    }
}

/**
 *   @brief  Generates list of Bridges connected to user account
 *
 *   @return  void
 *
 */
void BridgeScreenWidget::updateBridgeTable(){
    //remove any existing entries
    bridgeTable_->clear();

    //only populate if there are bridges existing
    if(account_->getNumBridges() > 0) {
        //create new row for headers <tr>
        WTableRow *tableRow = bridgeTable_->insertRow(bridgeTable_->rowCount());
        //table headers <th>
        tableRow->elementAt(0)->addWidget(new Wt::WText("Name"));
        tableRow->elementAt(1)->addWidget(new Wt::WText("Location"));
        tableRow->elementAt(2)->addWidget(new Wt::WText("URL"));
        tableRow->elementAt(3)->addWidget(new Wt::WText("Actions"));


        int counter = 0;
        //populate each row with user account bridges
        for(auto &bridge : account_->getBridges()) {
            //create new row for entry <tr>
            tableRow = bridgeTable_->insertRow(bridgeTable_->rowCount());

            //table data <td>
            tableRow->elementAt(0)->addWidget(new Wt::WText(bridge.getName()));
            tableRow->elementAt(1)->addWidget(new Wt::WText(bridge.getLocation()));

            string url = "http://" + bridge.getIP() + ":" + bridge.getPort() + "/api/" + bridge.getUsername();
            tableRow->elementAt(2)->addWidget(new Wt::WText(url));

            WPushButton *viewBridgeButton = new WPushButton("View");
            viewBridgeButton->clicked().connect(boost::bind(&BridgeScreenWidget::viewBridge, this, counter));

            WSplitButton *editBridgeButton = new WSplitButton("Edit");
            WPopupMenu *editBridgePopup = new WPopupMenu();
            WPopupMenuItem *share = new WPopupMenuItem("Share Bridge");
            editBridgePopup->addItem(share);

            editBridgeButton->dropDownButton()->setMenu(editBridgePopup);

            editBridgeButton->actionButton()->clicked().connect(boost::bind(&BridgeScreenWidget::editBridge, this, counter));

            share->triggered().connect(boost::bind(&BridgeScreenWidget::shareBridgeDialog, this, counter));

            WPushButton *removeBridgeButton = new WPushButton("Remove");
            removeBridgeButton->clicked().connect(boost::bind(&BridgeScreenWidget::removeBridge, this, counter));

            tableRow->elementAt(3)->addWidget(viewBridgeButton);
            tableRow->elementAt(3)->addWidget(editBridgeButton);
            tableRow->elementAt(3)->addWidget(removeBridgeButton);



            counter++;
        }
    }
}

/**
 *   @brief  View the specific Bridge connected to user account. Returns status message from Http method if the Bridge is unreachable.
 *
 *   @param   pos the position of the Bridge in user account vector to view
 *
 *   @return  void
 *
 */
void BridgeScreenWidget::viewBridge(int pos) {
    Bridge *bridge = account_->getBridgeAt(pos);
    string url = "http://" + bridge->getIP() + ":" + bridge->getPort() + "/api/" + bridge->getUsername();

    cout << "BRIDGE: Connecting to URL " << url << "\n";
    Wt::Http::Client *client = new Wt::Http::Client(this);
    client->setTimeout(2); //2 second timeout of request
    client->setMaximumResponseSize(1000000);
    client->done().connect(boost::bind(&BridgeScreenWidget::viewBridgeHttp, this, pos, _1, _2));

    if(client->get(url)) {
        WApplication::instance()->deferRendering();
    }
    else {
        cerr << "BRIDGE: Error in client->get(url) call\n";
    }
}

/**
 *   @brief  Function to handle the Http response generated by the Wt Http Client object in the viewBridge() function
 *
 *   @param  pos the position of the Bridge being accessed
 *   @param  *err stores the error code generated by an Http request, null if request was successful
 *   @param  &response stores the response message generated by the Http request
 *
 *   @return  void
 *
 */
void BridgeScreenWidget::viewBridgeHttp(int pos, boost::system::error_code err, const Wt::Http::Message &response)
{
    WApplication::instance()->resumeRendering();
    if (!err && response.status() == 200) {
        statusMessage_->setText("Successfully Connected to Bridge");
        statusMessage_->setHidden(false);

        Bridge *bridge = account_->getBridgeAt(pos);
        bridge->setJson(response.body());

        WApplication::instance()->setInternalPath("/bridges/" + to_string(pos), true);
    }
    else {
        cerr << "Error: " << err.message() << ", " << response.status() << "\n";
        statusMessage_->setText("Unable to connect to Bridge");
        statusMessage_->setHidden(false);
    }
}

/**
 *   @brief  Opens a WDialog box to edit info for specified Bridge
 *
 *   @param   pos the position of the Bridge in user account vector to view
 *
 *   @return  void
 *
 */
void BridgeScreenWidget::editBridge(int pos) {
    Bridge *bridge = account_->getBridgeAt(pos);

    bridgeEditDialog_ = new WDialog("Edit Bridge"); // title

    new WLabel("Bridge Name: ", bridgeEditDialog_->contents());
    bridgeEditName_ = new WLineEdit(bridgeEditDialog_->contents());
    bridgeEditName_->setValueText(bridge->getName());
    bridgeEditName_->setValidator(stringValidator_);
    new WBreak(bridgeEditDialog_->contents());

    new WLabel("Bridge Location: ", bridgeEditDialog_->contents());
    bridgeEditLocation_ = new WLineEdit(bridgeEditDialog_->contents());
    bridgeEditLocation_->setValueText(bridge->getLocation());
    bridgeEditLocation_->setValidator(stringValidator_);
    new WBreak(bridgeEditDialog_->contents());

    new WLabel("Bridge IP: ", bridgeEditDialog_->contents());
    bridgeEditIP_ = new WLineEdit(bridgeEditDialog_->contents());
    bridgeEditIP_->setValueText(bridge->getIP());
    bridgeEditIP_->setValidator(ipValidator_);
    new WBreak(bridgeEditDialog_->contents());

    new WLabel("Bridge Port: ", bridgeEditDialog_->contents());
    bridgeEditPort_ = new WLineEdit(bridgeEditDialog_->contents());
    bridgeEditPort_->setValueText(bridge->getPort());
    bridgeEditPort_->setValidator(portValidator_);
    new WBreak(bridgeEditDialog_->contents());

    new WLabel("Bridge Username: ", bridgeEditDialog_->contents());
    bridgeEditUsername_ = new WLineEdit(bridgeEditDialog_->contents());
    bridgeEditUsername_->setValueText(bridge->getUsername());
    bridgeEditUsername_->setValidator(stringValidator_);
    new WBreak(bridgeEditDialog_->contents());


    // make okay and cancel buttons, cancel sends a reject dialogstate, okay sends an accept
    WPushButton *ok = new WPushButton("OK", bridgeEditDialog_->contents());
    WPushButton *cancel = new WPushButton("Cancel", bridgeEditDialog_->contents());

    ok->clicked().connect(bridgeEditDialog_, &WDialog::accept);
    cancel->clicked().connect(bridgeEditDialog_, &WDialog::reject);

    // when the user is finished, call the updateBridge function
    bridgeEditDialog_->finished().connect(boost::bind(&BridgeScreenWidget::updateBridge, this, pos));
    bridgeEditDialog_->show();
}

/**
 *   @brief  Opens a WDialog box to share specified Bridge
 *
 *   @param   pos the position of the Bridge in user account vector to view
 *
 *   @return  void
 *
 */
void BridgeScreenWidget::shareBridgeDialog(int pos) {



    bridgeShareDialog_ = new WDialog("Share Bridge"); // title

    new WLabel("UserID to share with: ", bridgeShareDialog_->contents());
    bridgeShareUserid_ = new WLineEdit(bridgeShareDialog_->contents());

    bridgeShareUserid_->setValidator(useridValidator_);
    new WBreak(bridgeShareDialog_->contents());


    // make okay and cancel buttons, cancel sends a reject dialogstate, okay sends an accept
    WPushButton *ok = new WPushButton("OK", bridgeShareDialog_->contents());
    WPushButton *cancel = new WPushButton("Cancel", bridgeShareDialog_->contents());

    ok->clicked().connect(bridgeShareDialog_, &WDialog::accept);
    cancel->clicked().connect(bridgeShareDialog_, &WDialog::reject);

    // when the user is finished, call the shareBridge function
    bridgeShareDialog_->finished().connect(boost::bind(&BridgeScreenWidget::shareBridge, this, pos));

    bridgeShareDialog_->show();
}
/**
 *   @brief  share specific Bridge connected to user account to another user account. Returns status message from Http method if the Bridge is unreachable.
 *
 *   @param   pos the position of the Bridge in user account vector to update
 *
 *   @return  void
 *
 */
void BridgeScreenWidget::shareBridge(int pos) {
    // if user clicked "cancel" in dialog box, don't do anything
    if (bridgeShareDialog_->result() == WDialog::DialogCode::Rejected)
        return;

        ofstream outFile;
        ifstream inFile;
        string str;
        string filename = "credentials/" + bridgeShareUserid_->text().toUTF8() + ".txt";

        inFile.open(filename.c_str()); // opens username.txt


        Bridge *bridge = account_->getBridgeAt(pos);

    if(bridgeShareUserid_->validate() == 2 && inFile) {
        inFile.close();
        outFile.open(filename.c_str(), ios_base::app); // opens username.txt

        outFile << bridge->getName() << ", "
        << bridge->getLocation() << ", "
        << bridge->getIP() << ", "
        << bridge->getPort() << ", "
        << bridge->getUsername() << "\n";

        string successmsg = "Bridge share successful, next time they log in " + bridgeShareUserid_->text().toUTF8() + " will be able to control this bridge";

        statusMessage_->setText(successmsg);
        statusMessage_->removeStyleClass("error");
        statusMessage_->setHidden(false);
    }
    else {
        string errmsg = "Error sharing bridge: Invalid username input ";

        statusMessage_->setText(errmsg);
        statusMessage_->setStyleClass("error");
        statusMessage_->setHidden(false);
    }
}


/**
 *   @brief  Edit specific Bridge connected to user account. Returns status message from Http method if the Bridge is unreachable.
 *
 *   @param   pos the position of the Bridge in user account vector to update
 *
 *   @return  void
 *
 */
void BridgeScreenWidget::updateBridge(int pos) {
    // if user clicked "cancel" in dialog box, don't do anything
    if (bridgeEditDialog_->result() == WDialog::DialogCode::Rejected)
        return;

    if(bridgeEditName_->validate() == 2 &&
       bridgeEditLocation_->validate() == 2 &&
       bridgeEditIP_->validate() == 2 &&
       bridgeEditPort_->validate() == 2 &&
       bridgeEditUsername_->validate() == 2) {

        string url = "http://" + bridgeEditIP_->text().toUTF8() + ":" + bridgeEditPort_->text().toUTF8() + "/api/" + bridgeEditUsername_->text().toUTF8();

        cout << "BRIDGE: Connecting to URL " << url << "\n";
        Wt::Http::Client *client = new Wt::Http::Client(this);
        client->setTimeout(2); //2 second timeout of request
        client->setMaximumResponseSize(1000000);
        client->done().connect(boost::bind(&BridgeScreenWidget::updateBridgeHttp, this, pos, _1, _2));

        if(client->get(url)) {
            WApplication::instance()->deferRendering();
        }
        else {
            cerr << "BRIDGE: Error in client->get(url) call\n";
        }
    }
    else {
        string errmsg = "Error updating Bridge: Invalid input for: ";
        if(bridgeEditName_->validate() != 2) errmsg += "Name ";
        if(bridgeEditLocation_->validate() != 2) errmsg += "Location ";
        if(bridgeEditIP_->validate() != 2) errmsg += "IP ";
        if(bridgeEditPort_->validate() != 2) errmsg += "Port ";
        if(bridgeEditUsername_->validate() != 2) errmsg += "Username ";

        statusMessage_->setText(errmsg);
        statusMessage_->setHidden(false);
    }
}

/**
 *   @brief  Function to handle the Http response generated by the Wt Http Client object in the updateBridge() function
 *
 *   @param  pos the position of the Bridge being accessed
 *   @param  *err stores the error code generated by an Http request, null if request was successful
 *   @param  &response stores the response message generated by the Http request
 *
 *   @return  void
 *
 */
void BridgeScreenWidget::updateBridgeHttp(int pos, boost::system::error_code err, const Wt::Http::Message &response)
{
    WApplication::instance()->resumeRendering();
    if (!err && response.status() == 200) {
        statusMessage_->setText("Successfully updated Bridge.");
        statusMessage_->setHidden(false);

        Bridge *bridge = account_->getBridgeAt(pos);
        bridge->setName(bridgeEditName_->text().toUTF8());
        bridge->setLocation(bridgeEditLocation_->text().toUTF8());
        bridge->setIP(bridgeEditIP_->text().toUTF8());
        bridge->setPort(bridgeEditPort_->text().toUTF8());
        bridge->setUsername(bridgeEditUsername_->text().toUTF8());

        account_->writeFile();
        BridgeScreenWidget::updateBridgeTable();
    }
    else {
        cerr << "Error: " << err.message() << ", " << response.status() << "\n";
        statusMessage_->setText("Error updating Bridge: Invalid connection info.");
        statusMessage_->setHidden(false);
    }
}

/**
 *   @brief  Removes a Bridge from user account
 *
 *   @param   pos the position of the Bridge in the vector to remove
 *
 *   @return  void
 *
 */
void BridgeScreenWidget::removeBridge(int pos){
    if(account_->getNumBridges() == 0) {
        statusMessage_->setText("No bridges to delete!");
        statusMessage_->setHidden(false);
    }
    else {
        statusMessage_->setText("Bridge successfully removed!");
        statusMessage_->setHidden(false);
        account_->removeBridgeAt(pos);
        account_->writeFile(); //update credentials file
        BridgeScreenWidget::updateBridgeTable();
    }
}









