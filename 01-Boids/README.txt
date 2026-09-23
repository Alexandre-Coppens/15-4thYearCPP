BOIDS

Boid Simulation.

What's Implemented:
-Moving Around
-Dodging Other Boids
-Aligning with other Boids
-Cohesion with other Boids


Optimisations used:
-Fast Sqrt Distance check.
-Grid Checking (20 / 20)
-Sphere Check after grid check


Benchmarks:

=-= New Implementation =-= 100 Boids   =-= 1000 Boids  =-= 10000 Boids =-=

-Dodging		- 1370 FPS    	- 21 FPS	- 4s/Frame
-Aligning		- 800 FPS	- 10 FPS	- 9s/Frame
-Grid Checking		- 490 FPS	- 32 FPS	- 2 FPS
-Cohesion & SphereCheck - 350 FPS	- 25 FPS	- 1 FPS