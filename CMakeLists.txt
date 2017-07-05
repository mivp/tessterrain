#module_version(1.0.0)
#declare_native_module(tessterrain)

cmake_minimum_required(VERSION 2.8.1) 
find_package(Omegalib)

# Set the module name here
SET(MODULE_NAME tessterrain)

include_directories(
	${OMEGA_INCLUDE_DIRS}
	app/external/glm-0.9.4.0/
	)

message(${CMAKE_SOURCE_DIR})
add_definitions(-DSHADER_DIR=\"${CMAKE_SOURCE_DIR}/shaders/\")
add_definitions(-DOMEGALIB_MODULE)

# Set module name and source files here
add_library(${MODULE_NAME} MODULE 
		INIReader.h
		LRU.h
		LRU.cpp
		Thread.h
		Thread.cpp
		wqueue.h
		Program.h
		Program.cpp
		Material.h
		Material.cpp
		Terrain.h
		Terrain.cpp
		TerrainManager.h
		TerrainManager.cpp
		Texture.h
		Texture.cpp
		tessterrain.cpp 
        )

# Set the module library dependencies here
target_link_libraries(${MODULE_NAME}
        stdc++
        ${OMEGA_LIB}
        python2.7
        )

#------------------------------------------------------------------------------
# DO NOT MODIFY ANYTHING BELOW AFTER THIS LINE
set_target_properties(${MODULE_NAME} PROPERTIES PREFIX "")
if(WIN32)
        set_target_properties(${MODULE_NAME} PROPERTIES FOLDER modules SUFFIX ".pyd")
endif()

