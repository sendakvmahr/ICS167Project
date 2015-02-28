#pragma once
#include <random>
#include <math.h>
#include <Windows.h>
#include <iostream>
#include <time.h>
#include "Paddle.h"

class Ball{
public:
	Ball(int width, int height);
	~Ball();
	void Move();
	void Respawn();
	bool CheckCollision(Paddle* paddle);
	bool CheckLimits();
	enum Direction{ LEFT, RIGHT };
	void Bounce(bool WallCollision = false);
	int getX();
	int getY();
	int getHeight();
	int getWidth();
	Direction getLastDirection();
	float get_dir_x();
	float get_dir_y();
private:
	float  dir_x, dir_y, speed, originalSpeed;
	int posx, posy, height, width, s_width, s_height, hits, angle, paddle_x;
	Direction ballDirection;
};