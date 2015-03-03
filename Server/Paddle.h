#pragma once




class Paddle{
	public:
		Paddle(bool left, int s_width, int s_height);
		~Paddle();
		int getX();
		int getY();
		void move(bool up);
		int getHeight();
		int getWidth();
		int getSpeed();
		void setPosition(int x, int y);
	private:
		int height, width, posx, posy, speed, s_width, s_height;
};