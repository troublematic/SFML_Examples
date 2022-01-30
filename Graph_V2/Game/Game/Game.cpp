#include "Main.h"
#include "Helpers.h"
#include "Game.h"

// Define variables which determine how big the window will be
int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

// GameInit is called once, when the program starts. Its job is to do things which only happen once, at the start.
// E.g.
//		Create the window that the game runs in
//		Load any images or sounds that the game needs
void GameInit()
{
	// Create a window for the game
	// The numbers are the width and height in pixels. The text is the title of the window.
	window = new sf::RenderWindow(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "SFML works!");
}

float originX = SCREEN_WIDTH / 2.0f;
float originY = SCREEN_HEIGHT / 2.0f;
float scale = 60.0f;

void GraphDrawLine(float worldX1, float worldY1, float worldX2, float worldY2, sf::Color color)
{
	float screenX1 = originX + (worldX1 * scale);
	float screenY1 = originY - (worldY1 * scale);
	float screenX2 = originX + (worldX2 * scale);
	float screenY2 = originY - (worldY2 * scale);
	DrawLine(screenX1, screenY1, screenX2, screenY2, color);
}

void DrawAxes()
{
	// Draw axes
	const sf::Color color = sf::Color::Cyan;
	DrawLine(originX, 0, originX, (float)SCREEN_HEIGHT, color);
	DrawLine(0, originY, (float)SCREEN_WIDTH, originY, color);

	// Draw ticks
	int numTicks = 20;
	float halfTickHeight = 0.1f;
	for (float i = 1.0f; i <= (float)numTicks; i++)
	{
		GraphDrawLine(i, halfTickHeight, i, -halfTickHeight, color);	// Right
		GraphDrawLine(-i, halfTickHeight, -i, -halfTickHeight, color);	// Left
		GraphDrawLine(-halfTickHeight, i, halfTickHeight, i, color);	// Up
		GraphDrawLine(-halfTickHeight, -i, halfTickHeight, -i, color);	// Down
	}
}

float CalcGraphY(int curveType, float time, float x)
{
	float y = -9999;
	switch (curveType)
	{
	case 1:
		y = x * x;		// Y = X^2
		break;
	case 2:
		y = x * x * x;	// Y = X^3
		break;
	case 3:
		y = powf(2, x);	// Exponential
		break;
	case 4:
		y = sin(x);		// Sin
		break;
	case 5:
		y = sin((x + time) * 2);	// Sin for (x+time), scaled by 2 to make the frequency higher
		break;
	}
	return y;
}

void DrawCurve(int curveType, float time, sf::Color lineColor)
{
	// Where to start, end, and how much to step along the x axis (in world space)
	float xStart = -20.0f;
	float xEnd = 20.0f;
	float step = 0.1f;

	// Calculate first point on graph, storing it as 'previous' position
	float prevWorldX = xStart;
	float prevWorldY = CalcGraphY(curveType, time, xStart);

	// Loop through all the remaining x values
	for (float currWorldX = xStart + step; currWorldX <= xEnd; currWorldX += step)
	{
		// Calculate 'current' Y value
		float currWorldY = CalcGraphY(curveType, time, currWorldX);

		// Draw graph line from 'previous' to 'current' position
		GraphDrawLine(prevWorldX, prevWorldY, currWorldX, currWorldY, lineColor);

		// Store current value as previous value, so we can use it next loop
		prevWorldX = currWorldX;
		prevWorldY = currWorldY;
	}
}

// GameLoop is called repeatedly. Its job is to update the 'game', and draw the screen.
float totalTime = 0;
void GameLoop(float elapsedSeconds)
{
	// Keep a running total of how many seconds have passed since the program started
	totalTime += elapsedSeconds;

	// If mouse is clicked, move the graph origin to the mouse
	if (IsMouseButtonPressed())
	{
		originX = (float)GetMouseX();
		originY = (float)GetMouseY();
	}

	DrawAxes();

	// Draw graphs, if the number keys are pressed
	if (IsKeyPressed(sf::Keyboard::Num1))
	{
		DrawCurve(1, totalTime, sf::Color::Green);
	}
	if (IsKeyPressed(sf::Keyboard::Num2))
	{
		DrawCurve(2, totalTime, sf::Color::Yellow);
	}
	if (IsKeyPressed(sf::Keyboard::Num3))
	{
		DrawCurve(3, totalTime, sf::Color::Red);
	}
	if (IsKeyPressed(sf::Keyboard::Num4))
	{
		DrawCurve(4, totalTime, sf::Color::Magenta);
	}
	if (IsKeyPressed(sf::Keyboard::Num5))
	{
		DrawCurve(5, totalTime, sf::Color(rand()%256, rand() % 256, rand() % 256));
	}
}
