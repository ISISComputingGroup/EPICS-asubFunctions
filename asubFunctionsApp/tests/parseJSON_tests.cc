#include <string>
#include <epicsThread.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "gtest/gtest.h"

#include <shareLib.h>
#include "parseJSON.h"

#pragma warning( disable : 4129 )

json json_test_data = {
  {"f", 3.141},
  {"b", true},
  {"s", "Hello"},
  {"a", {
    {"i", 67}
  }},
  {"array", {1, 10, 12}},
  {"object", {
    {"val1", "Test"},
    {"val2", 23.4}
  }}
};

namespace {

    TEST(ParseJSONTests, test_GIVEN_action_only_THEN_parses_ok) {
        json value1 = getJSONValue(json_test_data, "/f");
        json value2 = getJSONValue(json_test_data, "/a/i");
        json value3 = getJSONValue(json_test_data, "/array");
        json value4 = getJSONValue(json_test_data, "/array/1");
        json value5 = getJSONValue(json_test_data, "/object/val1");
                
        // Then:
        ASSERT_EQ(value1.is_array(), false);
        ASSERT_EQ(value1, 3.141);
        ASSERT_EQ(value2, 67);
        ASSERT_EQ(value3.is_array(), true);
        ASSERT_EQ(value4, 10);
        ASSERT_EQ(value5, "Test");
    }


}
