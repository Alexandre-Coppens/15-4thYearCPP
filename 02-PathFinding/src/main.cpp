#include <iostream>
#include <string>
#include <raylib.h>
#include "Config.h"

using std::string;

int main()
{
    InitWindow(WindowWidth, WindowHeight, "02-PathFinding");
	//MaximizeWindow();
    //SetTargetFPS(60);

	srand(time(NULL));
    while (!WindowShouldClose()){

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(10, 10);
        EndDrawing();
	}

    CloseAudioDevice();
    CloseWindow();
}