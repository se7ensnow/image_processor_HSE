cmake_minimum_required(VERSION 3.17)
project(image_processor)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(cmake/TestSolution.cmake)

find_package(Catch REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")


add_executable(image_processor
        image_processor.cpp
        cmd_arg_parser.cpp
        cmd_arg_parser.h
        filter_pipeline_factory.cpp
        filter_pipeline_factory.h
        base_filter.h
        bitmap.h
        bitmap.cpp
        app.h
        app.cpp
        filter_pipeline.h
        filter_pipeline.cpp
        filters.h
        filters.cpp)

add_catch(image_processor_test
        test.cpp
        cmd_arg_parser.cpp
        filters.cpp
        filter_pipeline_factory.cpp
        filter_pipeline.cpp
        bitmap.cpp
        app.cpp
)