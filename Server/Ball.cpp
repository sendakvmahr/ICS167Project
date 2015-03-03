#include "Ball.h"


Ball::Ball(int width_s, int height_s):
s_width(width_s),
s_height(height_s)
{
	srand(time_t(NULL));
	posx = 210;
	posy = 50;
	height = 20;
	width = 20;
	originalSpeed = 10;
	hits = 0;
	speed = originalSpeed;
	angle = rand() % 12 + 60;
	dir_y = speed*sin(angle*3.14 / 180.0f);
	dir_x = speed*cos(angle*3.14 / 180.0f);
	paddle_x = 0;
}
Ball::~Ball(){

}
void Ball::Move(){
	posx += dir_x;
	posy += dir_y;
}

void Ball::Respawn(){
	speed = originalSpeed;
	angle = rand() % 12 + 60;
	posx = s_width / 2;
	posy = s_height / 2;
	hits = 0;
	int random = rand() % 10;
	int mult;
	random < 5 ? mult = -1 : mult = 1;
	dir_y = mult * speed*sin(angle*3.14 / 180.0f);
	dir_x = mult * speed*cos(angle*3.14 / 180.0f);
}

bool Ball::CheckCollision(Paddle* paddle){
	if (paddle_x == 0)
		paddle_x = paddle->getX();
	if (paddle->getX() > s_width / 2){ // right
		if (posy > paddle->getY() && posy < (paddle->getY() + paddle->getHeight())
			&& posx + abs(dir_x) + width < (paddle->getX() + paddle->getWidth()) && posx + width + abs(dir_x) > paddle->getX()){
			posx -= 2;
			Bounce();
			return true;
		}
	}
	else {
		if (posy > paddle->getY() && posy < (paddle->getY() + paddle->getHeight())
			&& posx - abs(dir_x) < (paddle->getX() + paddle->getWidth()) && posx - abs(dir_x) > paddle->getX()){
			posx += 2;
			Bounce();
			return true;
		}
	}
	return false;
}

bool Ball::CheckLimits(){
	if (posy + dir_y <= 0 || posy + height + dir_y >= s_height){
		Bounce(true);
	}
	else if (posx - abs(dir_x) < 0){
		ballDirection = Ball::LEFT;
		Respawn();
		return true;
	}
	else if (posx + width + dir_x >= s_width){
		ballDirection = Ball::RIGHT;
		Respawn();
		return true;
	}

	return false;
}

void Ball::Bounce(bool WallCollision){
	angle = rand() % 12 + 60;
	dir_y<0 ? dir_y = -1 * speed*sin(angle*3.14 / 180.0f) : dir_y = speed*sin(angle*3.14 / 180.0f);
	dir_x<0 ? dir_x = -1 * speed*cos(angle*3.14 / 180.0f) : dir_x = speed*cos(angle*3.14 / 180.0f);

	if (WallCollision) //top or bottom
		dir_y *= -1;
	else
		dir_x *= -1;

}
int Ball::getX(){
	return posx;
}
int Ball::getY(){
	return posy;
}
int Ball::getHeight(){
	return height;
}
int Ball::getWidth(){
	return width;
}

Ball::Direction Ball::getLastDirection(){
	return ballDirection;
}


float Ball::get_dir_x(){
	return dir_x;
}
float Ball::get_dir_y(){
	return dir_y;
}
