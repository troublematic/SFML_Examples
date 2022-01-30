#include "Main.h"
#include "Helpers.h"

// Define variables which determine how big the window will be
int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

sf::Texture ballTexture;

const bool debugMode = false;	// Whether to use autopilot

// Ball
class Ball
{
public:
	const float diameter = 8;
	float speedX = 0;	// Speed the ball moves in X
	float speedY = 0;	// Speed the ball moves in Y

	float xPos;
	float yPos;
	float xVel;
	float yVel;

	Ball()
	{
		xPos = 0;
		yPos = 0;
		xVel = 0;	// Velocity in pixels per second
		yVel = 0;
	}

	// Member function for setting the position of the ball
	void SetPos(float x, float y)
	{
		xPos = (float)x;
		yPos = (float)y;
	}

	// Member function for moving the ball by its velocity
	void Move(float elapsedSeconds)
	{
		xPos += xVel * elapsedSeconds;
		yPos += yVel * elapsedSeconds;
	}
};
Ball ball;

// Paddle variables
class Paddle
{
public:
	// Constants
	const float height = 10;
	const float width = 100;
	const float screenBottomOffset = 50;

	float x = 0;		// Paddle position (properly initialized in GameInit)
	float y = 0;
	float speed = 600;	// Speed in pixels per second
};
Paddle paddle;

// Player variables
const int initialLives = 3;
int currLives = initialLives;
int score = 0;

// Brick constants
const float BRICK_WIDTH = 40;
const float BRICK_HEIGHT = 20;

class Brick
{
private:
	bool alive;		// Whether the brick is alive (draws a rectangle and can be collided with)
	float x;		// Pixel position of left side of brick
	float y;		// Pixel position of top side of brick

public:
	Brick()
	{
		// Initialize brick to be dead, and have a bogus position
		alive = false;
		x = -99999;
		y = -99999;
	}

	bool IsAlive()
	{
		return alive;
	}

	void TakeDamage()
	{
		alive = false;
	}

	void Init(float xPos, float yPos, bool isAlive)
	{
		x = xPos;
		y = yPos;
		alive = isAlive;
	}

	void Draw()
	{
		DrawRectangle(x, y, BRICK_WIDTH, BRICK_HEIGHT, sf::Color::Cyan);
		DrawRectangle(x + 1, y + 1, BRICK_WIDTH - 2, BRICK_HEIGHT - 2, sf::Color::Red);
	}

	// Returns:
	// -1 = No collision
	//  0 = top, 1 = bottom, 2 = left, 3 = right
	int GetCollisionSide(float xPos, float yPos)
	{
		// Calculate the sides of the brick
		float brickTop = y;
		float brickBottom = y + BRICK_HEIGHT - 1;
		float brickLeft = x;
		float brickRight = x + BRICK_WIDTH - 1;

		if (ball.xPos > brickLeft &&
			ball.xPos < brickRight &&
			ball.yPos > brickTop &&
			ball.yPos < brickBottom)
		{
			// We know the ball is inside the brick
			// Work out which side the ball 'penetrates' the least, and treat that as the side which was hit

			// Use an int variable to store which side was hit
			int hitSide;	// 0 = top, 1 = bottom, 2 = left, 3 = right

			float shortestPenetration;	// Used for storing the lowest penetration amount we've found

			// Start with the top of the brick
			// Because it's the first side we're testing, this is the side which has been penetrated least
			hitSide = 0;
			shortestPenetration = abs(brickTop - ball.yPos);

			// Test the bottom
			float currPenetration = abs(brickBottom - ball.yPos);	// I use abs because I'm lazy, and want to avoid getting it the wrong way around
			if (currPenetration < shortestPenetration)
			{
				// The bottom is penetrated less. It's the new 'winner'.
				hitSide = 1;
				shortestPenetration = currPenetration;
			}

			// Test the left
			currPenetration = abs(brickLeft - ball.xPos);
			if (currPenetration < shortestPenetration)
			{
				// The left is penetrated less. It's the new 'winner'.
				hitSide = 2;
				shortestPenetration = currPenetration;
			}

			// Test the right
			currPenetration = abs(brickRight - ball.xPos);
			if (currPenetration < shortestPenetration)
			{
				// The right is penetrated less. It's the new 'winner'.
				hitSide = 3;
				shortestPenetration = currPenetration;
			}
			return hitSide;
		}
		else
		{
			return -1;	// No collision
		}
	}
};
const int BRICK_COLUMNS = 18;
const int BRICK_ROWS = 6;
const int MAX_BRICKS = BRICK_COLUMNS * BRICK_ROWS;
Brick bricks[MAX_BRICKS];


void ResetBricks()
{
	// Calculate offset for top left of the grid of bricks
	const float xOffset = (SCREEN_WIDTH / 2) - ((BRICK_COLUMNS / 2) * BRICK_WIDTH);
	const float yOffset = 50;

	// Initialize the bricks to be laid out in a grid
	int curr = 0;
	for (int y = 0; y < BRICK_ROWS; y++)
	{
		for (int x = 0; x < BRICK_COLUMNS; x++)
		{
			float xPos = x * BRICK_WIDTH + xOffset;
			float yPos = y * BRICK_HEIGHT + yOffset;
			bricks[curr].Init(xPos, yPos, true);
			curr++;
		}
	}
}

