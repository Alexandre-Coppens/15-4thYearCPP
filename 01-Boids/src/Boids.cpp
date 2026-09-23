#include "Boids.h"
#include "raymath.h"
#include "Config.h"
#include <iostream>
#include <string>

Boids::Boids():
	size(2.0f),
	speed(50.0f),
	dodgeCollisionSize(30.0f),
	alignCollisionSize(100.0f),
	gridSize(20)
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

		Vector2 dodgeDirection = GetDodgeDirection(i, neighbors);
		Vector2 alignDirection = GetAlignDirection(i, neighbors);

		Vector2 addedDirections = Vector2Add(Vector2Scale(dodgeDirection, 0.8f * GetFrameTime()), Vector2Scale(alignDirection, 0.8f * GetFrameTime()));
											 //Vector2Add(Vector2Scale(alignDirection, 0.8f * GetFrameTime()),
														//Vector2Scale(alignDirection, 0.8f * GetFrameTime())));
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

bool Boids::DistanceTesting(int _boidA, int _boidB, float _collisionSize)
{
	return Vector2DistanceSqr(position[_boidA], position[_boidB]) <= pow(_collisionSize, 2);
}

Vector2 Boids::GetDodgeDirection(int _id, std::vector<int> _neighbors)
{
	Vector2 direction = Vector2Zero();
	for (int i : _neighbors)
	{
		if (_id == i) continue;
		if (DistanceTesting(i, _id, dodgeCollisionSize))
		{
			direction = Vector2Add(direction, Vector2Scale(Vector2Subtract(position[i], position[_id]), -1.0f));
		}
	}
	return Vector2Normalize(direction);
}

Vector2 Boids::GetAlignDirection(int _id, std::vector<int> _neighbors)
{
	Vector2 direction = Vector2Zero();
	for (int i : _neighbors)
	{
		if (_id == i) continue;
		if (DistanceTesting(i, _id, alignCollisionSize))
		{
			direction = Vector2Add(direction, velocity[i]);
		}
	}
	return Vector2Normalize(direction);
}