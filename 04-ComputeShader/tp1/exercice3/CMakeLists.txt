cmake_minimum_required(VERSION 3.20)
project(Tp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Tp)

add_subdirectory(src)

add_library(glfw SHARED IMPORTED)
add_library(glew SHARED IMPORTED)

set(DEPENDENCIES_ROOT "F:/-1- Work Station-/Github/15-4thYearCPP/00-Dependencies" CACHE PATH "DEPENDENCIES Folder")

set(GLFW_ROOT "${DEPENDENCIES_ROOT}/glfw-3.5.1.bin.WIN64" CACHE PATH "GLFW Folder")
set_target_properties(glfw PROPERTIES
		IMPORTED_IMPLIB   "${GLFW_ROOT}/lib-vc2022/glfw3.lib"
		IMPORTED_LOCATION "${GLFW_ROOT}/lib-vc2022/glfw3.dll"
		INTERFACE_INCLUDE_DIRECTORIES "${GLFW_ROOT}/include/"
)

set(GLEW_ROOT "${DEPENDENCIES_ROOT}/glew-2.3.1" CACHE PATH "GLEW Folder")
set_target_properties(glew PROPERTIES
		IMPORTED_IMPLIB   "${GLEW_ROOT}/lib/Release/x64/glew32.lib"
		IMPORTED_LOCATION "${GLEW_ROOT}/bin/Release/x64/glew32.dll"
		INTERFACE_INCLUDE_DIRECTORIES "${GLEW_ROOT}/include/"
)

target_link_libraries(Tp
	PRIVATE glew
	PRIVATE glfw
	PRIVATE opengl32
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${GLFW_ROOT}/lib-vc2022/glfw3.dll
		${GLEW_ROOT}/bin/Release/x64/glew32.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
