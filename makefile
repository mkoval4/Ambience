EXE = Ambience
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

OBJS = MainApplication.o Hash.o WelcomeScreen.o Account.o LoginWidget.o CreateAccountWidget.o Bridge.o BridgeScreenWidget.o ProfileWidget.o LightManagementWidget.o Light.o Group.o Schedule.o ColourConvert.o

CC = g++
DEBUG = -g
CFLAGS = -Wall -c -std=c++11 -Iinclude -L/usr/local/lib $(DEBUG)
LFLAGS = -Wall -lwthttp -lwt -lboost_random -lboost_regex -lboost_signals -lboost_system -lboost_thread -lboost_filesystem -lboost_program_options -lboost_date_time -lcrypto $(DEBUG)

Ambience : $(OBJS)
	$(CC) $(OBJS) -o Ambience $(LFLAGS)

MainApplication.o : $(INC_DIR)/WelcomeScreen.h $(SRC_DIR)/MainApplication.cpp
	$(CC) $(CFLAGS) $(SRC_DIR)/MainApplication.cpp

Hash.o : $(INC_DIR)/Hash.h $(SRC_DIR)/Hash.cpp
	$(CC) $(CFLAGS) $(SRC_DIR)/Hash.cpp

WelcomeScreen.o : $(INC_DIR)/Account.h $(SRC_DIR)/WelcomeScreen.cpp	
	$(CC) $(CFLAGS) $(SRC_DIR)/WelcomeScreen.cpp

Account.o : $(INC_DIR)/Account.h $(SRC_DIR)/Account.cpp
	$(CC) $(CFLAGS) $(SRC_DIR)/Account.cpp

LoginWidget.o : $(INC_DIR)/LoginWidget.h $(SRC_DIR)/LoginWidget.cpp
	$(CC) $(CFLAGS) $(SRC_DIR)/LoginWidget.cpp

CreateAccountWidget.o : $(INC_DIR)/CreateAccountWidget.h $(SRC_DIR)/CreateAccountWidget.cpp
	$(CC) $(CFLAGS) $(SRC_DIR)/CreateAccountWidget.cpp

BridgeScreenWidget.o : $(INC_DIR)/BridgeScreenWidget.h $(SRC_DIR)/BridgeScreenWidget.cpp
	$(CC) $(CFLAGS) $(SRC_DIR)/BridgeScreenWidget.cpp

ProfileWidget.o : $(INC_DIR)/ProfileWidget.h $(SRC_DIR)/ProfileWidget.cpp
	$(CC) $(CFLAGS) $(SRC_DIR)/ProfileWidget.cpp

Bridge.o : $(INC_DIR)/Bridge.h $(SRC_DIR)/Bridge.cpp
	$(CC) $(CFLAGS) $(SRC_DIR)/Bridge.cpp

Light.o : $(INC_DIR)/Light.h $(SRC_DIR)/Light.cpp
	$(CC) $(CFLAGS) $(SRC_DIR)/Light.cpp

Group.o : $(INC_DIR)/Group.h $(SRC_DIR)/Group.cpp
	$(CC) $(CFLAGS) $(SRC_DIR)/Group.cpp

Schedule.o : $(INC_DIR)/Schedule.h $(SRC_DIR)/Schedule.cpp
	$(CC) $(CFLAGS) $(SRC_DIR)/Schedule.cpp
	
LightManagementWidget.o: $(INC_DIR)/LightManagementWidget.h $(SRC_DIR)/LightManagementWidget.cpp
	$(CC) $(CFLAGS) $(SRC_DIR)/LightManagementWidget.cpp

ColourConvert.o: $(INC_DIR)/ColourConvert.h $(SRC_DIR)/ColourConvert.cpp
	$(CC) $(CFLAGS) $(SRC_DIR)/ColourConvert.cpp

clean:
	rm $(OBJS) Ambience
