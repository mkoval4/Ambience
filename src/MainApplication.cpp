/**
 *  @file       MainApplication.cpp
 *  @author     CS 3307 - Team 13
 *  @date       10/7/2017
 *  @version    1.0
 *
 *  @brief      CS 3307, Hue Light Application main application environment
 *
 *  @section    DESCRIPTION
 *
 *              Main application that starts the WApplication and WServer to host the Wt server
 *              for the Hue Light App
 */

#include <Wt/WApplication>
#include <Wt/WServer>
#include <Wt/WLink>
#include <Wt/WBootstrapTheme>

#include "WelcomeScreen.h"

using namespace Wt;
using namespace std;

/**
 *   @brief  Function to create Wt application environment and adds WelcomeScreen as a widget on the WApplication root
 *
 *   @param   &env the WEnvironment to run the application
 *
 *   @return  void
 */
Wt::WApplication *createApplication(const Wt::WEnvironment& env)
{
    Wt::WApplication *app = new Wt::WApplication(env);

    app->setTitle("Ambience - CS3307 Team 13");

    app->setTheme(new Wt::WBootstrapTheme(app));
    app->useStyleSheet("style/stylesheet.css");


    new WelcomeScreen(app->root());
    return app;
}

int main(int argc, char **argv)
{
  try {
    Wt::WServer server(argc, argv, WTHTTP_CONFIGURATION);

    server.addEntryPoint(Wt::Application, createApplication);

    server.run();
  } catch (Wt::WServer::Exception& e) {
    std::cerr << e.what() << std::endl;
  } catch (std::exception &e) {
    std::cerr << "exception: " << e.what() << std::endl;
  }
}
