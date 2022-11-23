/*
*  @Filename : dump_tests.cc
*  @Description : unit tests for dumper
*  @Datatime : 2022/11/22 22:07:09
*  @Author : xushun
*/
#ifndef  __DUMP_TESTS_HH_
#define  __DUMP_TESTS_HH_




#include <gtest/gtest.h>
#include "../json.hh"







#define TEST_ROUNDTRIP(jsonString)\
    do {\
        json j;\
        EXPECT_EQ(json::JSON_PARSE_OK, j.parse(jsonString));\
        EXPECT_EQ(jsonString, j.dump());\
    } while(0)

TEST(DumpTest, DumpNumber) {
    using json = xushun::json;
    TEST_ROUNDTRIP("0");
    TEST_ROUNDTRIP("-0");
    TEST_ROUNDTRIP("1");
    TEST_ROUNDTRIP("-1");
    TEST_ROUNDTRIP("1.5");
    TEST_ROUNDTRIP("-1.5");
    TEST_ROUNDTRIP("3.25");
    TEST_ROUNDTRIP("1e+20");
    TEST_ROUNDTRIP("1.234e+20");
    TEST_ROUNDTRIP("1.234e-20");
    TEST_ROUNDTRIP("1.0000000000000002"); /* the smallest number > 1 */
    TEST_ROUNDTRIP("4.9406564584124654e-324"); /* minimum denormal */
    TEST_ROUNDTRIP("-4.9406564584124654e-324");
    TEST_ROUNDTRIP("2.2250738585072009e-308");  /* Max subnormal double */
    TEST_ROUNDTRIP("-2.2250738585072009e-308");
    TEST_ROUNDTRIP("2.2250738585072014e-308");  /* Min normal positive double */
    TEST_ROUNDTRIP("-2.2250738585072014e-308");
    TEST_ROUNDTRIP("1.7976931348623157e+308");  /* Max double */
    TEST_ROUNDTRIP("-1.7976931348623157e+308");
}

TEST(DumpTest, DumpString) {
    using json = xushun::json;
    TEST_ROUNDTRIP("\"\"");
    TEST_ROUNDTRIP("\"Hello\"");
    TEST_ROUNDTRIP("\"Hello\\nWorld\"");
    TEST_ROUNDTRIP("\"\\\" \\\\ / \\b \\f \\n \\r \\t\"");
    TEST_ROUNDTRIP("\"Hello\\u0000World\"");
    // utf-8 暂时还不能序列化这样的字符串
    // TEST_ROUNDTRIP("\"\\ud834\\udd1e\"");
}

TEST(DumpTest, DumpArray) {
    using json = xushun::json;
    TEST_ROUNDTRIP("[]");
    TEST_ROUNDTRIP("[null,false,true,123,\"abc\",[1,2,3]]");
}

TEST(DumpTest, DumpObject) {
    using json = xushun::json;
    TEST_ROUNDTRIP("{}");
    // using std::map as container of json_objects, original order cannot be retained
    // TEST_ROUNDTRIP("{\"n\":null,\"f\":false,\"t\":true,\"i\":123,\"s\":\"abc\",\"a\":[1,2,3],\"o\":{\"1\":1,\"2\":2,\"3\":3}}");
}








#endif // __DUMP_TESTS_HH_



