add_executable(fost-webserver webserver.cpp)
target_link_libraries(fost-webserver fost-cli fost-urlhandler)
install(TARGETS fost-webserver EXPORT fost-webserver RUNTIME DESTINATION bin)
