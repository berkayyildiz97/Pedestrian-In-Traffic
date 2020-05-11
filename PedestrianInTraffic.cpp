
#include <time.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <GL/glut.h>
#include <string>

using namespace std;

GLsizei winHeight = 600, winWidth = 500;

typedef struct {
	GLint x;
	GLint y;
} Point2D;

typedef struct {
	GLint upperLeftPointY;
	//0 is left, +1 is right
	GLint direction = 0;
} Road;

typedef struct {
	GLint width = 15;
	GLint height = 15;
	Point2D center;
	//0 is left, +1 is right
	GLint direction = 0;
	GLint velocity = 2;
	GLint roadNum;
} Car;

typedef struct {
	GLint width = 35;
	GLint height = 17;
	Point2D center;
	//0 is left, +1 is right
	GLint direction = 0;
	GLint velocity = 1;
	GLint roadNum;
} Truck;

typedef struct {
	GLint radius = 23;
	Point2D center;
	GLint roadNum;
	GLint remainingTime = 200;
} Coin;

typedef struct {
	Point2D tip;
	Point2D p1;
	Point2D p2;
	//0 is down, 1 is up
	GLint direction = 1;
	GLint score = 0;
	GLint timeLeft = 180;
} Agent;

bool isSpawningPlaceEmpty(Car car);
bool isSpawningPlaceEmpty(Truck truck);

Road roads[18];

vector<Car> cars;

vector<Truck> trucks;

vector<Coin> coins;

Agent agent;

bool isGameOver = false;

GLint maxScore = 0;

bool isGamePaused = false;
bool isOneStepNeeded = false;
bool isArrowInputAcceptable = false;

//Compares cars by their location. This is used by sort function to sort cars from up to down and from left to right.
bool compareCarsByLocation(const Car &a, const Car &b) {
	return (a.center.y > b.center.y) || (a.center.y == b.center.y && a.center.x < b.center.x);
}

//Compares trucks by their location. This is used by sort function to sort trucks from up to down and from left to right.
bool compareTrucksByLocation(const Truck& a, const Truck& b) {
	return (a.center.y > b.center.y) || (a.center.y == b.center.y && a.center.x < b.center.x);
}

//Initializes roads vector by creating roads with location and direction.
void initRoads() {
	int roadsUpperLeftPointY[18] = {575, 550, 525, 475, 450, 425, 400, 350, 325, 300, 275, 225, 200, 175, 125, 100, 75, 50};
	for (int i = 0; i < 18; i++) {
		roads[i].upperLeftPointY = roadsUpperLeftPointY[i];
		roads[i].direction = i % 2;
	}
}

//Initializes agent
void initAgent() {
	agent = Agent();
	agent.tip.x = 250;
	agent.tip.y = 25;
	agent.p1.x = 240;
	agent.p1.y = 0;
	agent.p2.x = 260;
	agent.p2.y = 0;
	agent.score = 0;
	agent.direction = 1;
	isGameOver = false;
	isGamePaused = false;
	isOneStepNeeded = false;
	isArrowInputAcceptable = false;
}

//Prints location of the agent
void printAgentLocation() {
	cout << "Agent :" << "tip :(" << agent.tip.x << ", " << agent.tip.y << ") p1 :(" << agent.p1.x << ", " << agent.p1.y << ") p2 :(" << agent.p2.x << ", " << agent.p2.y << ")\n";
}

//Prints locations of the cars
void printCarsLocations() {
	sort(cars.begin(), cars.end(), compareCarsByLocation);
	for (int i = 0; i < cars.size(); i++) {
		Car car = cars[i];
		cout << "Car " << i << " :(" << car.center.x << ", " << car.center.y << ")\n";
	}
}

//Prints locations of the trucks
void printTrucksLocations() {
	sort(trucks.begin(), trucks.end(), compareTrucksByLocation);
	for (int i = 0; i < trucks.size(); i++) {
		Truck truck = trucks[i];
		cout << "Truck " << i << " :(" << truck.center.x << ", " << truck.center.y << ")\n";
	}
}

//Creates a car with random direction in a random road
Car createCar() {
	int direction = rand() % 2;
	int roadRand = rand() % 9;
	int roadNum = 0;
	if (direction == 0) {
		roadNum = roadRand * 2;
	}
	else {
		roadNum = roadRand * 2 + 1;
	}
	Car car = Car();
	car.direction = direction;
	car.roadNum = roadNum;
	Point2D center = Point2D();
	if (direction == 1) {
		center.x = car.width / 2;
	}
	else {
		center.x = 499 - car.width / 2;
	}
	center.y = roads[roadNum].upperLeftPointY - 12;
	car.center = center;
	return car;
}

