/**
 *  @file       LightManagementWidget.cpp
 *  @author     CS 3307 - Team 13
 *  @date       11/16/2017
 *  @version    1.0
 *
 *  @brief      CS 3307, Hue Light Application screen for managing lights/groups/schedules on a bridge
 *
 *  @section    This class represents the light management screen. This screen allows the user to
 *              create, edit, and delete their lights/groups/schedules on the user selected bridge.
 *
 *
 */

#include <Wt/WText>
#include <string>
#include <vector>
#include <unistd.h>
#include "LightManagementWidget.h"
#include <Wt/WContainerWidget>
#include <Wt/WComboBox>
#include <Wt/WSplitButton>
#include <Wt/WPopupMenu>
#include <Wt/WPopupMenuItem>
#include <Wt/WColor>
#include <Wt/WCssDecorationStyle>
#include <Wt/WMenu>
#include <Wt/WStackedWidget>
#include <Wt/WImage>
#include <Wt/WBorderLayout>
#include <Wt/Json/Value>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
#include <Wt/Json/Serializer>
#include <Wt/Json/Array>

using namespace Wt;
using namespace std;

/**
 *   @brief  Light Management Widget constructor
 *
 *   @param  *parent is a pointer the the containerwidget that stores this widget
 *   @param  *bridge is a pointer to the Bridge object
 *   @param  *main is a pointer to the app's welcome screen
 */
LightManagementWidget::LightManagementWidget(WContainerWidget *parent, Bridge *bridge, WelcomeScreen *main):
WContainerWidget(parent),
overviewWidget_(0),
lightsWidget_(0),
groupsWidget_(0),
schedulesWidget_(0)
{
    setContentAlignment(AlignLeft);
    parent_ = main;
    bridge_ = bridge;
    setStyleClass("w3-animate-opacity");
}


/**
 *   @brief  Update function, clears the widget and re-populates with elements of the light management
 *
 *   @return  void
 *
 */
void LightManagementWidget::update()
{
    clear(); // everytime you come back to page, reset the widgets

    WBorderLayout *layout = new WBorderLayout();
    WContainerWidget *left = new WContainerWidget();
    WContainerWidget *central = new WContainerWidget();
    central->setContentAlignment(AlignCenter);

    layout->addWidget(left, WBorderLayout::West);
    layout->addWidget(central, WBorderLayout::Center);


    //create refreshButton for refreshing Bridge JSON data
    WPushButton *refreshButton = new WPushButton("Refresh Bridge");
    refreshButton->clicked().connect(boost::bind(&LightManagementWidget::refreshBridge, this));


    //Menu to navigate through different pages
    WMenu *menu = new WMenu();
    menu->setStyleClass("nav nav-pills nav-stacked");
    menu->setWidth(150);
    left->addWidget(menu);

    WMenuItem *overviewMenuItem = new WMenuItem("Overview");
    menu->addItem(overviewMenuItem);
    overviewMenuItem->triggered().connect(this, &LightManagementWidget::viewOverviewWidget);

    WMenuItem *lightMenuItem = new WMenuItem("Lights");
    menu->addItem(lightMenuItem);
    lightMenuItem->triggered().connect(this, &LightManagementWidget::viewLightsWidget);

    WMenuItem *groupsMenuItem = new WMenuItem("Groups");
    menu->addItem(groupsMenuItem);
    groupsMenuItem->triggered().connect(this, &LightManagementWidget::viewGroupsWidget);

    WMenuItem *schedulesMenuItem = new WMenuItem("Schedules");
    menu->addItem(schedulesMenuItem);
    schedulesMenuItem->triggered().connect(this, &LightManagementWidget::viewSchedulesWidget);

    //stack to handle different menu pages
    lightManagementStack_ = new WStackedWidget();
    lightManagementStack_->setContentAlignment(AlignCenter);
    central->addWidget(lightManagementStack_);
    new WBreak(central);
    central->addWidget(refreshButton);

    //create overviewWidget
    overviewWidget_ = new WContainerWidget(lightManagementStack_);
    overviewWidget_->setContentAlignment(AlignCenter);
    //Widget title
    WText *overviewTitle = new WText("Bridge Overview", overviewWidget_);
    overviewTitle->setStyleClass("title");
    new WBreak(overviewWidget_);
    new WBreak(overviewWidget_);
    //Widget Contents
    new WText("Bridge Name: " + bridge_->getName(), overviewWidget_);
    new WBreak(overviewWidget_);
    new WText("Bridge Location: " + bridge_->getLocation(), overviewWidget_);

    //create lightsWidget
    lightsWidget_ = new WContainerWidget(lightManagementStack_);
    lightsWidget_->setContentAlignment(AlignCenter);
    //Lights title
    WText *lightsTitle = new WText("Lights", lightsWidget_);
    lightsTitle->setStyleClass("title");
    new WBreak(lightsWidget_);
    new WBreak(lightsWidget_);
    //Lights table
    lightsTable_ = new WTable(lightsWidget_);
    lightsTable_->setHeaderCount(1); //set first row as header
    updateLightsTable();

    //create groupsWidget
    groupsWidget_ = new WContainerWidget(lightManagementStack_);
    groupsWidget_->setContentAlignment(AlignCenter);
    //Groups title
    WText *groupsTitle = new WText("Groups", groupsWidget_);
    groupsTitle->setStyleClass("title");
    new WBreak(groupsWidget_);
    new WBreak(groupsWidget_);
    //Groups table
    WPushButton *newGroupButton = new WPushButton("Add +");
    newGroupButton->clicked().connect(boost::bind(&LightManagementWidget::createGroupDialog, this));
    groupsWidget_->addWidget(newGroupButton);
    groupsTable_ = new WTable(groupsWidget_);
    groupsTable_->setHeaderCount(1); //set first row as header
    updateGroupsTable();

    //create schedulesWidget
    schedulesWidget_ = new WContainerWidget(lightManagementStack_);
    schedulesWidget_->setContentAlignment(AlignCenter);
    //Schedules title
    WText *schedulesTitle = new WText("Schedules", schedulesWidget_);
    schedulesTitle->setStyleClass("title");
    new WBreak(schedulesWidget_);
    new WBreak(schedulesWidget_);
    //Groups table
    WPushButton *newScheduleButton = new WPushButton("Add +");
    newScheduleButton->clicked().connect(boost::bind(&LightManagementWidget::createScheduleDialog, this));
    schedulesWidget_->addWidget(newScheduleButton);
    schedulesTable_ = new WTable(schedulesWidget_);
    schedulesTable_->setHeaderCount(1);
    updateSchedulesTable();

    //initialize page with Overview as initial view
    overviewMenuItem->select();

    //WContainer for RGB Colour Picker used for selecting Colours
    rgbContainer_ = new WContainerWidget();
    new WText("Red: ", rgbContainer_);
    redSlider = new WSlider(rgbContainer_);
    new WBreak(rgbContainer_);
    new WText("Green: ", rgbContainer_);
    greenSlider = new WSlider(rgbContainer_);
    new WBreak(rgbContainer_);
    new WText("Blue: ", rgbContainer_);
    blueSlider = new WSlider(rgbContainer_);
    new WBreak(rgbContainer_);
    redSlider->resize(200,20);
    greenSlider->resize(200,20);
    blueSlider->resize(200,20);

    redSlider->setMinimum(0);
    redSlider->setMaximum(255);
    greenSlider->setMinimum(0);
    greenSlider->setMaximum(255);
    blueSlider->setMinimum(0);
    blueSlider->setMaximum(255);

    WCssDecorationStyle *colour = new WCssDecorationStyle();

    colour->setBackgroundColor(WColor(redSlider->value(), greenSlider->value(), blueSlider->value()));
    rgbContainer_->setDecorationStyle(*colour);

    redSlider->valueChanged().connect(bind([=] {
        colour->setBackgroundColor(WColor(redSlider->value(),
                                          greenSlider->value(),
                                          blueSlider->value()));
        rgbContainer_->setDecorationStyle(*colour);
    }));

    greenSlider->valueChanged().connect(bind([=] {
        colour->setBackgroundColor(WColor(redSlider->value(),
                                          greenSlider->value(),
                                          blueSlider->value()));
        rgbContainer_->setDecorationStyle(*colour);
    }));

    blueSlider->valueChanged().connect(bind([=] {
        colour->setBackgroundColor(WColor(redSlider->value(),
                                          greenSlider->value(),
                                          blueSlider->value()));
        rgbContainer_->setDecorationStyle(*colour);
    }));

    //WContainer for Hue/Saturation Colour Picker used for selecting colours
    hueSatContainer_ = new WContainerWidget();
    new WText("Hue: ", hueSatContainer_);
    hueSlider = new WSlider(hueSatContainer_);
    new WBreak(hueSatContainer_);
    new WText("Saturation: ", hueSatContainer_);
    satSlider = new WSlider(hueSatContainer_);
    new WBreak(hueSatContainer_);
    new WText("Brightness: ", hueSatContainer_);
    briSlider = new WSlider(hueSatContainer_);
    new WBreak(hueSatContainer_);

    hueSlider->resize(200,20);
    satSlider->resize(200,20);
    briSlider->resize(200,20);

    hueSlider->setMinimum(0);
    hueSlider->setMaximum(65280);
    satSlider->setMinimum(0);
    satSlider->setMaximum(255);
    briSlider->setMinimum(0);
    briSlider->setMaximum(254);

    WCssDecorationStyle *hsColour = new WCssDecorationStyle();

    struct rgb *hsRgb = ColourConvert::hsv2rgb((float)hueSlider->value(),(float) satSlider->value(),
                                              (float)briSlider->value());
    hsColour->setBackgroundColor(WColor(hsRgb->r, hsRgb->g, hsRgb->b));


    hueSatContainer_->setDecorationStyle(*hsColour);

    hueSlider->valueChanged().connect(bind([=] {
        struct rgb *hsRgb = ColourConvert::hsv2rgb((float)hueSlider->value(),(float) satSlider->value(),
                                              (float)briSlider->value());
        hsColour->setBackgroundColor(WColor(hsRgb->r,
                                          hsRgb->g,
                                          hsRgb->b));
        hueSatContainer_->setDecorationStyle(*hsColour);
    }));
    satSlider->valueChanged().connect(bind([=] {
        struct rgb *hsRgb = ColourConvert::hsv2rgb((float)hueSlider->value(),(float) satSlider->value(),
                                              (float)briSlider->value());
        hsColour->setBackgroundColor(WColor(hsRgb->r,
                                          hsRgb->g,
                                          hsRgb->b));
        hueSatContainer_->setDecorationStyle(*hsColour);
    }));

    briSlider->valueChanged().connect(bind([=] {
        struct rgb *hsRgb = ColourConvert::hsv2rgb((float)hueSlider->value(),(float) satSlider->value(),
                                              (float)briSlider->value());
        hsColour->setBackgroundColor(WColor(hsRgb->r,
                                          hsRgb->g,
                                          hsRgb->b));
        hueSatContainer_->setDecorationStyle(*hsColour);
    }));

    setLayout(layout, AlignTop | AlignJustify);
}

