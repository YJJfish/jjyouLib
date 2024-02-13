#include <jjyou/io/Json.hpp>
using Json = jjyou::io::Json<int, float, std::string, bool>;
using JsonIterator = Json::iterator;
using JsonConstIterator = Json::const_iterator;

void exampleConstruct(void) {
	
	// Converting constructors `Json::Json(...)` do not have `explicit` specifier, so they can be called implicitly.
	
	Json json1;
	std::cout << json1.type() << std::endl;		// Null
	std::cout << json1.size() << std::endl;		// 0
	
	Json json2(1);								// Or `Json json2 = 1;`.
	std::cout << json2.type() << std::endl;		// Integer
	std::cout << json2.size() << std::endl;		// 1
	
	Json json3 = 1.0f;							// Or `Json json3 = 1.0f;`.
	std::cout << json3.type() << std::endl;		// Floating
	std::cout << json3.size() << std::endl;		// 1
	
	Json json4(false);							// Or `Json json4 = false;`.
	std::cout << json4.type() << std::endl;		// Bool
	std::cout << json4.size() << std::endl;		// 1
	
	Json json5("JSON");							// Or `Json json5 = "JSON";`.
	std::cout << json5.type() << std::endl;		// String
	std::cout << json5.size() << std::endl;		// 1
	
	Json json6({
		json1,
		json2,
		json3,
		json4,
		json5
	});											// Or `Json json6 = { ... };`.
	std::cout << json6.type() << std::endl;		// Array
	std::cout << json6.size() << std::endl;		// 5
	
	Json json7({
		{"1", json1},
		{"2", json2},
		{"3", json3},
		{"4", json4},
		{"5", json5},
		{"6", json6}
	});											// Or `Json json7 = { std::make_pair("1", json1), ... };`. Note that you cannot omit "std::make_pair".
	std::cout << json7.type() << std::endl;		// Object
	std::cout << json7.size() << std::endl;		// 6
	
	Json json8(json7);
	std::cout << json8.type() << std::endl;		// Object
	std::cout << json8.size() << std::endl;		// 6

	Json json9(std::move(json7));
	std::cout << json7.type() << std::endl;		// Null
	std::cout << json7.size() << std::endl;		// 0
	std::cout << json9.type() << std::endl;		// Object
	std::cout << json9.size() << std::endl;		// 6
	
}

void exampleIterate(void) {
	
	// The size of a null json is always 0, which means the loop is never executed.
	
	Json null;
	for (const Json& v : null)
		std::cout << v << " ";					///No output.
	
	// The size of a(n) integer/floating/string/bool json is always 1, which means the loop is executed exactly once.
	
	Json json1(123);
	for (const Json& v : json1)
		std::cout << v << " ";
	std::cout << std::endl;						// 123
	
	Json json2(102.4f);
	for (JsonIterator iter = json2.begin(); iter != json2.end(); ++iter)
		std::cout << *iter << " ";
	std::cout << std::endl;						// 102.4
	
	// The size of an array/object json equals to the number of elements/key-value pairs in the container.
	
	Json json3({1, 2, 3, 4, 5, 6});
	for (JsonConstIterator cIter = json3.cbegin(); cIter != json3.cend(); ++cIter)
		std::cout << *cIter << " ";
	std::cout << std::endl;						// 1 2 3 4 5 6
	
	Json json4({
		{"Key1", Json(true)},
		{"Key2", Json(2)},
		{"Key3", Json(3.5f)},
		{"Key4", Json("This is a string.")},
	});
	for (const Json& v : json4)
		std::cout << v << " ";
	std::cout << std::endl;						// true 2 3.5 "This is a string."
	
	// If the json is an object, you may also use `key()` method to get the keys.
	
	for (JsonConstIterator cIter = json4.cbegin(); cIter != json4.cend(); ++cIter)
		std::cout << "{ \"" << cIter.key() << "\", " << cIter.value() << " } ";
	std::cout << std::endl;						// { "Key1", true } { "Key2", 2 } { "Key3", 3.5 } { "Key4", "This is a string." }
	
}

void exampleConvert1(void) {
	
	// Converting operators `Json::operator T(void) const` have `explicit` specifier. They must be called explicitly.
	
	Json json = Json::parse(R"(
[
	2,
	3.5,
	"This is a string.",
	true
]
	)");
	
	int integer1(json[0]);						///OK.
	//int integer1 = json[0];					///Compiling error. Must explicitly call int(json[0]) or static_cast<int>(json[0]).
	std::cout << integer1 << std::endl;			// 2
	
	int integer2(json[1]);						///OK.
	//int integer2 = json[1];					///Compiling error. Must explicitly call int(json[1]) or static_cast<int>(json[1]).
	std::cout << integer2 << std::endl;			// 3
	
	int integer3(json[3]);						///OK.
	//int integer3 = json[3];					///Compiling error. Must explicitly call int(json[3]) or static_cast<int>(json[3]).
	std::cout << integer3 << std::endl;			// 1
	
	//int integer4(json[2]);					///Runtime error. Only integer, floating, and bool can be converted to each other.
	
	float floating1(json[0]);					///OK.
	//float floating1 = json[0];				///Compiling error. Must explicitly call float(json[0]) or static_cast<float>(json[0]).
	std::cout << floating1 << std::endl;		// 2
	
	float floating2(json[1]);					///OK.
	//float floating2 = json[1];				///Compiling error. Must explicitly call float(json[1]) or static_cast<float>(json[1]).
	std::cout << floating2 << std::endl;		// 3.5
	
	float floating3(json[3]);					///OK.
	//float floating3 = json[3];				///Compiling error. Must explicitly call float(json[3]) or static_cast<float>(json[3]).
	std::cout << floating3 << std::endl;		// 1
	
	//float floating4(json[2]);					///Runtime error. Only integer, floating, and bool can be converted to each other.
	
	//std::string str(json[0]);					///Runtime error.
	//std::string str(json[1]);					///Runtime error.
	//std::string str(json[3]);					///Runtime error.
	std::string str(json[2]);					///OK.
	std::cout << str << std::endl;				// This is a string.
	std::cout << json[2] << std::endl;			// "This is a string."
	
}

