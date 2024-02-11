#include <iostream>
#include "jjyou/io/Json.hpp"
using Json = jjyou::io::Json<int, float, std::string, bool>;
int main() {
	Json json1;
	std::cout << json1.type() << std::endl;
	std::cout << json1.size() << std::endl;
	Json json2(1);
	std::cout << json2.type() << std::endl;
	std::cout << json2.size() << std::endl;
	Json json3(1.0f);
	std::cout << json3.type() << std::endl;
	std::cout << json3.size() << std::endl;
	Json json4(false);
	std::cout << json4.type() << std::endl;
	std::cout << json4.size() << std::endl;
	Json json5({ json1, json2, json3, json4 });
	std::cout << json5.type() << std::endl;
	std::cout << json5.size() << std::endl;
	Json json6({
		{"1",json1},
		{"2",json2},
		{"3",json3},
		{"4",json4},
		{"5",json5}
	});
	std::cout << json6.type() << std::endl;
	std::cout << json6.size() << std::endl;
	Json json7(std::move(json6));
	std::cout << json6.type() << std::endl;
	std::cout << json6.size() << std::endl;
	std::cout << json7.type() << std::endl;
	std::cout << json7.size() << std::endl;
	std::cout << "=================" << std::endl;
}