/**
 *   @brief  View overview function, sets the current widget to the overview widget when clicked on
 *           the menu.
 *
 *   @return  void
 *
 */
void LightManagementWidget::viewOverviewWidget(){
    lightManagementStack_->setCurrentWidget(overviewWidget_);
}

/**
 *   @brief  View lights function, sets the current widget to the light widget when clicked on
 *           the menu.
 *
 *   @return  void
 *
 */
void LightManagementWidget::viewLightsWidget(){
    lightManagementStack_->setCurrentWidget(lightsWidget_);
}
/**
 *   @brief  View groups function, sets the current widget shown to groups widget when clicked on
 *           the menu.
 *
 *   @return  void
 *
 */
void LightManagementWidget::viewGroupsWidget(){
    lightManagementStack_->setCurrentWidget(groupsWidget_);
}

/**
 *   @brief  View schedule function, sets the current widget shown to schedule widget when clicked on
 *           the menu.
 *
 *   @return  void
 *
 */
void LightManagementWidget::viewSchedulesWidget(){
    lightManagementStack_->setCurrentWidget(schedulesWidget_);
}

/**
 *   @brief  Opens a WDialog box to edit rgb values for the specified light
 *
 *   @param   light is the current light object to edit
 *
 *   @return  void
 *
 */
void LightManagementWidget::editRGBDialog(Light *light) {

    editRGBDialog_ = new WDialog("Change Colour"); // title

    editRGBDialog_->contents()->addWidget(rgbContainer_);

    //get current RGB values and set the Slider and BG colours to match
    struct rgb *currentRGBVals = ColourConvert::xy2rgb((float)light->getX(),
                                                       (float)light->getY(),
                                                       (float)light->getBri());
    redSlider->setValue(currentRGBVals->r);
    greenSlider->setValue(currentRGBVals->g);
    blueSlider->setValue(currentRGBVals->b);
    redSlider->setDisabled(false);
    greenSlider->setDisabled(false);
    blueSlider->setDisabled(false);
    WCssDecorationStyle *colour = new WCssDecorationStyle();
    colour->setBackgroundColor(WColor(redSlider->value(), greenSlider->value(), blueSlider->value()));
    rgbContainer_->setDecorationStyle(*colour);

    new WBreak(editRGBDialog_->contents());

    // make okay and cancel buttons, cancel sends a reject dialogstate, okay sends an accept
    WPushButton *ok = new WPushButton("OK", editRGBDialog_->contents());
    WPushButton *cancel = new WPushButton("Cancel", editRGBDialog_->contents());

    ok->clicked().connect(editRGBDialog_ ,&WDialog::accept);
    cancel->clicked().connect(editRGBDialog_, &WDialog::reject);

    // when the user is finished, call the updateBridge function
    editRGBDialog_->finished().connect(boost::bind(&LightManagementWidget::updateLightXY, this, light));
    editRGBDialog_->show();
}

/**
 *   @brief  Opens a WDialog box to convert hue, saturation, and brightness to rgb color
 *
 *   @param   light is the current light object to edit
 *
 *   @return  void
 *
 */
void LightManagementWidget::editHueSatDialog(Light *light) {

    editHueSatDialog_ = new WDialog("Change Hue/Saturation"); // title

    editHueSatDialog_->contents()->addWidget(hueSatContainer_);

    //get current RGB values and set the Slider and BG colours to match
    int hue = light->getHue();
    int sat = light->getSat();
    int bri = light->getBri();

    hueSlider->setValue(hue);
    satSlider->setValue(sat);
    briSlider->setValue(bri);

    hueSlider->setDisabled(false);
    satSlider->setDisabled(false);
    briSlider->setDisabled(false);

    WCssDecorationStyle *hsColour = new WCssDecorationStyle();

    if (light->getColormode() == "hs") {
        hsColour->setBackgroundColor(WColor(hueSlider->value(), satSlider->value(), briSlider->value()));
        hueSatContainer_->setDecorationStyle(*hsColour);
    }

    new WBreak(editHueSatDialog_->contents());

    // make okay and cancel buttons, cancel sends a reject dialogstate, okay sends an accept
    WPushButton *ok = new WPushButton("OK", editHueSatDialog_->contents());
    WPushButton *cancel = new WPushButton("Cancel", editHueSatDialog_->contents());

    ok->clicked().connect(editHueSatDialog_ ,&WDialog::accept);
    cancel->clicked().connect(editHueSatDialog_, &WDialog::reject);

    // when the user is finished, call the updateBridge function
    editHueSatDialog_->finished().connect(boost::bind(&LightManagementWidget::updateLightHS, this, light));
    editHueSatDialog_->show();
}

/**
 *   @brief  Update lights table function, clears the current table and re-populates it with
 *           all the lights that are in the bridge. The user can edit the properties of lights
 *           from this table.
 *
 *   @return  void
 *
 */
void LightManagementWidget::updateLightsTable() {
    lightsTable_->clear();

    //create new row for headers <tr>
    WTableRow *tableRow = lightsTable_->insertRow(lightsTable_->rowCount());
    //table headers <th>
    tableRow->elementAt(0)->addWidget(new Wt::WText("Light #"));
    tableRow->elementAt(1)->addWidget(new Wt::WText("Name"));
    tableRow->elementAt(2)->addWidget(new Wt::WText("Transition"));
    tableRow->elementAt(3)->addWidget(new Wt::WText("Brightness"));
    tableRow->elementAt(4)->addWidget(new Wt::WText("Actions"));


    //convert json string into json object
    Json::Object bridgeJson;
    Json::parse(bridge_->getJson(), bridgeJson);
    Json::Object lights = bridgeJson.get("lights");

    set<string> data = lights.names();
    for(string num : data) {
        Json::Object lightData = lights.get(num);
        Light *light = new Light(num, lightData);

        //create new row for entry <tr>
        tableRow = lightsTable_->insertRow(lightsTable_->rowCount());

        //table data <td>
        tableRow->elementAt(0)->addWidget(new WText(light->getLightnum()));
        tableRow->elementAt(1)->addWidget(new WText(light->getName()));

        //transition definer
        WLineEdit *editLightTransition = new WLineEdit();
        editLightTransition->resize(40,20);
        //display transition time to user (transition time is stored as multiple of 100ms)
        editLightTransition->setValueText(boost::lexical_cast<string>(light->getTransition()));
        tableRow->elementAt(2)->addWidget(editLightTransition);
        editLightTransition->setDisabled(!light->getOn());  //disable if light off
        intValidator = new WIntValidator(0, 100, tableRow->elementAt(2)); //100 second maximum
        intValidator->setMandatory(true);
        editLightTransition->setValidator(intValidator);
        editLightTransition->changed().connect(bind([=] {
            light->setTransition(boost::lexical_cast<int>(editLightTransition->valueText()));
        }));
        tableRow->elementAt(2)->addWidget(new WText(" seconds"));

        //brightness slider
        WSlider *brightnessSlider_ = new WSlider();
        brightnessSlider_->resize(160,20);
        brightnessSlider_->setMinimum(0);
        brightnessSlider_->setMaximum(254);
        brightnessSlider_->setValue(light->getBri());
        brightnessSlider_->setDisabled(!light->getOn()); //disable if light off
        brightnessSlider_->valueChanged().connect(boost::bind(&LightManagementWidget::updateLightBri, this, brightnessSlider_, light));
        tableRow->elementAt(3)->addWidget(brightnessSlider_);

        string onButton = light->getOn() == 1 ? "On" : "Off";
        WPushButton *switchButton_ = new WPushButton(onButton);
        switchButton_->clicked().connect(boost::bind(&LightManagementWidget::updateLightOn, this, switchButton_, light));

        WPushButton *editLightButton_ = new WPushButton("Edit");
        editLightButton_->clicked().connect(boost::bind(&LightManagementWidget::editLightDialog, this, light));

        WSplitButton *colourButton_ = new WSplitButton("Colour");
        WPopupMenu *colourPopup = new WPopupMenu();
        colourButton_->dropDownButton()->setMenu(colourPopup);
        WPopupMenuItem *hsv = new WPopupMenuItem("Hue/Saturation");
        colourPopup->addItem(hsv);
        hsv->triggered().connect(boost::bind(&LightManagementWidget::editHueSatDialog, this, light));
        colourButton_->setDisabled(!light->getOn());  //disable if light off
        colourButton_->actionButton()->clicked().connect(boost::bind(&LightManagementWidget::editRGBDialog, this, light));

        tableRow->elementAt(4)->addWidget(switchButton_);
        tableRow->elementAt(4)->addWidget(colourButton_);
        tableRow->elementAt(4)->addWidget(editLightButton_);

        WPushButton *removeLightButton = new WPushButton("Remove");
        removeLightButton->clicked().connect(boost::bind(&LightManagementWidget::removeLight, this, light));
        tableRow->elementAt(4)->addWidget(removeLightButton);
    }
}

