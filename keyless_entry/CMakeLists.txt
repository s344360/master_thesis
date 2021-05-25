cmake_minimum_required(VERSION 3.17)
project(pubsub_console)

set(CMAKE_CXX_STANDARD 14)

include_directories(/usr/local/include/open62541)
include_directories(/usr/local/include/open62541/plugin)
include_directories(/usr/local/include/open62541/posix)
link_directories(/usr/local/lib)


add_executable(pub_console main.cpp Publisher.cpp UserInput.cpp InputArguments.cpp)
#target_link_libraries(pub_console ${PROJECT_SOURCE_DIR}/library/libopen62541.a pthread)
target_link_libraries(pub_console LINK_PUBLIC open62541 pthread)

#link_directories(${PROJECT_SOURCE_DIR}/library)
add_executable(door_handle door_handle.cpp Publisher.cpp UserInput.cpp InputArguments.cpp)
target_link_libraries(door_handle LINK_PUBLIC open62541 ${PROJECT_SOURCE_DIR}/library/libopen62541.a pthread)

#add_executable(publish subscribe_standalone_and_publish.cpp Publisher.cpp UserInput.cpp InputArguments.cpp)
#target_link_libraries(publish LINK_PUBLIC ${PROJECT_SOURCE_DIR}/library/libopen62541.a pthread)


add_executable(kessy_io kessy_io.cpp Publisher.cpp UserInput.cpp InputArguments.cpp)
target_link_libraries(kessy_io LINK_PUBLIC open62541 ${PROJECT_SOURCE_DIR}/library/libopen62541.a pthread)

add_executable(UID_key UID_key.cpp Publisher.cpp UserInput.cpp InputArguments.cpp)
target_link_libraries(UID_key LINK_PUBLIC open62541 ${PROJECT_SOURCE_DIR}/library/libopen62541.a pthread)

add_executable(BCM BCM.cpp Publisher.cpp UserInput.cpp InputArguments.cpp)
target_link_libraries(BCM LINK_PUBLIC open62541 ${PROJECT_SOURCE_DIR}/library/libopen62541.a pthread)

add_executable(door_modules door_modules.cpp)
target_link_libraries(door_modules LINK_PUBLIC open62541 ${PROJECT_SOURCE_DIR}/library/libopen62541.a)