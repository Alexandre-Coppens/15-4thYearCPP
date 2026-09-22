#include "Boids.h"
#include "raymath.h"
#include "Config.h"
#include <iostream>
#include <string>

Boids::Boids():
	size(2.0f),
	speed(50.0f),
	collisionSize(30.0f)
{
}

Boids::~Boids()
{
}

void Boids::Start(int _spawnNumber)
{
	for (int i = 0; i < _spawnNumber; i++)
	{
		position.push_back(Vector2{ static_cast<float>(GetRandomValue(0, WindowWidth)), static_cast<float>(GetRandomValue(0, WindowHeight)) });
		velocity.push_back(Vector2Normalize( Vector2{ static_cast<float>(GetRandomValue(-100, 100) * 0.01f), static_cast<float>(GetRandomValue(-100, 100) * 0.01f) }));
	}
}

void Boids::Update()
{
	for (int i = 0; i < position.size(); i++)
	{
		Vector2 dodgeDirection = GetDodgeDirection(i);

		velocity[i] = Vector2Add(velocity[i], Vector2Scale(dodgeDirection, 0.8f * GetFrameTime()));
		velocity[i] = Vector2Normalize(velocity[i]);

		if (position[i].x <= 0 || position[i].x >= WindowWidth)		velocity[i] = Vector2Multiply(velocity[i], Vector2{-1,  1});
		if (position[i].y <= 0 || position[i].y >= WindowHeight)	velocity[i] = Vector2Multiply(velocity[i], Vector2{ 1, -1});
		position[i] = Vector2Add(position[i], Vector2Scale(velocity[i], GetFrameTime() * speed));
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

bool Boids::DistanceTesting(int _boidA, int _boidB)
{
	return Vector2DistanceSqr(position[_boidA], position[_boidB]) <= pow(collisionSize, 2);
}

Vector2 Boids::GetDodgeDirection(int _id)
{
	Vector2 direction = Vector2Zero();
	for (int i = 0; i < position.size(); i++)
	{
		if (_id == i) continue;
		if (DistanceTesting(i, _id))
		{
			direction = Vector2Add(direction, Vector2Scale(Vector2Subtract(position[i], position[_id]), -1.0f));
		}
	}
	return Vector2Normalize(direction);
}