/**
 *   @brief  Edit lights function, when the edit button is clicked, a window where the user can
 *           change any property of the selected light appears.
 *
 *   @param  light is the light object that was selected to edit from the table.
 *
 *   @return  void
 *
 */
void LightManagementWidget::editLightDialog(Light *light) {
    editLightDialog_ = new WDialog("Edit Light #" + boost::lexical_cast<string>(light->getLightnum())); // title

    new WLabel("Light Name: ", editLightDialog_->contents());
    editLightName = new WLineEdit(editLightDialog_->contents());
    editLightName->setValueText(light->getName());
    new WBreak(editLightDialog_->contents());

    // make okay and cancel buttons, cancel sends a reject dialogstate, okay sends an accept
    WPushButton *ok = new WPushButton("OK", editLightDialog_->contents());
    WPushButton *cancel = new WPushButton("Cancel", editLightDialog_->contents());

    ok->clicked().connect(editLightDialog_, &WDialog::accept);
    cancel->clicked().connect(editLightDialog_, &WDialog::reject);

    // when the user is finished, call the updateLight function
    editLightDialog_->finished().connect(boost::bind(&LightManagementWidget::updateLightInfo, this, light));
    editLightDialog_->show();
}

/**
 *   @brief  Remove lights function, removes the selected light from the current
 *           bridge. Does not work on Hue Emulator but will work with real bridges.
 *
 *   @param  light is the light object to remove from the bridge.
 *
 *   @return  void
 *
 */
void LightManagementWidget::removeLight(Light *light) {
    string url = "http://" + bridge_->getIP() + ":" + bridge_->getPort() + "/api/" + bridge_->getUsername() + "/lights/" + light->getLightnum().toUTF8();
    deleteRequest(url);
}

/**
 *   @brief  Update lights function, creates a JSON request to change properties of
 *           a given light based on user's inputs in the edit light dialog box.
 *
 *   @param  light is the light object to update from the bridge.
 *
 *   @return  void
 *
 */
void LightManagementWidget::updateLightInfo(Light *light){
    if (editLightDialog_->result() == WDialog::DialogCode::Rejected)
        return;

    string url = "http://" + bridge_->getIP() + ":" + bridge_->getPort() + "/api/" + bridge_->getUsername() + "/lights/" + boost::lexical_cast<string>(light->getLightnum());

    Http::Message *data = new Http::Message();
    data->addBodyText("{\"name\":\"" + boost::lexical_cast<string>(editLightName->text().toUTF8()) + "\"}");

    cout << "Light: Updating light name at URL " << url << "\n";
    Wt::Http::Client *client = new Wt::Http::Client(this);
    client->setTimeout(2); //2 second timeout of request
    client->setMaximumResponseSize(1000000);
    client->done().connect(boost::bind(&LightManagementWidget::handlePutHttp, this, _1, _2));

    if(client->put(url, *data)) {
        WApplication::instance()->deferRendering();
    }
    else {
        cerr << "Light: Error in client->put(url) call\n";
    }
}

/**
 *   @brief  Update brightness function, creates a JSON request to update the selected light's
 *           brightness value given the brightness slider's value.
 *
 *   @param  slider_ is the brightness slider that contains a value between 0-254.
 *   @param  light is the light object to update the brightness value.
 *
 *   @return  void
 *
 */
void LightManagementWidget::updateLightBri(WSlider *slider_, Light *light){
    string url = "http://" + bridge_->getIP() + ":" + bridge_->getPort() + "/api/" + bridge_->getUsername() + "/lights/" + light->getLightnum().toUTF8() + "/state";
    Http::Message *data = new Http::Message();
    data->addBodyText("{\"bri\":" + boost::lexical_cast<string>(slider_->value()) + ", \"transitiontime\":" + boost::lexical_cast<string>(light->getTransition()) + "}");

    cout << "Light: Updating light brightness at URL " << url << "\n";
    Wt::Http::Client *client = new Wt::Http::Client(this);
    client->setTimeout(2); //2 second timeout of request
    client->setMaximumResponseSize(1000000);
    client->done().connect(boost::bind(&LightManagementWidget::handlePutHttp, this, _1, _2));

    if(client->put(url, *data)) {
        WApplication::instance()->deferRendering();
    }
    else {
        cerr << "Light: Error in client->put(url) call\n";
    }
}

/**
 *   @brief  Update on/off function, creates a JSON request to toggle the selected light's
 *           on/off value.
 *
 *   @param  button_ is the clickable button that contains on/off value.
 *   @param  light is the light object to update the on/off value.
 *
 *   @return  void
 *
 */
void LightManagementWidget::updateLightOn(WPushButton *button_, Light *light){
    //set value string to reflect current state of the button
    string value = button_->text() == "On" ? "False" : "True";

    string url = "http://" + bridge_->getIP() + ":" + bridge_->getPort() + "/api/" + bridge_->getUsername() + "/lights/" + light->getLightnum().toUTF8() + "/state";
    Http::Message *data = new Http::Message();

    //can only set transition time while light is on
    if(light->getOn()) {
        data->addBodyText("{\"on\":" + value + ", \"transitiontime\":" + boost::lexical_cast<string>(light->getTransition()) + "}");
    }
    else {
        data->addBodyText("{\"on\":" + value + "}");
    }

    cout << "Light: Updating light brightness at URL " << url << "\n";
    Wt::Http::Client *client = new Wt::Http::Client(this);
    client->setTimeout(2); //2 second timeout of request
    client->setMaximumResponseSize(1000000);
    client->done().connect(boost::bind(&LightManagementWidget::handlePutHttp, this, _1, _2));

    if(client->put(url, *data)) {
        WApplication::instance()->deferRendering();
    }
    else {
        cerr << "Light: Error in client->put(url) call\n";
    }
}

/**
 *   @brief  Update light's XY function, creates a JSON request to update the selected light's
 *           color value given the XY values.
 *
 *   @param  light is the light object to update the XY value.
 *
 *   @return  void
 *
 */
void LightManagementWidget::updateLightXY(Light *light){
    if (editRGBDialog_->result() == WDialog::DialogCode::Rejected)
        return;

    string url = "http://" + bridge_->getIP() + ":" + bridge_->getPort() + "/api/" + bridge_->getUsername() + "/lights/" + light->getLightnum().toUTF8() + "/state";

    struct xy *cols = ColourConvert::rgb2xy(redSlider->value(), greenSlider->value(), blueSlider->value());
    
    Http::Message *data = new Http::Message();
    data->addBodyText("{\"xy\":[" + boost::lexical_cast<string>(cols->x) + "," + boost::lexical_cast<string>(cols->y) + "]" + ", \"transitiontime\":" + boost::lexical_cast<string>(light->getTransition()) + ",\"bri\":" + boost::lexical_cast<string>((int)(cols->brightness)) + "}");

    cout << "Light: Updating light brightness at URL " << url << "\n";
    Wt::Http::Client *client = new Wt::Http::Client(this);
    client->setTimeout(2); //2 second timeout of request
    client->setMaximumResponseSize(1000000);
    client->done().connect(boost::bind(&LightManagementWidget::handlePutHttp, this, _1, _2));

    if(client->put(url, *data)) {
        WApplication::instance()->deferRendering();
    }
    else {
        cerr << "Light: Error in client->put(url) call\n";
    }
}

/**
 *   @brief  Update HS function, creates a JSON request to update the selected light's
 *           hue, saturation, brightness, and transition time values.
 *
 *   @param  light is the light object to update.
 *
 *   @return  void
 *
 */
