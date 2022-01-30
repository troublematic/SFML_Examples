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
float scale = 80.0f;

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
	int numTicks = 10;
	float halfTickHeight = 0.1f;
	for (float i = 1.0f; i <= (float)numTicks; i++)
	{
		GraphDrawLine(i, halfTickHeight, i, -halfTickHeight, color);	// Right
		GraphDrawLine(-i, halfTickHeight, -i, -halfTickHeight, color);	// Left
		GraphDrawLine(-halfTickHeight, i, halfTickHeight, i, color);	// Up
		GraphDrawLine(-halfTickHeight, -i, halfTickHeight, -i, color);	// Down
	}
}

// GameLoop is called repeatedly. Its job is to update the 'game', and draw the screen.
void GameLoop(float elapsedSeconds)
{
	DrawAxes();

	// Draw graph line from 0, 0 to 5, 5
	//GraphDrawLine(0, 0, 1, 1, sf::Color::Yellow);
	//GraphDrawLine(0, 0, -1, 1, sf::Color::Red);
	//GraphDrawLine(-2, -1, -4, -6, sf::Color::Green);

	// Y = X^2
	float xStart = -4.0f;
	float xEnd = 4.0f;
	float step = 0.1f;
	for (float worldX = xStart; worldX <= xEnd; worldX += step)
	{
		// Calculate Y value
		float worldY = worldX * worldX;

		// Calculate next X, Y
		float worldNextX = worldX + step;
		float worldNextY = worldNextX * worldNextX;

		GraphDrawLine(worldX, worldY, worldNextX, worldNextY, sf::Color::Green);
	}
}



/*
	// Keep a running total of how much time has passed
	totalTime = totalTime + elapsedSeconds;

	// Draw a circle
	DrawCircle(100, 50, 50, sf::Color::Green);

	// Draw a rectangle, whose width grows over time
	DrawRectangle(130, 60, totalTime * 10, 100, sf::Color::Magenta);

	// Draw a pixel
	DrawPixel(30, 180, sf::Color::Yellow);

	// Draw a line
	DrawLine(230, 80, 100, 400, sf::Color::Cyan);

	// Draw a triangle
	float triangleX = 160.0f;
	float triangleY = 80.0f;
	DrawTriangle(triangleX, triangleY, triangleX, triangleY + 60, triangleX + 120, triangleY + 60, sf::Color::Yellow);

	// Draw text
	std::string text = "Click the mouse!";
	DrawString(text, 300, 50, 32, sf::Color::Green);

	// Draw texture
	DrawTexture(500, 100, ghostTexture);
	DrawTexture(550, 100, 64, 32, ghostTexture);
	DrawTexture(650, 100, 16, 80, ghostTexture);

	// Draw something where the mouse is
	float mouseX = (float)GetMouseX();
	float mouseY = (float)GetMouseY();
	bool mousePressed = IsMouseButtonPressed();
	if (mousePressed)
	{
		// Draw a red circle if the mouse button is pressed
		DrawCircle(mouseX, mouseY, 40, sf::Color::Red);
		sound.play();
	}
	else
	{
		// When the mouse is not pressed, draw a rotating sprite
		// Note that X, Y are the center of the sprite, instead of the top left
		float angle = totalTime * 30.0f;
		DrawRotatedTexture(mouseX, mouseY, 64, 64, angle, ghostTexture);
	}

	// If the space bar is pressed, draw a rectangle
	if (IsKeyPressed(sf::Keyboard::Space))
	{
		DrawRectangle(30, 460, 50, 10, sf::Color::Green);
	}

	// Music
	DrawString("P = Play music, S = Stop music", 100, (float)(SCREEN_HEIGHT - 50), 32, sf::Color::Green);
	if (IsKeyPressed(sf::Keyboard::P))
	{
		PlayMusic("Music.wav");
	}
	if (IsKeyPressed(sf::Keyboard::S))
	{
		StopMusic();
	}
*/