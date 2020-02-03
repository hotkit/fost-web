add_library(fost-urlhandler
        config.cpp
        control.exception.cpp
        control.status-condition.cpp
        fost-urlhandler.cpp
        fsigma.cpp
        matcher.cpp
        middleware.logging.cpp
        middleware.request.cpp
        middleware.template.cpp
        mime-types.cpp
        precondition.cpp
        responses.cpp
        responses.301.cpp
        responses.302.cpp
        responses.303.cpp
        responses.401.cpp
        responses.405.cpp
        responses.file.cpp
        responses.pathname.cpp
        responses.pathprefix.cpp
        responses.proxy.cpp
        responses.static.cpp
        routing.cpp
        schema.validate.cpp
        test.throw.cpp
        view.cpp
        view.matcher.cpp
    )
target_include_directories(fost-urlhandler PUBLIC ../include)
target_link_libraries(fost-urlhandler fost-inet f5-json-schema)
set_target_properties(fost-urlhandler PROPERTIES DEBUG_POSTFIX "-d")
install(TARGETS fost-urlhandler LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(DIRECTORY ../include/fost DESTINATION include)

if(TARGET check)
    add_library(fost-urlhandler-smoke STATIC EXCLUDE_FROM_ALL
            control.exception.tests.cpp
            control.status-condition-tests.cpp
            matcher.tests.cpp
            precondition.tests.cpp
            responses.tests.cpp
            responses.301-tests.cpp
            responses.302-tests.cpp
            responses.303-tests.cpp
            responses.405-tests.cpp
            responses.static.tests.cpp
            routing-tests.cpp
            schema.validate-tests.cpp
            test.throw.tests.cpp
            view-tests.cpp
            view.matcher.tests.cpp
            view.request-id.tests.cpp
        )
    target_link_libraries(fost-urlhandler-smoke fost-urlhandler)
    smoke_test(fost-urlhandler-smoke)
endif()