void exampleConvert2(void) {
	
	// Json can be converted to `std::vector<T>`, if the Json's type is an array, and all the elements in the array (whose type are also Json) can be converted to the template type `T`.
	
	Json json1 = Json::parse(R"(
[
	2,
	3.5,
	false
]
	)");
	
	std::vector<int> vec1(json1);
	std::cout << vec1.size() << std::endl;		// 3
	for (const auto& v : vec1)
		std::cout << v << " ";
	std::cout << std::endl;						// 2 3 0
	
	std::vector<float> vec2(json1);
	std::cout << vec2.size() << std::endl;		// 3
	for (const auto& v : vec2)
		std::cout << v << " ";
	std::cout << std::endl;						// 2 3.5 0
	
	std::vector<bool> vec3(json1);
	std::cout << vec3.size() << std::endl;		// 3
	for (const auto& v : vec3)
		std::cout << std::boolalpha << v << " ";
	std::cout << std::endl;						// true true false
	
	Json json2 = Json::parse(R"(
[
	"This",
	"is",
	"an",
	"array",
	"."
]
	)");
	std::vector<std::string> vec4(json2);
	std::cout << vec4.size() << std::endl;		// 5
	for (const auto& v : vec4)
		std::cout << v << " ";
	std::cout << std::endl;						// This is an array .
	
}

void exampleConvert3(void) {
	
	// Json can be converted to `std::map<T>`, if the Json's type is an object, and all the values in the object (whose type are also Json) can be converted to the template type `T`.
	
	Json json1 = Json::parse(R"(
{
	"Key1" : 2,
	"Key2" : 3.5,
	"Key3" : false
}
	)");
	
	std::map<std::string, int> map1(json1);
	std::cout << map1.size() << std::endl;		// 3
	for (const auto& p : map1)
		std::cout << "{ \"" << p.first << "\", " << p.second << " } ";
	std::cout << std::endl;						// { "Key1", 2 } { "Key2", 3 } { "Key3", 0 }
	
	// A more complex example.
	
	Json json2 = Json::parse(R"(
{
	"Odd"				: [1, 3, 5, 7, 9],
	"Even"				: [2, 4, 6, 8, 10],
	"Perfect Square"	: [1, 4, 9, 16],
	"Prime"				: [2, 3, 5, 7, 11, 13],
	"Fibonacci"			: [0, 1, 1, 2, 3, 5, 8]
}
	)");
	
	std::map<std::string, std::vector<int>> map2(json2);
	std::cout << map2.size() << std::endl;		// 5
	for (const auto& p : map2) {
		std::cout << "\"" << p.first << "\"" << std::endl;
												///This prints the keys, i.e. "Even", "Fibonacci", "Odd", "Perfect Square", and "Prime".
		for (const auto& v : p.second)
			std::cout << v << " ";				///This prints the elements in the arrays.
		std::cout << std::endl;
	}
	 
}

void exampleWrite(void) {
	
	Json json = Json::parse(R"(
{
	"Key1" : 2,
	"Key2" : 3.5,
	"Key3" : false
}
	)");
	
	std::cout << json << std::endl;
	//std::cout << to_string(json) << std::endl;	///They are equivalent.
	
	//std::ofstream fout("./example.json", std::ios::out);	///Writing to a file.
	//fout << json << std::endl;
	//fout.close();
	
}

int main() {
	std::cout << "=========== exampleConstruct ===========" << std::endl;
	exampleConstruct();
	std::cout << std::endl;
	std::cout << "=========== exampleIterate ===========" << std::endl;
	exampleIterate();
	std::cout << std::endl;
	std::cout << "=========== exampleConvert1 ===========" << std::endl;
	exampleConvert1();
	std::cout << std::endl;
	std::cout << "=========== exampleConvert2 ===========" << std::endl;
	exampleConvert2();
	std::cout << std::endl;
	std::cout << "=========== exampleConvert3 ===========" << std::endl;
	exampleConvert3();
	std::cout << std::endl;
	std::cout << "=========== exampleWrite ===========" << std::endl;
	exampleWrite();
	std::cout << std::endl;
}