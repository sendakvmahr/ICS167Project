#define _CRT_SECURE_NO_WARNINGS
#include "Message.h"


Message::Message(string msg, int d, string _time){
	delay = d;
	timeCreated = _time;
	message = msg;
	game_obj = new Game();
}

Message::Message(int ID, string msg, int d, string _time){
	clientID = ID;
	delay = d;
	timeCreated = _time;
	message = msg;
	game_obj = new Game();

}
Message::~Message(){

}

bool Message::canSend(string newtime){
	string other = timeCreated;
	Game::TimeFormat original = game_obj->CreateTimeFormat(other);
	original.millisec += delay;
	if (original.millisec > 1000){
		original.millisec -= 1000;
		original.min += 1;
	}
	Game::TimeFormat now = game_obj->CreateTimeFormat(newtime);
	int latency = ((now.hour - original.hour) * 600000) + ((now.min - original.min) * 60000) + ((now.sec - original.sec) * 1000) + (now.millisec - original.millisec);
	
	if (latency>0) return true;
	
	return false;

}
string Message::getMSG(){
	return message;
}


int Message::getID(){
	return clientID;
}