//Adds a car to cars vector randomly. Adding rate increases while time left is decreasing.
void addCar() {
	int random;
	//To prevent exception caused by mod 0 
	if ((int)(agent.timeLeft / 10.0) == 0) {
		random = 0;
	}
	else {
		random = rand() % (int)(agent.timeLeft / 10.0);
	}
	if (random == 0) {
		Car car = createCar();
		if (isSpawningPlaceEmpty(car)) {
			cars.push_back(car);
		}
		
	}
}

//Draws car
void drawCar(Car car) {
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_POLYGON);
	int x = car.center.x;
	int y = car.center.y;
	glVertex2f(x + car.width / 2, y + car.height / 2);
	glVertex2f(x - car.width / 2, y + car.height / 2);
	glVertex2f(x - car.width / 2, y - car.height / 2);
	glVertex2f(x + car.width / 2, y - car.height / 2);
	glEnd();

}

//Creates a truck with random direction in a random road
Truck createTruck() {
	int direction = rand() % 2;
	int roadRand = rand() % 9;
	int roadNum = 0;
	if (direction == 0) {
		roadNum = roadRand * 2;
	}
	else {
		roadNum = roadRand * 2 + 1;
	}
	Truck truck = Truck();
	truck.direction = direction;
	truck.roadNum = roadNum;
	Point2D center = Point2D();
	if (direction == 1) {
		center.x = truck.width / 2;
	}
	else {
		center.x = 499 - truck.width / 2;
	}
	center.y = roads[roadNum].upperLeftPointY - 12;
	truck.center = center;
	return truck;
}

//Adds a truck to trucks vector randomly. Adding rate increases while time left is decreasing.
void addTruck() {
	int random;
	//To prevent exception caused by mod 0 
	if ((int)(agent.timeLeft / 10.0) == 0) {
		random = 0;
	}
	else {
		random = rand() % (int)(agent.timeLeft / 10.0);
	}
	if (random == 0) {
		Truck truck = createTruck();
		if (isSpawningPlaceEmpty(truck)) {
			trucks.push_back(truck);
		}
	}
}

//Draws truck
void drawTruck(Truck truck) {
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_POLYGON);
	int x = truck.center.x;
	int y = truck.center.y;
	glVertex2f(x + truck.width / 2, y + truck.height / 2);
	glVertex2f(x - truck.width / 2, y + truck.height / 2);
	glVertex2f(x - truck.width / 2, y - truck.height / 2);
	glVertex2f(x + truck.width / 2, y - truck.height / 2);
	glEnd();

}

//Creates a coin in a random road and a random location 
Coin createCoin() {
	int roadNum = rand() % 18;
	Coin coin = Coin();
	coin.roadNum = roadNum;
	Point2D center = Point2D();
	center.x = rand() % 500;
	center.y = roads[roadNum].upperLeftPointY - 12;
	coin.center = center;
	return coin;
}

//Adds a coin to coins vector randomly
void addCoin() {
	int random = rand() % 50;
	if (random == 0) {
		Coin coin = createCoin();
		coins.push_back(coin);
	}
}

//Draws coin
void drawCoin(Coin coin) {
	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_TRIANGLE_FAN);
	int x = coin.center.x;
	int y = coin.center.y;
	glVertex2f(x, y);
	for (int i = 0; i <= 20; i++) {
		glVertex2f(x + ((coin.radius / 2) * cos(i * 2.0f * 3.1415926f / 20)), y + ((coin.radius / 2) * sin(i * 2.0f * 3.1415926f / 20) ) );
	}
	glEnd();
}

//Draws agent
void drawAgent() {
	glBegin(GL_TRIANGLES);
	glColor3f(0.0, 1.0, 0.0);
	glVertex2f(agent.p1.x, agent.p1.y);
	glVertex2f(agent.p2.x, agent.p2.y);
	glVertex2f(agent.tip.x, agent.tip.y);
	glEnd();
}

