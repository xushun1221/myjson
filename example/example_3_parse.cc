/*
*  @Filename : example_3_parse.cc
*  @Description : example for simple parse
*  @Datatime : 2022/11/24 18:56:44
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
    std::string recvBuf = js.dump();

    json js2;
    js2.parse(recvBuf);

    std::cout << js2["msg_type"].getNumber() << std::endl;
    // std::cout << (double)js2["msg_type"] << std::endl; // same
    std::cout << js2["from"].getString() << std::endl;
    // std::cout << (std::string)js2["from"] << std::endl; // same
    std::cout << js2["to"].getString() << std::endl;
    std::cout << js2["msg"].getString() << std::endl;

    std::cout << js2.dump() << std::endl;
}

void func_2() {
    json js;
    std::vector<double> id{1,2,3,4,5};
    js["id"] = id;
    js["name"] = "Alice";
    js["msg"]["Alice"] = "hello";
    js["msg"]["Bob"] = "hi";
    std::string recvBuf = js.dump();

    json js2;
    js2.parse(recvBuf);

    for (int i = 0; i < js2["id"].getArraySize(); ++ i) {
        std::cout << js2["id"][i].getNumber() << " ";
    } std::cout << std::endl;
    std::cout << js2["name"].getString() << std::endl;
    std::cout << js2["msg"]["Alice"].getString() << std::endl;
    std::cout << js2["msg"]["Bob"].getString() << std::endl;

    std::cout << js2.dump() << std::endl;
}


int main(int argc, char** argv) {

    func_1();
    func_2();

    return 0;
}