CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(ASSIMP CXX)

FILE(GLOB_RECURSE SRC_LIST main.cpp mesh.h mesh.cpp myShader.h myShader.cpp Arcball.h Arcball.cpp model.h model.cpp)

SET(CMAKE_INSTALL_PATH ${PROJECT_BINARY_DIR}/Debug)

INSTALL(FILES D:/gldependence/glew-1.12.0/bin/Debug/x64/glew32d.dll DESTINATION ${CMAKE_INSTALL_PATH})

INSTALL(FILES D:/gldependence/freeglut-2.8.1/lib/x64/Debug/freeglut.dll DESTINATION ${CMAKE_INSTALL_PATH})

INSTALL(FILES D:/gldependence/assimp/bin/assimpd.dll DESTINATION ${CMAKE_INSTALL_PATH})

INCLUDE_DIRECTORIES(D:/gldependence/freeglut-2.8.1/include
					D:/gldependence/glew-1.12.0/include
					D:/gldependence/glm
					D:/gldependence/SOIL/include
                    D:/gldependence/assimp/include)

LINK_DIRECTORIES(D:/gldependence/freeglut-2.8.1/lib/x64/Debug
				 D:/gldependence/glew-1.12.0/lib/Debug/x64
				 D:/gldependence/freeglut-2.8.1/lib/x64/Debug
				 D:/gldependence/glew-1.12.0/bin/Debug/x64
				 D:/gldependence/SOIL/lib
                 D:/gldependence/assimp/lib)
				 
ADD_EXECUTABLE(ASSIMP ${SRC_LIST})
				 
TARGET_LINK_LIBRARIES(ASSIMP glew32d.dll glew32d.lib freeglut.lib freeglut.dll assimpd.lib assimpd.dll SOIL.lib)