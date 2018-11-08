#ifndef LIGHTMANAGEMENTWIDGET_H
#define LIGHTMANAGEMENTWIDGET_H

#include <Wt/WContainerWidget>
#include <Wt/WTable>
#include <Wt/WDialog>
#include <Wt/WLabel>
#include <Wt/WSlider>
#include <Wt/WIntValidator>
#include <Wt/WCalendar>
#include <Wt/WDate>
#include <Wt/WDateEdit>
#include <Wt/WTemplate>
#include <Wt/WTimeEdit>
#include <Wt/WButtonGroup>
#include <Wt/WGroupBox>
#include <Wt/WRadioButton>
#include <Wt/WCheckBox>
#include "WelcomeScreen.h"
#include "Bridge.h"
#include "Light.h"
#include "Group.h"
#include "Schedule.h"
#include "ColourConvert.h"

class LightManagementWidget: public Wt::WContainerWidget
{
public:
    LightManagementWidget(Wt::WContainerWidget *parent = 0,
                        Bridge *bridge = 0,
                        WelcomeScreen *main = 0);

    void update();
private:
    WelcomeScreen *parent_; // parent widget
    Bridge *bridge_; // current bridge

    Wt::WStackedWidget *lightManagementStack_; // main stack of the screen

    Wt::WContainerWidget *overviewWidget_; // overview container widget
    Wt::WContainerWidget *lightsWidget_; // lights container widget
    Wt::WContainerWidget *groupsWidget_; // groups container widget
    Wt::WContainerWidget *schedulesWidget_; // schedules container widget

    Wt::WTable *lightsTable_; // lights table
    Wt::WTable *groupsTable_; // groups table
    Wt::WTable *schedulesTable_; // schedules table

    // editRGBDialog function widgets
    Wt::WContainerWidget *rgbContainer_; //contains XY RGB slider
    Wt::WSlider *brightnessSlider_; // brightness value

    Wt::WDialog *editRGBDialog_; // edit RGB dialog
    Wt::WSlider *redSlider;
    Wt::WSlider *greenSlider;
    Wt::WSlider *blueSlider;

    // editHueSatDialog function widgets
    Wt::WContainerWidget *hueSatContainer_; //contains hue and sat sliders
    Wt::WDialog *editHueSatDialog_; // edit Hue/Saturation dialog
    Wt::WSlider *hueSlider;
    Wt::WSlider *satSlider;
    Wt::WSlider *briSlider;

    // lights page widgets
    Wt::WLineEdit *editLightTransition; // transition value
    Wt::WIntValidator *intValidator; // text box validator
    Wt::WDialog *editLightDialog_; // dialog to edit light
    Wt::WLineEdit *editLightName; // light name edit

    // groups page widgets
    Wt::WDialog *createGroupDialog_; // dialog to create group
    Wt::WLineEdit *groupName; // group name
    vector<WCheckBox*> lightBoxes; // vector of lights to add
    Wt::WDialog *editGroupDialog_; // dialog to edit group
    Wt::WLineEdit *editGroupName; // group name
    Wt::WDialog *groupAdvancedDialog_; // dialog for advanced settings in group
    Wt::WSlider *brightnessGroup; // brightness of group
    Wt::WLineEdit *hue;
    Wt::WLineEdit *saturation;
    Wt::WLineEdit *ct;
    Wt::WComboBox *alert;
    Wt::WComboBox *effect;
    WComboBox *reachable;

    // schedules page widgets
    Wt::WDialog *createScheduleDialog_; // dialog to create schedule
    Wt::WLineEdit *scheduleName; // schedule name
    Wt::WLineEdit *description; // schedule description
    Wt::WDateEdit *dateEdit; // date
    Wt::WTimeEdit *timeEdit; // time
    Wt::WButtonGroup *resourceButtonGroup; // container for resource buttons
    WLineEdit *resourceNum; // resource number
    Wt::WButtonGroup *actionButtonGroup; // container for action buttons
    Wt::WButtonGroup *onButtonGroup; // container for on/off buttons
    WSlider *brightnessSchedule; // brightness slider
    WLineEdit *transitionSchedule; // transition time
    Wt::WDialog *editScheduleDialog_; // dialog to edit schedule
    Wt::WLineEdit *editScheduleDescription; //schedule desc
    Wt::WLineEdit *editScheduleName; // schedule name
    Wt::WDateEdit *editDateEdit; // date for edit box
    Wt::WTimeEdit *editTimeEdit; // time for exit box

    void viewOverviewWidget();
    void viewLightsWidget();
    void viewGroupsWidget();
    void viewSchedulesWidget();
    void editRGBDialog(Light *light);
    void editHueSatDialog(Light *light);

    void updateLightsTable();
    void editLightDialog(Light *light);
    void removeLight(Light *light);
    void updateLightInfo(Light *light);
    void updateLightBri(WSlider *slider_, Light *light);
    void updateLightOn(WPushButton *button_, Light *light);
    void updateLightXY(Light *light);
    void updateLightHS(Light *light);

    void updateGroupsTable();
    void createGroupDialog();
    void groupAdvancedDialog(Group *group);
    void editGroupDialog(Group *group);
    void createGroup();
    void removeGroup(Group *group);
    void updateGroupInfo(Group *group);
    void groupUpdateAdvanced(Group *group);

    void updateSchedulesTable();
    void createScheduleDialog();
    void createSchedule();
    void editScheduleDialog(Schedule *schedule);
    void updateScheduleInfo(Schedule *schedule);
    void removeSchedule(Schedule *schedule);

    void deleteRequest(string url);
    void putRequest(string url, string json);
    void postRequest(string url, string json);
    void handlePutHttp(boost::system::error_code err, const Wt::Http::Message &response);
    void refreshBridge();
    void refreshBridgeHttp(boost::system::error_code err, const Wt::Http::Message &response);
};


#endif // LIGHTMANAGEMENTWIDGET_H
