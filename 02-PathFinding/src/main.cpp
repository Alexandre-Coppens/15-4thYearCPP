#include <iostream>
#include <string>
#include <raylib.h>
#include "Config.h"
#include "Boid.h"
#include "Benchmark.h"
#include "ImageLoader.h"
#include "Djikstra.h"

using std::string;

std::string Config::ApplicationName    = "Pathfinding";
std::string Config::ApplicationVersion = "0.2";

int Config::WindowWidth = 600;
int Config::WindowHeight = 600;

float Config::pixelWidth = 1;
float Config::pixelHeight = 1;

int main()
{
    InitWindow(Config::WindowWidth, Config::WindowHeight, "02-PathFinding");
	//MaximizeWindow();
    //SetTargetFPS(60);
    Benchmark::Init();
    Benchmark::LoadFile(Config::ApplicationName, Config::ApplicationVersion);

    Texture2D texture = LoadTexture("resources/Textures/Maps/CauldronMap.png");
    ImageLoader mapMaker;
    mapMaker.LoadBackground(texture);
    mapMaker.SetBorderSize(10);

    Boid boids;

    Djikstra djikstra = Djikstra(mapMaker.getMapData());

	srand(time(NULL));
    while (!WindowShouldClose()){
        //Check Inputs
        if (IsMouseButtonPressed(0))
        {
            boids.AddNewBoid(GetMousePosition());
        }
        if (IsMouseButtonPressed(1))
        {
            mapMaker.SetPointAt('e', GetMouseX(), GetMouseY());
            djikstra.Calculate(mapMaker.getMapData(), 9999);
            boids.ChangeTheMap(djikstra.GetMapData());
        }

        //Update
        djikstra.Update();
        boids.Update();

        //Draw
        BeginDrawing();
        ClearBackground(BLACK);

        //DrawTexturePro(texture, Rectangle{ 0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height) }, Rectangle{ 0, 0, static_cast<float>(WindowWidth), static_cast<float>(WindowHeight) }, Vector2{ 0, 0 }, 0, WHITE);
        //mapMaker.DrawImage();
        djikstra.Draw();
        boids.Draw();

        DrawFPS(10, 10);
        EndDrawing();
	}

    CloseAudioDevice();
    CloseWindow();
    Benchmark::PrintResults();
    Benchmark::SaveResults(Config::ApplicationName, Config::ApplicationVersion);
}