void LightManagementWidget::updateLightHS(Light *light){
    if (editHueSatDialog_->result() == WDialog::DialogCode::Rejected)
        return;

    string url = "http://" + bridge_->getIP() + ":" + bridge_->getPort() + "/api/" + bridge_->getUsername() + "/lights/" + light->getLightnum().toUTF8() + "/state";



    Http::Message *data = new Http::Message();
    data->addBodyText("{\"hue\":" + boost::lexical_cast<string>(hueSlider->value()) + ", \"sat\":" +
                      boost::lexical_cast<string>(satSlider->value()) +
                      ", \"bri\":" + boost::lexical_cast<string>(briSlider->value()) +
                      ", \"transitiontime\":" + boost::lexical_cast<string>(light->getTransition()) + "}");

    cout << "Light: Updating light hue/sat at URL " << url << "\n";
    Wt::Http::Client *client = new Wt::Http::Client(this);
    client->setTimeout(2); //2 second timeout of request
    client->setMaximumResponseSize(1000000);
    client->done().connect(boost::bind(&LightManagementWidget::handlePutHttp, this, _1, _2));

    if(client->put(url, *data)) {
        WApplication::instance()->deferRendering();
    }
    else {
        cerr << "Light: Error in client->put(url) call\n";
    }
}

/**
 *   @brief  Update groups table function, clears the current table and re-populates it with
 *           all the groups that are in the bridge.
 *
 *   @return  void
 *
 */
void LightManagementWidget::updateGroupsTable() {
    groupsTable_->clear();

    //create new row for headers <tr>
    WTableRow *tableRow = groupsTable_->insertRow(groupsTable_->rowCount());
    //table headers <th>
    tableRow->elementAt(0)->addWidget(new Wt::WText("Group #"));
    tableRow->elementAt(1)->addWidget(new Wt::WText("Name"));
    tableRow->elementAt(2)->addWidget(new Wt::WText("Lights"));
    tableRow->elementAt(3)->addWidget(new Wt::WText("Transitions"));
    tableRow->elementAt(4)->addWidget(new Wt::WText("Actions"));
    //convert json string into json object
    Json::Object bridgeJson;
    Json::parse(bridge_->getJson(), bridgeJson);
    Json::Object groups = bridgeJson.get("groups");

    set<string> data = groups.names();
    for(string num : data) {
        Json::Object groupData = groups.get(num);
        Group *group = new Group(num, groupData);

        //create new row for entry <tr>
        tableRow = groupsTable_->insertRow(groupsTable_->rowCount());

        //table data <td>
        tableRow->elementAt(0)->addWidget(new WText(group->getGroupnum()));
        tableRow->elementAt(1)->addWidget(new WText(group->getName()));

        for(WString lightNum : group->getLights()) {
            tableRow->elementAt(2)->addWidget(new WText(lightNum));
            tableRow->elementAt(2)->addWidget(new WBreak());
        }

        //transition definer
        WLineEdit *editGroupTransition = new WLineEdit();
        editGroupTransition->resize(40,20);
        //display transition time to user in seconds (transition time is stored as multiple of 100ms)
        editGroupTransition->setValueText(boost::lexical_cast<string>(group->getTransition()));
        tableRow->elementAt(3)->addWidget(editGroupTransition);
        intValidator = new WIntValidator(0, 100, tableRow->elementAt(2)); //100 second maximum
        intValidator->setMandatory(true);
        editGroupTransition->setValidator(intValidator);
        editGroupTransition->changed().connect(bind([=] {
            group->setTransition(boost::lexical_cast<int>(editGroupTransition->valueText()));
        }));
        tableRow->elementAt(3)->addWidget(new WText(" seconds"));

        WPushButton *editGroupButton_ = new WPushButton("Edit");
        editGroupButton_->clicked().connect(boost::bind(&LightManagementWidget::editGroupDialog, this, group));
        tableRow->elementAt(4)->addWidget(editGroupButton_);

        WPushButton *advancedButton_ = new WPushButton("Advanced");
        advancedButton_->clicked().connect(boost::bind(&LightManagementWidget::groupAdvancedDialog, this, group));
        tableRow->elementAt(4)->addWidget(advancedButton_);

        // remove group
        WPushButton *removeGroupButton = new WPushButton("Remove");
        removeGroupButton->clicked().connect(boost::bind(&LightManagementWidget::removeGroup, this, group));
        tableRow->elementAt(4)->addWidget(removeGroupButton);
    }
}

/**
 *   @brief  Create groups function, when the create button is clicked, a dialog appears where the user
 *           can create a group and select lights to be added to the group.
 *
 *   @return  void
 *
 */
void LightManagementWidget::createGroupDialog() {
    createGroupDialog_ = new WDialog("Create a Group"); // title

    new WLabel("Group Name: ", createGroupDialog_->contents());
    groupName = new WLineEdit(createGroupDialog_->contents());
    groupName->setValueText("Custom Group");
    new WBreak(createGroupDialog_->contents());

    new WLabel("Lights: ", createGroupDialog_->contents());
    Json::Object bridgeJson;
    Json::parse(bridge_->getJson(), bridgeJson);
    Json::Object lights = bridgeJson.get("lights");

    set<string> data = lights.names();
    lightBoxes.clear(); //empty lightbox
    for(string num : data) {
        WCheckBox *lightButton_ = new WCheckBox(num, createGroupDialog_->contents());
        lightBoxes.push_back(lightButton_);
    }
    new WBreak(createGroupDialog_->contents());

    // make okay and cancel buttons, cancel sends a reject dialogstate, okay sends an accept
    WPushButton *ok = new WPushButton("OK", createGroupDialog_->contents());
    WPushButton *cancel = new WPushButton("Cancel", createGroupDialog_->contents());

    ok->clicked().connect(createGroupDialog_, &WDialog::accept);
    cancel->clicked().connect(createGroupDialog_, &WDialog::reject);

    createGroupDialog_->finished().connect(boost::bind(&LightManagementWidget::createGroup, this));

    createGroupDialog_->show();
}

/**
 *   @brief  Create group function, creates a JSON request to add a new group to the
 *           bridge, using the user defined options selected in create group dialog.
 *
 *   @return  void
 *
 */
void LightManagementWidget::createGroup(){
    if (createGroupDialog_->result() == WDialog::DialogCode::Rejected)
        return;

    string name = groupName->valueText().toUTF8();

    //json formatting
    Json::Object groupJSON;
    if(name != "") groupJSON["name"] = Json::Value(name);
    if(!lightBoxes.empty()) {
        Json::Array lightsJSON;
        for(WCheckBox *lightBox : lightBoxes) {
            if(lightBox->isChecked()) {
                WString lightNum = lightBox->text();
                lightsJSON.push_back(Json::Value(lightNum.toUTF8()));
            }
        }
        //if at least one was checked
        if(!lightsJSON.empty()) {
            groupJSON["lights"] = Json::Value(lightsJSON);
        }
    }

    string url = "http://" + bridge_->getIP() + ":" + bridge_->getPort() + "/api/" + bridge_->getUsername() + "/groups";
    postRequest(url, Json::serialize(groupJSON));
}

/**
 *   @brief  Remove group function, creates a JSON request to remove the selected group
 *           from the current bridge.
 *
 *   @param  group is the group object to remove from the bridge.
 *
 *   @return  void
 *
 */
void LightManagementWidget::removeGroup(Group *group) {
    string url = "http://" + bridge_->getIP() + ":" + bridge_->getPort() + "/api/" + bridge_->getUsername() + "/groups/" + group->getGroupnum().toUTF8();
    deleteRequest(url);
}

/**
 *   @brief  Edit group function, when the edit button is clicked, a window where the user can
 *           change lights assigned to the selected group appears.
 *
 *   @param  group is the group object that was selected to edit from the table.
 *
 *   @return  void
 *
 */
void LightManagementWidget::editGroupDialog(Group *group) {
    editGroupDialog_ = new WDialog("Edit Group #" + group->getGroupnum().toUTF8()); // title

    new WLabel("Group Name: ", editGroupDialog_->contents());
    editGroupName = new WLineEdit(editGroupDialog_->contents());
    editGroupName->setValueText(group->getName().toUTF8());
    new WBreak(editGroupDialog_->contents());

    new WLabel("Lights: ", editGroupDialog_->contents());
    Json::Object bridgeJson;
    Json::parse(bridge_->getJson(), bridgeJson);
    Json::Object lights = bridgeJson.get("lights");

    set<string> data = lights.names();
    lightBoxes.clear(); //empty lightbox
    for(string num : data) {
        WCheckBox *lightButton_ = new WCheckBox(num, editGroupDialog_->contents());
        lightBoxes.push_back(lightButton_);
    }
    new WBreak(editGroupDialog_->contents());

    // make okay and cancel buttons, cancel sends a reject dialogstate, okay sends an accept
    WPushButton *ok = new WPushButton("OK", editGroupDialog_->contents());
    WPushButton *cancel = new WPushButton("Cancel", editGroupDialog_->contents());

    ok->clicked().connect(editGroupDialog_, &WDialog::accept);
    cancel->clicked().connect(editGroupDialog_, &WDialog::reject);

    editGroupDialog_->finished().connect(boost::bind(&LightManagementWidget::updateGroupInfo, this, group));

    editGroupDialog_->show();
}


