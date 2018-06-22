add_library(fost-webtestrunner
        testrunner.cpp
    )
target_link_libraries(fost-webtestrunner fost-urlhandler)
install(TARGETS fost-webtestrunner LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

