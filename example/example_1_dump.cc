/*
*  @Filename : example_1_dump.cc
*  @Description : example for simple dump
*  @Datatime : 2022/11/24 18:12:12
*  @Author : xushun
*/
#include "../json.hh"
#include <iostream>

using json = xushun::json;

void func_1() {
    json js;
    js["msg_type"] = 2.0;
    js["from"] = "Alice";
    js["to"] = "Bob";
    js["msg"] = "hello, XD";
    std::cout << js.dump() << std::endl;
}

void func_2() {
    json js;
    std::vector<double> id{1,2,3,4,5};
    js["id"] = id;
    js["name"] = "Alice";
    js["msg"]["Alice"] = "hello";
    js["msg"]["Bob"] = "hi";
    std::cout << js.dump() << std::endl;
}


int main(int argc, char** argv) {

    func_1();
    func_2();

    return 0;
}