//Returns true if vector (p1, p2) and vector (p3, p4) intersects. Otherwise, returns false.
bool doTwoLinesIntersect(Point2D p1, Point2D p2, Point2D p3, Point2D p4) {
	return (((p3.x - p1.x) * (p2.y - p1.y) - (p3.y - p1.y) * (p2.x - p1.x)) * ((p4.x - p1.x) * (p2.y - p1.y) - (p4.y - p1.y) * (p2.x - p1.x)) < 0) &&
		(((p1.x - p3.x) * (p4.y - p3.y) - (p1.y - p3.y) * (p4.x - p3.x)) * ((p2.x - p3.x) * (p4.y - p3.y) - (p2.y - p3.y) * (p4.x - p3.x)) < 0);
}

//Initializes the game
void myinit(void) {
	initRoads();

	initAgent();

	glViewport(0, 0, winWidth, winHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)winWidth, 0.0, (GLdouble)winHeight);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();

}

//Draws layout with sidewalks and roads
void drawLayout() {
	glColor3f(0.0, 0.0, 0.0);
	for (int i = 600; i >= 0; i--) {
		if (i % 25 == 0) {
			glEnable(GL_LINE_STIPPLE);
			glLineStipple(1, 0x00FF);
			glBegin(GL_LINES);
			glVertex2f(0, i);
			glVertex2f(500, i);
			glEnd();
			glDisable(GL_LINE_STIPPLE);
		}
		if (i > 575 || (i < 500 && i > 475) || (i < 375 && i > 350)|| (i < 250 && i > 225) || (i < 150 && i > 125) || (i < 25)) {
			glColor3f(0.0, 0.0, 0.0);
			glBegin(GL_LINES);
			glVertex2f(0, i);
			glVertex2f(500, i);
			glEnd();
		}
	}
	
}

//Moves cars and trucks according to their velocities and directions
void moveCarsAndTrucks() {
	for (Car &car : cars) {
		if (car.direction == 0) {
			car.center.x -= car.velocity;
		}
		else {
			car.center.x += car.velocity;
		}
	}

	for (Truck &truck : trucks) {
		if (truck.direction == 0) {
			truck.center.x -= truck.velocity;
		}
		else {
			truck.center.x += truck.velocity;
		}
	}
}

//Removes screened out cars and trucks from vectors
void removeScreenedOutCarsAndTrucks() {
	for (vector<Car>::iterator iter = cars.begin(); iter != cars.end();) {
		if (iter->center.x <= -iter->width / 2 || iter->center.x >= 499 + iter->width / 2)
			iter = cars.erase(iter);
		else {
		++iter;
		}
	}
	for (vector<Truck>::iterator iter = trucks.begin(); iter != trucks.end();) {
		if (iter->center.x <= -iter->width / 2 || iter->center.x >= 499 + iter->width / 2)
			iter = trucks.erase(iter);
		else {
			++iter;
		}
	}
}

//Decreases the remaining times of all coins by 1 
void decreaseCoinsRemainingTime() {
	for (Coin &coin : coins) {
		coin.remainingTime--;
	}
}

//Removes coins whose remaining time is less or equal to 0
void removeDeadCoins() {
	for (vector<Coin>::iterator iter = coins.begin(); iter != coins.end();) {
		if (iter->remainingTime <= 0)
			iter = coins.erase(iter);
		else {
			++iter;
		}
	}
}

