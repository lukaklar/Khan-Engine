#pragma once

#include <string>
#include <sstream>
#include <utility>

namespace Khan
{
	// String types
	using String = std::string;
	using WideString = std::wstring;
	using UTF16String = std::u16string;
	using UTF32String = std::u32string;

	// String streams
	using StringStream = std::stringstream;
	using WideStringStream = std::wstringstream;

	// Numeric to string conversions
	[[nodiscard]] inline String ToString(int value) { return std::to_string(value); }
	[[nodiscard]] inline String ToString(long value) { return std::to_string(value); }
	[[nodiscard]] inline String ToString(long long value) { return std::to_string(value); }
	[[nodiscard]] inline String ToString(unsigned int value) { return std::to_string(value); }
	[[nodiscard]] inline String ToString(unsigned long value) { return std::to_string(value); }
	[[nodiscard]] inline String ToString(unsigned long long value) { return std::to_string(value); }
	[[nodiscard]] inline String ToString(float value) { return std::to_string(value); }
	[[nodiscard]] inline String ToString(double value) { return std::to_string(value); }
	[[nodiscard]] inline String ToString(long double value) { return std::to_string(value); }

	// String to numeric conversions
	[[nodiscard]] inline int ToInt(const String& value, std::size_t* position = nullptr, int base = 10) { return std::stoi(value, position, base); }
	[[nodiscard]] inline long ToLong(const String& value, std::size_t* position = nullptr, int base = 10) { return std::stol(value, position, base); }
	[[nodiscard]] inline long long ToLongLong(const String& value, std::size_t* position = nullptr, int base = 10) { return std::stoll(value, position, base); }
	[[nodiscard]] inline unsigned long ToUnsignedLong(const String& value, std::size_t* position = nullptr, int base = 10) { return std::stoul(value, position, base); }
	[[nodiscard]] inline unsigned long long ToUnsignedLongLong(const String& value, std::size_t* position = nullptr, int base = 10) { return std::stoull(value, position, base); }
	[[nodiscard]] inline float ToFloat(const String& value, std::size_t* position = nullptr) { return std::stof(value, position); }
	[[nodiscard]] inline double ToDouble(const String& value, std::size_t* position = nullptr) { return std::stod(value, position); }
	[[nodiscard]] inline long double ToLongDouble(const String& value, std::size_t* position = nullptr) { return std::stold(value, position); }

	// Line input
	//inline String& GetLine(std::istream& stream, String& output) { return std::getline(stream, output); }
	//inline String& GetLine(std::istream& stream, String& output, char delimiter) { return std::getline(stream, output, delimiter); }
	//inline WideString& GetLine(std::wistream& stream, WideString& output) { return std::getline(stream, output); }
	//inline WideString& GetLine(std::wistream& stream, WideString& output, wchar_t delimiter) { return std::getline(stream, output, delimiter); }
}