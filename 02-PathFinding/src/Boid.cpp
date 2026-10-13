#include "Boid.h"
#include "Config.h"
#include <cmath>

Boid::Boid():
	maxNumberOfBoids(64),
	radius(3),
	speed(10),
	mapPointer(nullptr)
{
	positions.reserve(maxNumberOfBoids);
}

Boid::~Boid()
{
	positions.clear();
}

void Boid::ChangeTheMap(std::vector<std::vector<char>>* _mapPointer)
{
	mapPointer = _mapPointer;
}

void Boid::AddNewBoid(Vector2 _position)
{
	_position.x *= Config::pixelWidth;
	_position.y *= Config::pixelHeight;
	if (mapPointer == nullptr) return;
	if (mapPointer->size() == 0) return;
	if (mapPointer->size() <= _position.x || mapPointer->at(0).size() <= _position.y) return;

	char currentSpace = (*mapPointer)[_position.x][_position.y];
	if (currentSpace == '0') return;
	if (currentSpace == '1') return;
	if (currentSpace == 'e') return;
	if (currentSpace >= 65)  return;

	if (positions.size() >= maxNumberOfBoids) return;
	_position.x = (int)_position.x;
	_position.y = (int)_position.y;
	positions.push_back(_position);
	isGoalReached.push_back(false);
}

void Boid::Update()
{
	if (mapPointer == nullptr) return;
	for (int i = 0; i < positions.size(); i++)
	{
		if (isGoalReached[i])continue;
		float distanceToRemove = 0;
		float frameDistance = speed * GetFrameTime();
		//Not the best way but it works :/ 
		// I will try to find a way for the Boids to work with every Pathfindings
		while (frameDistance > 0)
		{
			char currentSpace = (*mapPointer)[positions[i].x][positions[i].y];
			//Left
			if (currentSpace == '4')
			{
				distanceToRemove = std::remainderf(positions[i].x, 1.0f);
				if (distanceToRemove == 0.0f) distanceToRemove = 1.0f;
				distanceToRemove = fminf(distanceToRemove, frameDistance);
				positions[i].x -= frameDistance;
			}
			//Right
			else if (currentSpace == '6')
			{
				distanceToRemove = std::remainderf(positions[i].x, 1.0f);
				if (distanceToRemove == 0.0f) distanceToRemove = 1.0f;
				else distanceToRemove = 1 - distanceToRemove;
				distanceToRemove = fminf(distanceToRemove, frameDistance);
				positions[i].x += frameDistance;
			}
			//Up
			else if (currentSpace == '2')
			{
				distanceToRemove = std::remainderf(positions[i].y, 1.0f);
				if (distanceToRemove == 0.0f) distanceToRemove = 1.0f;
				distanceToRemove = fminf(distanceToRemove, frameDistance);
				positions[i].y -= frameDistance;
			}
			//Down
			else if (currentSpace == '8')
			{
				distanceToRemove = std::remainderf(positions[i].y, 1.0f);
				if (distanceToRemove == 0.0f) distanceToRemove = 1.0f;
				else distanceToRemove = 1 - distanceToRemove;
				distanceToRemove = fminf(distanceToRemove, frameDistance);
				positions[i].y+= frameDistance;
			}
			frameDistance -= abs(distanceToRemove);
		}
	}
}

void Boid::Draw()
{
	for (Vector2 p : positions)
	{
		DrawEllipse(p.x * Config::pixelWidth, p.y * Config::pixelHeight, radius * Config::pixelWidth, radius * Config::pixelHeight, RED);
	}
}

bool Boid::IsFreeSpace(int _boidInstance, Vector2 _goalPosition)
{
	return false;
}