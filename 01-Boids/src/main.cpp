#include <iostream>
#include <string>
#include <raylib.h>
#include "Boids.h"
#include "Config.h"

using std::string;

Boids boidList;
int main()
{
    InitWindow(WindowWidth, WindowHeight, "01-Boids");
	//MaximizeWindow();
    //SetTargetFPS(60);

	srand(time(NULL));
    boidList.Start(1000);
    while (!WindowShouldClose()){
        boidList.Update();
        BeginDrawing();
        ClearBackground(Color{ 2, 5, 32 });
        DrawRectangleGradientV(0, 0, static_cast<float>(WindowWidth), static_cast<float>(WindowHeight), Color{ 83, 237, 237, 255 }, Color{ 16, 104, 117, 255});
        boidList.Draw();
        EndDrawing();
	}

    CloseAudioDevice();
    CloseWindow();
}