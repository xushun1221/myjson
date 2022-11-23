/*
*  @Filename : parse_tests.cc
*  @Description : unit tests for parser
*  @Datatime : 2022/11/22 22:16:36
*  @Author : xushun
*/
#ifndef  __PARSE_TESTS_HH_
#define  __PARSE_TESTS_HH_


#include <gtest/gtest.h>
#include "../json.hh"



// test null
TEST(ParseTest, ParseNull) {
    using json = xushun::json;
    json j;
    EXPECT_EQ(json::JSON_PARSE_OK, j.parse("null"));
    EXPECT_EQ(json::JSON_NULL, j.getType());
}

// test boolean
TEST(ParseTest, ParseBoolean) {
    using json = xushun::json;
    json j;
    EXPECT_EQ(json::JSON_PARSE_OK, j.parse("true"));
    EXPECT_EQ(json::JSON_TRUE, j.getType());
    EXPECT_EQ(json::JSON_PARSE_OK, j.parse("false"));
    EXPECT_EQ(json::JSON_FALSE, j.getType());
}

// test number
#define PARSE_NUMBER(expectNum, jsonString)\
    do {\
        json j;\
        EXPECT_EQ(json::JSON_PARSE_OK, j.parse(jsonString));\
        EXPECT_EQ(json::JSON_NUMBER, j.getType());\
        EXPECT_DOUBLE_EQ(expectNum, j.getNumber());\
    } while(0)

