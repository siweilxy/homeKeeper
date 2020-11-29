cmake_minimum_required(VERSION 3.5)
project(homeKeeper)

SET(EXECUTABLE_OUTPUT_PATH $ENV{HOME}/work/github/homeKeeper/bin)
SET(LIBRARY_OUTPUT_PATH $ENV{HOME}/work/github/homeKeeper/lib)

add_subdirectory(master)
add_subdirectory(homeKeeper)
add_subdirectory(common)
add_subdirectory(frame)
add_subdirectory(mailServer)
add_subdirectory(proxyServer)
add_subdirectory(sqlGenerate)
add_subdirectory(raft)
add_subdirectory(test)



