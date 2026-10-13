#pragma once
#include <raylib.h>
#include <vector>

class Boid
{
private:
	//Instances
	std::vector<Vector2> positions;
	std::vector<bool> isGoalReached;
	
	//Shared
	unsigned int maxNumberOfBoids;
	unsigned int radius;
	float speed;

	std::vector<std::vector<char>>* mapPointer;

	bool IsFreeSpace(int _boidInstance, Vector2 _goalPosition);

public:
	Boid();
	~Boid();

	void ChangeTheMap(std::vector<std::vector<char>>* _mapPointer);
	void AddNewBoid(Vector2 _position);

	void Update();
	void Draw();
};