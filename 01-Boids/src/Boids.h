#pragma once
#include <vector>
#include <raylib.h>

class Boids 
{
private:
	std::vector<Vector2> position, velocity;
	float size;
	float speed;

public:
	Boids();
	~Boids();

	void Start(int _spawnNumber, float _size, float speed);
	void Update();
	void Draw();
};