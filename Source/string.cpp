#include "string.h"

String::String() : size(0)
{
	str = new char[1];
	str[0] = '\0';
}

String::String(const char* input)
{
	if (input == nullptr) {
		str = new char[1];
		str[0] = '\0';
		size = 0;
	}
	else {
		size = length(input);
		str = new char[size + 1];

		stringCopy_s(str, size, input); // '\0' included
	}
}

String::String(const String& copy)
{
	size = copy.size;
	str = new char[size + 1];

	stringCopy_s(str, size, copy.str);
}

String::String(String&& move) noexcept
{
	str = move.str;
	size = move.size;
	move.str = nullptr;
	move.size = 0;
}

String String::operator+(const String& other)
{
	size_t totalL = size + other.size;
	
	char* buff = new char[totalL + 1];

	stringCopy_s(buff, size, str);
	stringCat_s(buff, totalL, other.str);

	String temp(buff);
	temp.size = totalL;
	delete[] buff;
	return temp;
}

String& String::operator=(const String& other)
{
	if (this == &other)
		return *this; // this as object

	delete[] str;

	size = other.size;
	str = new char[size + 1];
	stringCopy_s(str, size, other.str);

	return *this;
}

bool String::operator==(const char* other)
{
	if (other == nullptr || length(other) != size)
		return false;

	size_t i = 0;
	while (str[i] != '\0' && other[i] != '\0') {
		if (str[i] != other[i])
			return false;
		i++;
	}
	return true;
}

size_t String::length(const char* other) const {
	size_t i = 0;
	while (other[i] != '\0') {
		i++;
	}
	return i;
}

void String::clear()
{
	delete[] str;
	str = new char[1];
	str[0] = '\0';
	size = 0;
}

void String::stringCopy_s(char* first, size_t length, const char* second)
{
	for (size_t i = 0; i < length; i++)
	{
		first[i] = second[i];
	}
	first[length] = '\0';
}

void String::stringCat_s(char* first, size_t length, const char* second)
{
	size_t cont = 0;
	bool found = false;
	for (size_t i = 0; i < length; i++)
	{
		if (found) {
			first[i] = second[cont];
			cont++;
			continue;
		}
			
		if (first[i] == '\0' && !found) {
			first[i] = second[cont];
			cont++;
			found = true;
		}
	}
	first[length] = '\0';
}