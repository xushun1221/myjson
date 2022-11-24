/*
*  @Filename : example_2_dump.cc
*  @Description : example for vector and map
*  @Datatime : 2022/11/24 18:42:20
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
    std::cout << js.dump() << std::endl;
}


int main(int argc, char** argv) {

    func_3();

    return 0;
}