/**
    Copyright 2014-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-urlhandler.hpp"
#include <fost/urlhandler>
#include <fost/test>


FSL_TEST_SUITE(view);


FSL_TEST_FUNCTION(finds_view) {
    fostlib::json configuration;
    fostlib::insert(configuration, "view1", "view", "view2");
    fostlib::insert(configuration, "view2", "view", "final.view");
    fostlib::insert(configuration, "view2", "configuration", "config items");
    const fostlib::setting<fostlib::json> views(
            "view-tests.cpp", fostlib::urlhandler::c_views, configuration);

    std::pair<fostlib::string, fostlib::json> found(
            fostlib::urlhandler::view::find_view("view1", fostlib::json()));
    FSL_CHECK_EQ(found.first, "final.view");
    FSL_CHECK_EQ(found.second, fostlib::json("config items"));
}


FSL_TEST_FUNCTION(finds_view_in_other_configuration) {
    fostlib::json configuration1;
    fostlib::insert(configuration1, "view1", "view", "view2");
    const fostlib::setting<fostlib::json> views(
            "view-tests.cpp", fostlib::urlhandler::c_views, configuration1);

    fostlib::json configuration2;
    fostlib::insert(configuration2, "view", "final.view");
    fostlib::insert(configuration2, "configuration", "config items");
    const fostlib::setting<fostlib::json> view2(
            "view-tests.cpp", fostlib::urlhandler::c_views.section(),
            fostlib::urlhandler::c_views.name() + "/view2", configuration2);

    std::pair<fostlib::string, fostlib::json> found(
            fostlib::urlhandler::view::find_view("view1", fostlib::json()));
    FSL_CHECK_EQ(found.first, "final.view");
    FSL_CHECK_EQ(found.second, fostlib::json("config items"));
}
