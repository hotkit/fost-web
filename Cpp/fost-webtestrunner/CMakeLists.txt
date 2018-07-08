add_library(fost-webtestrunner
        testrunner.cpp
    )
target_link_libraries(fost-webtestrunner fost-urlhandler)
set_target_properties(fost-webtestrunner PROPERTIES DEBUG_POSTFIX "-d")
install(TARGETS fost-webtestrunner LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