/**
 *   @brief  Update groups function, creates a JSON request to change properties of
 *           a given group based on user's inputs in the edit group dialog box.
 *
 *   @param  group is the group object to update from the bridge.
 *
 *   @return  void
 *
 */
void LightManagementWidget::updateGroupInfo(Group *group){
    if (editGroupDialog_->result() == WDialog::DialogCode::Rejected)
        return;

    string name = editGroupName->valueText().toUTF8();

    //json formatting
    Json::Object groupJSON;
    if(name != "") groupJSON["name"] = Json::Value(name);
    cout << lightBoxes.size() << "\n";
    //if there are no lights on the bridge, basically
    if(!lightBoxes.empty()) {
        Json::Array lightsJSON;
        for(WCheckBox *lightBox : lightBoxes) {
            if(lightBox->isChecked()) {
                WString lightNum = lightBox->text();
                lightsJSON.push_back(Json::Value(lightNum.toUTF8()));
            }
        }
        //if at least one was checked
        if(!lightsJSON.empty()) {
            groupJSON["lights"] = Json::Value(lightsJSON);
        }
    }

    string url = "http://" + bridge_->getIP() + ":" + bridge_->getPort() + "/api/" + bridge_->getUsername() + "/groups/" + group->getGroupnum().toUTF8();
    putRequest(url, Json::serialize(groupJSON));
}

/**
 *   @brief  Update groups advanced function, creates a opens a dialog box to change
 *           any property of a given group based on user's inputs in the dialog box.
 *
 *   @param  group is the group object to update from the bridge.
 *
 *   @return  void
 *
 */
void LightManagementWidget::groupAdvancedDialog(Group *group) {
    groupAdvancedDialog_ = new WDialog("Advanced"); // title

    new WLabel("State: ", groupAdvancedDialog_->contents());
    onButtonGroup = new WButtonGroup(this);
    WRadioButton *onRadioButton;
    onRadioButton = new WRadioButton("On", groupAdvancedDialog_->contents());
    onButtonGroup->addButton(onRadioButton, 0);
    onRadioButton = new WRadioButton("Off", groupAdvancedDialog_->contents());
    onButtonGroup->addButton(onRadioButton, 1);
    onButtonGroup->setCheckedButton(onButtonGroup->button(0));
    new WBreak(groupAdvancedDialog_->contents());

    //brightness slider
    new WLabel("Brightness: ", groupAdvancedDialog_->contents());
    brightnessGroup = new WSlider(groupAdvancedDialog_->contents());
    brightnessGroup->resize(200,20);
    brightnessGroup->setMinimum(0);
    brightnessGroup->setMaximum(254);
    brightnessGroup->setValue(1);
    brightnessGroup->setDisabled(false);
    new WBreak(groupAdvancedDialog_->contents());

    // color
    new WLabel("Color: ", groupAdvancedDialog_->contents());
    groupAdvancedDialog_->contents()->addWidget(rgbContainer_);
    redSlider->setValue(1); //default values to show user has not changed rgb
    greenSlider->setValue(2); //default values to show user has not changed rgb
    blueSlider->setValue(3); //default values to show user has not changed rgb
    redSlider->setDisabled(false);
    greenSlider->setDisabled(false);
    blueSlider->setDisabled(false);
    WCssDecorationStyle *colour = new WCssDecorationStyle();
    colour->setBackgroundColor(WColor(redSlider->value(), greenSlider->value(), blueSlider->value()));
    rgbContainer_->setDecorationStyle(*colour);
    new WBreak(groupAdvancedDialog_->contents());

    // disable all fields while group is off
    onButtonGroup->checkedChanged().connect(bind([=] {
        bool onStatus = onButtonGroup->checkedButton()->text() == "On" ? 1 : 0;
        brightnessGroup->setDisabled(!onStatus);
        redSlider->setDisabled(!onStatus);
        greenSlider->setDisabled(!onStatus);
        blueSlider->setDisabled(!onStatus);
    }));

    // make okay and cancel buttons, cancel sends a reject dialogstate, okay sends an accept
    WPushButton *ok = new WPushButton("OK", groupAdvancedDialog_->contents());
    WPushButton *cancel = new WPushButton("Cancel", groupAdvancedDialog_->contents());

    ok->clicked().connect(groupAdvancedDialog_, &WDialog::accept);
    cancel->clicked().connect(groupAdvancedDialog_, &WDialog::reject);

    // when the user is finished, call function to update group
    groupAdvancedDialog_->finished().connect(boost::bind(&LightManagementWidget::groupUpdateAdvanced, this, group));
    groupAdvancedDialog_->show();
}

/**
 *   @brief  Update groups function, creates a JSON request to change properties of
 *           a given group based on user's inputs in the advanced group dialog box.
 *
 *   @param  light is the light object to update from the bridge.
 *
 *   @return  void
 *
 */
void LightManagementWidget::groupUpdateAdvanced(Group *group){
    if (groupAdvancedDialog_->result() == WDialog::DialogCode::Rejected)
        return;

    //string name = groupName->valueText().toUTF8();
    string on = "";
    string xval = "";
    string yval = "";
    string bri = "";

    if(onButtonGroup->checkedButton() != 0) {
        on = onButtonGroup->checkedButton()->text() == "On" ? "1" : "0";
    }

    //if value is 1 user did not change it
    if(brightnessGroup->value() != 1) {
        bri = boost::lexical_cast<string>(brightnessGroup->value());
    }

    //sets new xval and yval if the sliders have been changed from default values
    if(redSlider->value() != 1 || greenSlider->value() != 2 || blueSlider->value() != 3) {
        struct xy *cols = ColourConvert::rgb2xy(redSlider->value(), greenSlider->value(), blueSlider->value());

        xval = boost::lexical_cast<string>(cols->x);
        yval = boost::lexical_cast<string>(cols->y);
        bri = boost::lexical_cast<string>((int)(cols->brightness));
    }

    //json formatting
    Json::Object actionJSON;
    if(on != "") actionJSON["on"] = Json::Value((bool)boost::lexical_cast<int>(on));

    if(bri != "" && on != "0") actionJSON["bri"] = Json::Value(boost::lexical_cast<int>(bri));

    if(xval != "" && yval != "" && on != "0") {
        Json::Array xyJSON;
        xyJSON.push_back(Json::Value(boost::lexical_cast<double>(xval)));
        xyJSON.push_back(Json::Value(boost::lexical_cast<double>(yval)));
        actionJSON["xy"] = Json::Value(xyJSON);
    }

    if(group->getTransition() != 4 && on != "0") actionJSON["transitiontime"] = Json::Value(group->getTransition());

    string url = "http://" + bridge_->getIP() + ":" + bridge_->getPort() + "/api/" + bridge_->getUsername() + "/groups/" + group->getGroupnum().toUTF8() + "/action";
    putRequest(url, Json::serialize(actionJSON));
}

/**
 *   @brief  Update schedules table function, clears the current table and re-populates it with
 *           all the schedules that are in the bridge.
 *
 *   @return  void
 *
 */
void LightManagementWidget::updateSchedulesTable() {
    schedulesTable_->clear();

    // create row for headers table
    WTableRow *tableRow = schedulesTable_->insertRow(schedulesTable_->rowCount());
    //table headers <th>
    tableRow->elementAt(0)->addWidget(new WText("Name"));
    tableRow->elementAt(1)->addWidget(new WText("Description"));
    tableRow->elementAt(2)->addWidget(new WText("Action"));
    tableRow->elementAt(3)->addWidget(new WText("Time"));

    //convert json string into json object
    Json::Object bridgeJson;
    Json::parse(bridge_->getJson(), bridgeJson);
    Json::Object schedules = bridgeJson.get("schedules");

    set<string> data = schedules.names();
    for(string num : data) {
        Json::Object scheduleData = schedules.get(num);
        Schedule *schedule = new Schedule(num, scheduleData);

        tableRow = schedulesTable_->insertRow(schedulesTable_->rowCount());

        tableRow->elementAt(0)->addWidget(new WText(schedule->getName()));
        tableRow->elementAt(1)->addWidget(new WText(schedule->getDescription()));

        tableRow->elementAt(2)->addWidget(new WText(schedule->getMethod().toUTF8()));
        tableRow->elementAt(2)->addWidget(new WBreak());

        tableRow->elementAt(2)->addWidget(new WText(schedule->getAddress().toUTF8()));
        tableRow->elementAt(2)->addWidget(new WBreak());

        if(schedule->getMethod().toUTF8() != "DELETE") {
            string onState = schedule->getOn() == 1 ? "true" : "false";
            tableRow->elementAt(2)->addWidget(new WText("On: " + onState));
            tableRow->elementAt(2)->addWidget(new WBreak());

            if(schedule->getX() != -1 && schedule->getY() != -1) {
                tableRow->elementAt(2)->addWidget(new WText("Color: [" + boost::lexical_cast<string>(schedule->getX()) + "," + boost::lexical_cast<string>(schedule->getY()) + "]"));
                tableRow->elementAt(2)->addWidget(new WBreak());
            }
            if(schedule->getBri() != -1) {
                tableRow->elementAt(2)->addWidget(new WText("Bri: " + boost::lexical_cast<string>(schedule->getBri())));
                tableRow->elementAt(2)->addWidget(new WBreak());
            }
            if(schedule->getTransition() != 4) {
                tableRow->elementAt(2)->addWidget(new WText("Transition: " + boost::lexical_cast<string>(schedule->getTransition())));
            }
        }

        tableRow->elementAt(3)->addWidget(new WText(schedule->getTime()));

        WPushButton *editScheduleButton = new WPushButton("Edit");
        editScheduleButton->clicked().connect(boost::bind(&LightManagementWidget::editScheduleDialog, this, schedule));
        tableRow->elementAt(4)->addWidget(editScheduleButton);

        WPushButton *removeScheduleButton = new WPushButton("Remove");
        removeScheduleButton->clicked().connect(boost::bind(&LightManagementWidget::removeSchedule, this, schedule));
        tableRow->elementAt(4)->addWidget(removeScheduleButton);
    }
}

