#include <iostream>
#include <string>
#include <raylib.h>
#include "Config.h"
#include "ImageLoader.h"

using std::string;

int main()
{
    InitWindow(WindowWidth, WindowHeight, "02-PathFinding");
	//MaximizeWindow();
    //SetTargetFPS(60);

    Texture2D texture = LoadTexture("resources/MazeMap.png");
    ImageLoader mapMaker;
    mapMaker.LoadImage(texture);

	srand(time(NULL));
    while (!WindowShouldClose()){

        BeginDrawing();
        ClearBackground(RAYWHITE);

        //DrawTexturePro(texture, Rectangle{ 0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height) }, Rectangle{ 0, 0, static_cast<float>(WindowWidth), static_cast<float>(WindowHeight) }, Vector2{ 0, 0 }, 0, WHITE);
        mapMaker.DrawImage();

        if (IsMouseButtonPressed(0)) (mapMaker.SetPointAt('s', GetMouseX(), GetMouseY()));
        if (IsMouseButtonPressed(1)) (mapMaker.SetPointAt('e', GetMouseX(), GetMouseY()));

        DrawFPS(10, 10);
        EndDrawing();
	}

    CloseAudioDevice();
    CloseWindow();
}