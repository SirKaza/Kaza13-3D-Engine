#pragma once
#include <stddef.h>

class String
{
public:
	String(); // Empty constructor
	String(const char* input); // Constructor with 1 arg

	String(const String& copy); // Copy constructor
	String(String&& move) noexcept; // Move Constructor

	String operator+(const String& sum);
	String& operator=(const String& other);
	bool operator==(const char* other);

	// inline
	~String() { delete[] str;  } // Destructor
	const char* c_str() const { return str;  } // returns const
	char* getChar() const { return str; } 
	size_t length() const { return size; }

	size_t length(const char* other) const;
	void clear();
	void stringCopy_s(char* first, size_t length, const char* second);
	void stringCat_s(char* first, size_t length, const char* second);

private:
	char* str;
	size_t size;
};

