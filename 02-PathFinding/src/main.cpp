#include <iostream>
#include <string>
#include <raylib.h>
#include "Config.h"
#include "Benchmark.h"
#include "ImageLoader.h"
#include "Djikstra.h"

using std::string;

int main()
{
    InitWindow(WindowWidth, WindowHeight, "02-PathFinding");
	//MaximizeWindow();
    //SetTargetFPS(60);
    Benchmark::Init();

    Texture2D texture = LoadTexture("resources/CastleMap.png");
    ImageLoader mapMaker;
    mapMaker.LoadBackground(texture);
    mapMaker.SetBorderSize(10);

    Djikstra djikstra = Djikstra(mapMaker.getMapData());

	srand(time(NULL));
    while (!WindowShouldClose()){
        djikstra.Update();
        BeginDrawing();
        ClearBackground(BLACK);

        //DrawTexturePro(texture, Rectangle{ 0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height) }, Rectangle{ 0, 0, static_cast<float>(WindowWidth), static_cast<float>(WindowHeight) }, Vector2{ 0, 0 }, 0, WHITE);
        //mapMaker.DrawImage();
        djikstra.Draw();

        if (IsMouseButtonPressed(0))
        {
            mapMaker.SetPointAt('s', GetMouseX(), GetMouseY());
            djikstra.RoadToEnd(GetMousePosition());
        }
        if (IsMouseButtonPressed(1))
        {
            mapMaker.SetPointAt('e', GetMouseX(), GetMouseY());
            djikstra.Calculate(mapMaker.getMapData(), 20);
        }

        DrawFPS(10, 10);
        EndDrawing();
	}

    CloseAudioDevice();
    CloseWindow();
    Benchmark::PrintResults();
}