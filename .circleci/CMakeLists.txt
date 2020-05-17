cmake_minimum_required(VERSION 3.7)
project(fost-web-circleci)

## Opt in to tests
add_custom_target(check)

add_subdirectory(boost)
add_subdirectory(cord)
add_subdirectory(crypto)
add_subdirectory(fost-base)
add_subdirectory(fost-internet)
add_subdirectory(.. fost-web)
add_subdirectory(json-schema)
add_subdirectory(threading)

