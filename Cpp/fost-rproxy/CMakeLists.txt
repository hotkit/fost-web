add_library(fost-rproxy
        cache.cpp
        cache.db.cpp
        cache.hash.cpp
        config-view.cpp
        fost-rproxy.cpp
        proxy-view.cpp
        stats.cpp
    )
target_link_libraries(fost-rproxy fost-crypto fost-datetime fost-jsondb fost-urlhandler)
install(TARGETS fost-rproxy LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

add_library(fost-rproxy-smoke STATIC EXCLUDE_FROM_ALL
        cache-tests.cpp
        cache.db-tests.cpp
        cache.hash-tests.cpp
        config-view-tests.cpp
        proxy-view-tests.cpp
    )
target_link_libraries(fost-rproxy-smoke fost-rproxy)
smoke_test(fost-rproxy-smoke)
