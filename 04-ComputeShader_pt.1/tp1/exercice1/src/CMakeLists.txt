target_sources(Tp PRIVATE
	gl_utils.cpp
	gl_utils.hpp
	main.cpp
)

target_include_directories(Tp PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)