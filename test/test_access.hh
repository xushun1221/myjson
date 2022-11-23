/*
*  @Filename : test_access.hh
*  @Description : unit test for access
*  @Datatime : 2022/11/23 19:26:46
*  @Author : xushun
*/
#ifndef  __TEST_ACCESS_HH_
#define  __TEST_ACCESS_HH_


#include <gtest/gtest.h>
#include "../json.hh"



#define TEST_EQUAL(jsonString1, jsonString2, equality)\
    do {\
        json j1;\
        EXPECT_EQ(json::JSON_PARSE_OK, j1.parse(jsonString1));\
        json j2;\
        EXPECT_EQ(json::JSON_PARSE_OK, j2.parse(jsonString2));\
        EXPECT_EQ(equality, j1.isEqual(j2));\
    } while(0)

TEST(TestAccess, Equal) {
    using json = xushun::json;
    TEST_EQUAL("true", "true", true);
    TEST_EQUAL("true", "false", false);
    TEST_EQUAL("false", "false", true);
    TEST_EQUAL("null", "null", true);
    TEST_EQUAL("null", "0", false);
    TEST_EQUAL("123", "123", true);
    TEST_EQUAL("123", "456", false);
    TEST_EQUAL("\"abc\"", "\"abc\"", true);
    TEST_EQUAL("\"abc\"", "\"abcd\"", false);
    TEST_EQUAL("[]", "[]", true);
    TEST_EQUAL("[]", "null", false);
    TEST_EQUAL("[1,2,3]", "[1,2,3]", true);
    TEST_EQUAL("[1,2,3]", "[1,2,3,4]", false);
    TEST_EQUAL("[[]]", "[[]]", true);
    TEST_EQUAL("{}", "{}", true);
    TEST_EQUAL("{}", "null", false);
    TEST_EQUAL("{}", "[]", false);
    TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":2}", true);
    TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"b\":2,\"a\":1}", true);
    TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":3}", false);
    TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":2,\"c\":3}", false);
    TEST_EQUAL("{\"a\":{\"b\":{\"c\":{}}}}", "{\"a\":{\"b\":{\"c\":{}}}}", true);
    TEST_EQUAL("{\"a\":{\"b\":{\"c\":{}}}}", "{\"a\":{\"b\":{\"c\":[]}}}", false);
}

TEST(TestAccess, Constructor) {
    using json = xushun::json;
    // copy
    json j;
    EXPECT_EQ(json::JSON_PARSE_OK, j.parse("{\"n\":null,\"f\":false,\"t\":true,\"i\":123,\"s\":\"abc\",\"a\":[1,2,3],\"o\":{\"1\":1,\"2\":2,\"3\":3}}"));
    EXPECT_EQ(true, j.isEqual(json(j)));
    j.setNull();
    // string
    EXPECT_EQ(json::JSON_PARSE_OK, j.parse("\"hello world\""));
    EXPECT_EQ(true, j.isEqual(json("hello world")));
    j.setNull();
    // char*
    EXPECT_EQ(json::JSON_PARSE_OK, j.parse("\"hello world\""));
    EXPECT_EQ(true, j.isEqual(json(std::string("hello world"))));
    j.setNull();
    // number
    EXPECT_EQ(json::JSON_PARSE_OK, j.parse("123.456789"));
    EXPECT_EQ(true, j.isEqual(json(123.456789)));
    j.setNull(); 
    // boolean
    EXPECT_EQ(json::JSON_PARSE_OK, j.parse("true"));
    EXPECT_EQ(true, j.isEqual(json(true)));
    j.setNull(); 
    // default null
    json j1;
    EXPECT_EQ(json::JSON_NULL, j1.getType());
}

TEST(TestAccess, AccessNull) {

}

TEST(TestAccess, AccessBoolean) {
    
}

TEST(TestAccess, AccessNumber) {
    
}

TEST(TestAccess, AccessString) {
    
}

TEST(TestAccess, AccessArray) {
    
}

TEST(TestAccess, AccessObject) {
    
}
































#endif // __TEST_ACCESS_HH_