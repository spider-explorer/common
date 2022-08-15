/**
 ******************************************************************************
 *
 *  @mainpage strutil v1.0.1 - header-only string utility library documentation
 *  @see https://github.com/Shot511/strutil
 *
 *  @copyright  Copyright (C) 2020 Tomasz Galaj (Shot511)
 *  @file       strutil.h
 *  @brief      Library public interface header
 *
 *  @subsection Thank you for your contributions:
 *              - SomeRandomDev49
 *              - flying-tiger
 *
 *
 ******************************************************************************
 */

#pragma once

#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

//! The strutil namespace
namespace strutil
{
/**
 * @brief Converts any datatype into std::string.
 *        Datatype must support << operator.
 * @tparam X
 * @param value - will be converted into std::string.
 * @return Converted value as std::string.
 */
template <typename T, typename X>
static inline std::basic_string<T> to_string(X value)
{
    std::basic_stringstream<T> ss;
    ss << value;
    return ss.str();
}
template <typename X>
static inline std::string to_string(X value)
{
    return to_string<char>(value);
}
template <typename X>
static inline std::wstring to_wstring(X value)
{
    return to_string<wchar_t>(value);
}

/**
 * @brief Converts std::string into any datatype.
 *        Datatype must support << operator.
 * @tparam T
 * @param str - std::string that will be converted into datatype T.
 * @return Variable of datatype T.
 */
template <typename T, typename X>
static inline X parse_string(const std::basic_string<T> &str)
{
    X result;
    std::basic_istringstream<T>(str) >> result;
    return result;
}
template <typename X>
static inline X parse_string(const std::string &str)
{
    return parse_string<char, X>(str);
}
template <typename X>
static inline X parse_string(const std::wstring &str)
{
    return parse_string<wchar_t, X>(str);
}

/**
 * @brief Converts std::string to lower case.
 * @param str - std::string that needs to be converted.
 * @return Lower case input std::string.
 */
template <typename T>
static inline std::basic_string<T> to_lower(const std::basic_string<T> &str)
{
    auto result = str;
    std::transform(result.begin(), result.end(), result.begin(), [](T c) -> T
    {
        return static_cast<T>(std::tolower(c));
    });
    return result;
}
static inline std::string to_lower(const std::string &str)
{
    return to_lower<char>(str);
}
static inline std::wstring to_lower(const std::wstring &str)
{
    return to_lower<wchar_t>(str);
}

/**
 * @brief Converts std::string to upper case.
 * @param str - std::string that needs to be converted.
 * @return Upper case input std::string.
 */
template <typename T>
static inline std::basic_string<T> to_upper(const std::basic_string<T> &str)
{
    auto result = str;
    std::transform(result.begin(), result.end(), result.begin(), [](T c) -> T
    {
        return static_cast<unsigned char>(std::toupper(c));
    });
    return result;
}
static inline std::string to_upper(const std::string &str)
{
    return to_upper<char>(str);
}
static inline std::wstring to_upper(const std::wstring &str)
{
    return to_upper<wchar_t>(str);
}

/**
 * @brief Checks if input std::string str contains specified substring.
 * @param str - std::string to be checked.
 * @param substring - searched substring.
 * @return True if substring was found in str, false otherwise.
 */
template <typename T>
static inline bool contains(const std::basic_string<T> &str, const std::basic_string<T> &substring)
{
    return str.find(substring) != std::basic_string<T>::npos;
}
static inline bool contains(const std::string &str, const std::string &substring)
{
    return contains<char>(str, substring);
}
static inline bool contains(const std::wstring &str, const std::wstring &substring)
{
    return contains<wchar_t>(str, substring);
}

/**
 * @brief Checks if input std::string str contains specified character.
 * @param str - std::string to be checked.
 * @param character - searched character.
 * @return True if character was found in str, false otherwise.
 */
template <typename T>
static inline bool contains(const std::basic_string<T> &str, const T character)
{
    return contains(str, std::basic_string<T>(1, character));
}
static inline bool contains(const std::string &str, const char character)
{
    return contains<char>(str, character);
}
static inline bool contains(const std::wstring &str, const wchar_t character)
{
    return contains<wchar_t>(str, character);
}

/**
 * @brief Compares two std::strings ignoring their case (lower/upper).
 * @param str1 - std::string to compare
 * @param str2 - std::string to compare
 * @return True if str1 and str2 are equal, false otherwise.
 */
template <typename T>
static inline bool compare_ignore_case(const std::basic_string<T> &str1, const std::basic_string<T> &str2)
{
    return to_lower<T>(str1) == to_lower<T>(str2);
}
static inline bool compare_ignore_case(const std::string &str1, const std::string &str2)
{
    return compare_ignore_case<char>(str1, str2);
}
static inline bool compare_ignore_case(const std::wstring &str1, const std::wstring &str2)
{
    return compare_ignore_case<wchar_t>(str1, str2);
}

/**
 * @brief Trims (in-place) white spaces from the left side of std::string.
 *        Taken from: http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring.
 * @param str - input std::string to remove white spaces from.
 */
template <typename T>
static inline void trim_left(std::basic_string<T> &str)
{
    str.erase(
        str.begin(),
        std::find_if(
            str.begin(),
            str.end(),
            [](T ch)
    {
        return !std::isspace(ch);
    }));
}
static inline void trim_left(std::string &str)
{
    trim_left<char>(str);
}
static inline void trim_left(std::wstring &str)
{
    trim_left<wchar_t>(str);
}

/**
 * @brief Trims (in-place) white spaces from the right side of std::string.
 *        Taken from: http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring.
 * @param str - input std::string to remove white spaces from.
 */
template <typename T>
static inline void trim_right(std::basic_string<T> &str)
{
    str.erase(
        std::find_if(
            str.rbegin(),
            str.rend(),
            [](T ch)
    {
        return !std::isspace(ch);
    })
    .base(),
    str.end());
}
static inline void trim_right(std::string &str)
{
    trim_right<char>(str);
}
static inline void trim_right(std::wstring &str)
{
    trim_right<wchar_t>(str);
}

/**
 * @brief Trims (in-place) white spaces from the both sides of std::string.
 *        Taken from: http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring.
 * @param str - input std::string to remove white spaces from.
 */
template <typename T>
static inline void trim(std::basic_string<T> &str)
{
    trim_left<T>(str);
    trim_right<T>(str);
}
static inline void trim(std::string &str)
{
    trim<char>(str);
}
static inline void trim(std::wstring &str)
{
    trim<wchar_t>(str);
}

/**
  * @brief Trims white spaces from the left side of std::string.
  *        Taken from: http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring.
  * @param str - input std::string to remove white spaces from.
  * @return Copy of input str with trimmed white spaces.
  */
template <typename T>
static inline std::basic_string<T> trim_left_copy(const std::basic_string<T> &str)
{
    std::basic_string<T> copy = str;
    trim_left<T>(copy);
    return copy;
}
static inline std::string trim_left_copy(const std::string &str)
{
    return trim_left_copy<char>(str);
}
static inline std::wstring trim_left_copy(const std::wstring &str)
{
    return trim_left_copy<wchar_t>(str);
}

/**
  * @brief Trims white spaces from the right side of std::string.
  *        Taken from: http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring.
  * @param str - input std::string to remove white spaces from.
  * @return Copy of input str with trimmed white spaces.
  */
template <typename T>
static inline std::basic_string<T> trim_right_copy(const std::basic_string<T> &str)
{
    std::basic_string<T> copy = str;
    trim_right<T>(copy);
    return copy;
}
static inline std::string trim_right_copy(const std::string &str)
{
    return trim_right_copy<char>(str);
}
static inline std::wstring trim_right_copy(const std::wstring &str)
{
    return trim_right_copy<wchar_t>(str);
}

/**
  * @brief Trims white spaces from the both sides of std::string.
  *        Taken from: http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring.
  * @param str - input std::string to remove white spaces from.
  * @return Copy of input str with trimmed white spaces.
  */
template <typename T>
static inline std::basic_string<T> trim_copy(const std::basic_string<T> &str)
{
    std::basic_string<T> copy = str;
    trim<T>(copy);
    return copy;
}
static inline std::string trim_copy(const std::string &str)
{
    return trim_copy<char>(str);
}
static inline std::wstring trim_copy(const std::wstring &str)
{
    return trim_copy<wchar_t>(str);
}

/**
 * @brief Replaces (in-place) the first occurance of target with replacement.
 *        Taken from: http://stackoverflow.com/questions/3418231/c-replace-part-of-a-string-with-another-string.
 * @param str - input std::string that will be modified.
 * @param target - substring that will be replaced with replacement.
 * @param replacement - substring that will replace target.
 * @return True if replacement was successfull, false otherwise.
 */
template <typename T>
static inline bool replace_first(std::basic_string<T> &str, const std::basic_string<T> &target, const std::basic_string<T> &replacement)
{
    size_t start_pos = str.find(target);
    if (start_pos == std::basic_string<T>::npos)
    {
        return false;
    }
    str.replace(start_pos, target.length(), replacement);
    return true;
}
static inline bool replace_first(std::string &str, const std::string &target, const std::string &replacement)
{
    return replace_first<char>(str, target, replacement);
}
static inline bool replace_first(std::wstring &str, const std::wstring &target, const std::wstring &replacement)
{
    return replace_first<wchar_t>(str, target, replacement);
}

/**
 * @brief Replaces (in-place) last occurance of target with replacement.
 *        Taken from: http://stackoverflow.com/questions/3418231/c-replace-part-of-a-string-with-another-string.
 * @param str - input std::string that will be modified.
 * @param target - substring that will be replaced with replacement.
 * @param replacement - substring that will replace target.
 * @return True if replacement was successfull, false otherwise.
 */
template <typename T>
static inline bool replace_last(std::basic_string<T> &str, const std::basic_string<T> &target, const std::basic_string<T> &replacement)
{
    size_t start_pos = str.rfind(target);
    if (start_pos == std::basic_string<T>::npos)
    {
        return false;
    }
    str.replace(start_pos, target.length(), replacement);
    return true;
}
static inline bool replace_last(std::string &str, const std::string &target, const std::string &replacement)
{
    return replace_last<char>(str, target, replacement);
}
static inline bool replace_last(std::wstring &str, const std::wstring &target, const std::wstring &replacement)
{
    return replace_last<wchar_t>(str, target, replacement);
}

/**
 * @brief Replaces (in-place) all occurances of target with replacement.
 *        Taken from: http://stackoverflow.com/questions/3418231/c-replace-part-of-a-string-with-another-string.
 * @param str - input std::string that will be modified.
 * @param target - substring that will be replaced with replacement.
 * @param replacement - substring that will replace target.
 * @return True if replacement was successfull, false otherwise.
 */
template <typename T>
static inline bool replace_all(std::basic_string<T> &str, const std::basic_string<T> &target, const std::basic_string<T> &replacement)
{
    if (target.empty())
    {
        return false;
    }
    size_t start_pos = 0;
    const bool found_substring = str.find(target, start_pos) != std::basic_string<T>::npos;
    while ((start_pos = str.find(target, start_pos)) != std::basic_string<T>::npos)
    {
        str.replace(start_pos, target.length(), replacement);
        start_pos += replacement.length();
    }
    return found_substring;
}
static inline bool replace_all(std::string &str, const std::string &target, const std::string &replacement)
{
    return replace_all<char>(str, target, replacement);
}
static inline bool replace_all(std::wstring &str, const std::wstring &target, const std::wstring &replacement)
{
    return replace_all<wchar_t>(str, target, replacement);
}

/**
 * @brief Checks if std::string str ends with specified suffix.
 * @param str - input std::string that will be checked.
 * @param suffix - searched suffix in str.
 * @return True if suffix was found, false otherwise.
 */
template <typename T>
static inline bool ends_with(const std::basic_string<T> &str, const std::basic_string<T> &suffix)
{
    const auto pos = str.rfind(suffix);
    return (pos != std::basic_string<T>::npos) && (pos == (str.length() - suffix.length()));
}
static inline bool ends_with(const std::string &str, const std::string &suffix)
{
    return ends_with<char>(str, suffix);
}
static inline bool ends_with(const std::wstring &str, const std::wstring &suffix)
{
    return ends_with<wchar_t>(str, suffix);
}

/**
 * @brief Checks if std::string str ends with specified character.
 * @param str - input std::string that will be checked.
 * @param suffix - searched character in str.
 * @return True if ends with character, false otherwise.
 */
template <typename T>
static inline bool ends_with(const std::basic_string<T> &str, const T suffix)
{
    return (str.size() > 0) && (*(str.end() - 1) == suffix);
}
static inline bool ends_with(const std::string &str, const char suffix)
{
    return ends_with<char>(str, suffix);
}
static inline bool ends_with(const std::wstring &str, const wchar_t suffix)
{
    return ends_with<wchar_t>(str, suffix);
}

/**
 * @brief Checks if std::string str starts with specified prefix.
 * @param str - input std::string that will be checked.
 * @param prefix - searched prefix in str.
 * @return True if prefix was found, false otherwise.
 */
template <typename T>
static inline bool starts_with(const std::basic_string<T> &str, const std::basic_string<T> &prefix)
{
    return str.find(prefix) == 0;
}
static inline bool starts_with(const std::string &str, const std::string &prefix)
{
    return starts_with<char>(str, prefix);
}
static inline bool starts_with(const std::wstring &str, const std::wstring &prefix)
{
    return starts_with<wchar_t>(str, prefix);
}

/**
 * @brief Checks if std::string str starts with specified character.
 * @param str - input std::string that will be checked.
 * @param prefix - searched character in str.
 * @return True if starts with character, false otherwise.
 */
template <typename T>
static inline bool starts_with(const std::basic_string<T> &str, const T prefix)
{
    return (str.size() > 0) && (str[0] == prefix);
}
static inline bool starts_with(const std::string &str, const char prefix)
{
    return starts_with<char>(str, prefix);
}
static inline bool starts_with(const std::wstring &str, const wchar_t prefix)
{
    return starts_with<wchar_t>(str, prefix);
}

/**
 * @brief Splits input std::string str according to input delim.
 * @param str - std::string that will be splitted.
 * @param delim - the delimiter.
 * @return std::vector<std::string> that contains all splitted tokens.
 */
static inline std::vector<std::string> split(const std::string &str, const char delim)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim))
    {
        tokens.push_back(token);
    }
    // Match semantics of split(str,str)
    if (str.size() == 0 || ends_with(str, delim))
    {
        tokens.push_back("");
    }
    return tokens;
}

