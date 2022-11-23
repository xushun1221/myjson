/*
*  @Filename : test_error.hh
*  @Description : unit test for parse error
*  @Datatime : 2022/11/23 18:34:26
*  @Author : xushun
*/
#ifndef  __TEST_ERROR_HH_
#define  __TEST_ERROR_HH_


#include <gtest/gtest.h>
#include "../json.hh"





#define TEST_ERROR(error, jsonString)\
    do {\
        json j;\
        j.setBoolean(false);\
        EXPECT_EQ(error, j.parse(jsonString));\
        EXPECT_EQ(json::JSON_NULL, j.getType());\
    } while(0)

TEST(ErrorTest, ExpectValue) {
    using json = xushun::json;
    TEST_ERROR(json::JSON_PARSE_EXPECT_VALUE, "");
    TEST_ERROR(json::JSON_PARSE_EXPECT_VALUE, " ");
}

TEST(ErrorTest, InvalidValue) {
    using json = xushun::json;
    TEST_ERROR(json::JSON_PARSE_INVALID_VALUE, "nul");
    TEST_ERROR(json::JSON_PARSE_INVALID_VALUE, "?");
    // 测试不合法的数字
    TEST_ERROR(json::JSON_PARSE_INVALID_VALUE, "+0");
    TEST_ERROR(json::JSON_PARSE_INVALID_VALUE, "+1");
    TEST_ERROR(json::JSON_PARSE_INVALID_VALUE, ".123"); /* at least one digit before '.' */
    TEST_ERROR(json::JSON_PARSE_INVALID_VALUE, "1.");   /* at least one digit after '.' */
    TEST_ERROR(json::JSON_PARSE_INVALID_VALUE, "INF");
    TEST_ERROR(json::JSON_PARSE_INVALID_VALUE, "inf");
    TEST_ERROR(json::JSON_PARSE_INVALID_VALUE, "NAN");
    TEST_ERROR(json::JSON_PARSE_INVALID_VALUE, "nan");
    // 测试非法数组
    TEST_ERROR(json::JSON_PARSE_INVALID_VALUE, "[1,]");
    TEST_ERROR(json::JSON_PARSE_INVALID_VALUE, "[\"a\", nul]");
}


TEST(ErrorTest, RootNotSingular) {
    using json = xushun::json;
    TEST_ERROR(json::JSON_PARSE_ROOT_NOT_SINGULAR, "null x");
    // 测试不合法的数字 0之后应该是'.','e','E'或者空白
    TEST_ERROR(json::JSON_PARSE_ROOT_NOT_SINGULAR, "0123"); /* after zero should be '.','e','E' or nothing */
    TEST_ERROR(json::JSON_PARSE_ROOT_NOT_SINGULAR, "0x0");
    TEST_ERROR(json::JSON_PARSE_ROOT_NOT_SINGULAR, "0x123");
}


TEST(ErrorTest, NumberTooBig) {
    using json = xushun::json;
    TEST_ERROR(json::JSON_PARSE_NUMBER_TOO_BIG, "1e309");
    TEST_ERROR(json::JSON_PARSE_NUMBER_TOO_BIG, "-1e309");
}

TEST(ErrorTest, MissingQuotationMark) {
    using json = xushun::json;
    TEST_ERROR(json::JSON_PARSE_MISS_QUOTATION_MARK, "\"");
    TEST_ERROR(json::JSON_PARSE_MISS_QUOTATION_MARK, "\"abc");
}

TEST(ErrorTest, InvalidStringEscape) {
    using json = xushun::json;
    TEST_ERROR(json::JSON_PARSE_INVALID_STRING_ESCAPE, "\"\\v\"");
    TEST_ERROR(json::JSON_PARSE_INVALID_STRING_ESCAPE, "\"\\'\"");
    TEST_ERROR(json::JSON_PARSE_INVALID_STRING_ESCAPE, "\"\\0\"");
    TEST_ERROR(json::JSON_PARSE_INVALID_STRING_ESCAPE, "\"\\x12\"");
}

