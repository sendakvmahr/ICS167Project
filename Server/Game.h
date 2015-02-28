#pragma once

#include <iostream>
#include <sstream>
#include <time.h>
#include <vector>
#include "Ball.h"
#include "Paddle.h"


class Game{
	public:
		Game(int w = 1000, int h=600);
		~Game();
		void getInput(int playerId,std::string input);
		void savePlayerID(int playerId, std::string id);
		void CalculateLatency(int playerId, std::string ClientMessageTimes);
		void Update();
		int getHeight();
		int getWidth();
		Ball* getBall();
		Paddle* getLeftPaddle();
		int getLeftPoints();
		int getRightPoints();
		std::string getPlayerOneID();
		std::string getPlayerTwoID();
		Paddle* getRightPaddle();
		enum GameState{ Waiting, Playing, Disconnected };
		GameState getState();
		void PlayerDisconnected();
		struct TimeFormat{
			int hour;
			int min;
			int sec;
			int millisec;
		};
		TimeFormat CreateTimeFormat(std::string& msg);
	private:
		
		int SCREEN_WIDTH, SCREEN_HEIGHT;
		int pointsRight, pointsLeft;
		Ball* _ball;
		Paddle* _paddle_left;
		Paddle* _paddle_right;
		std::string PlayerOneID, PlayerTwoID;
		GameState g_state; 
		bool paddle1Moving, paddle1Up, paddle2Moving, paddle2Up;
};
