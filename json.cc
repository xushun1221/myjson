/*
*  @Filename : json.cc
*  @Description : implementation of class json
*  @Datatime : 2022/11/20 15:49:16
*  @Author : xushun
*/

#include "json.hh"




using json = xushun::json;




json::json() {
    type_ = JSON_NULL;
}

json::json(const json& src) {
    key_ = src.key_;
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
    key_ = src.key_;
    type_ = src.type_;
    object_ = src.object_;
    array_ = src.array_;
    string_ = src.string_;
    number_ = src.number_;
    return *this;
}


// to do
void json::parse(std::string jsonString) {

}


void json::contextPush(std::string& context, std::string&& s) {
    context += s;
}
void json::contextPop(std::string& context, int n) {
    while (n --) { context.pop_back(); }
}
void json::dumpString(std::string& context, std::string& s) {
    const char hexDigits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    context += '\"';
    for (unsigned char ch : s) {
        switch (ch) {
            case '\"': context += "\\\"";   break;
            case '\\': context += "\\\\";   break;
            case '\b': context += "\\b";    break;
            case '\f': context += "\\f";    break;
            case '\n': context += "\\n";    break;
            case '\r': context += "\\r";    break;
            case '\t': context += "\\t";    break;
            default:
                if (ch < 0x20) {
                    context += "\\u00";
                    context += hexDigits[ch >> 4];
                    context += hexDigits[ch & 15];
                } else {
                    context += ch;
                }
        }
    }
    context += '\"';
}
void json::dumpValue(std::string& context) {
    switch (type_) {
        case JSON_NULL:   contextPush(context, "null");  break;
        case JSON_TRUE:   contextPush(context, "true");  break;
        case JSON_FALSE:  contextPush(context, "false"); break;
        case JSON_NUMBER:
            char num[32];
            sprintf(num, "%.17g", number_);
            contextPush(context, std::string(num));      break;
        case JSON_STRING: dumpString(context, string_);  break;
        case JSON_ARRAY:
            contextPush(context, "[");
            for (int i = 0; i < array_.size(); ++ i) {
                if (i > 0) { contextPush(context, ","); }
                array_[i].dumpValue(context);
            }
            contextPush(context, "]");                   break;
        case JSON_OBJECT:
            contextPush(context, "{");
            for (auto itr = object_.begin(); itr != object_.end(); ++ itr) {
                if (itr != object_.begin()) { contextPush(context, ","); }
                dumpString(context, itr->second.key_);
                contextPush(context, ":");
                itr->second.dumpValue(context);
            }
            contextPush(context, "}");                   break;
        default: 
            break;
    }
}
std::string json::dump() {
    std::string context;
    dumpValue(context);
    return context;
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
    key_.clear();
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

    json jarr;
    jarr.pushbackArray(jtrue);
    jarr.pushbackArray(jfalse);
    
    j.setObject();
    j.insertObjectElement("jtrue", jtrue);
    j.insertObjectElement("jfalse", jfalse);
    j.insertObjectElement("jstr", jstr);
    j.insertObjectElement("jarr", jarr);





    std::cout << j.dump() << std::endl;

    return 0;
}