/**
 *   @brief  Create schedule function, when the create button is clicked, a dialog appears where the user
 *           can create a schedule and select their options.
 *
 *   @return  void
 *
 */
void LightManagementWidget::createScheduleDialog() {
    createScheduleDialog_ = new WDialog("Create a Schedule"); // title

    new WLabel("Schedule Name: ", createScheduleDialog_->contents());
    scheduleName = new WLineEdit(createScheduleDialog_->contents());
    scheduleName->setValueText("schedule");
    new WBreak(createScheduleDialog_->contents());

    new WLabel("Description: ", createScheduleDialog_->contents());
    description = new WLineEdit(createScheduleDialog_->contents());
    new WBreak(createScheduleDialog_->contents());

    // Container for Resource Buttons
    Wt::WGroupBox *resourceGroupContainer = new Wt::WGroupBox("Resource", createScheduleDialog_->contents());
    resourceButtonGroup = new Wt::WButtonGroup(this);
    Wt::WRadioButton *resourceRadioButton;
    resourceRadioButton = new Wt::WRadioButton("Light", resourceGroupContainer);
    resourceButtonGroup->addButton(resourceRadioButton, 0);
    resourceRadioButton = new Wt::WRadioButton("Group", resourceGroupContainer);
    resourceButtonGroup->addButton(resourceRadioButton, 1);
    resourceButtonGroup->setCheckedButton(resourceButtonGroup->button(0));
    new WBreak(resourceGroupContainer);
    new WLabel("Light/Group Num: ", resourceGroupContainer);
    resourceNum = new WLineEdit(resourceGroupContainer);
    resourceNum->setValueText("1");
    intValidator = new WIntValidator(0, 100, resourceGroupContainer); //max 100
    intValidator->setMandatory(true);
    resourceNum->setValidator(intValidator);
    new WBreak(createScheduleDialog_->contents());

    // Container for Action Buttons
    Wt::WGroupBox *actionGroupContainer = new Wt::WGroupBox("Action", createScheduleDialog_->contents());
    actionButtonGroup = new Wt::WButtonGroup(this);
    Wt::WRadioButton *actionRadioButton;
    actionRadioButton = new Wt::WRadioButton("Change", actionGroupContainer);
    actionButtonGroup->addButton(actionRadioButton, 0);
    actionRadioButton = new Wt::WRadioButton("Add", actionGroupContainer);
    actionButtonGroup->addButton(actionRadioButton, 1);
    actionRadioButton = new Wt::WRadioButton("Remove", actionGroupContainer);
    actionButtonGroup->addButton(actionRadioButton, 2);
    actionButtonGroup->setCheckedButton(actionButtonGroup->button(0));
    new WBreak(createScheduleDialog_->contents());

    // Container for Body Buttons
    Wt::WGroupBox *bodyGroupContainer = new Wt::WGroupBox("State", createScheduleDialog_->contents());
    // on or off
    onButtonGroup = new Wt::WButtonGroup(this);
    Wt::WRadioButton *onRadioButton;
    onRadioButton = new Wt::WRadioButton("On", bodyGroupContainer);
    onButtonGroup->addButton(onRadioButton, 0);
    onRadioButton = new Wt::WRadioButton("Off", bodyGroupContainer);
    onButtonGroup->addButton(onRadioButton, 1);
    onButtonGroup->setCheckedButton(onButtonGroup->button(0));
    onButtonGroup->checkedChanged().connect(bind([=] {
        bool onStatus = onButtonGroup->checkedButton()->text() == "On" ? 1 : 0;
        brightnessSchedule->setDisabled(!onStatus);
        redSlider->setDisabled(!onStatus);
        greenSlider->setDisabled(!onStatus);
        blueSlider->setDisabled(!onStatus);
        transitionSchedule->setDisabled(!onStatus);
    }));

    new WBreak(bodyGroupContainer);
    // brightness slider
    new WLabel("Brightness: ", bodyGroupContainer);
    brightnessSchedule = new WSlider(bodyGroupContainer);
    brightnessSchedule->resize(200,20);
    brightnessSchedule->setMinimum(0);
    brightnessSchedule->setMaximum(254);
    brightnessSchedule->setValue(1);
    brightnessSchedule->setDisabled(false);
    new WBreak(bodyGroupContainer);
    // color picker
    new WLabel("Color: ", bodyGroupContainer);
    bodyGroupContainer->addWidget(rgbContainer_);
    redSlider->setValue(1); //default values to show user has not changed rgb
    greenSlider->setValue(2); //default values to show user has not changed rgb
    blueSlider->setValue(3); //default values to show user has not changed rgb
    redSlider->setDisabled(false);
    greenSlider->setDisabled(false);
    blueSlider->setDisabled(false);
    WCssDecorationStyle *colour = new WCssDecorationStyle();
    colour->setBackgroundColor(WColor(redSlider->value(), greenSlider->value(), blueSlider->value()));
    rgbContainer_->setDecorationStyle(*colour);
    new WBreak(bodyGroupContainer);
    // transition time
    new WLabel("Transition time: ", bodyGroupContainer);
    transitionSchedule = new WLineEdit(bodyGroupContainer);
    transitionSchedule->resize(40,20);
    transitionSchedule->setValueText("4");
    intValidator = new WIntValidator(0, 100, bodyGroupContainer);//100 maximum
    transitionSchedule->setValidator(intValidator);
    new WText(" seconds", bodyGroupContainer);
    new WBreak(bodyGroupContainer);

    // Container for Date and Time
    Wt::WGroupBox *datetimeGroupContainer = new Wt::WGroupBox("Date & Time", createScheduleDialog_->contents());
    dateEdit = new WDateEdit(datetimeGroupContainer);
    dateEdit->setDate(WDate::currentServerDate());
    timeEdit = new WTimeEdit(datetimeGroupContainer);
    timeEdit->setTime(WTime::currentTime());

    new WBreak(createScheduleDialog_->contents());

    // make okay and cancel buttons, cancel sends a reject dialogstate, okay sends an accept
    WPushButton *ok = new WPushButton("OK", createScheduleDialog_->contents());
    WPushButton *cancel = new WPushButton("Cancel", createScheduleDialog_->contents());

    ok->clicked().connect(createScheduleDialog_, &WDialog::accept);
    cancel->clicked().connect(createScheduleDialog_, &WDialog::reject);

    // when the user is finished, call the ADD SCHEDULE function
    createScheduleDialog_->finished().connect(boost::bind(&LightManagementWidget::createSchedule, this));
    createScheduleDialog_->show();
}

/**
 *   @brief  Create schedule function, creates a JSON request to add a new schedule to the
 *           bridge, using the user defined options selected in create schedule dialog.
 *
 *   @return  void
 *
 */