TEST(ErrorTest, InvalidStringChar) {
    using json = xushun::json;
    TEST_ERROR(json::JSON_PARSE_INVALID_STRING_CHAR, "\"\x01\""); // "(0x01)" 单个char值
    TEST_ERROR(json::JSON_PARSE_INVALID_STRING_CHAR, "\"\x1F\"");
}

TEST(ErrorTest, InvalidUnicodeHex) {
    using json = xushun::json;
    TEST_ERROR(json::JSON_PARSE_INVALID_UNICODE_HEX, "\"\\u\"");
    TEST_ERROR(json::JSON_PARSE_INVALID_UNICODE_HEX, "\"\\u0\"");
    TEST_ERROR(json::JSON_PARSE_INVALID_UNICODE_HEX, "\"\\u01\"");
    TEST_ERROR(json::JSON_PARSE_INVALID_UNICODE_HEX, "\"\\u012\"");
    TEST_ERROR(json::JSON_PARSE_INVALID_UNICODE_HEX, "\"\\u/000\"");
    TEST_ERROR(json::JSON_PARSE_INVALID_UNICODE_HEX, "\"\\uG000\"");
    TEST_ERROR(json::JSON_PARSE_INVALID_UNICODE_HEX, "\"\\u0/00\"");
    TEST_ERROR(json::JSON_PARSE_INVALID_UNICODE_HEX, "\"\\u0G00\"");
    TEST_ERROR(json::JSON_PARSE_INVALID_UNICODE_HEX, "\"\\u00/0\"");
    TEST_ERROR(json::JSON_PARSE_INVALID_UNICODE_HEX, "\"\\u00G0\"");
    TEST_ERROR(json::JSON_PARSE_INVALID_UNICODE_HEX, "\"\\u000/\"");
    TEST_ERROR(json::JSON_PARSE_INVALID_UNICODE_HEX, "\"\\u000G\"");
    TEST_ERROR(json::JSON_PARSE_INVALID_UNICODE_HEX, "\"\\u 123\"");
}

TEST(ErrorTest, InvalidUnicodeSurrogate) {
    using json = xushun::json;
    TEST_ERROR(json::JSON_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\"");
    TEST_ERROR(json::JSON_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uDBFF\"");
    TEST_ERROR(json::JSON_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\\\\"");
    TEST_ERROR(json::JSON_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\uDBFF\"");
    TEST_ERROR(json::JSON_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\uE000\"");
}

TEST(ErrorTest, MissCommaOrSquareBracket) {
    using json = xushun::json;
    TEST_ERROR(json::JSON_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1");
    TEST_ERROR(json::JSON_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1}");
    TEST_ERROR(json::JSON_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1 2");
    TEST_ERROR(json::JSON_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[[]");
}

TEST(ErrorTest, MissKey) {
    using json = xushun::json;
    TEST_ERROR(json::JSON_PARSE_MISS_KEY, "{:1,");
    TEST_ERROR(json::JSON_PARSE_MISS_KEY, "{1:1,");
    TEST_ERROR(json::JSON_PARSE_MISS_KEY, "{true:1,");
    TEST_ERROR(json::JSON_PARSE_MISS_KEY, "{false:1,");
    TEST_ERROR(json::JSON_PARSE_MISS_KEY, "{null:1,");
    TEST_ERROR(json::JSON_PARSE_MISS_KEY, "{[]:1,");
    TEST_ERROR(json::JSON_PARSE_MISS_KEY, "{{}:1,");
    TEST_ERROR(json::JSON_PARSE_MISS_KEY, "{\"a\":1,");
}

TEST(ErrorTest, MissColon) {
    using json = xushun::json;
    TEST_ERROR(json::JSON_PARSE_MISS_COLON, "{\"a\"}");
    TEST_ERROR(json::JSON_PARSE_MISS_COLON, "{\"a\",\"b\"}");
}

TEST(ErrorTest, MissCommaOrCurlyBracket) {
    using json = xushun::json;
    TEST_ERROR(json::JSON_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1");
    TEST_ERROR(json::JSON_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1]");
    TEST_ERROR(json::JSON_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1 \"b\"");
    TEST_ERROR(json::JSON_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":{}");
}













#endif // __TEST_ERROR_HH_