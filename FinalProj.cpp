// PongGame.cpp : Pong game using c++ and openGL *sigh*.

#include "stdafx.h"

#include <string>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <sstream>
#include <math.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include "GL/freeglut.h"
#pragma comment(lib, "OpenGL32.lib") //specify linker options

//key define
#define VK_W 0x57
#define VK_S 0x53
#define VK_A 0x41
#define VK_D 0x44
#define VK_F 0x46
#define VK_L 0x4C
#define VK_P 0x50
#define VK_R 0x52
#define VK_SPACE 0x20

using namespace std;
//classes
class platform
{
public:
	float x, y;
	float width, height;
};

class bullet
{
public:
	float x;
	float y;
	float segments = 8;
	float radius = 5;
	float speedx = 8;
	flaot speedy = 1;
};

class vec2
{
public:
	float x, y;
	vec2();
	vec2(float, float);

	vec2 vecSum(vec2 a, vec2 b)
	{
		vec2 sum;
		sum.x = a.x + b.x;
		sum.y = a.y + b.y;
		return sum;
	}

	float getVectorX() { return x; }
	float getVectorY() { return y; }

	float dot(vec2 a, vec2 b)
	{
		int dprod = (a.x*b.x) + (a.y*b.y);
		return dprod;
	}
};

vec2::vec2()
{
	x = 0;
	y = 0;
}
vec2::vec2(float a, float b)
{
	x = a;
	y = b;
}

class player
{
public:
	float x;
	float y;
	float height = 10;
	float width = 20;
	float speed = 5;
	float spawnx;
	float spawny;
	float lives = 5;
	float score;
	float floorHeight;
	bool fired = false;
	bool dir = true;
	bool lock = false;
};

player p1, p2;
bullet b1, b2;
platform land;

//window size and refresh rate
int width = 1000;
int height = 500;
int refresh = 1000 / 60;

//text
float charSize = 10;

//pause
bool gamePause = false;


//counter
float t, dt;
float startTime;
const int milli = 1000;

//jump variables
//jump trials
const float degtorad = 3.1415769 / 180.0f;
//initial velocity
float iVel = 7.0f;
//angle being fired
float theta = 75.0f;
//initial x and y velocity calculated
float ivelY = iVel*sin(theta*degtorad);
//new velocity
float velY = 0.0f;

//functions
//int to string function
string inttostr(int x) {
	//converts an int to a string using sstream lib
	stringstream convs;
	convs << x;
	return convs.str();
}

/*
//clamp function
float clamp(float n, float lower, float upper) {
n = (n > lower) * n + !(n > lower) * lower;
return (n < upper) * n + !(n < upper) * upper;
}
*/

//void functions
//keyboard controls
void keyboard() {
	if (GetAsyncKeyState(VK_P))
	{
		gamePause = !gamePause;
	}
	//p1 controls
	if (GetAsyncKeyState(VK_W))
	{
		p1.y += velY*1.5;
	}

	if (GetAsyncKeyState(VK_S))
	{

	}

	if (GetAsyncKeyState(VK_A))
	{
		if (p1.x > 0) {
			p1.x -= p1.speed;
		}
		if (p1.lock == false)
		{
			p1.dir = false;
		}
	}

	if (GetAsyncKeyState(VK_D))
	{
		if (p1.x + p1.width < width) {
			p1.x += p1.speed;
		}
		if (p1.lock == false)
		{
			p1.dir = true;
		}
	}

	if (GetAsyncKeyState(VK_F))
	{
		if(p1.fired == false)
		{
		b1.x = p1.x;
		b1.y = p1.y;
		p1.lock = true;
		p1.fired == true;
		}
	}

	//p2 controls
	if (GetAsyncKeyState(VK_UP))
	{
		p2.y += velY*1.5;
	}

	if (GetAsyncKeyState(VK_DOWN))
	{

	}

	if (GetAsyncKeyState(VK_RIGHT))
	{
		if (p2.x + p2.width < width) {
			p2.x += p2.speed;
		}
		if (p2.lock == false)
		{
			p2.dir = true;
		}
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		if (p2.x > 0) {
			p2.x -= p2.speed;
		}
		if (p2.lock == false)
		{
			p2.dir = false;
		}
	}

	if (GetAsyncKeyState(VK_L))
	{
		if (p2.fired == false)
		{
		b2.x = p2.x;
		b2.y = p2.y;
		p2.lock = true;
		p2.fired = true;
		}
	}
}