void ResetBallAndPaddlePosition()
{
	// Reset paddle
	paddle.x = (float)(SCREEN_WIDTH / 2 - paddle.width / 2);
	paddle.y = (float)SCREEN_HEIGHT - paddle.screenBottomOffset;

	// Reset ball
	const float centerPosOffset = 200;
	ball.SetPos(paddle.x + paddle.width / 2, paddle.y - ball.diameter / 2);
	ball.xVel = ball.speedX;	// Send ball right
	ball.yVel = -ball.speedY;	// Send ball up
}

// Draw a rectangle made of lines. Can be useful for debugging.
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
	window = new sf::RenderWindow(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "SFML works!");

	if (debugMode)
	{
		// Move ball quickly in debug mode
		ball.speedX = 600;	// Speed the ball moves in X
		ball.speedY = 700;	// Speed the ball moves in Y
	}
	else
	{
		// Move ball at normal speed when not in debug mode
		ball.speedX = 300;	// Speed the ball moves in X
		ball.speedY = 350;	// Speed the ball moves in Y
	}

	// Load a texture
	if (!ballTexture.loadFromFile("Ball.png"))
	{
		printf("Texture failed to load!\n");
	}

	// Reset the ball, paddle and bricks
	ResetBallAndPaddlePosition();
	ResetBricks();
}

// GameLoop is called repeatedly. Its job is to update the 'game', and draw the screen.
void GameLoop(float elapsedSeconds)
{
	bool playerAlive = currLives > 0;

	if (playerAlive)
	{
		// Move ball
		ball.Move(elapsedSeconds);
	}

	// Debug move ball to mouse position when mouse is clicked
	if (debugMode && IsMouseButtonPressed())
	{
		ball.SetPos((float)GetMouseX(), (float)GetMouseY());
	}


	// Bounce ball off sides and top of the screen
	if (ball.yPos < 0)
	{
		// Hit top of screen
		ball.yPos = 0;
		ball.yVel = ball.speedY;
	}
	if (ball.xPos >= SCREEN_WIDTH)
	{
		// Hit right side
		ball.xPos = (float)SCREEN_WIDTH;
		ball.xVel = -ball.speedX;
	}
	if (ball.xPos <= 0)
	{
		// Hit left side
		ball.xPos = 0;
		ball.xVel = ball.speedX;
	}

	// If the ball goes off the bottom, 'lose a life'
	if (ball.yPos > SCREEN_HEIGHT)
	{
		// Bottom side bounce
		ResetBallAndPaddlePosition();
		currLives--;
	}

	// Move paddle
	if (playerAlive)
	{
		if (IsKeyPressed(sf::Keyboard::Left) || IsKeyPressed(sf::Keyboard::A))
		{
			paddle.x -= paddle.speed * elapsedSeconds;
		}
		if (IsKeyPressed(sf::Keyboard::Right) || IsKeyPressed(sf::Keyboard::D))
		{
			paddle.x += paddle.speed * elapsedSeconds;
		}
	}

	// In debug mode, automatically move paddle to always be under the ball
	if (debugMode)
	{
		paddle.x = ball.xPos - paddle.width / 2;
	}

	// Limit paddle to screen
	if (paddle.x < 0)
	{
		paddle.x = 0;
	}
	if (paddle.x > SCREEN_WIDTH - paddle.width)
	{
		paddle.x = SCREEN_WIDTH - paddle.width;
	}

	// Bounce ball off paddle
	if (ball.xPos >= paddle.x &&
		ball.xPos <= paddle.x + paddle.width &&
		ball.yPos >= paddle.y &&
		ball.yPos <= paddle.y + paddle.height)
	{
		ball.yVel = -ball.speedY;
	}

	// Draw ball
	// ballX, ballY is the center of the ball. DrawTexture takes the top left,
	// so we need to subtract (ball.diameter/2) to calculate the top left.
	DrawTexture(ball.xPos - (ball.diameter / 2), ball.yPos - (ball.diameter / 2), ball.diameter, ball.diameter, ballTexture);
	DrawCircle(ball.xPos, ball.yPos, ball.diameter/2.0f, sf::Color::Yellow);

	// Draw paddles
	DrawRectangle(paddle.x, paddle.y, paddle.width, paddle.height, sf::Color::White);

	// Test collision with bricks and draw them
	for (int i = 0; i < MAX_BRICKS; i++)
	{
		if (bricks[i].IsAlive())
		{
			int hitSide = bricks[i].GetCollisionSide(ball.xPos, ball.yPos);

			// If the ball hit the brick, kill the brick and increase score.
			if (hitSide > -1)
			{
				bricks[i].TakeDamage();
				score++;

				// Depending on which side was penetrated most, change the ball velocity
				switch (hitSide)
				{
				case 0:		// Top
					ball.yVel = -ball.speedY;
					break;
				case 1:		// Bottom
					ball.yVel = ball.speedY;
					break;
				case 2:		// Left
					ball.xVel = -ball.speedX;
					break;
				case 3:		// Right
					ball.xVel = ball.speedX;
					break;
				}
			}
		}

		// The brick may have just been destroyed, so we need to check if it's alive again
		if (bricks[i].IsAlive())
		{
			bricks[i].Draw();
		}
	}

	// Draw lives and score text
	std::string scoreText = "Lives: " + std::to_string(currLives) + "   Score: " + std::to_string(score);
	DrawString(scoreText, 8, (float)SCREEN_HEIGHT - 24, 16, sf::Color::Cyan);

	// Detect all bricks dead
	// Start assuming there are no bricks, and if one is found, set noBricks to false
	bool noBricks = true;
	for (int i = 0; i < MAX_BRICKS; i++)
	{
		if (bricks[i].IsAlive())
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
