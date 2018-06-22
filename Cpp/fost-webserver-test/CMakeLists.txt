add_library(fost-webserver-test
        testrunner.cpp
    )
target_link_libraries(fost-webserver-test fost-urlhandler)
install(TARGETS fost-webserver-test LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