TEST(ParseTest, ParseNumber) {
    using json = xushun::json;
    PARSE_NUMBER(0.0, "0");
    PARSE_NUMBER(0.0, "-0");
    PARSE_NUMBER(0.0, "-0.0");
    PARSE_NUMBER(1.0, "1");
    PARSE_NUMBER(-1.0, "-1");
    PARSE_NUMBER(1.5, "1.5");
    PARSE_NUMBER(-1.5, "-1.5");
    PARSE_NUMBER(3.1416, "3.1416");
    PARSE_NUMBER(1E10, "1E10");
    PARSE_NUMBER(1e10, "1e10");
    PARSE_NUMBER(1E+10, "1E+10");
    PARSE_NUMBER(1E-10, "1E-10");
    PARSE_NUMBER(-1E10, "-1E10");
    PARSE_NUMBER(-1e10, "-1e10");
    PARSE_NUMBER(-1E+10, "-1E+10");
    PARSE_NUMBER(-1E-10, "-1E-10");
    PARSE_NUMBER(1.234E+10, "1.234E+10");
    PARSE_NUMBER(1.234E-10, "1.234E-10");
    PARSE_NUMBER(0.0, "1e-10000"); /* must underflow */
    // 测试一些double的边界情况
    PARSE_NUMBER(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
    PARSE_NUMBER( 4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
    PARSE_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
    PARSE_NUMBER( 2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
    PARSE_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    PARSE_NUMBER( 2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
    PARSE_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    PARSE_NUMBER( 1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
    PARSE_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}

// test string
#define PARSE_STRING(expectString, jsonString)\
    do {\
        json j;\
        EXPECT_EQ(json::JSON_PARSE_OK, j.parse(jsonString));\
        EXPECT_EQ(json::JSON_STRING, j.getType());\
        EXPECT_EQ(expectString, j.getString());\
    } while(0)

TEST(ParseTest, ParseString) {
    using json = xushun::json;
    PARSE_STRING("", "\"\"");
    PARSE_STRING("Hello", "\"Hello\"");
    // 测试带转义字符的json string
    PARSE_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
    PARSE_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
    // utf8测试
    PARSE_STRING("Hello\0World", "\"Hello\\u0000World\"");
    PARSE_STRING("\x24", "\"\\u0024\"");         /* Dollar sign U+0024 */
    PARSE_STRING("\xC2\xA2", "\"\\u00A2\"");     /* Cents sign U+00A2 */
    PARSE_STRING("\xE2\x82\xAC", "\"\\u20AC\""); /* Euro sign U+20AC */
    PARSE_STRING("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");  /* G clef sign U+1D11E */
    PARSE_STRING("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");  /* G clef sign U+1D11E */
}


// test array
TEST(ParseTest, ParseArray) {
    using json = xushun::json;
    json j;
    // empty array
    EXPECT_EQ(json::JSON_PARSE_OK, j.parse("[ ]"));
    EXPECT_EQ(json::JSON_ARRAY, j.getType());
    EXPECT_EQ(0, j.getArraySize());
    j.setNull();
    // simple array
    EXPECT_EQ(json::JSON_PARSE_OK, j.parse("[ null , false , true , 123 , \"abc\" ]"));
    EXPECT_EQ(json::JSON_ARRAY, j.getType());
    EXPECT_EQ(5, j.getArraySize());
    EXPECT_EQ(json::JSON_NULL, j.getArrayElement(0).getType());
    EXPECT_EQ(json::JSON_FALSE, j.getArrayElement(1).getType());
    EXPECT_EQ(json::JSON_TRUE, j.getArrayElement(2).getType());
    EXPECT_EQ(json::JSON_NUMBER, j.getArrayElement(3).getType());
    EXPECT_EQ(json::JSON_STRING, j.getArrayElement(4).getType());
    EXPECT_DOUBLE_EQ(123.0, j.getArrayElement(3).getNumber());
    EXPECT_EQ("abc", j.getArrayElement(4).getString());
    j.setNull();
    // array of array
    EXPECT_EQ(json::JSON_PARSE_OK, j.parse("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]"));
    EXPECT_EQ(json::JSON_ARRAY, j.getType());
    EXPECT_EQ(4, j.getArraySize());
    for (int i = 0; i < 4; ++ i) {
        json t = j.getArrayElement(i);
        EXPECT_EQ(json::JSON_ARRAY, t.getType());
        EXPECT_EQ(i, t.getArraySize());
        for (int jj = 0; jj < i; ++ jj) {
            json e = t.getArrayElement(jj);
            EXPECT_EQ(json::JSON_NUMBER, e.getType());
            EXPECT_DOUBLE_EQ((double)jj, e.getNumber());
        }
    }
    j.setNull();    
}

// test object
TEST(ParseTest, ParseObject) {
    using json = xushun::json;
    json j;
    // empty object
    EXPECT_EQ(json::JSON_PARSE_OK, j.parse(" { } "));
    EXPECT_EQ(json::JSON_OBJECT, j.getType());
    EXPECT_EQ(0, j.getObjectSize());
    j.setNull();
    // complex object
    EXPECT_EQ(json::JSON_PARSE_OK, j.parse(
        " { "
        "\"n\" : null , "
        "\"f\" : false , "
        "\"t\" : true , "
        "\"i\" : 123 , "
        "\"s\" : \"abc\", "
        "\"a\" : [ 1, 2, 3 ],"
        "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
        " } "
    ));
    EXPECT_EQ(json::JSON_OBJECT, j.getType());
    EXPECT_EQ(7, j.getObjectSize());
    EXPECT_EQ(true, j.existObjectElement("n"));
    EXPECT_EQ(json::JSON_NULL, j.findObjectElement("n").getType());
    EXPECT_EQ(true, j.existObjectElement("f"));
    EXPECT_EQ(json::JSON_FALSE, j.findObjectElement("f").getType());
    EXPECT_EQ(true, j.existObjectElement("t"));
    EXPECT_EQ(json::JSON_TRUE, j.findObjectElement("t").getType());
    EXPECT_EQ(true, j.existObjectElement("i"));
    EXPECT_EQ(json::JSON_NUMBER, j.findObjectElement("i").getType());
    EXPECT_DOUBLE_EQ(123.0, j.findObjectElement("i").getNumber());
    EXPECT_EQ(true, j.existObjectElement("s"));
    EXPECT_EQ(json::JSON_STRING, j.findObjectElement("s").getType());
    EXPECT_EQ("abc", j.findObjectElement("s").getString());
    EXPECT_EQ(true, j.existObjectElement("a"));
    EXPECT_EQ(json::JSON_ARRAY, j.findObjectElement("a").getType());
    EXPECT_EQ(3, j.findObjectElement("a").getArraySize());
    for (int i = 0; i < 3; ++ i) {
        json e = j.findObjectElement("a").getArrayElement(i);
        EXPECT_EQ(json::JSON_NUMBER, e.getType());
        EXPECT_DOUBLE_EQ(i + 1.0, e.getNumber());
    }
    EXPECT_EQ(true, j.existObjectElement("o"));
    EXPECT_EQ(json::JSON_OBJECT, j.findObjectElement("o").getType());
    EXPECT_EQ(3, j.findObjectElement("o").getObjectSize());
    for (int i = 1; i <= 3; ++ i) {
        EXPECT_EQ(true, j.findObjectElement("o").existObjectElement(std::to_string(i)));
        json e = j.findObjectElement("o").findObjectElement(std::to_string(i));
        EXPECT_EQ(json::JSON_NUMBER, e.getType());
        EXPECT_EQ(i, e.getNumber());
    }
    j.setNull();
}



















#endif // __PARSE_TESTS_HH_
