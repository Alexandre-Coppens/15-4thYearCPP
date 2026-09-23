#include "Boids.h"
#include "raymath.h"
#include "Config.h"
#include <iostream>
#include <string>

Boids::Boids() :
	size(2.0f),
	speed(50.0f),
	dodgeCollisionSize(30.0f),
	visionCollisionSize(100.0f),
	visionCone(0.3f),
	dodgeImportance(1.5f),
	alignImportance(2.0f),
	cohesionImportance(1.5f),
	gridSize(50)
{
	//Please do not look at this
	_gridIndices = std::vector< std::vector<std::vector<int>>>(gridSize, std::vector<std::vector<int>>(gridSize, std::vector<int>()));
}

Boids::~Boids()
{
}

void Boids::Start(int _spawnNumber)
{
	for (int i = 0; i < _spawnNumber; i++)
	{
		position.push_back(Vector2{ static_cast<float>(GetRandomValue(0, WindowWidth - 1)), static_cast<float>(GetRandomValue(0, WindowHeight - 1)) });
		velocity.push_back(Vector2Normalize( Vector2{ static_cast<float>(GetRandomValue(-100, 100) * 0.01f), static_cast<float>(GetRandomValue(-100, 100) * 0.01f) }));
		_gridIndices[floor(position[i].x / (WindowWidth / gridSize))][floor(position[i].y / (WindowHeight / gridSize))].push_back(i);
	}
}

void Boids::Update()
{
	for (int i = 0; i < position.size(); i++)
	{
		int posX = position[i].x / (WindowWidth / gridSize);
		int posY = position[i].y / (WindowHeight / gridSize);

		std::vector<int> neighbors = GridChecking(posX, posY);
		neighbors = FOVTesting(i, neighbors);
		neighbors = DistanceTesting(i, neighbors, visionCollisionSize);

		Vector2 dodgeDirection = GetDodgeDirection(i, neighbors);
		Vector2 alignDirection = GetAlignDirection(i, neighbors);
		Vector2 cohesionDirection = GetCohesionDirection(i, neighbors);

		Vector2 addedDirections = Vector2Add(Vector2Scale(dodgeDirection, dodgeImportance * GetFrameTime()),
											 Vector2Add(Vector2Scale(alignDirection, alignImportance * GetFrameTime()),
														Vector2Scale(cohesionDirection, cohesionImportance * GetFrameTime())));

		velocity[i] = Vector2Add(velocity[i], addedDirections);
		velocity[i] = Vector2Normalize(velocity[i]);

		if (position[i].x <= 0 || position[i].x >= WindowWidth - 1)		velocity[i] = Vector2Multiply(velocity[i], Vector2{-1,  1});
		if (position[i].y <= 0 || position[i].y >= WindowHeight - 1)	velocity[i] = Vector2Multiply(velocity[i], Vector2{ 1, -1});
		position[i] = Vector2Add(position[i], Vector2Scale(velocity[i], GetFrameTime() * speed));
		position[i].x = Clamp(position[i].x, 0, WindowWidth - 1);
		position[i].y = Clamp(position[i].y, 0, WindowHeight - 1);


		int newPosX = position[i].x / (WindowWidth / gridSize);
		int newPosY = position[i].y / (WindowHeight / gridSize);

		if (posX == newPosX && posY == newPosY) continue;

		_gridIndices[posX][posY].erase(std::find(_gridIndices[posX][posY].begin(), _gridIndices[posX][posY].end(), i));
		_gridIndices[newPosX][newPosY].push_back(i);
	}
}

void Boids::Draw()
{
	for (int i = 0; i < position.size(); i++)
	{
		//DrawCircleV(position[i], collisionSize, Color{102, 242, 198, 100});
		DrawCircleV(position[i], size,			Color{5, 70, 40, 255});
		//DrawLineV(position[i], Vector2Add(position[i], Vector2Scale(velocity[i], 20.0f)), RED);
	}
}

std::vector<int> Boids::GridChecking(int _posX, int _posY)
{
	std::vector<int> boidsInRange;
	std::vector<int> addToList;

	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			addToList = CaseChecking(_posX + i, _posY + j);
			boidsInRange.insert(boidsInRange.end(), addToList.begin(), addToList.end());
		}
	}

	return boidsInRange;
}

std::vector<int> Boids::CaseChecking(int _x, int _y)
{
	if (_x < 0)	return std::vector<int>();
	if (_y < 0)	return std::vector<int>();
	if (_x >= _gridIndices.size())		return std::vector<int>();
	if (_y >= _gridIndices[_x].size())	return std::vector<int>();

	return _gridIndices[_x][_y];
}

std::vector<int> Boids::FOVTesting(int _id, std::vector<int> _neighbors)
{
	std::vector<int> frontNeighbors = std::vector<int>();
	for (int i : _neighbors)
	{
		if (Vector2DotProduct(velocity[i], Vector2Subtract(position[_id], position[i])) >= visionCone)
		{
			frontNeighbors.push_back(i);
		}
	}
	return frontNeighbors;
}

std::vector<int> Boids::DistanceTesting(int _id, std::vector<int> _neighbors, float _collisionSize)
{
	std::vector<int> closeNeighbors = std::vector<int>();
	for (int i : _neighbors)
	{
		if (Vector2DistanceSqr(position[_id], position[i]) <= pow(_collisionSize, 2))
		{
			closeNeighbors.push_back(i);
		}
	}
	return closeNeighbors;
}

Vector2 Boids::GetDodgeDirection(int _id, std::vector<int> _neighbors)
{
	Vector2 direction = Vector2Zero();
	_neighbors = DistanceTesting(_id, _neighbors, dodgeCollisionSize);
	for (int i : _neighbors)
	{
		if (_id == i) continue;
		direction = Vector2Add(direction, Vector2Scale(Vector2Subtract(position[i], position[_id]), -1.0f));
	}
	return Vector2Normalize(direction);
}

Vector2 Boids::GetAlignDirection(int _id, std::vector<int> _neighbors)
{
	Vector2 direction = Vector2Zero();
	for (int i : _neighbors)
	{
		if (_id == i) continue;
		direction = Vector2Add(direction, velocity[i]);
	}
	return Vector2Normalize(direction);
}

Vector2 Boids::GetCohesionDirection(int _id, std::vector<int> _neighbors)
{
	Vector2 direction = Vector2Zero();
	Vector2 center = Vector2Zero();
	int successTest = 0;
	for (int i : _neighbors)
	{
		if (_id == i) continue;
		center = Vector2Add(center, position[i]);
		successTest++;
	}
	if(successTest == 0)return center;
	center = Vector2Divide(center, Vector2{ static_cast<float>(successTest), static_cast<float>(successTest) });
	direction = Vector2Subtract(center, position[_id]);

	return Vector2Normalize(direction);
}
