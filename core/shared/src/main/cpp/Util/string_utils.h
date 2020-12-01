/**
 * Copyright (C) 2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 */

#include <set>
#include <string>
#include <vector>

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

namespace string_utils {

// ============================= CONCATENATE =================================
extern std::string
StrCat(const std::string & str1, const std::string & str2);
extern std::string StrCat(
    const std::string & str1,
    const std::string & str2,
    const std::string & str3);
extern std::string StrCat(
    const std::string & str1,
    const std::string & str2,
    const std::string & str3,
    const std::string & str4);
extern std::string StrCat(
    const std::string & str1,
    const std::string & str2,
    const std::string & str3,
    const std::string & str4,
    const std::string & str5);
extern std::string StrCat(
    const std::string & str1,
    const std::string & str2,
    const std::string & str3,
    const std::string & str4,
    const std::string & str5,
    const std::string & str6);
// ============================= END CONCATENATE ===============================

// ================================ NUMERIC ====================================

// ========================== NUMERIC VALUE -> STRING ==========================
// Converts an (int, short, long, double, float) to a string.
extern std::string Itoa(const int i);
extern std::string Itoa(const unsigned int i);
extern std::string Itoa(const bool b);
// NOTE: We cannot have the following two, as [u]int64_t are actually not
// concrete types, whose underlying types (long or long long) depend on
// the compilier. Thus, we cannot support both API for input 'long' (or
// 'long long' for that matter) and int64_t, because for a compiler that
// treats int64_t as long (resp. long long), it will have ambiguous
// overload error. However, we don't need to explicitly have an API
// for [u]int64_t, since the compiler will either cast this as
// [unsigned] long or [unsigned] long long, and use those API's appropriately.
//extern std::string Itoa(const int64_t i);
//extern std::string Itoa(const uint64_t i);
extern std::string Itoa(const short i);
extern std::string Itoa(const unsigned short i);
extern std::string Itoa(const long & l);
extern std::string Itoa(const unsigned long & l);
extern std::string Itoa(const long long & l);
extern std::string Itoa(const unsigned long long & l);
extern std::string Itoa(const double & d, const int precision);
// Same as above, with default precision set to 6 digits.
extern std::string Itoa(const double & d);
extern std::string Itoa(const float & f, const int precision);
// Same as above, with default precision set to 6 digits.
extern std::string Itoa(const float & f);

// ========================== STRING -> NUMERIC VALUE ==========================
// Checks whether the input can be viewed as a number. Returns true for:
//   - (Signed) Integers: i.e. a string of digits 0-9, possibly with leading
//     plus/minus sign ("-", "+")
//   - Decimals: Same as above, with a single "." somewhere in the string
//   - Scientific Notation
//   - Values in Binary (string of 0's and 1's) or Hex (prefix "0x")
//   - Values enclosed in parentheses (even multiple) will be accepted
extern bool IsNumeric(const std::string & input);
// Checks whether the input string represents a numeric value in Scientific
// notation, and if so, returns true and populates value with the value.
extern bool
IsScientificNotation(const std::string & input, double * value);
// Same as above, but demands presence of "+" or "-" after the "e" or "E".
extern bool
IsScientificNotationWithSign(const std::string & input, double * value);
// Checks if all characters are a digit 0-9.
extern bool IsAllDigits(const std::string & input);
// Replaces any instances of Scientific Notation with the numeric equivalent, e.g.
//   5.1e-05 -> 5.1 * 0.00001
extern std::string RemoveScientificNotation(const std::string & input);

// Coverts the input string 'str' to a numeric value (of the appropriate type).
// Returns true if the conversion was successful, false otherwise.
// This function only necessary because C++ has no good way to convert a
// string to an int, and check that the conversion doesn't have errors.
// Note that std::stoi() does this, but is not an option for me, since
// this was introduced in C++11, and this particular function is not
// compatible using MinGW on Windows.
// NOTE: Hex is handled. For binary or 2's complement strings, use
// BinaryStringToValue() or TwosComplementStringToValue() below.
// WARNING: The below functions happily return true, even when input string
// is not actually a numeric value. If you need to demand that the input string
// is actually a numeric value (and in particular, that the returned value is
// actually what the string represents), first call IsNumeric() above.
extern bool Stoi(const std::string & str, bool * i);
extern bool Stoi(const std::string & str, char * i);
extern bool Stoi(const std::string & str, unsigned char * i);
extern bool Stoi(const std::string & str, short * i);
extern bool Stoi(const std::string & str, unsigned short * i);
extern bool Stoi(const std::string & str, int * i);
extern bool Stoi(const std::string & str, unsigned int * i);
//extern bool Stoi(const std::string& str, int64_t* i);
//extern bool Stoi(const std::string& str, uint64_t* i);
extern bool Stoi(const std::string & str, long * i);
extern bool Stoi(const std::string & str, unsigned long * i);
extern bool Stoi(const std::string & str, long long * i);
extern bool Stoi(const std::string & str, unsigned long long * i);
// Same as above, with input a vector<char> instead of string.
inline bool Stoi(const std::vector<char> input, bool * i) {
  return Stoi(std::string(input.begin(), input.end()), i);
}
inline bool Stoi(const std::vector<char> input, char * i) {
  return Stoi(std::string(input.begin(), input.end()), i);
}
inline bool Stoi(const std::vector<char> input, unsigned char * i) {
  return Stoi(std::string(input.begin(), input.end()), i);
}
inline bool Stoi(const std::vector<char> input, short * i) {
  return Stoi(std::string(input.begin(), input.end()), i);
}
inline bool Stoi(const std::vector<char> input, unsigned short * i) {
  return Stoi(std::string(input.begin(), input.end()), i);
}
inline bool Stoi(const std::vector<char> input, int * i) {
  return Stoi(std::string(input.begin(), input.end()), i);
}
inline bool Stoi(const std::vector<char> input, unsigned int * i) {
  return Stoi(std::string(input.begin(), input.end()), i);
}
inline bool Stoi(const std::vector<char> input, long * i) {
  return Stoi(std::string(input.begin(), input.end()), i);
}
inline bool Stoi(const std::vector<char> input, unsigned long * i) {
  return Stoi(std::string(input.begin(), input.end()), i);
}
inline bool Stoi(const std::vector<char> input, long long * i) {
  return Stoi(std::string(input.begin(), input.end()), i);
}
inline bool
Stoi(const std::vector<char> input, unsigned long long * i) {
  return Stoi(std::string(input.begin(), input.end()), i);
}
// Same as above, where input is a char array instead of a string.
// NOTE: It is assumed 'input' has a terminating NULL ('\0') char,
// as otherwise the underlying strtoXXX function may parse extra
// garbage bytes after the intended bytes.
extern bool
Stoi(const char * input, const unsigned int num_bytes, bool * i);
extern bool
Stoi(const char * input, const unsigned int num_bytes, char * i);
extern bool Stoi(
    const char * input,
    const unsigned int num_bytes,
    unsigned char * i);
extern bool
Stoi(const char * input, const unsigned int num_bytes, short * i);
extern bool Stoi(
    const char * input,
    const unsigned int num_bytes,
    unsigned short * i);
extern bool
Stoi(const char * input, const unsigned int num_bytes, int * i);
extern bool Stoi(
    const char * input, const unsigned int num_bytes, unsigned int * i);
extern bool
Stoi(const char * input, const unsigned int num_bytes, long * i);
extern bool Stoi(
    const char * input,
    const unsigned int num_bytes,
    unsigned long * i);
extern bool
Stoi(const char * input, const unsigned int num_bytes, long long * i);
extern bool Stoi(
    const char * input,
    const unsigned int num_bytes,
    unsigned long long * i);
// Coverts the input string 'str' to a double value, storing the result in 'd'.
// Returns true if the conversion was successful, false otherwise.
// NOTE: Scientific notation is handled.
extern bool Stod(const std::string & str, double * d);
extern bool Stod(const std::string & str, long double * d);
extern bool Stof(const std::string & str, float * f);

// Similar to above, but input string should be binary representation of an int.
// NOTE: For Value -> BinaryString, use API ToBinaryString() in
// MathUtils/number_conversion_utils.h
extern bool
BinaryStringToValue(const std::string & str, unsigned char * i);
extern bool
BinaryStringToValue(const std::string & str, unsigned short * i);
extern bool
BinaryStringToValue(const std::string & str, unsigned int * i);
extern bool
BinaryStringToValue(const std::string & str, unsigned long * i);
extern bool
BinaryStringToValue(const std::string & str, unsigned long long * i);
// Same as above, but cast as a signed int: Interpret the binary string as
// the 2's-complement representation; i.e. leading bit is -2^n, and the rest
// of the bits are the usual 2^i.
extern bool
TwosComplementStringToValue(const std::string & str, char * i);
extern bool
TwosComplementStringToValue(const std::string & str, short * i);
extern bool
TwosComplementStringToValue(const std::string & str, int * i);
extern bool
TwosComplementStringToValue(const std::string & str, long * i);
extern bool
TwosComplementStringToValue(const std::string & str, long long * i);

// Binary String <-> vector<char> (appends to output, if output is non-empty).
// NOTES:
//   1) This function converts to a char vector in an ENDIANESS-independent way;
//      in particular, the first byte of output will be the leading (high-order)
//      bits of the binary string.
//   2) Since we're converting bits to bytes (unsigned char), if the length
//      of the input string is not a factor of 8 (CHAR_BIT), then we pad
//      the binary string with *leading* zeros (i.e. to get an equivalent
//      representation of the same number).
//   3) It doesn't matter if the input binary string has spaces (e.g. between
//      'bytes') or not; both are handled.
extern void BinaryStringToCharVector(
    const std::string & str, std::vector<char> * output);
// Same as above, but put into unsigned char vector.
extern void BinaryStringToCharVector(
    const std::string & str, std::vector<unsigned char> * output);
// The reverse of above. Again, this is Endian-independent (see comments above).
extern std::string CharVectorToBinaryString(
    const bool print_byte_spaces,
    const std::vector<unsigned char> & input);
// Same as above, defaults 'print_byte_spaces' to true.
inline std::string
CharVectorToBinaryString(const std::vector<unsigned char> & input) {
  return CharVectorToBinaryString(true, input);
}
// Same as above, with different API (output is an input parameter).
inline void CharVectorToBinaryString(
    const bool print_byte_spaces,
    const std::vector<unsigned char> & input,
    std::string * str) {
  *str = CharVectorToBinaryString(print_byte_spaces, input);
}
// Same as above, defaults 'print_byte_spaces' to true.
inline void CharVectorToBinaryString(
    const std::vector<unsigned char> & input, std::string * str) {
  CharVectorToBinaryString(true, input, str);
}
// Same as above, with (unsigned) char* instead of vector<unsigned char>.
extern std::string CharVectorToBinaryString(
    const bool print_byte_spaces,
    const unsigned char * input,
    const int num_bytes);
// Same as above, defaults 'print_byte_spaces' to true.
inline std::string CharVectorToBinaryString(
    const unsigned char * input, const int num_bytes) {
  return CharVectorToBinaryString(true, input, num_bytes);
}
// Same as above, with different API (output is an input parameter).
inline void CharVectorToBinaryString(
    const bool print_byte_spaces,
    const unsigned char * input,
    const int num_bytes,
    std::string * str) {
  *str = CharVectorToBinaryString(print_byte_spaces, input, num_bytes);
}
// Same as above, defaults 'print_byte_spaces' to true.
inline void CharVectorToBinaryString(
    const unsigned char * input,
    const int num_bytes,
    std::string * str) {
  CharVectorToBinaryString(true, input, num_bytes, str);
}
// Same as above, for (signed) char array.
extern std::string CharVectorToBinaryString(
    const bool print_byte_spaces, const std::vector<char> & input);
// Same as above, defaults 'print_byte_spaces' to true.
inline std::string
CharVectorToBinaryString(const std::vector<char> & input) {
  return CharVectorToBinaryString(true, input);
}
// Same as above, with different API (output is an input parameter).
inline void CharVectorToBinaryString(
    const bool print_byte_spaces,
    const std::vector<char> & input,
    std::string * str) {
  *str = CharVectorToBinaryString(print_byte_spaces, input);
}
// Same as above, defaults 'print_byte_spaces' to true.
inline void CharVectorToBinaryString(
    const std::vector<char> & input, std::string * str) {
  CharVectorToBinaryString(true, input, str);
}
// Same as above, with char* instead of vector<char>.
extern std::string CharVectorToBinaryString(
    const bool print_byte_spaces,
    const char * input,
    const int num_bytes);
// Same as above, defaults 'print_byte_spaces' to true.
inline std::string
CharVectorToBinaryString(const char * input, const int num_bytes) {
  return CharVectorToBinaryString(true, input, num_bytes);
}
// Same as above, with different API (output is an input parameter).
inline void CharVectorToBinaryString(
    const bool print_byte_spaces,
    const char * input,
    const int num_bytes,
    std::string * str) {
  *str = CharVectorToBinaryString(print_byte_spaces, input, num_bytes);
}
// Same as above, defaults 'print_byte_spaces' to true.
inline void CharVectorToBinaryString(
    const char * input, const int num_bytes, std::string * str) {
  CharVectorToBinaryString(true, input, num_bytes, str);
}

// Reads a char array, and parses various numeric types.
extern double ParseDouble(const char * input, const int start_byte);
extern float ParseFloat(const char * input, const int start_byte);
extern int ParseInt(const char * input, const int start_byte);

// ============================ END NUMERIC ==================================

// ============================ JOIN and SPLIT ===============================
// Join Char Containers.
// Joins the elements of 'input' into a single string, seperated by 'delimiter'.
extern bool Join(
    const std::vector<char> & input,
    const std::string & delimiter,
    std::string * output);
// Same as above, but returns a string instead of populating an input pointer.
extern std::string
Join(const std::vector<char> & input, const std::string & delimiter);
// Same as above with default delimiter space (" ").
inline bool
Join(const std::vector<char> & input, std::string * output) {
  return Join(input, " ", output);
}
// Same as above with default delimiter space (" ").
inline std::string Join(const std::vector<char> & input) {
  return Join(input, " ");
}

// Join String Containers.
// Joins the elements of 'input' into a single string, seperated by
// 'delimiter'.
extern bool Join(
    const std::vector<std::string> & input,
    const std::string & delimiter,
    std::string * output);
// Same as above, but for set container.
extern bool Join(
    const std::set<std::string> & input,
    const std::string & delimiter,
    std::string * output);
// Same as above, but returns a string instead of populating an input pointer.
extern std::string Join(
    const std::vector<std::string> & input,
    const std::string & delimiter);
// Same as above, but for set container.
extern std::string Join(
    const std::set<std::string> & input, const std::string & delimiter);
// Same as above with default delimiter space (" ").
inline bool
Join(const std::vector<std::string> & input, std::string * output) {
  return Join(input, " ", output);
}
// Same as above, but for set container.
inline bool
Join(const std::set<std::string> & input, std::string * output) {
  return Join(input, " ", output);
}
// Same as above with default delimiter space (" ").
inline std::string Join(const std::vector<std::string> & input) {
  return Join(input, " ");
}
// Same as above, but for set container.
inline std::string Join(const std::set<std::string> & input) {
  return Join(input, " ");
}

// Use the following API to Join vectors whose elements are numeric values
// (More precisely: value_type must be one that is supported by Itoa()).
template<typename value_type>
inline std::string JoinValues(
    const std::vector<value_type> & values,
    const std::string & delimiter) {
  std::string to_return = "";
  for (const value_type & itr : values) {
    if (to_return.empty()) {
      to_return.assign(Itoa(itr));
    } else {
      to_return += delimiter + Itoa(itr);
    }
  }
  return to_return;
}
// Same as above, with default delimiter ", ".
template<typename value_type>
inline std::string JoinValues(const std::vector<value_type> & values) {
  return JoinValues<value_type>(values, ", ");
}
// Same as above, for set container instead of vector.
template<typename value_type>
inline std::string JoinValues(
    const std::set<value_type> & values,
    const std::string & delimiter) {
  std::string to_return = "";
  for (const value_type & itr : values) {
    if (to_return.empty()) {
      to_return.assign(Itoa(itr));
    } else {
      to_return += delimiter + Itoa(itr);
    }
  }
  return to_return;
}
// Same as above, with default delimiter ", ".
template<typename value_type>
inline std::string JoinValues(const std::set<value_type> & values) {
  return JoinValues<value_type>(values, ", ");
}

// Use the following API to Join vectors whose elements are Objects (i.e. structs
// or classes). More precisely: Object T must have a ToString() function defined.
template<typename T>
inline std::string JoinObjects(
    const std::vector<T> & values, const std::string & delimiter) {
  std::string to_return = "";
  for (const T & itr : values) {
    if (to_return.empty()) {
      to_return.assign(itr.ToString());
    } else {
      to_return += delimiter + itr.ToString();
    }
  }
  return to_return;
}
// Same as above, for set container instead of vector.
template<typename T>
inline std::string
JoinObjects(const std::set<T> & values, const std::string & delimiter) {
  std::string to_return = "";
  for (const T & itr : values) {
    if (to_return.empty()) {
      to_return.assign(itr.ToString());
    } else {
      to_return += delimiter + itr.ToString();
    }
  }
  return to_return;
}

// TODO(PHB): All of the below Join(container<bool>) functions should be
// deprecated, and use the JoinValues API above instead. We leave them for
// backwards-compatibility.

// Join Boolean Containers.
extern bool Join(
    const std::vector<bool> & input,
    const std::string & delimiter,
    std::string * output);
// Same as above, but for set container.
extern bool Join(
    const std::set<bool> & input,
    const std::string & delimiter,
    std::string * output);
// Same as above with default delimiter space (" ").
inline bool
Join(const std::vector<bool> & input, std::string * output) {
  return Join(input, " ", output);
}
// Same as above, but for set container.
inline bool Join(const std::set<bool> & input, std::string * output) {
  return Join(input, " ", output);
}
// Same as above, but returns string instead of bool.
extern std::string
Join(const std::vector<bool> & input, const std::string & delimiter);
// Same as above, but for set container.
extern std::string
Join(const std::set<bool> & input, const std::string & delimiter);
// Same as above with default delimiter space (" ").
inline std::string Join(const std::vector<bool> & input) {
  return Join(input, " ");
}
// Same as above, but for set container.
inline std::string Join(const std::set<bool> & input) {
  return Join(input, " ");
}

// Join Integer Containers.
extern bool Join(
    const std::vector<int> & input,
    const std::string & delimiter,
    std::string * output);
// Same as above, but for set container.
extern bool Join(
    const std::set<int> & input,
    const std::string & delimiter,
    std::string * output);
// Same as above with default delimiter space (" ").
inline bool Join(const std::vector<int> & input, std::string * output) {
  return Join(input, " ", output);
}
// Same as above, but for set container.
inline bool Join(const std::set<int> & input, std::string * output) {
  return Join(input, " ", output);
}
// Same as above, but returns string instead of bool.
extern std::string
Join(const std::vector<int> & input, const std::string & delimiter);
// Same as above, but for set container.
extern std::string
Join(const std::set<int> & input, const std::string & delimiter);
// Same as above with default delimiter space (" ").
inline std::string Join(const std::vector<int> & input) {
  return Join(input, " ");
}
// Same as above, but for set container.
inline std::string Join(const std::set<int> & input) {
  return Join(input, " ");
}

// Join Double Containers.
extern bool Join(
    const std::vector<double> & input,
    const std::string & delimiter,
    std::string * output);
// Same as above, but for set container.
extern bool Join(
    const std::set<double> & input,
    const std::string & delimiter,
    std::string * output);
// Same as above with default delimiter space (" ").
inline bool
Join(const std::vector<double> & input, std::string * output) {
  return Join(input, " ", output);
}
// Same as above, but for set container.
inline bool Join(const std::set<double> & input, std::string * output) {
  return Join(input, " ", output);
}
// Same as above, but returns string instead of bool.
extern std::string
Join(const std::vector<double> & input, const std::string & delimiter);
// Same as above, but for set container.
extern std::string
Join(const std::set<double> & input, const std::string & delimiter);
// Same as above with default delimiter space (" ").
inline std::string Join(const std::vector<double> & input) {
  return Join(input, " ");
}
// Same as above, but for set container.
inline std::string Join(const std::set<double> & input) {
  return Join(input, " ");
}

// Returns the first index of 'substring' in 'input', returning string::npos
// if not found. This is the same as the standard C++ 'substr' method, except
// that it handles the special input chars: "\t", "\n", and "\s".
extern size_t
FindSubstring(const std::string & input, const std::string & substring);

// Separates 'input' at each instance of 'delimiter', putting each
// segment into 'output' (optionally omitting empty strings based on
// 'skip_empty'). Note that this respects the following special characters:
//   - "\t": Tab
//   - "\s": Space
extern bool Split(
    const std::string & input,
    const std::string & delimiter,
    const bool skip_empty,
    std::vector<std::string> * output);
// Same as above, with default value for skip_empty = true.
inline bool Split(
    const std::string & input,
    const std::string & delimiter,
    std::vector<std::string> * output) {
  return Split(input, delimiter, true, output);
}
// Same as above, but will split at any delimeter in the set of 'delimiters'.
// NOTE: Unexpected behavior may result if any items in delimiters are a
// substring of another; and even more generally, if there are any overlapping
// characters. For example, if "foo" and "of" are both in delimiters, then
// should split "roofoor" as [ro, oor] or as [roo, r]?
extern bool Split(
    const std::string & input,
    const std::set<std::string> & delimiters,
    const bool skip_empty,
    std::vector<std::string> * output);
// Same as above, with default value for skip_empty = true.
// NOTE: See Note above about uncertainty in output if delimiters contains
// overlapping strings.
inline bool Split(
    const std::string & input,
    const std::set<std::string> & delimiters,
    std::vector<std::string> * output) {
  return Split(input, delimiters, true, output);
}
// Same as above, but set of delimeters is of type char.
extern bool Split(
    const std::string & input,
    const std::set<char> & delimiters,
    const bool skip_empty,
    std::vector<std::string> * output);
// Same as above, with default value for skip_empty = true.
inline bool Split(
    const std::string & input,
    const std::set<char> & delimiters,
    std::vector<std::string> * output) {
  return Split(input, delimiters, true, output);
}
// The below are the same as the above, but they put things in a set
// instead of a vector.
extern bool Split(
    const std::string & input,
    const std::string & delimiter,
    const bool skip_empty,
    std::set<std::string> * output);
inline bool Split(
    const std::string & input,
    const std::string & delimiter,
    std::set<std::string> * output) {
  return Split(input, delimiter, true, output);
}
extern bool Split(
    const std::string & input,
    const std::set<std::string> & delimiters,
    const bool skip_empty,
    std::set<std::string> * output);
inline bool Split(
    const std::string & input,
    const std::set<std::string> & delimiters,
    std::set<std::string> * output) {
  return Split(input, delimiters, true, output);
}
extern bool Split(
    const std::string & input,
    const std::set<char> & delimiters,
    const bool skip_empty,
    std::set<std::string> * output);
inline bool Split(
    const std::string & input,
    const std::set<char> & delimiters,
    std::set<std::string> * output) {
  return Split(input, delimiters, true, output);
}
// ========================== END JOIN and SPLIT =============================

// =============================== STRIP =====================================
// If 'to_match' is a prefix of 'input', then returns true and populates
// output with this prefix removed from input. Otherwise returns false
// and populates output with input.
extern bool StripPrefixString(
    const std::string & input,
    const std::string & to_match,
    std::string * output);
// Same above, but different API: returns the string instead of taking in a
// pointer. Returns (copy of) original string if prefix is not found.
extern std::string StripPrefixString(
    const std::string & input, const std::string & to_match);
// Returns true if 'to_match' is a prefix of 'input'; false otherwise.
extern bool HasPrefixString(
    const std::string & input, const std::string & to_match);
// If 'to_match' is a suffix of 'input', then returns true and populates
// output with this suffix removed from input. Otherwise returns false
// and populates output with input.
extern bool StripSuffixString(
    const std::string & input,
    const std::string & to_match,
    std::string * output);
// Same above, but different API: returns the string instead of taking in a
// pointer. Returns (copy of) original string if suffix is not found.
extern std::string StripSuffixString(
    const std::string & input, const std::string & to_match);
// Returns true if 'to_match' is a suffix of 'input'; false otherwise.
extern bool HasSuffixString(
    const std::string & input, const std::string & to_match);
// Removes Leading whitespace from input and puts resulting string in output.
extern void RemoveLeadingWhitespace(
    const std::string & input, std::string * output);
// Same as above with different API (returns original string if no whitespace)
extern std::string RemoveLeadingWhitespace(const std::string & input);
// Removes Trailing whitespace from input and puts resulting string in output.
extern void RemoveTrailingWhitespace(
    const std::string & input, std::string * output);
// Same as above with different API (returns original string if no whitespace)
extern std::string RemoveTrailingWhitespace(const std::string & input);
// Removes Leading, Trailing, and Consecutive whitespace from input, and
// puts the resulting string in output.
extern void
RemoveExtraWhitespace(const std::string & input, std::string * output);
// Same as above with different API (returns original string if no whitespace)
extern std::string RemoveExtraWhitespace(const std::string & input);
// Removes all whitespace from input, and puts the resulting string in output.
extern void
RemoveAllWhitespace(const std::string & input, std::string * output);
// Same as above with different API (returns original string if no whitespace)
extern std::string RemoveAllWhitespace(const std::string & input);
// Removes all instances of 'to_match' from input and places result in output.
// NOTE: It is okay if 'output' is (the address of) 'input'.
extern void Strip(
    const std::string & input,
    const std::string & to_match,
    std::string * output);
// Same as above, returns result.
extern std::string
Strip(const std::string & input, const std::string & to_match);
// Removes "" from the start/end of a string (only removes if both are present).
extern std::string StripQuotes(const std::string & input);
// Removes '' from the start/end of a string (only removes if both are present).
extern std::string StripSingleQuotes(const std::string & input);
// Removes () from the start/end of a string (only removes if both are present).
// WARNING: Be careful when using this for e.g. parsing math formulas, as this
// blindly strips parentheses as long as there is a leading '(' and trailing ')',
// without regard to whether these parentheses actually match or not. For example:
//   (3 + 2) * (1 + 4)
// Then calling StripParentheses() on the above string would indeed strip them,
// i.e. would retur '3 + 2) * (1 + 4', even though the stripped parentheses don't match.
// To do smart parsing (including removal of enclosing parentheses), use the
// Expression/Formula parsing functions in data_structures.h and formula_utils.h,
// e.g. GetClosingParentheses(), GetClosingSymbol(), and CountOpenCloseParentheses().
extern std::string StripParentheses(const std::string & input);
// Removes {} from the start/end of a string (only removes if both are present).
extern std::string StripBraces(const std::string & input);
// Removes [] from the start/end of a string (only removes if both are present).
extern std::string StripBrackets(const std::string & input);
// Removes <> from the start/end of a string (only removes if both are present).
extern std::string StripAngleBrackets(const std::string & input);
extern std::string
StripAllEnclosingPunctuation(const std::string & input);
extern std::string
StripAllEnclosingPunctuationAndWhitespace(const std::string & input);
// ============================= END STRIP ===================================

// ============================ MISCELLANEOUS ================================
// Returns the number of times 'target' appears in 'value'.
extern int
CountOccurrences(const std::string & value, const char target);
// Returns the number of times 'target' appears in 'value'.
extern int
CountOccurrences(const std::string & value, const std::string & target);

// Replaces all occurrences of 'target' in 'orig' with 'replace with'.
// Note: This iterates from the string start to the end, replacing occurences
// as they appear. There is no danger of infinite loop, even if 'replace_with'
// equals 'target', as the code will always increment search position by
// |replace_with|. For example, for orig = "foobaaaaar", target = "aaa", and
// replace_with = "aa", it will return: "foobaaaar", with steps:
//   1) foob(aaa)aar -> foob(aa)aar
// (notice the "aaaa" in the result isn't replaced, since the search start position
// gets set to (what is now) the third 'a' after the first replacement.
extern std::string Replace(
    const std::string & orig,
    const std::string & target,
    const std::string & replace_with);

// Returns the string, with all letters converted to lowercase.
extern std::string ToLowerCase(const std::string & input);
// Returns the string, with all letters converted to uppercase.
extern std::string ToUpperCase(const std::string & input);
// Returns whether the two strings are equal, up to case.
extern bool
EqualsIgnoreCase(const std::string & one, const std::string & two);
// ========================== END MISCELLANEOUS ==============================

} // namespace string_utils

#endif
