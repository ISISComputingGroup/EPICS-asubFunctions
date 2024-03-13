#include <string>

#include "gtest/gtest.h"

#include <shareLib.h>
#include "convertUNIXTime.h"

#pragma warning( disable : 4129 )


namespace {

    TEST(convertUNIXTimeTests, test_GIVEN_data_and_path_THEN_parses_ok) {
        // Given
        
        char outstr[128];
        convertUNIXTimeImpl(0.0, "%Y-%m-%dT%H:%M:%S.%%d", true, outstr, sizeof(outstr));
        ASSERT_STREQ(outstr, "1970-01-01T00:00:00.0");
        convertUNIXTimeImpl(12.345, "%Y-%m-%dT%H:%M:%S.%%d", true, outstr, sizeof(outstr));
        ASSERT_STREQ(outstr, "1970-01-01T00:00:12.345");
    }
}
