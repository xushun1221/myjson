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