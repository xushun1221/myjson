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
    object_ = src.object_;
    array_ = src.array_;
    string_ = src.string_;
    number_ = src.number_;
    return *this;
}









void json::parseWhitespace(parseContext& context) {
    const char* p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
        ++ p;
    }
    c->json = p;
}
json::jsonError json::parseLiteral(parseContext& context, std::string&& literal, jsonType type) {
    EXPECT(c, literal[0]);
    int i = 1;
    while (literal[i] != '\0') {
        if (literal[i] != *c->json) {
            return JSON_PARSE_INVALID_VALUE;
        } else {
            c->json ++;
            i ++;
        }
    }
    v->type = type;
    return JSON_PARSE_OK;
}
json::jsonError json::parseNumber(parseContext& context) {
    const char* p = c->json;
    if (*p == '-') {
        p ++;
    }
    if (*p == '0') {
        p ++;
    } else {
        if (!ISDIGIT_1TO9(*p)) {
            return JSON_PARSE_INVALID_VALUE;
        }
        for (p ++; ISDIGIT(*p); p ++);
    }
    if (*p == '.') {
        p ++;
        if (!ISDIGIT(*p)) {
            return JSON_PARSE_INVALID_VALUE;
        }
        for (p ++; ISDIGIT(*p); p ++);
    }
    if (*p == 'e' || *p == 'E') {
        p ++;
        if (*p == '+' || *p == '-') {
            p ++;
        }
        if (!ISDIGIT(*p)) {
            return JSON_PARSE_INVALID_VALUE;
        }
        for (p ++; ISDIGIT(*p); p ++);
    }
    errno = 0;
    v->u.n = strtod(c->json, NULL);
    if (errno == ERANGE && (v->u.n == HUGE_VAL || v->u.n == -HUGE_VAL)) {
        return JSON_PARSE_NUMBER_TOO_BIG;
    }
    v->type = JSON_NUMBER;
    c->json = p;
    return JSON_PARSE_OK;
}
bool json::parseHex4(parseContext& context, unsigned& u) {
    *u = 0;
    for (int i = 0; i < 4; i++) {
        char ch = *p++;
        *u <<= 4;
        if      (ch >= '0' && ch <= '9')  *u |= ch - '0';
        else if (ch >= 'A' && ch <= 'F')  *u |= ch - ('A' - 10);
        else if (ch >= 'a' && ch <= 'f')  *u |= ch - ('a' - 10);
        else return NULL;
    }
    return p;
}
void json::encodeUtf8(parseContext& context, unsigned u) {
    if (u < 0x0080) {
        PUTC(c, 0x00 | (u & 0x7f));
    } else if (u >= 0x0080 && u < 0x0800) {
        PUTC(c, 0xc0 | ((u >> 6 ) & 0x1f));
        PUTC(c, 0x80 | ((u      ) & 0x3f));
    } else if (u >= 0x0800 && u < 0x10000) {
        PUTC(c, 0xe0 | ((u >> 12) & 0xff));
        PUTC(c, 0x80 | ((u >> 6 ) & 0x3f));
        PUTC(c, 0x80 | ((u      ) & 0x3f));
    } else if (u >= 0x10000 && u <= 0x10ffff) {
        PUTC(c, 0xf0 | ((u >> 18) & 0x07));
        PUTC(c, 0x80 | ((u >> 12) & 0x3f));
        PUTC(c, 0x80 | ((u >> 6 ) & 0x3f));
        PUTC(c, 0x80 | ((u      ) & 0x3f));
    }
}
#define STRING_ERROR(ret) do { c->top = head; return ret; } while(0)
json::jsonError json::parseStringRaw(parseContext& context, std::string& dst) {
    size_t head = c->top;
    EXPECT(c, '\"');
    const char* p = c->json;
    for (;;) {
        char ch = *p ++;
        switch (ch) {
            case '\"': {
                *len = c->top - head; 
                *str = json_context_pop(c, *len);
                c->json = p;
                return JSON_PARSE_OK;
            }
            case '\0': STRING_ERROR(JSON_PARSE_MISS_QUOTATION_MARK);
            case '\\': {
                switch (*p ++) {
                    case '\"': PUTC(c, '\"'); break;
                    case '\\': PUTC(c, '\\'); break;
                    case '/':  PUTC(c, '/');  break;
                    case 'b':  PUTC(c, '\b'); break;
                    case 'f':  PUTC(c, '\f'); break;
                    case 'n':  PUTC(c, '\n'); break;
                    case 'r':  PUTC(c, '\r'); break;
                    case 't':  PUTC(c, '\t'); break;
                    case 'u': {
                        unsigned u;
                        if (!(p = json_parse_hex4(p, &u))) {
                            STRING_ERROR(JSON_PARSE_INVALID_UNICODE_HEX);
                        }                       
                        if (u >= 0xd800 && u <= 0xdbff) {
                            if (*p ++ != '\\' || *p ++ != 'u') {
                                STRING_ERROR(JSON_PARSE_INVALID_UNICODE_SURROGATE);
                            }
                            unsigned lowu;
                            if (!(p = json_parse_hex4(p, &lowu))) {                              
                                STRING_ERROR(JSON_PARSE_INVALID_UNICODE_HEX);
                            }
                            if (!(lowu >= 0xdc00 && lowu <= 0xdfff)) {                                
                                STRING_ERROR(JSON_PARSE_INVALID_UNICODE_SURROGATE);
                            }
                            u = 0x10000 + (u - 0xd800) * 0x400 + (lowu - 0xdc00);
                        }
                        json_encode_utf8(c, u);
                        break;
                    }
                    default: STRING_ERROR(JSON_PARSE_INVALID_STRING_ESCAPE);
                }
                break;
            }
            default: { 
                if ((unsigned char)ch < 0x20) {
                    STRING_ERROR(JSON_PARSE_INVALID_STRING_CHAR);
                }
                PUTC(c, ch);
            }
        }
    }
}
json::jsonError json::parseString(parseContext& context) {
    char* s;
    size_t len;
    int ret = json_parse_string_raw(c, &s, &len);
    if (ret == JSON_PARSE_OK) {
        json_set_string(v, s, len);
    }
    return ret;
}
json::jsonError json::parseArray(parseContext& context) {
    EXPECT(c, '[');
    json_parse_whitespace(c);
    if (*c->json == ']') {
        c->json ++;
        json_set_array(v, 0);
        return JSON_PARSE_OK;
    }
    int ret;
    size_t size = 0;
    for (;;) {
        json_value e;
        json_init(&e);
        ret = json_parse_value(c, &e);
        if (ret != JSON_PARSE_OK) {
            break;
        }
        memcpy(json_context_push(c, sizeof(json_value)), &e, sizeof(json_value));
        size ++;
        json_parse_whitespace(c);
        if (*c->json == ',') {
            c->json ++;
            json_parse_whitespace(c);
        } else if (*c->json == ']') {
            c->json ++;
            json_set_array(v, size);
            memcpy(v->u.a.e, json_context_pop(c, size * sizeof(json_value)), size * sizeof(json_value));
            v->u.a.size = size;
            return JSON_PARSE_OK;
        } else {
            ret = JSON_PARSE_MISS_COMMA_OR_SQUARE_BRACKET;
            break;
        }
    }
    for (int i = 0; i < size; i ++) {
        json_free((json_value*)json_context_pop(c, sizeof(json_value)));
    }
    return ret;
}
json::jsonError json::parseObject(parseContext& context) {
    EXPECT(c, '{');
    json_parse_whitespace(c);
    if (*c->json == '}') {
        c->json ++;
        json_set_object(v, 0);
        return JSON_PARSE_OK;
    }
    int ret;
    size_t size = 0;
    json_member m;
    m.k = NULL;
    for (;;) {
        json_init(&m.v);
        char* str;
        if (*c->json != '\"') {
            ret = JSON_PARSE_MISS_KEY;
            break;
        }
        ret = json_parse_string_raw(c, &str, &m.klen);
        if (ret != JSON_PARSE_OK) {
            break;
        }
        memcpy(m.k = (char*)malloc(m.klen + 1), str, m.klen);
        m.k[m.klen] = '\0';
        json_parse_whitespace(c);
        if (*c->json != ':') {
            ret = JSON_PARSE_MISS_COLON;
            break;
        }
        c->json ++;
        json_parse_whitespace(c);
        ret = json_parse_value(c, &m.v);
        if (ret != JSON_PARSE_OK) {
            break;
        }
        size ++;
        memcpy(json_context_push(c, sizeof(json_member)), &m, sizeof(json_member));
        m.k = NULL;

        json_parse_whitespace(c);
        if (*c->json == ',') {
            c->json ++;
            json_parse_whitespace(c);
        } else if (*c->json == '}') {
            c->json ++;
            json_set_object(v, size);
            memcpy(v->u.o.m, json_context_pop(c, size * sizeof(json_member)), size * sizeof(json_member));
            v->u.o.size = size;
            return JSON_PARSE_OK;
        } else {
            ret = JSON_PARSE_MISS_COMMA_OR_CURLY_BRACKET;
            break;
        }
    }
    free(m.k);
    for (int i = 0; i < size; i ++) {
        json_member* m = (json_member*)json_context_pop(c, sizeof(json_member));
        free(m->k);
        json_free(&m->v);
    }
    v->type = JSON_NULL;
    return ret;
}
json::jsonError json::parseValue(parseContext& context) {
    switch (*c->json) {
        case 'n': return json_parse_literal(c, v, "null", JSON_NULL);
        case 't': return json_parse_literal(c, v, "true", JSON_TRUE);
        case 'f': return json_parse_literal(c, v, "false", JSON_FALSE);
        case '\0': return JSON_PARSE_EXPECT_VALUE;
        case '\"': return json_parse_string(c, v);
        case '[': return json_parse_array(c, v);
        case '{': return json_parse_object(c, v);
        default: return json_parse_number(c, v);
    }
}
json::jsonError json::parse(std::string& jsonString) {
    assert(v != NULL);
    json_context c;
    c.json = json;
    c.stack = NULL;
    c.size = c.top = 0;
    json_init(v);
    json_parse_whitespace(&c);
    int ret = json_parse_value(&c, v);
    if (ret == JSON_PARSE_OK) {
        json_parse_whitespace(&c);
        if (*c.json != '\0') {
            v->type = JSON_NULL;
            return JSON_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    assert(c.top == 0);
    free(c.stack);
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






// #include <iostream>

// // test
// int main() {

//     json j;

//     json jtrue;
//     jtrue.setBoolean(true);
    
//     json jfalse;
//     jfalse.setBoolean(false);

//     json jstr;
//     jstr.setString("testString");

//     json jnum;
//     jnum.setNumber(12.3424446453766);

//     json jarr;
//     jarr.setArray();
//     jarr.pushbackArray(jtrue);
//     jarr.pushbackArray(jfalse);
    
//     json jobj;
//     jobj.setObject();

//     j.setObject();
//     j.insertObjectElement("jtrue", jtrue);
//     j.insertObjectElement("jfalse", jfalse);
//     j.insertObjectElement("jstr", jstr);
//     j.insertObjectElement("jarr", jarr);
//     j.insertObjectElement("jnum", jnum);
//     j.insertObjectElement("jobj", jobj);



//     std::cout << j.dump() << std::endl;

//     return 0;
// }