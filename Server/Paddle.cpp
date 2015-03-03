#include "Paddle.h"

Paddle::Paddle(bool left, int s_width, int s_height):
s_width(s_width),
s_height(s_height)
{
	height = (s_height/4);
	width = 20;
	posy = (s_height / 2 - (height / 2));
	speed = 5;
	left ? posx = 10 : posx = (s_width - width - 10);
}
Paddle::~Paddle(){

}
int Paddle::getX(){
	return posx;
}
int Paddle::getY(){
	return posy;
}
void Paddle::move(bool up){
	if (posy >= speed && up)
		posy -= speed;
	else if (posy + height + speed <= s_height && !up)
		posy += speed;
}
int Paddle::getHeight(){
	return height;
}
int Paddle::getWidth(){
	return width;
}

int Paddle::getSpeed(){
	return speed;
}

void Paddle::setPosition(int x, int y){
	posx = x;
	posy = y;
}