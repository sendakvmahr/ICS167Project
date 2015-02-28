#pragma once

#include <iostream>
#include "Game.h"
#include <time.h>
using namespace std;

class Message{
public:
	Message(string msg, int delay, string _time);
	Message(int clientID, string msg, int delay, string _time);
	~Message();
	bool canSend(string now);
	string getMSG();
	int getID();
private:
	int sec, min,delay, clientID;
	string timeCreated, message;
	Game* game_obj;
};