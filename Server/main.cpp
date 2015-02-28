#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <chrono>
#include <vector>
#include "websocket.h"
#include "Game.h"
#include "Paddle.h"
#include "Ball.h"
#include "Message.h"

Game* _game = new Game();
webSocket* game_server = new webSocket();
Paddle* _paddle_l = _game->getLeftPaddle();
Paddle* _paddle_r = nullptr;
Ball* _ball = _game->getBall();
vector<Message*> msg_buffer;
vector<Message*> read_msg_buffer;

void newConnectionHandler(int clientID){
	if (clientID==1 && game_server->getClientIDs().size() == 2 && _game->getState() == _game->Waiting){
		_paddle_r = _game->getRightPaddle();
	}
}

void DisconnectHandler(int clientID){
	string player;
	ostringstream disconnect_info;
	
	if (clientID == 0)
		player = _game->getPlayerOneID();
	else if (clientID == 1)
		player = _game->getPlayerTwoID();
	
	_game->PlayerDisconnected();

	disconnect_info << player << " has disconnected, restart the server to play again." << endl;
	cout << disconnect_info.str() << endl;
}
 

void sendMessageToClients(string msg){

	vector<int> clientIDs = game_server->getClientIDs();
	for (int i = 0; i < clientIDs.size(); i++)
		game_server->wsSend(clientIDs[i], msg);

}

string CalculateTime(){
	time_t now2 = time(0);
	tm* _servertime = localtime(&now2);
	auto end = chrono::system_clock::now();
	chrono::high_resolution_clock::time_point p = chrono::high_resolution_clock::now();
	chrono::milliseconds ms2 = chrono::duration_cast<chrono::milliseconds>(p.time_since_epoch());
	std::size_t fractional_seconds = ms2.count() % 1000;

	ostringstream timeTosend;
	timeTosend << _servertime->tm_hour << ":" << _servertime->tm_min << ":" << _servertime->tm_sec << ":" << fractional_seconds;

	time_t end_time = std::chrono::system_clock::to_time_t(end);

	return timeTosend.str();
}

void inputHandler(int clientID, string message){
		int delay = 0;
		Message* new_msg = new Message(clientID,message, delay, CalculateTime() + ":");
		read_msg_buffer.push_back(new_msg);
}

void ReadMessages(){
	string message;
	int clientID;
	for (int x = 0; x < read_msg_buffer.size(); x++){
		if (read_msg_buffer[x]->canSend(CalculateTime() + ":")){
			message = read_msg_buffer[x]->getMSG();
			clientID = read_msg_buffer[x]->getID();
			if (message.substr(0, 3) == "id:"){
				_game->savePlayerID(clientID, message);
			}
			else if (message.substr(0, 5) == "Time:"){
				_game->CalculateLatency(clientID, message);
			}
			else
				_game->getInput(clientID, message);

			ostringstream echo;
			echo << "echo " << message << " " << CalculateTime();
			game_server->wsSend(clientID, echo.str());

			read_msg_buffer.erase(read_msg_buffer.begin() + x);
			read_msg_buffer.shrink_to_fit();
		}
	}

}


void updateHandler(){
	//send paddle and ball coordinates
	int next_msg_timer = 33;
	static clock_t next = clock() + next_msg_timer;
	clock_t current = clock();

	int delay = 500; // delay in MS
	
	ReadMessages();
	if (current >= next){
		if (_game->getState() == _game->Playing){
			time_t now = time(0);
			tm* localtm = localtime(&now);

			

			_game->Update();

			ostringstream gameinfo;

			gameinfo << _paddle_l->getX() << " " << _paddle_l->getY() << " " << _paddle_r->getX() << " " << _paddle_r->getY()
				<< " " << _ball->getX() << " " << _ball->getY() << " " << _game->getLeftPoints() << " " << _game->getRightPoints()
				<< " " << _game->getPlayerOneID() << " " << _game->getPlayerTwoID() << " " << CalculateTime() << " " <<
				_game->getBall()->get_dir_x() << " " << _game->getBall()->get_dir_y();

			Message* new_msg = new Message(gameinfo.str(), delay, CalculateTime()+":");
			msg_buffer.push_back(new_msg);


			for (int x = 0; x < msg_buffer.size(); x++){
				if (msg_buffer[x]->canSend(CalculateTime()+":")){
					sendMessageToClients(msg_buffer[x]->getMSG());	
					msg_buffer.erase(msg_buffer.begin()+x);
					msg_buffer.shrink_to_fit();
				}
			}
		}
		next = clock() + next_msg_timer;
	}
}

int main(int argc, char **argv){

	int port = 8080;

	cout << "Please set server port: ";
	//cin >> port;
	
	game_server->setOpenHandler(newConnectionHandler);
	game_server->setCloseHandler(DisconnectHandler);
	game_server->setMessageHandler(inputHandler);
	game_server->setPeriodicHandler(updateHandler);
	game_server->startServer(port);

	return 0;

}