//Checks if there is a collision between any vehicle and the agent. If so, game over.
//It actually catches the collision by checking if an edge of the agent (triangle) intersects with any edge of any vehicle (rectangle).
void checkCarAndTruckCollision() {
	Point2D	agentTriangleCorners[3];
	agentTriangleCorners[0] = agent.tip;
	agentTriangleCorners[1] = agent.p1;
	agentTriangleCorners[2] = agent.p2;
	
	Point2D carRectangleCorners[4];
	for (Car car : cars) {
		carRectangleCorners[0].x = car.center.x - car.width / 2;
		carRectangleCorners[0].y = car.center.y + car.height / 2;
		carRectangleCorners[1].x = car.center.x + car.width / 2;
		carRectangleCorners[1].y = car.center.y + car.height / 2;
		carRectangleCorners[2].x = car.center.x + car.width / 2;
		carRectangleCorners[2].y = car.center.y - car.height / 2;
		carRectangleCorners[3].x = car.center.x - car.width / 2;
		carRectangleCorners[3].y = car.center.y - car.height / 2;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				if (i == 2 && j == 3) {
					if (doTwoLinesIntersect(agentTriangleCorners[i], agentTriangleCorners[0], carRectangleCorners[j], carRectangleCorners[0])) {
						isGameOver = true;
						if (agent.score > maxScore) {
							maxScore = agent.score;
						}
					}
				}
				else if (i == 2) {
					if (doTwoLinesIntersect(agentTriangleCorners[i], agentTriangleCorners[0], carRectangleCorners[j], carRectangleCorners[j + 1])) {
						isGameOver = true;
						if (agent.score > maxScore) {
							maxScore = agent.score;
						}
					}
				}
				else if (j == 3 ) {
					if (doTwoLinesIntersect(agentTriangleCorners[i], agentTriangleCorners[i + 1], carRectangleCorners[j], carRectangleCorners[0])) {
						isGameOver = true;
						if (agent.score > maxScore) {
							maxScore = agent.score;
						}
					}
				}
				else {
					if (doTwoLinesIntersect(agentTriangleCorners[i], agentTriangleCorners[i + 1], carRectangleCorners[j], carRectangleCorners[j + 1])) {
						isGameOver = true;
						if (agent.score > maxScore) {
							maxScore = agent.score;
						}
					}
				}
			}
		}
	}
	Point2D truckRectangleCorners[4];
	for (Truck truck : trucks) {
		truckRectangleCorners[0].x = truck.center.x - truck.width / 2;
		truckRectangleCorners[0].y = truck.center.y + truck.height / 2;
		truckRectangleCorners[1].x = truck.center.x + truck.width / 2;
		truckRectangleCorners[1].y = truck.center.y + truck.height / 2;
		truckRectangleCorners[2].x = truck.center.x + truck.width / 2;
		truckRectangleCorners[2].y = truck.center.y - truck.height / 2;
		truckRectangleCorners[3].x = truck.center.x - truck.width / 2;
		truckRectangleCorners[3].y = truck.center.y - truck.height / 2;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				if (i == 2 && j == 3) {
					if (doTwoLinesIntersect(agentTriangleCorners[i], agentTriangleCorners[0], truckRectangleCorners[j], truckRectangleCorners[0])) {
						isGameOver = true;
						if (agent.score > maxScore) {
							maxScore = agent.score;
						}
					}
				}
				else if (i == 2) {
					if (doTwoLinesIntersect(agentTriangleCorners[i], agentTriangleCorners[0], truckRectangleCorners[j], truckRectangleCorners[j + 1])) {
						isGameOver = true;
						if (agent.score > maxScore) {
							maxScore = agent.score;
						}
					}
				}
				else if (j == 3) {
					if (doTwoLinesIntersect(agentTriangleCorners[i], agentTriangleCorners[i + 1], truckRectangleCorners[j], truckRectangleCorners[0])) {
						isGameOver = true;
						if (agent.score > maxScore) {
							maxScore = agent.score;
						}
					}
				}
				else {
					if (doTwoLinesIntersect(agentTriangleCorners[i], agentTriangleCorners[i + 1], truckRectangleCorners[j], truckRectangleCorners[j + 1])) {
						isGameOver = true;
						if (agent.score > maxScore) {
							maxScore = agent.score;
						}
					}
				}
			}
		}
	}
}

//Checks if there is a vehicle infront of any car. If so, the car slows down to velocity of the vehicle.
void checkIsThereVehicleInfrontOfCars() {
	for (Car &car : cars) {
		for (Truck truck : trucks) {
			if (truck.roadNum == car.roadNum &&
				((car.direction == 1 && truck.center.x > car.center.x && truck.center.x - car.center.x < car.width / 2 + truck.width / 2 + 5)
					|| (car.direction == 0 && truck.center.x < car.center.x && car.center.x - truck.center.x < car.width / 2 + truck.width / 2 + 5))) {
				car.velocity = truck.velocity;
			}
		}
		for (Car checkingCar : cars) {
			if (checkingCar.roadNum == car.roadNum &&
				((car.direction == 1 && checkingCar.center.x > car.center.x&& checkingCar.center.x - car.center.x < car.width / 2 + car.width / 2 + 5)
					|| (car.direction == 0 && checkingCar.center.x < car.center.x && car.center.x - checkingCar.center.x < car.width / 2 + car.width / 2 + 5))) {
				car.velocity = checkingCar.velocity;
			}
		}
	}
}