void LightManagementWidget::createSchedule() {
    if (createScheduleDialog_->result() == WDialog::DialogCode::Rejected)
        return;
    if (resourceNum->validate() != 2)
        return;

    string name = scheduleName->valueText().toUTF8();
    string desc = description->valueText().toUTF8();
    string date = dateEdit->valueText().toUTF8();
    string time = timeEdit->valueText().toUTF8();
    string localtime = date + "T" + time;

    string resource = "";
    string resourceTwo = "";
    string action = "";
    string on = "";
    string xval = "";
    string yval = "";
    string bri = "";
    if(resourceButtonGroup->checkedButton()->text() == "Light") {
        resource = "/lights";
        resourceTwo = "/state";
    }
    else {
        resource = "/groups";
        resourceTwo = "/action";
    }
    string num = "/" + resourceNum->valueText().toUTF8();

    if(actionButtonGroup->checkedButton()->text() == "Change") {
        action = "PUT";
    }
    else if(actionButtonGroup->checkedButton()->text() == "Add") {
        action = "POST";
        resourceTwo = "";
        num = "";
    }
    else {
        action = "DELETE";
        resourceTwo = "";
    }

    if(onButtonGroup->checkedButton() != 0) {
        on = onButtonGroup->checkedButton()->text() == "On" ? "1" : "0";
    }

    //if value is 1 user did not change it
    if(brightnessSchedule->value() != 1) {
        bri = boost::lexical_cast<string>(brightnessSchedule->value());
    }

    //sets new xval and yval if the sliders have been changed from default values
    if(redSlider->value() != 1 || greenSlider->value() != 2 || blueSlider->value() != 3) {
        struct xy *cols = ColourConvert::rgb2xy(redSlider->value(), greenSlider->value(), blueSlider->value());

        xval = boost::lexical_cast<string>(cols->x);
        yval = boost::lexical_cast<string>(cols->y);
        bri = boost::lexical_cast<string>((int)(cols->brightness));
    }

    //if value is 4 user did not change it
    int transition = boost::lexical_cast<int>(transitionSchedule->valueText());

    //json formatting
    Json::Object scheduleJSON;
    if(name != "") scheduleJSON["name"] = Json::Value(name);
    if(desc != "") scheduleJSON["description"] = Json::Value(desc);

    Json::Object commandJSON;
    commandJSON["address"] = Json::Value("/api/" + bridge_->getUsername() + resource + num + resourceTwo);
    commandJSON["method"] = Json::Value(action);
    commandJSON["body"] = Json::Value(Json::ObjectType);

    Json::Object bodyJSON;
    if(on != "") bodyJSON["on"] = Json::Value((bool)boost::lexical_cast<int>(on));

    if(bri != "" && on != "0") bodyJSON["bri"] = Json::Value(boost::lexical_cast<int>(bri));

    if(xval != "" && yval != "" && on != "0") {
        Json::Array xyJSON;
        xyJSON.push_back(Json::Value(boost::lexical_cast<double>(xval)));
        xyJSON.push_back(Json::Value(boost::lexical_cast<double>(yval)));
        bodyJSON["xy"] = Json::Value(xyJSON);
    }

    if(transition != 4 && on != "0") bodyJSON["transitiontime"] = Json::Value(transition);

    commandJSON["body"] = Json::Value(bodyJSON);
    scheduleJSON["command"] = Json::Value(commandJSON);
    scheduleJSON["time"] = Json::Value(localtime);

    string url = "http://" + bridge_->getIP() + ":" + bridge_->getPort() + "/api/" + bridge_->getUsername() + "/schedules";
    postRequest(url, Json::serialize(scheduleJSON));
}

/**
 *   @brief  Edit schedule function, when the edit button is clicked, a window where the user can
 *           change properties for the selected schedule appears.
 *
 *   @param  schedule is the schedule object that was selected to edit from the table.
 *
 *   @return  void
 *
 */
void LightManagementWidget::editScheduleDialog(Schedule *schedule) {
    editScheduleDialog_ = new WDialog("Edit Schedule #" + schedule->getSchedulenum().toUTF8());

    new WLabel("Schedule Name: ", editScheduleDialog_->contents());
    editScheduleName = new WLineEdit(editScheduleDialog_->contents());
    editScheduleName->setValueText("schedule");
    new WBreak(editScheduleDialog_->contents());

    new WLabel("Description: ", editScheduleDialog_->contents());
    editScheduleDescription = new WLineEdit(editScheduleDialog_->contents());
    new WBreak(editScheduleDialog_->contents());

    // Container for Resource Buttons
    Wt::WGroupBox *resourceGroupContainer = new Wt::WGroupBox("Resource", editScheduleDialog_->contents());
    resourceButtonGroup = new Wt::WButtonGroup(this);
    Wt::WRadioButton *resourceRadioButton;
    resourceRadioButton = new Wt::WRadioButton("Light", resourceGroupContainer);
    resourceButtonGroup->addButton(resourceRadioButton, 0);
    resourceRadioButton = new Wt::WRadioButton("Group", resourceGroupContainer);
    resourceButtonGroup->addButton(resourceRadioButton, 1);
    resourceButtonGroup->setCheckedButton(resourceButtonGroup->button(0));
    new WBreak(resourceGroupContainer);
    new WLabel("Light/Group Num: ", resourceGroupContainer);
    resourceNum = new WLineEdit(resourceGroupContainer);
    resourceNum->setValueText("1");
    intValidator = new WIntValidator(0, 100, resourceGroupContainer); //max 100
    intValidator->setMandatory(true);
    resourceNum->setValidator(intValidator);
    new WBreak(editScheduleDialog_->contents());

    // Container for Action Buttons
    Wt::WGroupBox *actionGroupContainer = new Wt::WGroupBox("Action", editScheduleDialog_->contents());
    actionButtonGroup = new Wt::WButtonGroup(this);
    Wt::WRadioButton *actionRadioButton;
    actionRadioButton = new Wt::WRadioButton("Change", actionGroupContainer);
    actionButtonGroup->addButton(actionRadioButton, 0);
    actionRadioButton = new Wt::WRadioButton("Add", actionGroupContainer);
    actionButtonGroup->addButton(actionRadioButton, 1);
    actionRadioButton = new Wt::WRadioButton("Remove", actionGroupContainer);
    actionButtonGroup->addButton(actionRadioButton, 2);
    actionButtonGroup->setCheckedButton(actionButtonGroup->button(0));
    new WBreak(editScheduleDialog_->contents());

    // Container for Body Buttons
    Wt::WGroupBox *bodyGroupContainer = new Wt::WGroupBox("State", editScheduleDialog_->contents());
    // on or off
    onButtonGroup = new Wt::WButtonGroup(this);
    Wt::WRadioButton *onRadioButton;
    onRadioButton = new Wt::WRadioButton("On", bodyGroupContainer);
    onButtonGroup->addButton(onRadioButton, 0);
    onRadioButton = new Wt::WRadioButton("Off", bodyGroupContainer);
    onButtonGroup->addButton(onRadioButton, 1);
    onButtonGroup->setCheckedButton(onButtonGroup->button(0));
    onButtonGroup->checkedChanged().connect(bind([=] {
        bool onStatus = onButtonGroup->checkedButton()->text() == "On" ? 1 : 0;
        brightnessSchedule->setDisabled(!onStatus);
        redSlider->setDisabled(!onStatus);
        greenSlider->setDisabled(!onStatus);
        blueSlider->setDisabled(!onStatus);
        transitionSchedule->setDisabled(!onStatus);
    }));

    new WBreak(bodyGroupContainer);
    // brightness slider
    new WLabel("Brightness: ", bodyGroupContainer);
    brightnessSchedule = new WSlider(bodyGroupContainer);
    brightnessSchedule->resize(200,20);
    brightnessSchedule->setMinimum(0);
    brightnessSchedule->setMaximum(254);
    brightnessSchedule->setValue(1);
    brightnessSchedule->setDisabled(false);
    new WBreak(bodyGroupContainer);
    // color picker
    new WLabel("Color: ", bodyGroupContainer);
    bodyGroupContainer->addWidget(rgbContainer_);
    redSlider->setValue(1); //default values to show user has not changed rgb
    greenSlider->setValue(2); //default values to show user has not changed rgb
    blueSlider->setValue(3); //default values to show user has not changed rgb
    redSlider->setDisabled(false);
    greenSlider->setDisabled(false);
    blueSlider->setDisabled(false);
    WCssDecorationStyle *colour = new WCssDecorationStyle();
    colour->setBackgroundColor(WColor(redSlider->value(), greenSlider->value(), blueSlider->value()));
    rgbContainer_->setDecorationStyle(*colour);
    new WBreak(bodyGroupContainer);
    // transition time
    new WLabel("Transition time: ", bodyGroupContainer);
    transitionSchedule = new WLineEdit(bodyGroupContainer);
    transitionSchedule->resize(40,20);
    transitionSchedule->setValueText("4");
    intValidator = new WIntValidator(0, 100, bodyGroupContainer);//100 maximum
    transitionSchedule->setValidator(intValidator);
    new WText(" seconds", bodyGroupContainer);
    new WBreak(bodyGroupContainer);

    // Container for Date and Time
    Wt::WGroupBox *datetimeGroupContainer = new Wt::WGroupBox("Date & Time", editScheduleDialog_->contents());
    editDateEdit = new WDateEdit(datetimeGroupContainer);
    editDateEdit->setDate(WDate::currentServerDate());
    editTimeEdit = new WTimeEdit(datetimeGroupContainer);
    editTimeEdit->setTime(WTime::currentTime());

    new WBreak(editScheduleDialog_->contents());

    // make okay and cancel buttons, cancel sends a reject dialogstate, okay sends an accept
    WPushButton *ok = new WPushButton("OK", editScheduleDialog_->contents());
    WPushButton *cancel = new WPushButton("Cancel", editScheduleDialog_->contents());

    ok->clicked().connect(editScheduleDialog_, &WDialog::accept);
    cancel->clicked().connect(editScheduleDialog_, &WDialog::reject);

    editScheduleDialog_->finished().connect(boost::bind(&LightManagementWidget::updateScheduleInfo, this, schedule));

    editScheduleDialog_->show();
}

/**
 *   @brief  Update schedule function, creates a JSON request to change properties of
 *           a given schedule based on user's inputs in the edit schedule dialog box.
 *
 *   @param  schedule is the schedule object to update from the bridge.
 *
 *   @return  void
 *
 */
