//
// breakout.c
//
// Computer Science 50
// Problem Set 3
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include <spl/gevents.h>
#include <spl/gobjects.h>
#include <spl/gwindow.h>

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

//height and width of the paddle
#define paddleWidth 50
#define paddleHeight 10

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3


// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
GLabel initLiveCounter(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
void updateLives(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));
    double yVelocity = drand48() + .1;
    double xVelocity = drand48() + .1;

    // counter for score
    int score = 0;
    
    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);
    
    // instantiate lives, located in the bottom-left corner of the screen
    GLabel liveCounter = initLiveCounter(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;

    // wait for the user to click before starting the game
    waitForClick();
    
    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
        // the paddle follows the mouse, on the x-axis only
        GEvent guide = getNextEvent(MOUSE_EVENT);
        if (guide != NULL)
        {
            if (getEventType(guide) == MOUSE_MOVED)
            {
                // sets the paddles x-coord to the mouse's x-coord
                double x = getX(guide) - getWidth(paddle)/2;
                setLocation(paddle, x, 500);
            }
        }
        // moves the ball in a certain direction depending on
        // the random velocities
        move(ball, xVelocity, yVelocity);
        pause(2);
        // check if the ball collided with anything
        GObject object = detectCollision(window, ball);
        // if the ball hit a side wall, make it bounce off of the wall
        if (getX(ball) + getWidth(ball) >= WIDTH)
        {
            xVelocity = -xVelocity;
        }
        // if the ball hit a side wall, make it bounce off of the wall
        else if (getX(ball) <= 0)
        {
            xVelocity = -xVelocity;
        }
        // if the ball hit the top, make it bounce off
        else if (getY(ball) <= 0)
        {
            yVelocity = -yVelocity;
        }
        // if the ball hit the paddle, make it bounce off and speed up a bit
        else if (object == paddle)
        {
            yVelocity = -yVelocity;
            yVelocity = yVelocity + yVelocity * 0.01;
        }
        // if the ball is hitting something...
        else if (object != NULL)
        {
            // if the ball is hitting a rectangle, it must be a brick
            if (strcmp(getType(object), "GRect") == 0)
            {
                // make the ball bounce off of the brick,
                // remove the brick from the window, and update the score
                yVelocity = -yVelocity;
                removeGWindow(window, object);
                bricks--;
                score++;
                updateScoreboard(window, label, score);
            }
        }
        // if the ball passes the paddle, the player loses a life
        if (getY(ball) >= HEIGHT)
        {
            lives--;
            setLocation(ball, WIDTH/2-10, HEIGHT/2+10);
            setLocation(paddle, WIDTH/2-25, HEIGHT-100);
            updateLives(window, liveCounter, lives);
            // waits for the player to click before starting again
            if (lives > 0)
            {
                waitForClick();
            }
        }
    }
    // if the player broke all of the bricks, tell them they won
    if (bricks == 0)
    {
        GLabel win = newGLabel("YOU WIN!");
        setFont(win, "SansSerif-48");
        setColor(win, "BLACK");
        double x = (getWidth(window) - getWidth(win)) / 2;
        double y = (getHeight(window) - getHeight(win)) / 2;
        setLocation(win, x, y);
        add(window, win);
    }
    // if the player lost all of their lives, tell them they lost
    else
    {
        GLabel lose = newGLabel("YOU LOSE.");
        setFont(lose, "SansSerif-48");
        setColor(lose, "BLACK");
        double x = (getWidth(window) - getWidth(lose)) / 2;
        double y = (getHeight(window) - getHeight(lose)) / 2;
        setLocation(lose, x, y);
        add(window, lose);
    }
    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            GRect brick = newGRect(12+j*38, 10+i*18, 29, 12);
            if (i == 0)
            {
                setColor(brick, "RED");
            }
            else if (i == 1)
            {
                setColor(brick, "ORANGE");
            }
            else if (i == 2)
            {
                setColor(brick, "YELLOW");
            }
            else if (i == 3)
            {
                setColor(brick, "GREEN");
            }
            else if (i == 4)
            {
                setColor(brick, "BLUE");
            }
            setFilled(brick, true);
            add(window, brick);
        }
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    GOval ball = newGOval(WIDTH/2-10, HEIGHT/2+10, 20, 20);
    setColor(ball, "BLACK");
    setFilled(ball, true);
    add(window, ball);
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    GRect paddle = newGRect(WIDTH/2-25, HEIGHT-100, paddleWidth, paddleHeight);
    setColor(paddle, "BLACK");
    setFilled(paddle, true);
    add(window, paddle);
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    GLabel label = newGLabel("");
    setLabel(label, "0");
    setFont(label, "SansSerif-36");
    setColor(label, "LIGHT_GRAY");
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
    add(window, label);
    return label;
}

/**
 * Instantiates, configures, and returns label for liveCounter.
 */
GLabel initLiveCounter(GWindow window)
{
    GLabel lives = newGLabel("Lives:");
    setFont(lives, "SansSerif-15");
    setColor(lives, "GRAY");
    setLocation(lives, 5, 530);
    add(window, lives);
    GLabel liveCounter = newGLabel("3");
    setFont(liveCounter, "SansSerif-15");
    setColor(liveCounter, "DARK_GRAY");
    setLocation(liveCounter, 48, 530);
    add(window, liveCounter);
    return liveCounter;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Updates lives' label.
 */
void updateLives(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