//Returns true if location of the spawning car is empty. Otherwise, returns false.
bool isSpawningPlaceEmpty(Car spawningCar) {
	//Added agent's width (20) + 10 pixels between spawning cars/trucks to give the agent a change for passing.
	for (Truck truck : trucks) {
		if (truck.roadNum == spawningCar.roadNum &&
			((spawningCar.direction == 1 && truck.center.x - spawningCar.center.x < spawningCar.width / 2 + truck.width + 30)
				|| (spawningCar.direction == 0 && spawningCar.center.x - truck.center.x < spawningCar.width / 2 + truck.width + 30))) {
			return false;
		}
	}
	for (Car car : cars) {
		if (car.roadNum == spawningCar.roadNum &&
			((spawningCar.direction == 1 && car.center.x - spawningCar.center.x < spawningCar.width / 2 + car.width / 2 + 30)
				|| (spawningCar.direction == 0 && spawningCar.center.x - car.center.x < spawningCar.width / 2 + car.width / 2 + 30))) {
			return false;
		}
	}
	return true;
}

//Returns true if location of the spawning truck is empty. Otherwise, returns false.
bool isSpawningPlaceEmpty(Truck spawningTruck) {
	//Added agent's width (20) + 10 pixels between spawning cars/trucks to give the agent a change for passing.
	for (Truck truck : trucks) {
		if (truck.roadNum == spawningTruck.roadNum && 
			( (spawningTruck.direction == 1 && truck.center.x - spawningTruck.center.x < spawningTruck.width / 2 + truck.width / 2 + 30)
				|| (spawningTruck.direction == 0 && spawningTruck.center.x - truck.center.x < spawningTruck.width / 2 + truck.width / 2 + 30) ) ) {
			return false;
		}
	}
	for (Car car : cars) {
		if (car.roadNum == spawningTruck.roadNum &&
			((spawningTruck.direction == 1 && car.center.x - spawningTruck.center.x < spawningTruck.width / 2 + car.width / 2 + 30)
				|| (spawningTruck.direction == 0 && spawningTruck.center.x - car.center.x < spawningTruck.width / 2 + car.width / 2 + 30))) {
			return false;
		}
	}
	return true;
}

//Checks if there is a collision between any coin and the agent. If so, the agent gains 5 points.
//It actually catches the collision by checking if an edge of the agent (triangle) intersects with any edge of the bounding box of the coin.
void checkCoinCollision() {
	Point2D	agentTriangleCorners[3];

	agentTriangleCorners[0] = agent.tip;
	agentTriangleCorners[1] = agent.p1;
	agentTriangleCorners[2] = agent.p2;

	Point2D coinBoundingBoxCorners[4];
	for (vector<Coin>::iterator coin = coins.begin(); coin != coins.end();) {
		coinBoundingBoxCorners[0].x = coin->center.x + coin->radius / 2;
		coinBoundingBoxCorners[0].y = coin->center.y + coin->radius / 2;
		coinBoundingBoxCorners[1].x = coin->center.x + coin->radius / 2;
		coinBoundingBoxCorners[1].y = coin->center.y - coin->radius / 2;
		coinBoundingBoxCorners[2].x = coin->center.x - coin->radius / 2;
		coinBoundingBoxCorners[2].y = coin->center.y + coin->radius / 2;
		coinBoundingBoxCorners[3].x = coin->center.x - coin->radius / 2;
		coinBoundingBoxCorners[3].y = coin->center.y - coin->radius / 2;
		bool isCoinDeleted = false;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				if (i == 2 && j == 3) {
					if (doTwoLinesIntersect(agentTriangleCorners[i], agentTriangleCorners[0], coinBoundingBoxCorners[j], coinBoundingBoxCorners[0])) {
						agent.score += 5;
						coin = coins.erase(coin);
						isCoinDeleted = true;
						break;
					}
				}
				else if (i == 2) {
					if (doTwoLinesIntersect(agentTriangleCorners[i], agentTriangleCorners[0], coinBoundingBoxCorners[j], coinBoundingBoxCorners[j + 1])) {
						agent.score += 5;
						coin = coins.erase(coin);
						isCoinDeleted = true;
						break;
					}
				}
				else if (j == 3) {
					if (doTwoLinesIntersect(agentTriangleCorners[i], agentTriangleCorners[i + 1], coinBoundingBoxCorners[j], coinBoundingBoxCorners[0])) {
						agent.score += 5;
						coin = coins.erase(coin);
						isCoinDeleted = true;
						break;
					}
				}
				else {
					if (doTwoLinesIntersect(agentTriangleCorners[i], agentTriangleCorners[i + 1], coinBoundingBoxCorners[j], coinBoundingBoxCorners[j + 1])) {
						agent.score += 5;
						coin = coins.erase(coin);
						isCoinDeleted = true;
						break;
					}
				}
			}
			if (isCoinDeleted) {
				break;
			}
		}
		if (!isCoinDeleted) {
			++coin;
		}
	}
}

