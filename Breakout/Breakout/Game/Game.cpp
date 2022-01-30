#include "Main.h"
#include "Helpers.h"

// Define variables which determine how big the window will be
int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

sf::Texture ballTexture;

const bool debugMode = false;	// Whether to use autopilot

// Ball variables
const float ballSize = 8;
float ballX = (float)SCREEN_WIDTH / 2;		// Center of ball (not the top left)
float ballY = (float)SCREEN_HEIGHT / 2;
float ballSpeedX;	// Speed the ball moves in X
float ballSpeedY;	// Speed the ball moves in Y
float ballVelX = ballSpeedX;	// Velocity is initialized to 'ballSpeed'
float ballVelY = ballSpeedY;

// Paddle variables
const float paddleHeight = 10;
const float paddleWidth = 100;
const float paddleInset = 50;
float paddleX;				// Paddle position (initialized in GameInit)
float paddleY;
float paddleSpeed = 600;	// Speed in pixels per second

// Player variables
const int initialLives = 3;
int currLives = initialLives;
int score = 0;

// Brick variables
const int BRICK_COLUMNS = 18;
const int BRICK_ROWS = 6;
const int MAX_BRICKS = BRICK_COLUMNS * BRICK_ROWS;
const float BRICK_WIDTH = 40;
const float BRICK_HEIGHT = 20;
bool brickAlive[MAX_BRICKS];	// Whether the bricks exist
float brickX[MAX_BRICKS];		// x position of bricks
float brickY[MAX_BRICKS];		// y position of bricks

void ResetBricks()
{
	// Calculate offsets for the grid of bricks
	const float xOffset = (SCREEN_WIDTH / 2) - ((BRICK_COLUMNS / 2) * BRICK_WIDTH);
	const float yOffset = 50;

	// Initialize all the bricks to be in a grid, and alive
	int curr = 0;
	for (int y = 0; y < BRICK_ROWS; y++)
	{
		for (int x = 0; x < BRICK_COLUMNS; x++)
		{
			brickX[curr] = x * BRICK_WIDTH + xOffset;
			brickY[curr] = y * BRICK_HEIGHT + yOffset;
			brickAlive[curr] = true;
			curr++;
		}
	}
}

void ResetBallAndPaddlePosition()
{
	// Reset paddle to the middle of the screen
	paddleX = (float)(SCREEN_WIDTH / 2 - paddleWidth / 2);
	paddleY = (float)SCREEN_HEIGHT - paddleInset;

	// Reset ball to just above the paddle
	const float centerPosOffset = 200;
	ballX = paddleX + paddleWidth / 2;
	ballY = paddleY - ballSize / 2;
	ballVelX = ballSpeedX;	// Send ball right
	ballVelY = -ballSpeedY;	// Send ball up
}

// Draws a box made of lines. Can be useful while debugging.
void DrawDebugBox(float x1, float y1, float x2, float y2, sf::Color color)
{
	DrawLine(x1, y1, x2, y1, color);	// Top
	DrawLine(x2, y1, x2, y2, color);	// Right
	DrawLine(x2, y2, x1, y2, color);	// Bottom
	DrawLine(x1, y2, x1, y1, color);	// Left
}

