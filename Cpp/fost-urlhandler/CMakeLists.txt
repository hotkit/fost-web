add_library(fost-urlhandler
        config.cpp
        fost-urlhandler.cpp
        file.cpp
        middleware.logging.cpp
        middleware.request.cpp
        middleware.template.cpp
        mime-types.cpp
        responses.200.cpp
        responses.301.cpp
        responses.302.cpp
        responses.303.cpp
        responses.401.cpp
        responses.403.cpp
        responses.404.cpp
        responses.405.cpp
        responses.410.cpp
        responses.412.cpp
        responses.500.cpp
        responses.501.cpp
        responses.503.cpp
        responses.pathname.cpp
        responses.pathprefix.cpp
        responses.static.cpp
        routing.cpp
        view.cpp
    )
target_include_directories(fost-urlhandler PUBLIC ../include)
target_link_libraries(fost-urlhandler fost-inet)
install(TARGETS fost-urlhandler LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

add_library(fost-urlhandler-smoke STATIC EXCLUDE_FROM_ALL
        responses.301-tests.cpp
        responses.302-tests.cpp
        responses.303-tests.cpp
        responses.405-tests.cpp
        routing-tests.cpp
        view-tests.cpp
    )
target_link_libraries(fost-urlhandler-smoke fost-urlhandler)
smoke_test(fost-urlhandler-smoke)
