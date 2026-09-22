target_sources(01-Boids PRIVATE
	main.cpp 
	Boids.cpp
	Boids.h
 "Config.h")

target_include_directories(01-Boids PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)