# Pedestrian In Traffic Game

This is an OpenGL project written in C++ for Computer Graphics class.

![Image](https://user-images.githubusercontent.com/36446866/81580199-cf081b00-93b5-11ea-9eb8-18336c275fbe.png)

## How To Play
You can move the triangle agent with arrow keys. You gain 1 point per step towards your direction. If you hit a coin, you gain 5 points. 

If you move opposite to your direction, game over. If you hit a car or truck, game over. If the agent is out of bounds of the screen, game over. If there is no time left, game over. 

The game becomes harder as time goes. If the game is over, you can press R to replay or Q to quit. 

You can pause the game by clicking left or right mouse buttons. Then you can resume the game by clicking left mouse button or you can click right mouse button to perform a step.

## Extra Features

* If a car is getting close to a vehicle that is slower than the car, the car slows down to velocity of the vehicle.
* Maximum score is implemented and displayed.
* User can press R to replay the game.
* Time left is implemented and displayed.
* The spawning rate of the vehicles increases while time left is decreasing.
* If the agent is out of bounds from a lateral edge, game over.
* Spawning on another vehicle is prevented.
* Time left is decreasing also in step mode.
* Printing information to console in normal mode is implemented as well as step mode.
* It prints locations of the vehicles as sorted from up to down and from left to right.
* Game paused information is displayed.