/**
 * @brief Splits input std::string str according to input std::string delim.
 *        Taken from: https://stackoverflow.com/a/46931770/1892346.
 * @param str - std::string that will be split.
 * @param delim - the delimiter.
 * @return std::vector<std::string> that contains all splitted tokens.
 */
static inline std::vector<std::string> split(const std::string &str, const std::string &delim)
{
    size_t pos_start = 0, pos_end, delim_len = delim.length();
    std::string token;
    std::vector<std::string> tokens;
    while ((pos_end = str.find(delim, pos_start)) != std::string::npos)
    {
        token = str.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        tokens.push_back(token);
    }
    tokens.push_back(str.substr(pos_start));
    return tokens;
}

/**
 * @brief Splits input string using any delimiter in the given set.
 * @param str - std::string that will be split.
 * @param delims - the set of delimiter characters.
 * @return vector of resulting tokens.
 */
static inline std::vector<std::string> split_any(const std::string &str, const std::string &delims)
{
    std::string token;
    std::vector<std::string> tokens;
    size_t pos_start = 0;
    for (size_t pos_end = 0; pos_end < str.length(); ++pos_end)
    {
        if (contains(delims, str[pos_end]))
        {
            token = str.substr(pos_start, pos_end - pos_start);
            tokens.push_back(token);
            pos_start = pos_end + 1;
        }
    }
    tokens.push_back(str.substr(pos_start));
    return tokens;
}

