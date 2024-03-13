#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace nlohmann::literals;

#include "gtest/gtest.h"

#include <shareLib.h>
#include "parseJSON.h"

#pragma warning( disable : 4129 )

static json json_test_data = {
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

static json json_test_data2 = R"(
  [10, 11, 12]
)"_json;

static json json_test_data3 = R"(
  23
)"_json;

static json json_test_data4 = R"(
  [null,{"t":150.3}]
)"_json;

static json json_test_data5 = R"(
  [200,{"t":150.5}]
)"_json;

static json json_test_data6 = R"(
  ["ProtocolError","unknown action", {}]
)"_json;

namespace {

    TEST(ParseJSONTests, test_GIVEN_data_and_path_THEN_parses_ok) {
        // Given
        json value1 = getJSONValue(json_test_data, "/f");
        json value2 = getJSONValue(json_test_data, "/a/i");
        json value3 = getJSONValue(json_test_data, "/array");
        json value4 = getJSONValue(json_test_data, "/array/1");
        json value5 = getJSONValue(json_test_data, "/object/val1");
        json value6 = getJSONValue(json_test_data2, "/1");
        json value7 = getJSONValue(json_test_data2, "/");
        json value8 = getJSONValue(json_test_data3, "/");
        json value9 = getJSONValue(json_test_data4, "/1/t");
        json value10 = getJSONValue(json_test_data5, "/0");
        json value11 = getJSONValue(json_test_data6, "/0");
                
        // Then:
        ASSERT_EQ(value1.is_array(), false);
        ASSERT_EQ(value1, 3.141);
        ASSERT_EQ(value2, 67);
        ASSERT_EQ(value3.is_array(), true);
        ASSERT_EQ(value4, 10);
        ASSERT_EQ(value5, "Test");
        ASSERT_EQ(value6, 11);
        ASSERT_EQ(value7.is_array(), true);
        ASSERT_EQ(value7.size(), 3);
        ASSERT_EQ(value7[1], 11);
        ASSERT_EQ(value8, 23);
        ASSERT_EQ(value9, 150.3);
        ASSERT_EQ(value10, 200);
        ASSERT_EQ(value11, "ProtocolError");
    }
}
