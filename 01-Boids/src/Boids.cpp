#include "Boids.h"
#include "raymath.h"
#include "Config.h"

Boids::Boids():
	size(0.0f),
	speed(0.0f)
{
}

Boids::~Boids()
{
}

void Boids::Start(int _spawnNumber, float _size, float _speed)
{
	for (int i = 0; i < _spawnNumber; i++)
	{
		position.push_back(Vector2{ static_cast<float>(GetRandomValue(0, WindowWidth)), static_cast<float>(GetRandomValue(0, WindowHeight)) });
		velocity.push_back(Vector2Normalize( Vector2{ static_cast<float>(GetRandomValue(-100, 100) * 0.01f), static_cast<float>(GetRandomValue(-100, 100) * 0.01f) }));
	}
	size = _size;
	speed = _speed;
}

void Boids::Update()
{
	for (int i = 0; i < position.size(); i++)
	{
		if (position[i].x <= 0 || position[i].x >= WindowWidth)		velocity[i] = Vector2Multiply(velocity[i], Vector2{-1,  1});
		if (position[i].y <= 0 || position[i].y >= WindowHeight)	velocity[i] = Vector2Multiply(velocity[i], Vector2{ 1, -1});
		position[i] = Vector2Add(position[i], Vector2Scale(velocity[i], GetFrameTime() * speed));
	}
}

void Boids::Draw()
{
	for (Vector2 p : position)
	{
		DrawCircle(p.x, p.y, size * 1.75f, Color{ 102, 242, 198, 100 });
		DrawCircle(p.x, p.y, size, Color{9, 130, 92, 255});
	}
}