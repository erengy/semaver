cmake_minimum_required(VERSION 3.20)

project(semaver
	VERSION 1.0.0
	DESCRIPTION "Semantic versioning library"
	HOMEPAGE_URL https://github.com/erengy/semaver
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(semaver INTERFACE)

target_include_directories(semaver INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