/**
 * @brief Joins all elements of std::vector tokens into one std::string with delimiter delim.
 * @param tokens - vector of tokens.
 * @param delim - the delimiter.
 * @return std::string with joined elements of vector tokens with delimiter delim.
 */
template <typename T>
static inline std::basic_string<T> join(const std::vector<std::basic_string<T>> &tokens, const std::basic_string<T> &delim)
{
    std::basic_ostringstream<T> result;
    for (auto it = tokens.begin(); it != tokens.end(); ++it)
    {
        if (it != tokens.begin())
        {
            result << delim;
        }
        result << *it;
    }
    return result.str();
}
static inline std::string join(const std::vector<std::string> &tokens, const std::string &delim)
{
    return join<char>(tokens, delim);
}
static inline std::wstring join(const std::vector<std::wstring> &tokens, const std::wstring &delim)
{
    return join<wchar_t>(tokens, delim);
}

/**
 * @brief Inplace removal of all empty strings in a vector<string>
 * @param tokens - vector of strings.
 */
template <typename T>
static inline void drop_empty(std::vector<std::basic_string<T>> &tokens)
{
    auto last = std::remove_if(tokens.begin(), tokens.end(),
                               [](const std::basic_string<T> &s)
    {
        return s.size() == 0;
    });
    tokens.erase(last, tokens.end());
}
static inline void drop_empty(std::vector<std::string> &tokens)
{
    return drop_empty<char>(tokens);
}
static inline void drop_empty(std::vector<std::wstring> &tokens)
{
    return drop_empty<wchar_t>(tokens);
}