void GameInit()
{
	// Create a window for the game
	// The numbers are the width and height in pixels. The text is the title of the window.
	window = new sf::RenderWindow(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Breakout");

	// Initialize ball speed
	if (debugMode)
	{
		// Move ball quickly in debug mode
		ballSpeedX = 600;	// Speed the ball moves in X
		ballSpeedY = 700;	// Speed the ball moves in Y
	}
	else
	{
		// Move ball at normal speed when not in debug mode
		ballSpeedX = 300;	// Speed the ball moves in X
		ballSpeedY = 350;	// Speed the ball moves in Y
	}

	// Load a texture
	if (!ballTexture.loadFromFile("Ball.png"))
	{
		printf("Texture failed to load!\n");
	}

	ResetBallAndPaddlePosition();
	ResetBricks();
}

// GameLoop is called repeatedly. Its job is to update the 'game', and draw the screen.
void GameLoop(float elapsedSeconds)
{
	bool playerAlive = currLives > 0;

	// If the player is alive, move the ball
	if (playerAlive)
	{
		ballX += ballVelX * elapsedSeconds;
		ballY += ballVelY * elapsedSeconds;
	}

	// Debug move ball to mouse position when mouse is clicked
	if (debugMode && IsMouseButtonPressed())
	{
		ballX = (float)GetMouseX();
		ballY = (float)GetMouseY();
	}

	// Bounce ball off sides and top of the screen
	if (ballY < 0)
	{
		// Hit top of screen
		ballY = 0;
		ballVelY = ballSpeedY;
	}
	if (ballX >= SCREEN_WIDTH)
	{
		// Hit right side
		ballX = (float)SCREEN_WIDTH;
		ballVelX = -ballSpeedX;
	}
	if (ballX <= 0)
	{
		// Hit left side
		ballX = 0;
		ballVelX = ballSpeedX;
	}

	// If the ball goes off the bottom, 'lose a life' and reset ball and paddle
	if (ballY > SCREEN_HEIGHT)
	{
		ResetBallAndPaddlePosition();
		currLives--;
	}

	// Move paddle
	if (playerAlive)
	{
		if (IsKeyPressed(sf::Keyboard::Left) || IsKeyPressed(sf::Keyboard::A))
		{
			paddleX -= paddleSpeed * elapsedSeconds;
		}
		if (IsKeyPressed(sf::Keyboard::Right) || IsKeyPressed(sf::Keyboard::D))
		{
			paddleX += paddleSpeed * elapsedSeconds;
		}
	}

	// In debug mode, automatically move paddle to always be under the ball
	if (debugMode)
	{
		paddleX = ballX - paddleWidth / 2;
	}

	// Limit paddle to screen
	if (paddleX < 0)
	{
		paddleX = 0;
	}
	if (paddleX > SCREEN_WIDTH - paddleWidth)
	{
		paddleX = SCREEN_WIDTH - paddleWidth;
	}

	// Bounce ball off paddle
	if (ballX >= paddleX &&
		ballX <= paddleX + paddleWidth &&
		ballY >= paddleY &&
		ballY <= paddleY + paddleHeight)
	{
		ballVelY = -ballSpeedY;
	}

	// Draw ball
	// ballX, ballY is the center of the ball. DrawTexture takes the top left,
	// so we need to subtract (ballSize/2) to calculate the top left.
	DrawTexture(ballX - (ballSize / 2), ballY - (ballSize / 2), ballSize, ballSize, ballTexture);

	// Draw paddle
	DrawRectangle(paddleX, paddleY, paddleWidth, paddleHeight, sf::Color::White);

	// Test collision between ball and bricks and draw them
	for (int i = 0; i < MAX_BRICKS; i++)
	{
		if (brickAlive[i])
		{
			// Calculate the sides of the brick
			float brickTop = brickY[i];
			float brickBottom = brickY[i] + BRICK_HEIGHT - 1;
			float brickLeft = brickX[i];
			float brickRight = brickX[i] + BRICK_WIDTH - 1;
			//DrawDebugBox(brickLeft, brickTop, brickRight, brickBottom, sf::Color::Cyan);

			if (ballX > brickLeft &&
				ballX < brickRight &&
				ballY > brickTop &&
				ballY < brickBottom)
			{
				// Ball has hit the brick. Kill the brick and increase score.
				brickAlive[i] = false;
				score++;

				// We know the ball is inside the brick
				// Work out which side the ball 'penetrates' the least, and treat that as the side which was hit

				// Use an int variable to store which side was hit
				int hitSide;	// 0 = top, 1 = bottom, 2 = left, 3 = right

				float shortestPenetration;	// Used for storing the lowest penetration amount we've found

				// Start with the top of the brick
				// Because it's the first side we're testing, this is the side which has been penetrated least
				hitSide = 0;
				shortestPenetration = abs(brickTop - ballY);

				// Test the bottom
				float currPenetration = abs(brickBottom - ballY);	// I use abs because I'm lazy, and want to avoid getting it the wrong way around
				if (currPenetration < shortestPenetration)
				{
					// The bottom is penetrated less. It's the new 'winner'.
					hitSide = 1;
					shortestPenetration = currPenetration;
				}

				// Test the left
				currPenetration = abs(brickLeft - ballX);
				if (currPenetration < shortestPenetration)
				{
					// The left is penetrated less. It's the new 'winner'.
					hitSide = 2;
					shortestPenetration = currPenetration;
				}

				// Test the right
				currPenetration = abs(brickRight - ballX);
				if (currPenetration < shortestPenetration)
				{
					// The right is penetrated less. It's the new 'winner'.
					hitSide = 3;
					shortestPenetration = currPenetration;
				}

				// Depending on which side was penetrated most, change the ball velocity
				switch (hitSide)
				{
				case 0:		// Top
					ballVelY = -ballSpeedY;
					break;
				case 1:		// Bottom
					ballVelY = ballSpeedY;
					break;
				case 2:		// Left
					ballVelX = -ballSpeedX;
					break;
				case 3:		// Right
					ballVelX = ballSpeedX;
					break;
				}
			}
		}

		// The brick may have just been destroyed, so we need to check again
		if (brickAlive[i])
		{
			//DrawRectangle(brickX[i], brickY[i], BRICK_WIDTH - 1, BRICK_HEIGHT - 1, sf::Color::Red);
			DrawRectangle(brickX[i], brickY[i], BRICK_WIDTH, BRICK_HEIGHT, sf::Color::Cyan);
			DrawRectangle(brickX[i] + 1, brickY[i] + 1, BRICK_WIDTH - 2, BRICK_HEIGHT - 2, sf::Color::Red);
		}
	}

	// Draw lives and score text
	std::string scoreText = "Lives: " + std::to_string(currLives) + "   Score: " + std::to_string(score);
	DrawString(scoreText, 8, (float)SCREEN_HEIGHT - 24, 16, sf::Color::Cyan);

	// Detect if all bricks are dead
	// Start by assuming that there are no bricks, and if we find one, set noBricks to false
	bool noBricks = true;
	for (int i = 0; i < MAX_BRICKS; i++)
	{
		if (brickAlive[i])
		{
			noBricks = false;
			break;
		}
	}
	// If all bricks are dead, reset for next round
	if (noBricks)
	{
		ResetBricks();
		ResetBallAndPaddlePosition();
	}


	// Draw Game Over text
	if (!playerAlive)
	{
		DrawString("Game Over!", SCREEN_WIDTH / 2 - 150.0f, (float)SCREEN_HEIGHT / 2, 50, sf::Color::Red);
		DrawString("Press P to play again", (SCREEN_WIDTH / 2.0f) - 100.0f, (float)SCREEN_HEIGHT / 2 + 100, 20, sf::Color::Red);

		if (IsKeyPressed(sf::Keyboard::P))
		{
			ResetBricks();
			ResetBallAndPaddlePosition();
			currLives = initialLives;
		}
	}
}
