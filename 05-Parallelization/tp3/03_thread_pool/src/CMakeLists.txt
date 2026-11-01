target_sources(Tp PRIVATE
	boids.cpp
	boids.hpp
	gl_utils.cpp
	gl_utils.hpp
	thread_pool.cpp
	thread_pool.hpp
	main.cpp
)

target_include_directories(Tp PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)