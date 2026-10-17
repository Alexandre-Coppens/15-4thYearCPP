#include "Djikstra.h"
#include "Config.h"
#include "Benchmark.h"
#include <string>
#include <iostream>
#include "Boid.h"

Djikstra::Djikstra(std::vector<std::vector<char>> _newMap) :
	needToBeRebuild(false),
	currentIterations(1)
{
	nextCaseToTest = std::vector<Vector2>();
	mapData = _newMap;
	needRedraw = true;
}

Djikstra::~Djikstra()
{
	UnloadTexture(mTexture);
}

std::vector<std::vector<char>>* Djikstra::getMapData()
{
	return &mapData;
}

void Djikstra::Calculate(std::vector<std::vector<char>> _newMap, int _iterations)
{
	mapData = _newMap;
	Vector2 start, end = Vector2{-1, -1};

	for (int x = 0; x < mapData.size(); x++)
	{
		for (int y = 0; y < mapData[x].size(); y++)
		{
			if (mapData[x][y] == 's') start = Vector2{ static_cast<float>(x), static_cast<float>(y) };
			if (mapData[x][y] == 'e') end = Vector2{ static_cast<float>(x), static_cast<float>(y) };
		}
	}

	if (Vector2Equals(end, Vector2{ -1, -1 })) return;

	currentIterations = std::max(_iterations, 1);
	needToBeRebuild = true;
	currentValue = 0;
	pixelDrawn = 0;
	testingThisFrame.clear();
	nextCaseToTest.clear();
	nextCaseToTest.reserve(_newMap.size()* _newMap.size());
	VisitNextCase(end.x, end.y);
}

void Djikstra::RoadToEnd(Vector2 _start)
{
	_start.x *= Config::pixelWidth;
	_start.y *= Config::pixelHeight;
	if (mapData.size() == 0) return;
	if (mapData.size() <= _start.x || mapData[0].size() <= _start.y) return;
	if (mapData[_start.x][_start.y] == '0') return;
	if (mapData[_start.x][_start.y] == '1') return;
	if (mapData[_start.x][_start.y] == 'e') return;
	if (mapData[_start.x][_start.y] >= 65)  return;

	Benchmark::StartClock("Dijkstra Pathfind");
	pathFind.clear();
	bool notFoundEnd = true;
	char currentSign;
	Vector2 currentLocation = Vector2{ _start.x, _start.y };
	while (notFoundEnd)
	{
		pathFind.push_back(currentLocation);
		currentSign = mapData[currentLocation.x][currentLocation.y];
		if (currentSign == 'e') notFoundEnd = false;
		if (currentSign == '4') currentLocation.x += 1;
		if (currentSign == '6') currentLocation.x -= 1;
		if (currentSign == '8') currentLocation.y += 1;
		if (currentSign == '2') currentLocation.y -= 1;
	}
	needRedraw = true;
	Benchmark::StopClock("Dijkstra Pathfind");
}

void Djikstra::Update()
{
	if (!needToBeRebuild) return;
	Benchmark::StartClock("Dijkstra Rebuild One Iteration");
	for (int i = 0; i < currentIterations; i++)
	{
		currentValue++;
		testingThisFrame = nextCaseToTest;
		nextCaseToTest.clear();
		std::cout << "TotalPixels: " + std::to_string(pixelDrawn) + " || To do this frame: " + std::to_string(testingThisFrame.size()) + "\n";
		for (Vector2 pos : testingThisFrame)
		{
			VisitNextCase(pos.x, pos.y);
		}
		testingThisFrame.clear();
		std::cout << "testThisFrame: " + std::to_string(testingThisFrame.size()) + " || Next to Test: " + std::to_string(nextCaseToTest.size()) + "\n";
		
		if (nextCaseToTest.size() == 0)
		{
			needToBeRebuild = false;
			break;
		}
	}

	needRedraw = true;
	Benchmark::StopClock("Dijkstra Rebuild One Iteration");
}

void Djikstra::Draw()
{
	if (needRedraw)
	{
		Benchmark::StartClock("Dijkstra Redraw");
		if (mapData.size() == 0) return;
		Image image = GenImageColor(mapData.size(), mapData[0].size(), BLACK);
		for (int x = 0; x < mapData.size(); x++)
		{
			for (int y = 0; y < mapData[x].size(); y++)
			{
				if (mapData[x][y] == '1') continue;
				Color color;
				switch (mapData[x][y])
				{
				case '0':
					color = RAYWHITE;
					break;

				case 's':
					color = BLUE;
					break;

				case 'e':
					color = ORANGE;
					break;

				case '.':
					color = GRAY;
					break;

				case '8':
					color = GREEN;
					break;

				case '4':
					color = BLUE;
					break;

				case '6':
					color = ORANGE;
					break;

				case '2':
					color = RED;
					break;

				default:
					color = GRAY;
					break;
				}
				ImageDrawPixel(&image, x, y, color);
			}
		}
		if (pathFind.size() != 0)
		{
			for (Vector2 p : pathFind)
			{
				ImageDrawPixel(&image, (int)p.x, (int)p.y, RED);
			}
		}
		mTexture = LoadTextureFromImage(image);
		UnloadImage(image);
		needRedraw = false;
		Benchmark::StopClock("Dijkstra Redraw");
	}

	DrawTexturePro(mTexture, Rectangle{ 0, 0, static_cast<float>(mTexture.width), static_cast<float>(mTexture.height) }, Rectangle{ 0, 0, static_cast<float>(Config::WindowWidth), static_cast<float>(Config::WindowHeight) }, Vector2{ 0, 0 }, 0, WHITE);
}

void Djikstra::VisitNextCase(int x, int y)
{
	pixelDrawn++;
	if (x < mapData.size() - 1)		
		if (mapData[x + 1][y] == '0')
		{
			nextCaseToTest.push_back(Vector2{ static_cast<float>(x + 1), static_cast<float>(y) });
			mapData[x + 1][y] = '6';
		}
	if (x > 0)						
		if (mapData[x - 1][y] == '0')
		{
			nextCaseToTest.push_back(Vector2{ static_cast<float>(x - 1), static_cast<float>(y) });
			mapData[x - 1][y] = '4';
		}
	if (y < mapData[x].size() - 1)	
		if (mapData[x][y + 1] == '0')
		{
			nextCaseToTest.push_back(Vector2{ static_cast<float>(x), static_cast<float>(y + 1) });
			mapData[x][y + 1] = '2';
		}
	if (y > 0)						
		if (mapData[x][y - 1] == '0')
		{
			nextCaseToTest.push_back(Vector2{ static_cast<float>(x), static_cast<float>(y - 1) });
			mapData[x][y - 1] = '8';
		}
}