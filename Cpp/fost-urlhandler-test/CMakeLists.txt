if(TARGET stress)
    add_library(fost-urlhandler-test STATIC EXCLUDE_FROM_ALL
            web-proxy.cpp
        )
    target_link_libraries(fost-urlhandler-test fost-urlhandler)
    stress_test(fost-urlhandler-test)
endif()
