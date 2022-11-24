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

TEST(AccessTest, Equal) {
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

TEST(AccessTest, EqualForAll) {
    using json = xushun::json;
    json j;
    // string
    j.setString("abc");
    EXPECT_EQ(true, j.isEqual(std::string("abc")));
    EXPECT_EQ(false, j.isEqual(std::string("abcd")));
    j.setNull();
    // const char*
    j.setString("abc");
    EXPECT_EQ(true, j.isEqual("abc"));
    EXPECT_EQ(false, j.isEqual("abcd"));
    j.setNull();
    // number double
    j.setNumber(123.456789);
    EXPECT_EQ(true, j.isEqual(123.456789));
    EXPECT_EQ(false, j.isEqual(123.45678));
    j.setNull();
    // boolean
    j.setBoolean(true);
    EXPECT_EQ(true, j.isEqual(true));
    EXPECT_EQ(false, j.isEqual(false));
    j.setBoolean(false);
    EXPECT_EQ(true, j.isEqual(false));
    EXPECT_EQ(false, j.isEqual(true));
    j.setNull();
    // array vector
    EXPECT_EQ(json::JSON_PARSE_OK, j.parse("[1,2,3]"));
    std::vector<double> vec{1,2,3};
    EXPECT_EQ(true, j.isEqual(vec));
    std::vector<double> vec1{1,2,3,4};
    EXPECT_EQ(false, j.isEqual(vec1));
    std::vector<double> vec2{1,2,4};
    EXPECT_EQ(false, j.isEqual(vec2));
    std::vector<std::string> vec3{"1", "2", "3"};
    EXPECT_EQ(false, j.isEqual(vec3));
    j.setNull();
    // object map
    EXPECT_EQ(json::JSON_PARSE_OK, j.parse("{\"a\":1,\"b\":2}"));
    std::map<std::string,double> mp{{"a", 1}, {"b", 2}};
    EXPECT_EQ(true, j.isEqual(mp));
    std::map<std::string,double> mp1{{"a", 1}, {"b", 2}, {"c", 3}};
    EXPECT_EQ(false, j.isEqual(mp1));
    std::map<std::string,double> mp2{{"a", 1}, {"b", 3}};
    EXPECT_EQ(false, j.isEqual(mp2));
    std::map<std::string,std::string> mp3{{"a", "1"}, {"b", "2"}};
    EXPECT_EQ(false, j.isEqual(mp3));
    j.setNull();
}

TEST(AccessTest, OperatorEqual) {
    using json = xushun::json;
    json j;
    // string
    j.setString("abc");
    EXPECT_EQ(true, j == (std::string("abc")));
    EXPECT_EQ(false, j == (std::string("abcd")));
    j.setNull();
    // const char*
    j.setString("abc");
    EXPECT_EQ(true, j == ("abc"));
    EXPECT_EQ(false, j == ("abcd"));
    j.setNull();
    // number double
    j.setNumber(123.456789);
    EXPECT_EQ(true, j == (123.456789));
    EXPECT_EQ(false, j == (123.45678));
    j.setNull();
    // boolean
    j.setBoolean(true);
    EXPECT_EQ(true, j == (true));
    EXPECT_EQ(false, j == (false));
    j.setBoolean(false);
    EXPECT_EQ(true, j == (false));
    EXPECT_EQ(false, j == (true));
    j.setNull();
    // array vector
    EXPECT_EQ(json::JSON_PARSE_OK, j.parse("[1,2,3]"));
    std::vector<double> vec{1,2,3};
    EXPECT_EQ(true, j == (vec));
    std::vector<double> vec1{1,2,3,4};
    EXPECT_EQ(false, j == (vec1));
    std::vector<double> vec2{1,2,4};
    EXPECT_EQ(false, j == (vec2));
    std::vector<std::string> vec3{"1", "2", "3"};
    EXPECT_EQ(false, j == (vec3));
    j.setNull();
    // object map
    EXPECT_EQ(json::JSON_PARSE_OK, j.parse("{\"a\":1,\"b\":2}"));
    std::map<std::string,double> mp{{"a", 1}, {"b", 2}};
    EXPECT_EQ(true, j == (mp));
    std::map<std::string,double> mp1{{"a", 1}, {"b", 2}, {"c", 3}};
    EXPECT_EQ(false, j == (mp1));
    std::map<std::string,double> mp2{{"a", 1}, {"b", 3}};
    EXPECT_EQ(false, j == (mp2));
    std::map<std::string,std::string> mp3{{"a", "1"}, {"b", "2"}};
    EXPECT_EQ(false, j == (mp3));
    j.setNull();
}

TEST(AccessTest, Constructor) {
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
    // array vector
    EXPECT_EQ(json::JSON_PARSE_OK, j.parse("[1,2,3]"));
    std::vector<double> vec{1,2,3};
    json j2(vec);
    EXPECT_EQ(true, j.isEqual(j2));
    j.setNull();
    // object vector
    EXPECT_EQ(json::JSON_PARSE_OK, j.parse("{\"a\":1,\"b\":2}"));
    std::map<std::string,double> mp{{"a", 1}, {"b", 2}};
    json j3(mp);
    EXPECT_EQ(true, j.isEqual(j3));
    j.setNull();
}

TEST(AccessTest, OperatorAssign) {
    using json = xushun::json;
    json j;
    // object
    j = json("hello world");
    EXPECT_EQ(json::JSON_STRING, j.getType());
    j.setNull();
    // string
    j = std::string("hello world");
    EXPECT_EQ(json::JSON_STRING, j.getType());
    EXPECT_EQ("hello world", j.getString());
    j.setNull();
    // const char*
    j = "hello world";
    EXPECT_EQ(json::JSON_STRING, j.getType());
    EXPECT_EQ("hello world", j.getString());
    j.setNull();
    // number double
    j = 123.456789;
    EXPECT_EQ(json::JSON_NUMBER, j.getType());
    EXPECT_DOUBLE_EQ(123.456789, j.getNumber());
    j.setNull();
    // boolean
    j = true;
    EXPECT_EQ(json::JSON_TRUE, j.getType());
    j = false;
    EXPECT_EQ(json::JSON_FALSE, j.getType());
    j.setNull();
    // array vector
    j = std::vector<double>{1,2,3};
    EXPECT_EQ(json::JSON_ARRAY, j.getType());
    EXPECT_EQ(3, j.getArraySize());
    EXPECT_DOUBLE_EQ(1, j.getArrayElement(0).getNumber());
    EXPECT_DOUBLE_EQ(2, j.getArrayElement(1).getNumber());
    EXPECT_DOUBLE_EQ(3, j.getArrayElement(2).getNumber());
    j.setNull();
    // object map
    j = std::map<std::string,double>{{"1", 1}, {"2", 2}};
    EXPECT_EQ(json::JSON_OBJECT, j.getType());
    EXPECT_EQ(2, j.getObjectSize());
    EXPECT_DOUBLE_EQ(1, j.findObjectElement("1").getNumber());
    EXPECT_DOUBLE_EQ(2, j.findObjectElement("2").getNumber());
    j.setNull();
}

TEST(AccessTest, AccessNull) {
    using json = xushun::json;
    json j;
    j.setString("abc");
    j.setNull();
    EXPECT_EQ(json::JSON_NULL, j.getType());
}

TEST(AccessTest, AccessBoolean) {
    using json = xushun::json;
    json j;
    j.setString("abc");
    j.setBoolean(true);
    EXPECT_EQ(json::JSON_TRUE, j.getType());
    EXPECT_EQ(true, j.getBoolean());
    j.setBoolean(false);
    EXPECT_EQ(json::JSON_FALSE, j.getType());
    EXPECT_EQ(false, j.getBoolean());
}

TEST(AccessTest, AccessNumber) {
    using json = xushun::json;
    json j;
    j.setString("abc");
    j.setNumber(123.456789);
    EXPECT_EQ(json::JSON_NUMBER, j.getType());
    EXPECT_DOUBLE_EQ(123.456789, j.getNumber());
}

TEST(AccessTest, AccessString) {
    using json = xushun::json;
    json j;
    j.setString("");
    EXPECT_EQ(json::JSON_STRING, j.getType());
    EXPECT_EQ("", j.getString());
    j.setString("hello");
    EXPECT_EQ(json::JSON_STRING, j.getType());
    EXPECT_EQ("hello", j.getString());
}

TEST(AccessTest, AccessArray) {
    using json = xushun::json;
    json a;
    for (int i = 0; i <= 5; ++ i) {
        a.setArray();
        EXPECT_EQ(json::JSON_ARRAY, a.getType());
        EXPECT_EQ(0, a.getArraySize());
        for (int j = 0; j < 10; ++ j) {
            json e;
            e.setNumber(j);
            a.pushbackArray(e);
        }
        EXPECT_EQ(10, a.getArraySize());
        for (int j = 0; j < 10; ++ j) {
            EXPECT_DOUBLE_EQ((double)j, a.getArrayElement(j).getNumber());
        }
    }
    a.popbackArray();
    EXPECT_EQ(9, a.getArraySize());
    for (int j = 0; j < 9; ++ j) {
        EXPECT_DOUBLE_EQ((double)j, a.getArrayElement(j).getNumber());
    }
    a.eraseArrayElement(4, 0);
    EXPECT_EQ(9, a.getArraySize());
    for (int j = 0; j < 9; ++ j) {
        EXPECT_DOUBLE_EQ((double)j, a.getArrayElement(j).getNumber());
    }
    a.eraseArrayElement(8, 1);
    EXPECT_EQ(8, a.getArraySize());
    for (int j = 0; j < 8; ++ j) {
        EXPECT_DOUBLE_EQ((double)j, a.getArrayElement(j).getNumber());
    }
    a.eraseArrayElement(0, 2);
    EXPECT_EQ(6, a.getArraySize());
    for (int j = 0; j < 6; ++ j) {
        EXPECT_DOUBLE_EQ((double)j + 2, a.getArrayElement(j).getNumber());
    }
    for (int j = 0; j < 2; ++ j) {
        json e;
        e.setNumber(j);
        a.insertArrayElement(j, e);
    }
    EXPECT_EQ(8, a.getArraySize());
    for (int j = 0; j < 8; ++ j) {
        EXPECT_DOUBLE_EQ((double)j, a.getArrayElement(j).getNumber());
    }
    a.clearArray();
    EXPECT_EQ(0, a.getArraySize());
    // operator[]
    for (int i = 0; i <= 5; ++ i) {
        a.setArray();
        EXPECT_EQ(json::JSON_ARRAY, a.getType());
        EXPECT_EQ(0, a.getArraySize());
        for (int j = 0; j < 10; ++ j) {
            json e;
            e.setNumber(j);
            a.pushbackArray(e);
        }
        EXPECT_EQ(10, a.getArraySize());
        for (int j = 0; j < 10; ++ j) {
            EXPECT_DOUBLE_EQ((double)j, a[j].getNumber());
        }
    }
}

TEST(AccessTest, AccessObject) {
    using json = xushun::json;
    json o;
    for (int i = 0; i <= 5; ++ i) {
        o.setObject();
        EXPECT_EQ(json::JSON_OBJECT, o.getType());
        EXPECT_EQ(0, o.getObjectSize());
        for (int j = 0; j < 10; ++ j) {
            std::string key = "a";
            key.at(0) += j;
            json v;
            v.setNumber(j);
            o.insertObjectElement(key, v);
        }
        EXPECT_EQ(10, o.getObjectSize());
        for (int j = 0; j < 10; ++ j) {
            std::string key = "a";
            key.at(0) += j;
            EXPECT_EQ(true, o.existObjectElement(key));
            json v = o.findObjectElement(key);
            EXPECT_DOUBLE_EQ((double)j, v.getNumber());
        }
    }
    EXPECT_EQ(10, o.getObjectSize());
    EXPECT_EQ(true, o.existObjectElement("j"));
    o.eraseObjectElement("j");
    EXPECT_EQ(false, o.existObjectElement("j"));
    EXPECT_EQ(9, o.getObjectSize());
    EXPECT_EQ(true, o.existObjectElement("a"));
    o.eraseObjectElement("a");
    EXPECT_EQ(false, o.existObjectElement("a"));
    EXPECT_EQ(8, o.getObjectSize());
    for (int i = 0; i < 8; ++ i) {
        std::string key = "a";
        key.at(0) += 1 + i;
        EXPECT_DOUBLE_EQ((double)i + 1, o.findObjectElement(key).getNumber());
    }
    o.clearObject();
    EXPECT_EQ(0, o.getObjectSize());
    // operator[]
    for (int i = 0; i <= 5; ++ i) {
        o.setObject();
        EXPECT_EQ(json::JSON_OBJECT, o.getType());
        EXPECT_EQ(0, o.getObjectSize());
        for (int j = 0; j < 10; ++ j) {
            std::string key = "a";
            key.at(0) += j;
            json v;
            v.setNumber(j);
            o[key] = v;
        }
        EXPECT_EQ(10, o.getObjectSize());
        for (int j = 0; j < 10; ++ j) {
            std::string key = "a";
            key.at(0) += j;
            EXPECT_EQ(true, o.existObjectElement(key));
            json v = o[key];
            EXPECT_DOUBLE_EQ((double)j, v.getNumber());
        }
    }
}
































#endif // __TEST_ACCESS_HH_