void collisionChecker() {
	//left wall collision
	if (b1.x < 0)
	{
		b1.x = 0;
		b1.y = 0;
		p1.lock = false;
		p1.fired = false;
	}
	if (b2.x < 0)
	{
		b2.x = 0;
		b2.y = 0;
		p2.lock = false;
		p2.fired = false;	
	}
	//right wall collision
	if (b1.x > width)
	{
		b1.x = 0;
		b1.y = 0;
		p1.lock = false;
		p1.fired = false;
	}
	if (b2.x > width) {
		b2.x = 0;
		b2.y = 0;
		p2.fired = false;
		p2.lock = false;
	}


	//top /bot wall collision
	if (b1.y > height - 20 || b1.y < 10)
	{
		b1.x = 0;
		b1.y = 0;
		p1.lock = false;
		p1.fired = false;
	}
	if (b2.y > height - 20 || b2.y < 10)
	{
		b2.x = 0;
		b2.y = 0;
		p2.lock = false;
		p2.fired = false;
	}

	//player collision
	if ((b2.x >= p1.x) &&
		(b2.x <= p1.x + p1.width) &&
		(b2.y <= p1.y + p1.height) &&
		(b2.y >= p1.y))
	{
		b2.x = 0;
		b2.y = 0;
		p2.lock = false;
		p2.fired = false;
		p1.x = p1.spawnx;
		p1.y = p1.spawny;
		p1.lives--;
	}
	if ((b1.x >= p2.x) &&
		(b1.x <= p2.x + p2.width) &&
		(b1.y <= p2.y + p2.height) &&
		(b1.y >= p2.y))
	{
		b1.x = 0;
		b1.y = 0;
		p1.lock = false;
		p1.fired = false;
		p2.x = p2.spawnx;
		p2.y = p2.spawny;
		p2.lives--;
	}
}

//makes gl recognized 2d usage
void use2D(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, width, 0.0f, height, 0.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//prints text on screen
void textDraw(float x, float y, string text) {
	glRasterPos2f(x, y);
	glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*)text.c_str());
}

//draw paddles
void boxDraw(float x, float y, float width, float height) {
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + width, y);
	glVertex2f(x + width, y + height);
	glVertex2f(x, y + height);
	glEnd();
}

//draw the ball
void ballDraw(float cx, float cy, float r, int segments) {
	float theta = 2 * 3.1415926 / float(segments);

	//sin and cos calculation
	float cos = cosf(theta);
	float sin = sinf(theta);
	float t;

	float varx = r; //start at angle 0
	float vary = 0;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < segments; i++) {
		glVertex2f(varx + cx, vary + cy); //outputs vertex

										  //apply rotation matrix
		t = varx;
		varx = cos * varx - sin * vary;
		vary = sin * t + cos * vary;
	}
	glEnd();
}

//normalize vectors
void vecnorm(float &x, float &y) {
	float length = sqrt((x * x) + (y * y));
	if (length != 0.0f) {
		length = 1.0f / length;
		x *= length;
		y *= length;
	}
}

//bullet movement
void bulletMove() {
	if (p1.dir == true)
	{
		b1.x += b1.speedx;
	}
	else if (p1.dir == false)
	{
		b1.x -= b1.speedx;
	}

	if (p2.dir == true)
	{
		b2.x += b2.speedx;
	}
	else if (p2.dir == false)
	{
		b2.x -= b2.speedx;
	}
}

//draw on screen
void draw() {
	//clearing the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	//put draw codes below
	if (p1.y > land.height + p1.height)
	{
		p1.y -= velY*0.8; //*0.8 for smoothing
	}
	if (p2.y > land.height + p2.height)
	{
		p2.y -= velY*0.8; //*0.8 for smoothing
	}
	velY = ivelY - 9.8*dt;

	//ball display
	ballDraw(b1.x, b1.y, b1.radius, b1.segments);
	ballDraw(b2.x, b2.y, b2.radius, b2.segments);

	//paddle display
	boxDraw(p1.x, p1.y, p1.width, p1.height);
	boxDraw(p2.x, p2.y, p2.width, p2.height);
	boxDraw(land.x, land.y, land.width, land.height);

	//score display
	textDraw(width / 2 - 30, height - 30, inttostr(p1.score) + " : " + inttostr(p2.score));

	//swapping buffers
	glutSwapBuffers();
}

//screen update handler
void update(int upvalue) {
	//input
	keyboard();

	//timer
	/*
	static float prevTime = -1;
	t = glutGet(GLUT_ELAPSED_TIME) / (float)milli - startTime;
	if (prevTime < 0.0) {
		prevTime = t;
		return;
	}
	dt = t - prevTime;
	prevTime = t;
	*/

	bulletMove();

	//calls update in millisecs
	glutTimerFunc(refresh, update, 0);

	//Redisplay Frame
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	//initiliaze opengl using glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutCreateWindow("Le Shoot Em Up");

	//uses the void functions
	glutDisplayFunc(draw);
	glutTimerFunc(refresh, update, 0);

	//platform init
	land.width = width;
	land.height = 30;

	//player init
	p1.spawnx = 10;
	p1.spawny = land.height + 2;
	p1.x = p1.spawnx;
	p1.y = p1.spawny;

	p2.spawnx = width - 40;
	p2.spawny = land.height + 2;
	p2.x = p2.spawnx;
	p2.y = p2.spawny;

	//setup the 2d and set draw color to black
	use2D(width, height);
	glColor3f(1.0f, 1.0f, 1.0f);

	//program loop
	glutMainLoop();

	return 0;
}

//there's no god in coding
