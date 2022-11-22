/*
*  @Filename : json.hh
*  @Description : json parser and serializer
*  @Datatime : 2022/11/20 14:16:48
*  @Author : xushun
*/
#ifndef  __JSON_HH_
#define  __JSON_HH_


#include <vector>
#include <string>
#include <map>


namespace xushun {

    class json {


        public: // enums
            enum jsonType { 
                JSON_NULL, 
                JSON_FALSE, 
                JSON_TRUE, 
                JSON_NUMBER, 
                JSON_STRING, 
                JSON_ARRAY, 
                JSON_OBJECT
            };
            enum jsonError {
                JSON_PARSE_OK = 0,                      // 成功解析
                JSON_PARSE_EXPECT_VALUE,                // 只有空白字符
                JSON_PARSE_INVALID_VALUE,               // 非法json值(非法的字面量)
                JSON_PARSE_ROOT_NOT_SINGULAR,           // value whitespace 后还有其他 value
                JSON_PARSE_NUMBER_TOO_BIG,              // 数字超过double的范围
                JSON_PARSE_MISS_QUOTATION_MARK,         // 字符串的第二个双引号丢失
                JSON_PARSE_INVALID_STRING_ESCAPE,       // 非法的转义字符
                JSON_PARSE_INVALID_STRING_CHAR,         // 非法的字符串字符
                JSON_PARSE_INVALID_UNICODE_HEX,         // 非法的Unicode \u之后不是4位16进制数
                JSON_PARSE_INVALID_UNICODE_SURROGATE,   // 只有高代理项而欠缺低代理项 或低代理项不在合法码点范围中
                JSON_PARSE_MISS_COMMA_OR_SQUARE_BRACKET,// 逗号或方括号丢失
                JSON_PARSE_MISS_KEY,                    // json对象成员的key丢失
                JSON_PARSE_MISS_COLON,                  // 冒号丢失
                JSON_PARSE_MISS_COMMA_OR_CURLY_BRACKET  // 逗号或大括号丢失
            };




        private: // dumper
            std::string dumpedString_;

            void dumpValue(std::string& dumpedString);
            void dumpString(std::string& dumpedString, const std::string& s);
        public:
            std::string dump();




        private: // parser
            class parseContext {
                private:
                    std::string unparsed_;
                    int idx_;
                    std::string stack_;
                public:
                    parseContext(std::string& jsonString);
                    int idx();
                    void resetIdx(int idx);
                    char cur();
                    char curPass();
                    std::string subUnparsed(int startIdx, int len);
                    void stackPushCh(char ch);
                    void stackPushStr(std::string str);
                    std::string stackPop(int len);
                    int stackSize();
            };

            void parseWhitespace(parseContext& context);
            jsonError parseLiteral(parseContext& context, std::string&& literal, jsonType type);
            jsonError parseNumber(parseContext& context);
            bool parseHex4(parseContext& context, unsigned& u);
            void encodeUtf8(parseContext& context, unsigned u);
            jsonError parseStringRaw(parseContext& context, std::string& dst);
            jsonError parseString(parseContext& context);
            jsonError parseArray(parseContext& context);
            jsonError parseObject(parseContext& context);
            jsonError parseValue(parseContext& context);
        public:
            jsonError parse(std::string& jsonString);




        private: // json value
            jsonType type_;
            std::string key_;
            
            std::map<std::string, json> object_; // JSON_OBJECT
            std::vector<json> array_;  // JSON_ARRAY
            std::string string_;       // JSON_STRING
            double number_;            // JSON_NUMBER
        public:
            json();
            json(const json& src);
            json& operator=(const json& src);

            jsonType getType();
            bool isEqual(const json& rhs);
            bool operator==(const json& rhs);
            void setNull();
            
            // bool
            bool getBoolean();
            void setBoolean(bool b);
            // number
            double getNumber();
            void setNumber(double n);
            // string
            std::string getString();
            void setString(std::string s);
            // array
            void setArray();
            int getArraySize();
            void clearArray();
            json& getArrayElement(int index);
            void pushbackArray(json& j);
            void popbackArray();
            void insertArrayElement(int index, json& j);
            void eraseArrayElement(int index, int count);
            // object
            void setObject();
            int getObjectSize();
            void clearObject();
            bool existObjectElement(std::string key);
            json& findObjectElement(std::string key);
            void eraseObjectElement(std::string key);
            void insertObjectElement(std::string key, json& j);
    };







}


#endif // __JSON_HH_