//Draws score
void drawScore() {
	glColor3f(1.0, 0.0, 1.0);
	glRasterPos2f(0, 580);
	string scoreStr = string("Score :") + to_string(agent.score);
	for (int i = 0; i < scoreStr.length(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, scoreStr[i]);
	}
}

//Draws time left
void drawTimeLeft() {
	glColor3f(1.0, 0.0, 1.0);
	glRasterPos2f(345, 580);
	string timeLeftStr = string("Time Left :") + to_string(agent.timeLeft / 60) + string(":");
	if (agent.timeLeft % 60 < 10) {
		timeLeftStr = timeLeftStr + string("0") + to_string(agent.timeLeft % 60);
	}
	else {
		timeLeftStr = timeLeftStr + to_string(agent.timeLeft % 60);
	}
	for (int i = 0; i < timeLeftStr.length(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, timeLeftStr[i]);
	}
}

//Draws "GAME OVER" and related information
void drawGameOver() {
	glColor3f(1.0, 0.0, 0.0);
	int sideWalkPixels[] = { 5, 230, 480};
	for (int i : sideWalkPixels) {
		glRasterPos2f(180, i);
		string gameOverStr = string("GAME OVER");
		for (int i = 0; i < gameOverStr.length(); i++)
		{

			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, gameOverStr[i]);
		}
	}
	glRasterPos2f(140, 355);
	string pressRStr = string("PRESS R TO REPLAY");
	for (int i = 0; i < pressRStr.length(); i++)
	{

		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, pressRStr[i]);
	}

	glRasterPos2f(150, 130);
	string pressQStr = string("PRESS Q TO QUIT");

	for (int i = 0; i < pressQStr.length(); i++)
	{

		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, pressQStr[i]);
	}
	
}

//Draws "GAME PAUSED" and related information
void drawGamePaused() {
	glColor3f(1.0, 0.0, 0.0);
	int sideWalkPixels[] = { 5, 230, 480 };
	for (int i : sideWalkPixels) {
		glRasterPos2f(180, i);
		string gameOverStr = string("GAME PAUSED");
		for (int i = 0; i < gameOverStr.length(); i++)
		{

			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, gameOverStr[i]);
		}
	}
	glRasterPos2f(10, 355);

	string pressRStr = string("CLICK LEFT MOUSE BUTTON TO RESUME");
	for (int i = 0; i < pressRStr.length(); i++)
	{

		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, pressRStr[i]);
	}

	glRasterPos2f(20, 130);
	string pressQStr = string("CLICK RIGHT MOUSE BUTTON TO STEP");
	for (int i = 0; i < pressQStr.length(); i++)
	{

		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, pressQStr[i]);
	}

}

//Draws max score
void drawMaxScore() {
	glColor3f(1.0, 0.0, 1.0);
	glRasterPos2f(150, 580);
	string maxScoreStr = string("Max Score :") + to_string(maxScore);

	for (int i = 0; i < maxScoreStr.length(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, maxScoreStr[i]);
	}
}

//Display callback
void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	drawLayout();

	for (Car car : cars) {
		drawCar(car);
	}
	for (Truck truck : trucks) {
		drawTruck(truck);
	}
	for (Coin coin : coins) {
		drawCoin(coin);
	}

	drawScore();
	drawTimeLeft();
	drawMaxScore();
	drawAgent();

	if (isGameOver) {
		drawGameOver();
	}

	else if (isGamePaused) {
		drawGamePaused();
	}

	glFlush();
	glutSwapBuffers();

	checkCarAndTruckCollision();
	checkCoinCollision();
	checkIsThereVehicleInfrontOfCars();
}

//Timer funtion for game steps
void myTimer(int te)
{
	if ((! (isGameOver || isGamePaused)) || isOneStepNeeded) {
		removeScreenedOutCarsAndTrucks();
		removeDeadCoins();
		moveCarsAndTrucks();
		decreaseCoinsRemainingTime();
		addCar();
		addTruck();
		addCoin();
		if (isOneStepNeeded) {
			cout << "NEXT STEP\n";
			printAgentLocation();
			printCarsLocations();
			printTrucksLocations();
		}
		isOneStepNeeded = false;
		isArrowInputAcceptable = true;
	}

	glutTimerFunc(16, myTimer, 0);
	glutPostRedisplay();
}

