/*
*  @Filename : json.cc
*  @Description : implementation of class json
*  @Datatime : 2022/11/20 15:49:16
*  @Author : xushun
*/

#include "json.hh"
#include <cerrno>   // strtod()
#include <cmath>    // HUGE_VAL

using json = xushun::json;



json::json() {
    type_ = JSON_NULL;
}
json::json(const json& src) {
    type_ = src.type_;
    key_ = src.key_;
    switch (type_) {
        case JSON_OBJECT:
            object_ = src.object_; break;
        case JSON_ARRAY:
            array_ = src.array_; break;
        case JSON_STRING:
            string_ = src.string_; break;
        case JSON_NUMBER:
            number_ = src.number_; break;
        default: break;
    }
}
json& json::operator=(const json& src) {
    if (&src == this) {
        return * this;
    }
    type_ = src.type_;
    key_ = src.key_;
    object_ = src.object_;
    array_ = src.array_;
    string_ = src.string_;
    number_ = src.number_;
    return *this;
}




json::parseContext::parseContext(std::string& jsonString) {
    unparsed_ = jsonString;
    idx_ = 0;
}
int json::parseContext::idx() {
    return idx_;
}
void json::parseContext::resetIdx(int idx) {
    idx_ = idx;
}
char json::parseContext::cur() {
    return unparsed_[idx_];
}
char json::parseContext::curPass() {
    return unparsed_[idx_ ++];
}
std::string json::parseContext::subUnparsed(int startIdx, int len) {
    return unparsed_.substr(startIdx, len);
}
void json::parseContext::stackPushCh(char ch) {
    stack_ += ch;
}
void json::parseContext::stackPushStr(std::string str) {
    stack_ += str;
}
std::string json::parseContext::stackPop(int len) {
    std::string ret = stack_.substr(stack_.size() - len, len);
    stack_.resize(stack_.size() - len);
    return ret;
}
int json::parseContext::stackSize() {
    return stack_.size();
}