/**
 * @brief Inplace removal of all empty strings in a vector<string>
 * @param tokens - vector of strings.
 * @return vector of non-empty tokens.
 */
template <typename T>
static inline std::vector<std::basic_string<T>> drop_empty_copy(const std::vector<std::basic_string<T>> &tokens)
{
    std::vector<std::basic_string<T>> copy = tokens;
    drop_empty(copy);
    return copy;
}
static inline std::vector<std::string> drop_empty_copy(const std::vector<std::string> &tokens)
{
    return drop_empty_copy<char>(tokens);
}
static inline std::vector<std::wstring> drop_empty_copy(const std::vector<std::wstring> &tokens)
{
    return drop_empty_copy<wchar_t>(tokens);
}

/**
 * @brief Creates new std::string with repeated n times substring str.
 * @param str - substring that needs to be repeated.
 * @param n - number of iterations.
 * @return std::string with repeated substring str.
 */
template <typename T>
static inline std::basic_string<T> repeat(const std::basic_string<T> &str, unsigned n)
{
    std::basic_string<T> result;
    for (unsigned i = 0; i < n; ++i)
    {
        result += str;
    }
    return result;
}
static inline std::string repeat(const std::string &str, unsigned n)
{
    return repeat<char>(str, n);
}
static inline std::wstring repeat(const std::wstring &str, unsigned n)
{
    return repeat<wchar_t>(str, n);
}

/**
 * @brief Creates new std::string with repeated n times char c.
 * @param c - char that needs to be repeated.
 * @param n - number of iterations.
 * @return std::string with repeated char c.
 */
template <typename T>
static inline std::basic_string<T> repeat(T c, unsigned n)
{
    return std::basic_string<T>(n, c);
}
static inline std::string repeat(char c, unsigned n)
{
    return repeat<char>(c, n);
}
static inline std::wstring repeat(wchar_t c, unsigned n)
{
    return repeat<wchar_t>(c, n);
}

/**
 * @brief Checks if input std::string str matches specified reular expression regex.
 * @param str - std::string to be checked.
 * @param regex - the std::regex regular expression.
 * @return True if regex matches str, false otherwise.
 */
template <typename T>
static inline bool matches(const std::basic_string<T> &str, const std::basic_regex<T> &regex)
{
    return std::regex_match(str, regex);
}
static inline bool matches(const std::string &str, const std::regex &regex)
{
    return matches<char>(str, regex);
}
static inline bool matches(const std::wstring &str, const std::wregex &regex)
{
    return matches<wchar_t>(str, regex);
}
} // namespace strings