//Timer function for timer left
void myTimerForTimeLeft(int te) {

	if (!(isGameOver || (isGamePaused && ( !isOneStepNeeded ) ))) {
		agent.timeLeft--;
		if (agent.timeLeft == 0) {
			isGameOver = true;
			if (agent.score > maxScore) {
				maxScore = agent.score;
			}
		}
		if (!isGamePaused) {
			printAgentLocation();
			printCarsLocations();
			printTrucksLocations();
		}
	}
	glutTimerFunc(1000, myTimerForTimeLeft, 0);
	glutPostRedisplay();
}

//Special input function for arrow inputs
void mySpecialInput(int key, int x, int y)
{
	if (isGameOver || (isGamePaused && (!isArrowInputAcceptable))) {
		return;
	}
	switch (key)
	{
	case GLUT_KEY_UP:
		if (agent.direction == 0) {
			isGameOver = true;
			if (agent.score > maxScore) {
				maxScore = agent.score;
			}
		}
		else {
			agent.p1.y += 25;
			agent.p2.y += 25;
			agent.tip.y += 25;
			if (agent.tip.y == 600) {
				agent.p1.y += 25;
				agent.p2.y += 25;
				agent.tip.y -= 25;
				agent.direction = 0;
			}
			agent.score += 1;
		}
		break;
	case GLUT_KEY_DOWN:
		if (agent.direction == 1) {
			isGameOver = true;
			if (agent.score > maxScore) {
				maxScore = agent.score;
			}
		}
		else {
			agent.p1.y -= 25;
			agent.p2.y -= 25;
			agent.tip.y -= 25;
			if (agent.tip.y == 0) {
				agent.p1.y -= 25;
				agent.p2.y -= 25;
				agent.tip.y += 25;
				agent.direction = 1;
			}
			agent.score += 1;
		}
		break;
	case GLUT_KEY_LEFT:
		agent.p1.x -= 20;
		agent.p2.x -= 20;
		agent.tip.x -= 20;
		if (agent.tip.x <= 0) {
			isGameOver = true;
			if (agent.score > maxScore) {
				maxScore = agent.score;
			}
		}
		break;
	case GLUT_KEY_RIGHT:
		agent.p1.x += 20;
		agent.p2.x += 20;
		agent.tip.x += 20;
		if (agent.tip.x >= 499) {
			isGameOver = true;
			if (agent.score > maxScore) {
				maxScore = agent.score;
			}
		}
		break;
	}
	isArrowInputAcceptable = false;
	checkCarAndTruckCollision();
	checkCoinCollision();
	glutPostRedisplay();
}

//Keyboard input function for replay and quit
void myKeyboardInput(unsigned char key, int x, int y) {
	switch (key) {
	case 'q' :
	case 'Q':
		exit(0);
		break;
	case 'r':
	case 'R':
		if (agent.score > maxScore) {
			maxScore = agent.score;
		}
		isGameOver = false;
		cars.clear();
		trucks.clear();
		coins.clear();
		initAgent();
		break;
	}
	glutPostRedisplay();
}

//Mouse input function for pausing, resuming and step mode
//If game is resuming and left/right mouse button is clicked, game pauses. If game is paused and left mouse button is clicked, game resumes.
//If game is paused and right mouse button is clicked, it performs one step in step mode.
void myMouseInput(int button, int state, int x, int y) {
	if (isGameOver) {
		return;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (isGamePaused) {
			isGamePaused = false;
		}
		else {
			isGamePaused = true;
			isArrowInputAcceptable = false;
		}
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		if (isGamePaused) {
			isOneStepNeeded = true;
		}
		else {
			isGamePaused = true;
		}
	}
	glutPostRedisplay();
}

//Main function
int main(int argc, char** argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Pedestrian In Traffic ");
	myinit();
	glutDisplayFunc(myDisplay);
	glutSpecialFunc(mySpecialInput);
	glutKeyboardFunc(myKeyboardInput);
	glutMouseFunc(myMouseInput);

	srand(time(NULL));
	myTimer(0);
	myTimerForTimeLeft(0);

	glutMainLoop();
}