void json::parseWhitespace(parseContext& context) {
    char ch = context.cur();
    while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
        ch = context.curPass();
    }
}
json::jsonError json::parseLiteral(parseContext& context, std::string&& literal, jsonType type) {
    for (char ch : literal) {
        if (ch != context.curPass()) {
            return JSON_PARSE_INVALID_VALUE;
        }
    }
    type_ = type;
    return JSON_PARSE_OK;
}
static bool isDigit(char ch) { return ch >= '0' && ch <= '9'; }
static bool isDigit1To9(char ch) { return ch >='1' && ch <= '9'; }
json::jsonError json::parseNumber(parseContext& context) {
    int startIdx = context.idx();
    if (context.cur() == '-') { context.curPass(); }
    if (context.cur() == '0') { 
        context.curPass();
    } else {
        if (!isDigit1To9(context.cur())) { return JSON_PARSE_INVALID_VALUE; }
        while (isDigit(context.curPass())) { }
    }
    if (context.cur() == '.') {
        context.curPass();
        if (!isDigit(context.cur())) { return JSON_PARSE_INVALID_VALUE; }
        while (isDigit(context.curPass())) { }
    }
    if (context.cur() == 'e' || context.cur() == 'E') {
        context.curPass();
        if (context.cur() == '+' || context.cur() == '-') {
            context.curPass();
        }
        if (!isDigit(context.cur())) { return JSON_PARSE_INVALID_VALUE; }
        while (isDigit(context.curPass())) { }
    }
    errno = 0;
    number_ = strtod(context.subUnparsed(startIdx, context.cur() - startIdx).c_str(), nullptr);
    if (errno == ERANGE && (number_ == HUGE_VAL || number_ == -HUGE_VAL)) {
        return JSON_PARSE_NUMBER_TOO_BIG;
    }
    type_ = JSON_NUMBER;
    return JSON_PARSE_OK;
}
bool json::parseHex4(parseContext& context, unsigned& u) {
    u = 0;
    for (int i = 0; i < 4; i++) {
        char ch = context.curPass();
        u <<= 4;
        if      (ch >= '0' && ch <= '9')  { u |= ch - '0';        }
        else if (ch >= 'A' && ch <= 'F')  { u |= ch - ('A' - 10); }
        else if (ch >= 'a' && ch <= 'f')  { u |= ch - ('a' - 10); }
        else { return false; }
    }
    return true;
}
void json::encodeUtf8(parseContext& context, unsigned u) {
    if (u < 0x0080) {
        context.stackPushCh(0x00 | (u & 0x7f));
    } else if (u >= 0x0080 && u < 0x0800) {
        context.stackPushCh(0xc0 | ((u >> 6 ) & 0x1f));
        context.stackPushCh(0x80 | ((u      ) & 0x3f));
    } else if (u >= 0x0800 && u < 0x10000) {
        context.stackPushCh(0xe0 | ((u >> 12) & 0xff));
        context.stackPushCh(0x80 | ((u >> 6 ) & 0x3f));
        context.stackPushCh(0x80 | ((u      ) & 0x3f));
    } else if (u >= 0x10000 && u <= 0x10ffff) {
        context.stackPushCh(0xf0 | ((u >> 18) & 0x07));
        context.stackPushCh(0x80 | ((u >> 12) & 0x3f));
        context.stackPushCh(0x80 | ((u >> 6 ) & 0x3f));
        context.stackPushCh(0x80 | ((u      ) & 0x3f));
    }
}
json::jsonError json::parseStringRaw(parseContext& context, std::string& dst) {
    int startIdx = context.idx();
    int startStackSize = context.stackSize();
    context.curPass(); // '\"'
    for (;;) {
        char ch = context.curPass();
        switch (ch) {
            case '\"': {
                int len = context.stackSize() - startStackSize;
                dst = context.stackPop(len);
                return JSON_PARSE_OK;
            }
            case '\0': context.resetIdx(startIdx); return JSON_PARSE_MISS_QUOTATION_MARK;
            case '\\': {
                switch (context.curPass()) {
                    case '\"': context.stackPushCh('\"'); break;
                    case '\\': context.stackPushCh('\\'); break;
                    case '/':  context.stackPushCh('/');  break;
                    case 'b':  context.stackPushCh('\b'); break;
                    case 'f':  context.stackPushCh('\f'); break;
                    case 'n':  context.stackPushCh('\n'); break;
                    case 'r':  context.stackPushCh('\r'); break;
                    case 't':  context.stackPushCh('\t'); break;
                    case 'u': {
                        unsigned u;
                        if (parseHex4(context, u) == false) {
                            context.resetIdx(startIdx); 
                            return JSON_PARSE_INVALID_UNICODE_HEX;
                        }                     
                        if (u >= 0xd800 && u <= 0xdbff) {
                            if (context.curPass() != '\\' || context.curPass() != 'u') {
                                context.resetIdx(startIdx); 
                                return JSON_PARSE_INVALID_UNICODE_SURROGATE;
                            }
                            unsigned lowu;
                            if (parseHex4(context, lowu) == false) {
                                context.resetIdx(startIdx); 
                                return JSON_PARSE_INVALID_UNICODE_HEX;
                            } 
                            if (!(lowu >= 0xdc00 && lowu <= 0xdfff)) {                                
                                context.resetIdx(startIdx); 
                                return JSON_PARSE_INVALID_UNICODE_SURROGATE;
                            }
                            u = 0x10000 + (u - 0xd800) * 0x400 + (lowu - 0xdc00);
                        }
                        encodeUtf8(context, u);
                        break;
                    }
                    default: context.resetIdx(startIdx); return JSON_PARSE_INVALID_STRING_ESCAPE;
                }
                break;
            }
            default: { 
                if ((unsigned char)ch < 0x20) {
                    context.resetIdx(startIdx);
                    return JSON_PARSE_INVALID_STRING_CHAR;
                }
                context.stackPushCh(ch);
            }
        }
    }
}
json::jsonError json::parseString(parseContext& context) {
    std::string s;
    jsonError ret = parseStringRaw(context, s);
    if (ret == JSON_PARSE_OK) {
        setString(s);
    }
    return ret;
}
json::jsonError json::parseArray(parseContext& context) {
    context.curPass(); // '['
    parseWhitespace(context);
    if (context.cur() == ']') {
        context.curPass();
        setArray();
        return JSON_PARSE_OK;
    }
    jsonError ret;
    for (;;) {
        json elem;
        elem.setNull();
        ret = elem.parseValue(context);
        if (ret != JSON_PARSE_OK) {
            break;
        }
        pushbackArray(elem);
        parseWhitespace(context);
        if (context.cur() == ',') {
            context.curPass();
            parseWhitespace(context);
        } else if (context.cur() == ']') {
            context.curPass();
            setArray();
            return JSON_PARSE_OK;
        } else {
            ret = JSON_PARSE_MISS_COMMA_OR_SQUARE_BRACKET;
            break;
        }
    }
    return ret;
}
json::jsonError json::parseObject(parseContext& context) {
    context.curPass(); // '{'
    parseWhitespace(context);
    if (context.cur() == '}') {
        context.curPass();
        setObject();
        return JSON_PARSE_OK;
    }
    jsonError ret;
    for (;;) {
        json elem;
        elem.setNull();
        if (context.cur() != '\"') {
            ret = JSON_PARSE_MISS_KEY;
            break;
        }
        std::string key;
        ret = parseStringRaw(context, key);
        if (ret != JSON_PARSE_OK) {
            break;
        }
        parseWhitespace(context);
        if (context.cur() != ':') {
            ret = JSON_PARSE_MISS_COLON;
            break;
        }
        context.curPass();
        parseWhitespace(context);
        ret = elem.parseValue(context);
        if (ret != JSON_PARSE_OK) {
            break;
        }
        insertObjectElement(key, elem);

        parseWhitespace(context);
        if (context.cur() == ',') {
            context.curPass();
            parseWhitespace(context);
        } else if (context.cur() == '}') {
            context.curPass();
            setObject();
            return JSON_PARSE_OK;
        } else {
            ret = JSON_PARSE_MISS_COMMA_OR_CURLY_BRACKET;
            break;
        }
    }
    setNull();
    return ret;
}
json::jsonError json::parseValue(parseContext& context) {
    switch (context.cur()) {
        case 'n':  return parseLiteral(context, "null",  JSON_NULL);
        case 't':  return parseLiteral(context, "true",  JSON_TRUE);
        case 'f':  return parseLiteral(context, "false", JSON_FALSE);
        case '\0': return JSON_PARSE_EXPECT_VALUE;
        case '\"': return parseString(context);
        case '[':  return parseArray(context);
        case '{':  return parseObject(context);
        default:   return parseNumber(context);
    }
}
json::jsonError json::parse(std::string& jsonString) {
    parseContext context(jsonString);
    setNull();
    parseWhitespace(context);
    jsonError ret = parseValue(context);
    if (ret == JSON_PARSE_OK) {
        parseWhitespace(context);
        if (context.idx() != jsonString.size()) {
            setNull();
            return JSON_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}










void json::dumpString(std::string& dumpedString, const std::string& s) {
    const char hexDigits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    dumpedString += '\"';
    for (unsigned char ch : s) {
        switch (ch) {
            case '\"': dumpedString += "\\\"";   break;
            case '\\': dumpedString += "\\\\";   break;
            case '\b': dumpedString += "\\b";    break;
            case '\f': dumpedString += "\\f";    break;
            case '\n': dumpedString += "\\n";    break;
            case '\r': dumpedString += "\\r";    break;
            case '\t': dumpedString += "\\t";    break;
            default:
                if (ch < 0x20) {
                    dumpedString += "\\u00";
                    dumpedString += hexDigits[ch >> 4];
                    dumpedString += hexDigits[ch & 15];
                } else {
                    dumpedString += ch;
                }
        }
    }
    dumpedString += '\"';
}
void json::dumpValue(std::string& dumpedString) {
    switch (type_) {
        case JSON_NULL:     dumpedString += "null";      break;
        case JSON_TRUE:     dumpedString += "true";      break;
        case JSON_FALSE:    dumpedString += "false";     break;
        case JSON_NUMBER:
            char num[32];
            sprintf(num, "%.17g", number_);
            dumpedString += std::string(num);            break;
        case JSON_STRING: dumpString(dumpedString, string_);           
                                                         break;
        case JSON_ARRAY:
            dumpedString += "[";
            for (int i = 0; i < array_.size(); ++ i) {
                if (i > 0) { dumpedString += ","; }
                array_[i].dumpValue(dumpedString);
            }
            dumpedString += "]";                         break;
        case JSON_OBJECT:
            dumpedString += "{";
            for (auto itr = object_.begin(); itr != object_.end(); ++ itr) {
                if (itr != object_.begin()) { dumpedString += ","; }
                dumpString(dumpedString, itr->first);
                dumpedString += ":";
                itr->second.dumpValue(dumpedString);
            }
            dumpedString += "}";                         break;
        default: 
                                                         break;
    }
}
std::string json::dump() {
    dumpValue(dumpedString_);
    return dumpedString_;
}


json::jsonType json::getType() {
    return type_;
}
bool json::isEqual(const json& rhs) {
    if (type_ != rhs.type_) { return false; }
    switch (type_) {
        case JSON_OBJECT:
            if (object_.size() != rhs.object_.size()) { return false; }
            for (std::pair<std::string, json> pr : object_) {
                if (rhs.object_.find(pr.first) == rhs.object_.end()) { return false; }
                if (!pr.second.isEqual(rhs.object_.at(pr.first))) { return false; }
            }
            return true;
        case JSON_ARRAY:
            if (array_.size() != rhs.array_.size()) { return false; }
            for (int i = 0; i < array_.size(); ++ i) {
                if (!array_[i].isEqual(rhs.array_[i])) { return false; }
            }
            return true;
        case JSON_STRING:
            return string_ == rhs.string_;
        case JSON_NUMBER:
            return number_ == rhs.number_;
        default:
            return true;
    }
}
bool json::operator==(const json& rhs) {
    return isEqual(rhs);
}
void json::setNull() {
    type_ = JSON_NULL;
    object_.clear();
    array_.clear();
    string_.clear();
}

// boolean
bool json::getBoolean() {
    return type_ == JSON_TRUE;
}
void json::setBoolean(bool b) {
    setNull();
    type_ = b ? JSON_TRUE : JSON_FALSE;
}


// number
double json::getNumber() {
    return number_;
}
void json::setNumber(double n) {
    setNull();
    type_ = JSON_NUMBER;
    number_ = n;
}

// string
std::string json::getString() {
    return string_;
}
void json::setString(std::string s) {
    setNull();
    type_ = JSON_STRING;
    string_ = s;
}


// array
void json::setArray() {
    setNull();
    type_ = JSON_ARRAY;
}
int json::getArraySize() {
    return array_.size();
}
void json::clearArray() {
    array_.clear();
}
json& json::getArrayElement(int index) {
    return array_[index];
}
void json::pushbackArray(json& j) {
    array_.push_back(j);
}
void json::popbackArray() {
    array_.pop_back();
}
void json::insertArrayElement(int index, json& j) {
    array_.insert(array_.begin() + index, j);
}
void json::eraseArrayElement(int index, int count) {
    array_.erase(array_.begin() + index, array_.begin() + index + count);
}


// object
void json::setObject() {
    setNull();
    type_ = JSON_OBJECT;
}
int json::getObjectSize() {
    return object_.size();
}
void json::clearObject() {
    object_.clear();
}
bool json::existObjectElement(std::string key) {
    return object_.find(key) != object_.end();
}
json& json::findObjectElement(std::string key) { // assert
    return object_[key];
}
void json::eraseObjectElement(std::string key) {
    object_.erase(key);
}
void json::insertObjectElement(std::string key, json& j) {
    object_.insert({key, j});
}






#include <iostream>

// test
int main() {

    json j;

    json jtrue;
    jtrue.setBoolean(true);
    
    json jfalse;
    jfalse.setBoolean(false);

    json jstr;
    jstr.setString("testString");

    json jnum;
    jnum.setNumber(12.3424446453766);

    json jarr;
    jarr.setArray();
    jarr.pushbackArray(jtrue);
    jarr.pushbackArray(jfalse);
    
    json jobj;
    jobj.setObject();

    j.setObject();
    j.insertObjectElement("jtrue", jtrue);
    j.insertObjectElement("jfalse", jfalse);
    j.insertObjectElement("jstr", jstr);
    j.insertObjectElement("jarr", jarr);
    j.insertObjectElement("jnum", jnum);
    j.insertObjectElement("jobj", jobj);



    std::cout << j.dump() << std::endl;

    std::string testString = "true";
    std::string jsonString = j.dump();
    json jparse;
    jparse.parse(jsonString);
    std::cout << jparse.dump() << std::endl;
    json jp;
    jp.parse(testString);
    std::cout << jp.dump() << std::endl;
    return 0;
}