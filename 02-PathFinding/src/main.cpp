#include <iostream>
#include <string>
#include <raylib.h>
#include "Config.h"
#include "ImageLoader.h"
#include "Djikstra.h"

using std::string;

int main()
{
    InitWindow(WindowWidth, WindowHeight, "02-PathFinding");
	//MaximizeWindow();
    //SetTargetFPS(60);

    Texture2D texture = LoadTexture("resources/MazeMap.png");
    ImageLoader mapMaker;
    mapMaker.LoadImage(texture);

    Djikstra djikstra;

	srand(time(NULL));
    while (!WindowShouldClose()){
        djikstra.Update();
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexturePro(texture, Rectangle{ 0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height) }, Rectangle{ 0, 0, static_cast<float>(WindowWidth), static_cast<float>(WindowHeight) }, Vector2{ 0, 0 }, 0, WHITE);
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
            djikstra.Calculate(mapMaker.getMapData());
        }

        DrawFPS(10, 10);
        EndDrawing();
	}

    CloseAudioDevice();
    CloseWindow();
}