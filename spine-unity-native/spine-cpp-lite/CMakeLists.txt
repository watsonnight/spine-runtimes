if(MSVC)
	message("MSVC detected")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Zi")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /DEBUG")
else()
	message("clang detected")
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -std=c89 -g")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wnon-virtual-dtor -pedantic -std=c++11 -fno-exceptions -fno-rtti -g")
endif()

message(${PROJECT_SOURCE_DIR})

SET(SPINE_CPP_DIR "${PROJECT_SOURCE_DIR}/../../spine-cpp/spine-cpp")

file(GLOB INCLUDES "${SPINE_CPP_DIR}/include/**/*.h")
file(GLOB SOURCES "${SPINE_CPP_DIR}/src/**/*.cpp")

message(${SPINE_CPP_DIR})

add_library(spine-cpp STATIC ${SOURCES} ${INCLUDES})
target_include_directories(spine-cpp PUBLIC ${SPINE_CPP_DIR}/include)

file(GLOB NATIVE_INCLUDES "spine-cpp-lite/*.h")
file(GLOB NATIVE_SOURCES "spine-cpp-lite/*.cpp")

add_library(spine-cpp-lite STATIC ${SOURCES} ${INCLUDES} ${NATIVE_INCLUDES} ${NATIVE_SOURCES})
target_include_directories(spine-cpp-lite PUBLIC ${SPINE_CPP_DIR}/include)

# Install target
install(TARGETS spine-cpp EXPORT spine-cpp_TARGETS DESTINATION dist/lib)
install(FILES ${INCLUDES} DESTINATION dist/include)

# Export target
export(
	EXPORT spine-cpp_TARGETS
	FILE ${CMAKE_CURRENT_BINARY_DIR}/spine-cppTargets.cmake
	NAMESPACE "Spine::")