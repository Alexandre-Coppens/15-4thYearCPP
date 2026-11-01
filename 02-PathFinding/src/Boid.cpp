#include "Boid.h"
#include "Config.h"
#include "raymath.h"
#include <cmath>
#include <iostream>

Boid::Boid():
	maxNumberOfBoids(64),
	radius(7),
	speed(100)
{
	positions.reserve(maxNumberOfBoids);
	mapPointer = nullptr;
}

Boid::~Boid()
{
	positions.clear();
}

void Boid::setMapPointer(std::vector<std::vector<char>>* _newMap)
{
	mapPointer = _newMap;
}

void Boid::AddNewBoid(Vector2 _position)
{
	if (positions.size() >= maxNumberOfBoids) return;
	_position.x = (int)(static_cast<float>(_position.x) * Config::pixelWidth);
	_position.y = (int)(static_cast<float>(_position.y) * Config::pixelHeight);
	if (mapPointer == nullptr) return;
	if (mapPointer->size() == 0) return;
	if (mapPointer->size() <= _position.x || mapPointer->at(0).size() <= _position.y) return;
	char currentSpace = mapPointer->at(_position.x).at(_position.y);
	if (currentSpace == '0') return;
	if (currentSpace == '1') return;
	if (currentSpace == 'e') return;
	if (currentSpace == '.')  return;
	positions.push_back(_position);
	isGoalReached.push_back(false);
}

void Boid::KillAllBoids()
{
	positions.clear();
	isGoalReached.clear();
}

void Boid::Update()
{
	for (int i = 0; i < positions.size(); i++)
	{
		if (isGoalReached[i])continue;
		if (mapPointer == nullptr)
		{
			std::cout << "ERROR::BOIDS::Map Pointer is NULL\n";
			return;
		}
		float distanceToRemove = 0;
		float frameDistance = speed * GetFrameTime();
		//Not the best way but it works :/ 
		// I will try to find a way for the Boids to work with every Pathfindings
		while (frameDistance > 0 && !isGoalReached[i])
		{
			char currentSpace = mapPointer->at(positions[i].x).at(positions[i].y);
			//Right
			if (currentSpace == '4')
			{
				if (!IsFreeSpace(i, Vector2Add(positions[i], Vector2{ 1, 0 })))
				{
					frameDistance = 0;
					continue;
				}
				distanceToRemove = GetDistanceToRemove(positions[i].x, frameDistance);
				positions[i].x += distanceToRemove;
			}
			//Left
			else if (currentSpace == '6')
			{
				if (!IsFreeSpace(i, Vector2Add(positions[i], Vector2{ -1, 0 })))
				{
					frameDistance = 0;
					continue;
				}
				distanceToRemove = GetDistanceToRemove(positions[i].x, frameDistance);
				positions[i].x -= distanceToRemove;
			}
			//Up
			else if (currentSpace == '2')
			{
				if (!IsFreeSpace(i, Vector2Add(positions[i], Vector2{ 0, 1 })))
				{
					frameDistance = 0;
					continue;
				}
				distanceToRemove = GetDistanceToRemove(positions[i].y, frameDistance);
				positions[i].y -= distanceToRemove;
			}
			//Down
			else if (currentSpace == '8')
			{
				if (!IsFreeSpace(i, Vector2Add(positions[i], Vector2{ 0, -1 })))
				{
					frameDistance = 0;
					continue;
				}
				distanceToRemove = GetDistanceToRemove(positions[i].y, frameDistance);
				positions[i].y += distanceToRemove;
			}
			else if (currentSpace == 'e')
			{
				isGoalReached[i] = true;
			}
			frameDistance -= abs(distanceToRemove);
		}
	}
}

float Boid::GetDistanceToRemove(float _currentPositionToCheck, float _frameDistance)
{
	float result = std::remainderf(_currentPositionToCheck, 1.0f);
	if (result == 0.0f) result = 1.0f;
	else result = 1 - result;
	result = fminf(result, _frameDistance);
	return result;
}

void Boid::Draw()
{
	for (Vector2 p : positions)
	{
		DrawEllipse(p.x / Config::pixelWidth, p.y / Config::pixelHeight, radius / Config::pixelWidth, radius / Config::pixelHeight, RED);
	}
}

bool Boid::IsFreeSpace(int _boidInstance, Vector2 _goalPosition)
{
	char currentSpace = mapPointer->at(positions[_boidInstance].x).at(positions[_boidInstance].y);
	for (int i = 0; i < positions.size(); i++)
	{
		if (i == _boidInstance) continue;
		if (Vector2DistanceSqr(positions[i], _goalPosition) <= radius * radius)
		{
			if (isGoalReached[i]) isGoalReached[_boidInstance] = true;
			return false;
		}
	}
	return true;
}