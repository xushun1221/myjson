/*
*  @Filename : example_4_parse.cc
*  @Description : example for vector and map
*  @Datatime : 2022/11/24 19:07:43
*  @Author : xushun
*/
#include "../json.hh"
#include <iostream>

using json = xushun::json;

void func_3() {
    json js;
    std::vector<double> vec{1,2,3};
    js["list"] = vec;
    std::map<std::string,std::string> mp{{"1", "Alice"}, {"2", "Bob"}};
    js["user"] = mp;
    std::string recvBuf = js.dump();

    json js2;
    js2.parse(recvBuf);

    for (int i = 0; i < js2["list"].getArraySize(); ++ i) {
        std::cout << js2["list"][i].getNumber() << " ";
    } std::cout << std::endl;
    std::cout << js2["user"]["1"].getString() << std::endl;
    std::cout << js2["user"]["2"].getString() << std::endl;

    std::cout << js2.dump() << std::endl;
}


int main(int argc, char** argv) {

    func_3();

    return 0;
}