void LightManagementWidget::updateScheduleInfo(Schedule *schedule){
    if (editScheduleDialog_->result() == WDialog::DialogCode::Rejected)
        return;
    if (resourceNum->validate() != 2)
        return;

    string name = editScheduleName->valueText().toUTF8();
    string desc = editScheduleDescription->valueText().toUTF8();
    string date = editDateEdit->valueText().toUTF8();
    string time = editTimeEdit->valueText().toUTF8();
    string localtime = date + "T" + time;

    string resource = "";
    string resourceTwo = "";
    string action = "";
    string on = "";
    string xval = "";
    string yval = "";
    string bri = "";
    if(resourceButtonGroup->checkedButton()->text() == "Light") {
        resource = "/lights";
        resourceTwo = "/state";
    }
    else {
        resource = "/groups";
        resourceTwo = "/action";
    }
    string num = "/" + resourceNum->valueText().toUTF8();

    if(actionButtonGroup->checkedButton()->text() == "Change") {
        action = "PUT";
    }
    else if(actionButtonGroup->checkedButton()->text() == "Add") {
        action = "POST";
        resourceTwo = "";
        num = "";
    }
    else {
        action = "DELETE";
        resourceTwo = "";
    }

    if(onButtonGroup->checkedButton() != 0) {
        on = onButtonGroup->checkedButton()->text() == "On" ? "1" : "0";
    }

    //if value is 1 user did not change it
    if(brightnessSchedule->value() != 1) {
        bri = boost::lexical_cast<string>(brightnessSchedule->value());
    }

    //sets new xval and yval if the sliders have been changed from default values
    if(redSlider->value() != 1 || greenSlider->value() != 2 || blueSlider->value() != 3) {
        struct xy *cols = ColourConvert::rgb2xy(redSlider->value(), greenSlider->value(), blueSlider->value());

        xval = boost::lexical_cast<string>(cols->x);
        yval = boost::lexical_cast<string>(cols->y);
        bri = boost::lexical_cast<string>((int)(cols->brightness));
    }

    //if value is 4 user did not change it
    int transition = boost::lexical_cast<int>(transitionSchedule->valueText());

    //json formatting
    Json::Object scheduleJSON;
    if(name != "") scheduleJSON["name"] = Json::Value(name);
    if(desc != "") scheduleJSON["description"] = Json::Value(desc);

    Json::Object commandJSON;
    commandJSON["address"] = Json::Value("/api/" + bridge_->getUsername() + resource + num + resourceTwo);
    commandJSON["method"] = Json::Value(action);
    commandJSON["body"] = Json::Value(Json::ObjectType);

    Json::Object bodyJSON;
    if(on != "") bodyJSON["on"] = Json::Value((bool)boost::lexical_cast<int>(on));

    if(bri != "" && on != "0") bodyJSON["bri"] = Json::Value(boost::lexical_cast<int>(bri));

    if(xval != "" && yval != "" && on != "0") {
        Json::Array xyJSON;
        xyJSON.push_back(Json::Value(boost::lexical_cast<double>(xval)));
        xyJSON.push_back(Json::Value(boost::lexical_cast<double>(yval)));
        bodyJSON["xy"] = Json::Value(xyJSON);
    }

    if(transition != 4 && on != "0") bodyJSON["transitiontime"] = Json::Value(transition);

    commandJSON["body"] = Json::Value(bodyJSON);
    scheduleJSON["command"] = Json::Value(commandJSON);
    scheduleJSON["time"] = Json::Value(localtime);

    cout << Json::serialize(scheduleJSON) << "\n";

    string url = "http://" + bridge_->getIP() + ":" + bridge_->getPort() + "/api/" + bridge_->getUsername() + "/schedules/" + schedule->getSchedulenum().toUTF8();
    putRequest(url, Json::serialize(scheduleJSON));
}

/**
 *   @brief  Remove schedule function, creates a JSON request to remove the selected
 *           schedule from the current bridge.
 *
 *   @param  schedule is the schedule object to remove from the bridge.
 *
 *   @return  void
 *
 */
void LightManagementWidget::removeSchedule(Schedule *schedule) {
    string url = "http://" + bridge_->getIP() + ":" + bridge_->getPort() + "/api/" + bridge_->getUsername() + "/schedules/" + schedule->getSchedulenum().toUTF8();
    deleteRequest(url);
}

/**
 *   @brief  Function that creates a new HTTP Client and generates a Client DELETE request to access the Hue API for the current resource specified in URL. Calls handlePutHttp() function once client is done the DELETE call to handle the response.
 *
 *   @param  url the url of the resource to DELETE
 *
 *   @return  void
 *
 */
void LightManagementWidget::deleteRequest(string url) {
    Http::Message *data = new Http::Message();

    cout << "DELETE: Deleting at URL " << url << "\n";
    Wt::Http::Client *client = new Wt::Http::Client(this);
    client->setTimeout(2); //2 second timeout of request
    client->setMaximumResponseSize(1000000);
    client->done().connect(boost::bind(&LightManagementWidget::handlePutHttp, this, _1, _2));

    if(client->deleteRequest(url, *data)) {
        WApplication::instance()->deferRendering();
    }
    else {
        cerr << "DELETE: Error in client->deleteRequest() call\n";
    }
}

/**
 *   @brief  Function that creates a new HTTP Client and generates a Client PUT request to access the Hue API for the current resource specified in URL. Calls handlePutHttp() function once client is done the PUT call to handle the response.
 *
 *   @param  url the url of the resource to PUT the json data to
 *   @param  json the body json data to PUT to the Hue API
 *
 *   @return  void
 *
 */
void LightManagementWidget::putRequest(string url, string json){
    Http::Message *data = new Http::Message();
    data->addBodyText(json);

    cout << "PUT: Updating at URL " << url << "\n";
    Wt::Http::Client *client = new Wt::Http::Client(this);
    client->setTimeout(2); //2 second timeout of request
    client->setMaximumResponseSize(1000000);
    client->done().connect(boost::bind(&LightManagementWidget::handlePutHttp, this, _1, _2));

    if(client->put(url, *data)) {
        WApplication::instance()->deferRendering();
    }
    else {
        cerr << "Light: Error in client->put(url) call\n";
    }
}

/**
 *   @brief  Function that creates a new HTTP Client and generates a Client POST request to access the Hue API for the current resource specified in URL. Calls handlePutHttp() function once client is done the POST call to handle the response.
 *
 *   @param  url the url of the resource to POST the json data to
 *   @param  json the body json data to POST to the Hue API
 *
 *   @return  void
 *
 */
void LightManagementWidget::postRequest(string url, string json) {
    Http::Message *data = new Http::Message();
    data->addBodyText(json);

    cout << "POST: Updating at URL " << url << "\n";
    Wt::Http::Client *client = new Wt::Http::Client(this);
    client->setTimeout(2); //2 second timeout of request
    client->setMaximumResponseSize(1000000);
    client->done().connect(boost::bind(&LightManagementWidget::handlePutHttp, this, _1, _2));

    if(client->post(url, *data)) {
        WApplication::instance()->deferRendering();
    }
    else {
        cerr << "POST: Error in client->post() call\n";
    }
}

/**
 *   @brief  Function to handle the Http response generated by the Wt Http Client object. This function handles the done() signal sent when doing Client's put, post, and deleteRequest functions even though it is named handlePutHttp.
 *
 *   @param  *err stores the error code generated by an Http request, null if request was successful
 *   @param  &response stores the response message generated by the Http request
 *
 *   @return  void
 *
 */
void LightManagementWidget::handlePutHttp(boost::system::error_code err, const Wt::Http::Message &response){
    WApplication::instance()->resumeRendering();
    if (!err && response.status() == 200) {
        cout << "Successful update" << "\n";
        refreshBridge();
    }
    else {
        cerr << "Error: " << err.message() << ", " << response.status() << "\n";
    }
}

/**
 *   @brief  Function that creates a new HTTP Client and generates a Client GET request to access the Hue API for the current Bridge. Calls refreshBridgeHttp() function once client is done the GET call to handle the response.
 *
 *   @return  void
 *
 */
void LightManagementWidget::refreshBridge() {
    string url = "http://" + bridge_->getIP() + ":" + bridge_->getPort() + "/api/" + bridge_->getUsername();

    cout << "BRIDGE: Connecting to URL " << url << "\n";
    Wt::Http::Client *client = new Wt::Http::Client(this);
    client->setTimeout(2); //2 second timeout of request
    client->setMaximumResponseSize(1000000);
    client->done().connect(boost::bind(&LightManagementWidget::refreshBridgeHttp, this, _1, _2));

    if(client->get(url)) {
        WApplication::instance()->deferRendering();
    }
    else {
        cerr << "BRIDGE: Error in client->get(url) call\n";
    }
}

/**
 *   @brief  Function to handle the Http response generated by the Wt Http Client object
 *
 *   @param  *err stores the error code generated by an Http request, null if request was successful
 *   @param  &response stores the response message generated by the Http request
 *
 *   @return  void
 *
 */
void LightManagementWidget::refreshBridgeHttp(boost::system::error_code err, const Wt::Http::Message &response){
    WApplication::instance()->resumeRendering();
    if (!err && response.status() == 200) {
        bridge_->setJson(response.body());

        //update tables with new JSON information
        updateLightsTable();
        updateGroupsTable();
        updateSchedulesTable();
    }
    else {
        cerr << "Error: " << err.message() << ", " << response.status() << "\n";
    }
}

