#include <foo.h>
#include <iostream>

using namespace std::chrono;

foo::foo()
{
	name = "myName";
	age = 7;
}

// getters

int foo::getAge() {
	return age;
}

std::string foo::getName() {
	struct timeval tp;
	long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
	std::cout << "timestamp: " << ms << std::endl;
	return name;
}

// setters

void foo::setName(std::string a) {
	name = a;
}

void foo::setAge(int a) {
	age = a;
}

// misc.

void saysometing(void) {
	std::cout << "woof." << std::endl;
}
