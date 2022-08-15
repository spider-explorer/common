/* strconv2.h v2.0.8               */
/* Last Modified: 2022/05/20 02:17 */
#ifndef STRCONV2_H
#define STRCONV2_H

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <stdarg.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <codecvt>
#include <locale>

#if !defined(_WIN32) || defined(STRCONV_USE_ICONV)
#include <iconv.h>
template <typename X, typename Y>
static inline std::basic_string<X> iconv_translit(
    std::string to_code,
    std::string from_code,
    const std::basic_string<Y> &from_str)
{
#if defined(_WIN32)
    to_code += "//IGNORE";
#else
    to_code += "//TRANSLIT";
#endif
    void *from_begin = (void *)from_str.data();
    void *from_end = (void *)(from_str.data() + from_str.size());
    size_t buff_len = ((char *)from_end) - ((char *)from_begin);
    iconv_t icd = iconv_open(to_code.c_str(), from_code.c_str());
    if (icd == ((iconv_t) -1))
    {
        std::basic_string<X> empty;
        return empty;
    }
    while (true)
    {
        std::vector<char> buff(buff_len + 1);
        char *src_top = (char *)from_begin;
        size_t src_len = ((char *)from_end) - ((char *)from_begin);
        char *dest_top = &buff[0];
        size_t dest_len = buff_len;
        size_t ret = iconv(icd, &src_top, &src_len, &dest_top, &dest_len);
        if (ret == ((size_t) -1))
        {
            if (errno == E2BIG)
            {
                buff_len *= 2;
                continue;
            }
            else
            {
                //std::cerr << errno << std::endl;
                //iconv_close(icd);
                //std::basic_string<X> empty;
                //return empty;
            }
        }
        iconv_close(icd);
        buff.resize(buff_len - dest_len);
        std::basic_string<X> result((X *)buff.data(), (X *)(buff.data() + buff.size()));
        return result;
    }
}
#endif // !defined(_WIN32) || defined(STRCONV_USE_ICONV)

#if defined(_WIN32) && !defined(STRCONV_USE_ICONV)
static inline std::wstring cp_to_wide(const std::string &s, unsigned int codepage)
{
    int in_length = (int)s.length();
    int out_length = MultiByteToWideChar(codepage, 0, s.c_str(), in_length, 0, 0);
    std::wstring result(out_length, L'\0');
    if (out_length)
        MultiByteToWideChar(codepage, 0, s.c_str(), in_length, &result[0], out_length);
    return result;
}
static inline std::string wide_to_cp(const std::wstring &s, unsigned int codepage)
{
    int in_length = (int)s.length();
    int out_length = WideCharToMultiByte(codepage, 0, s.c_str(), in_length, 0, 0, 0, 0);
    std::string result(out_length, '\0');
    if (out_length)
        WideCharToMultiByte(codepage, 0, s.c_str(), in_length, &result[0], out_length, 0, 0);
    return result;
}
#else // !defined(_WIN32)
static inline std::wstring cp_to_wide(const std::string &s, unsigned int codepage)
{
    switch(codepage)
    {
    case 65001:
        return iconv_translit<wchar_t>("WCHAR_T", "UTF-8", s);
    default:
        std::string from_code = "CP" + std::to_string(codepage);
        return iconv_translit<wchar_t>("WCHAR_T", from_code, s);
    }
}
static inline std::string wide_to_cp(const std::wstring &s, unsigned int codepage)
{
    switch(codepage)
    {
    case 65001:
        return iconv_translit<char>("UTF-8", "WCHAR_T", s);
    default:
        std::string to_code = "CP" + std::to_string(codepage);
        return iconv_translit<char>(to_code, "WCHAR_T", s);
    }
}
#endif // _WIN32

static inline std::string cp_to_utf8(const std::string &s, unsigned int codepage)
{
    if (codepage == 65001)
        return s;
    std::wstring wide = cp_to_wide(s, codepage);
    return wide_to_cp(wide, 65001);
}
static inline std::string utf8_to_cp(const std::string &s, unsigned int codepage)
{
    if (codepage == 65001)
        return s;
    std::wstring wide = cp_to_wide(s, 65001);
    return wide_to_cp(wide, codepage);
}

static inline std::wstring ansi_to_wide(const std::string &s)
{
#if defined(_WIN32)
    return cp_to_wide(s, CP_ACP);
#else
    return cp_to_wide(s, 65001);
#endif
}
static inline std::string wide_to_ansi(const std::wstring &s)
{
#if defined(_WIN32)
    return wide_to_cp(s, CP_ACP);
#else
    return wide_to_cp(s, 65001);
#endif
}

static inline std::wstring sjis_to_wide(const std::string &s)
{
    return cp_to_wide(s, 932);
}
static inline std::string wide_to_sjis(const std::wstring &s)
{
    return wide_to_cp(s, 932);
}

static inline std::string ansi_to_utf8(const std::string &s)
{
#if defined(_WIN32)
    return cp_to_utf8(s, CP_ACP);
#else
    //return cp_to_utf8(s, 65001);
    return s;
#endif
}
static inline std::string utf8_to_ansi(const std::string &s)
{
#if defined(_WIN32)
    return utf8_to_cp(s, CP_ACP);
#else
    //return utf8_to_cp(s, 65001);
    return s;
#endif
}

static inline std::string sjis_to_utf8(const std::string &s)
{
    return cp_to_utf8(s, 932);
}
static inline std::string utf8_to_sjis(const std::string &s)
{
    return utf8_to_cp(s, 932);
}

static inline std::wstring utf8_to_wide(const std::string &s)
{
    return cp_to_wide(s, 65001);
}
static inline std::string wide_to_utf8(const std::wstring &s)
{
    return wide_to_cp(s, 65001);
}

#ifdef __cpp_char8_t
static inline std::u8string utf8_to_char8(const std::string &s)
{
    return std::u8string(s.begin(), s.end());
}
static inline std::string char8_to_utf8(const std::u8string &s)
{
    return std::string(s.begin(), s.end());
}

static inline std::wstring char8_to_wide(const std::u8string &s)
{
    return cp_to_wide(char8_to_utf8(s), 65001);
}
static inline std::u8string wide_to_char8(const std::wstring &s)
{
    return utf8_to_char8(wide_to_cp(s, 65001));
}

static inline std::u8string cp_to_char8(const std::string &s, unsigned int codepage)
{
    return utf8_to_char8(cp_to_utf8(s, codepage));
}
static inline std::string char8_to_cp(const std::u8string &s, unsigned int codepage)
{
    return utf8_to_cp(char8_to_utf8(s), codepage);
}

static inline std::u8string ansi_to_char8(const std::string &s)
{
#if defined(_WIN32)
    return cp_to_char8(s, CP_ACP);
#else
    //return cp_to_char8(s, 65001);
    return utf8_to_char8(s);
#endif
}
static inline std::string char8_to_ansi(const std::u8string &s)
{
#if defined(_WIN32)
    return char8_to_cp(s, CP_ACP);
#else
    //return char8_to_cp(s, 65001);
    return char8_to_utf8(s);
#endif
}

static inline std::u8string sjis_to_char8(const std::string &s)
{
    return cp_to_char8(s, 932);
}
static inline std::string char8_to_sjis(const std::u8string &s)
{
    return char8_to_cp(s, 932);
}
#endif

static inline std::string char16_to_utf8(const std::u16string &s)
{
    std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> convert;
    return convert.to_bytes(s);
}

static inline std::u16string utf8_to_char16(const std::string &s)
{
    std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> convert;
    return convert.from_bytes(s);
}

static inline std::string char16_to_cp(const std::u16string &s, unsigned int codepage)
{
    return utf8_to_cp(char16_to_utf8(s), codepage);
}

static inline std::string char32_to_utf8(const std::u32string &s)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
    return convert.to_bytes(s);
}

static inline std::u32string utf8_to_char32(const std::string &s)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
    return convert.from_bytes(s);
}

static inline std::string char32_to_cp(const std::u32string &s, unsigned int codepage)
{
    return utf8_to_cp(char32_to_utf8(s), codepage);
}

#ifdef __cpp_char8_t
static inline std::u8string char16_to_char8(const std::u16string &s)
{
    std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> convert;
    return utf8_to_char8(convert.to_bytes(s));
}

static inline std::u16string char8_to_char16(const std::u8string &s)
{
    std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> convert;
    return convert.from_bytes(char8_to_utf8(s));
}

static inline std::u8string char32_to_char8(const std::u32string &s)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
    return utf8_to_char8(convert.to_bytes(s));
}

static inline std::u32string char8_to_char32(const std::u8string &s)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
    return convert.from_bytes(char8_to_utf8(s));
}
#endif

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

static inline std::string vformat(const char *format, va_list args)
{
    va_list args_copy;
    va_copy(args_copy, args);
    int len = vsnprintf(0, 0, format, args);
    if (len < 0)
    {
        va_end(args_copy);
        return "";
    }
    std::vector<char> buffer(len + 1);
    len = vsnprintf(&buffer[0], len + 1, format, args_copy);
    va_end(args_copy);
    if (len < 0)
    {
        return "";
    }
    buffer[len] = '\0';
    return &buffer[0];
}
#ifdef __cpp_char8_t
static inline std::u8string vformat(const char8_t *format, va_list args)
{
    va_list args_copy;
    va_copy(args_copy, args);
    int len = vsnprintf(0, 0, (const char *)format, args);
    if (len < 0)
    {
        va_end(args_copy);
        return u8"";
    }
    std::vector<char> buffer(len + 1);
    len = vsnprintf(&buffer[0], len + 1, (const char *)format, args_copy);
    va_end(args_copy);
    if (len < 0)
    {
        return u8"";
    }
    buffer[len] = '\0';
    return (char8_t *)&buffer[0];
}
#endif

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

static inline std::string format(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    std::string s = vformat(format, args);
    va_end(args);
    return s;
}
#ifdef __cpp_char8_t
static inline std::u8string format(const char8_t *format, ...)
{
    va_list args;
    va_start(args, format);
    std::u8string s = vformat(format, args);
    va_end(args);
    return s;
}
#endif

#if defined(_WIN32)
static inline HANDLE handle_for_ostream(std::ostream &ostrm)
{
    if (&ostrm == &std::cout)
    {
        return GetStdHandle(STD_OUTPUT_HANDLE);
    }
    else if (&ostrm == &std::cerr)
    {
        return GetStdHandle(STD_ERROR_HANDLE);
    }
    return INVALID_HANDLE_VALUE;
}
template <typename X>
static inline void output_to_ostream(std::ostream &ostrm, const std::basic_string<X> &s, unsigned int target_cp)
{
    static_assert(sizeof(X)==1);
    HANDLE h = handle_for_ostream(ostrm);
    if (h == INVALID_HANDLE_VALUE)
    {
        ostrm << utf8_to_cp((const char *)s.c_str(), target_cp==((unsigned int) -1) ? 65001 : target_cp) << std::flush;
    }
    DWORD dwNumberOfCharsWrite;
    if (GetFileType(h) != FILE_TYPE_CHAR)
    {
        std::string s2 = utf8_to_cp((const char *)s.c_str(), target_cp==((unsigned int) -1) ? GetConsoleOutputCP() : target_cp);
        WriteFile(h, s2.c_str(), (DWORD)s2.size(), &dwNumberOfCharsWrite, NULL);
    }
    else
    {
        std::wstring ws = utf8_to_wide((const char *)s.c_str());
        WriteConsoleW(h,
                      ws.c_str(),
                      (DWORD)ws.size(),
                      &dwNumberOfCharsWrite,
                      NULL);
    }
}
static inline void format(std::ostream &ostrm, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    std::string s = vformat(format, args);
    va_end(args);
    output_to_ostream(ostrm, s, ((unsigned int) -1));
}
#ifdef __cpp_char8_t
static inline void format(std::ostream &ostrm, const char8_t *format, ...)
{
    va_list args;
    va_start(args, format);
    std::u8string s = vformat(format, args);
    va_end(args);
    output_to_ostream(ostrm, s, ((unsigned int) -1));
}
#endif
#else // !defined(_WIN32)
static inline void format(std::ostream &ostrm, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    std::string s = vformat(format, args);
    va_end(args);
    ostrm << s << std::flush;
}
#ifdef __cpp_char8_t
static inline void format(std::ostream &ostrm, const char8_t *format, ...)
{
    va_list args;
    va_start(args, format);
    std::u8string s = vformat(format, args);
    va_end(args);
    ostrm << char8_to_utf8(s) << std::flush;
}
#endif
#endif // _WIN32

static inline std::string formatA(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    std::string s = vformat(format, args);
    va_end(args);
#if defined(_WIN32)
    return utf8_to_ansi(s);
#else
    return s;
#endif
}
#ifdef __cpp_char8_t
static inline std::string formatA(const char8_t *format, ...)
{
    va_list args;
    va_start(args, format);
    std::u8string s = vformat(format, args);
    va_end(args);
#if defined(_WIN32)
    return char8_to_ansi(s);
#else
    return char8_to_utf8(s);
#endif
}
#endif

#if defined(_WIN32)
static inline void formatA(std::ostream &ostrm, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    std::string s = vformat(format, args);
    va_end(args);
    output_to_ostream(ostrm, s, CP_ACP);
}
#ifdef __cpp_char8_t
static inline void formatA(std::ostream &ostrm, const char8_t *format, ...)
{
    va_list args;
    va_start(args, format);
    std::u8string s = vformat(format, args);
    va_end(args);
    output_to_ostream(ostrm, s, CP_ACP);
}
#endif
#else // !defined(_WIN32)
static inline void formatA(std::ostream &ostrm, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    std::string s = vformat(format, args);
    va_end(args);
    ostrm << s << std::flush;
}
#ifdef __cpp_char8_t
static inline void formatA(std::ostream &ostrm, const char8_t *format, ...)
{
    va_list args;
    va_start(args, format);
    std::u8string s = vformat(format, args);
    va_end(args);
    ostrm << char8_to_utf8(s) << std::flush;
}
#endif
#endif // _WIN32

static inline std::string formatUtf8(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    std::string s = vformat(format, args);
    va_end(args);
    return s;
}
#ifdef __cpp_char8_t
static inline std::string formatUtf8(const char8_t *format, ...)
{
    va_list args;
    va_start(args, format);
    std::u8string s = vformat(format, args);
    va_end(args);
    return char8_to_utf8(s);
}
#endif

#if defined(_WIN32)
static inline void formatUtf8(std::ostream &ostrm, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    std::string s = vformat(format, args);
    va_end(args);
    output_to_ostream(ostrm, s, 65001);
}
#ifdef __cpp_char8_t
static inline void formatUtf8(std::ostream &ostrm, const char8_t *format, ...)
{
    va_list args;
    va_start(args, format);
    std::u8string s = vformat(format, args);
    va_end(args);
    output_to_ostream(ostrm, s, 65001);
}
#endif
#else // !defined(_WIN32)
static inline void formatUtf8(std::ostream &ostrm, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    std::string s = vformat(format, args);
    va_end(args);
    ostrm << s << std::flush;
}
#ifdef __cpp_char8_t
static inline void formatUtf8(std::ostream &ostrm, const char8_t *format, ...)
{
    va_list args;
    va_start(args, format);
    std::u8string s = vformat(format, args);
    va_end(args);
    ostrm << char8_to_utf8(s) << std::flush;
}
#endif
#endif // _WIN32

class unicode_ostream
{
private:
    std::ostream *m_ostrm;
#if defined(_WIN32)
    unsigned int m_target_cp;
    bool is_ascii(const std::string &s)
    {
        for (std::size_t i = 0; i < s.size(); i++)
        {
            unsigned char c = (unsigned char)s[i];
            if (c > 0x7f)
                return false;
        }
        return true;
    }
#endif
public:
    unicode_ostream(std::ostream &ostrm, unsigned int dummy = 0) : m_ostrm(&ostrm)
    {
        (void)dummy;
#if defined(_WIN32)
        m_target_cp = GetConsoleOutputCP();
#endif
    }
    template <typename T>
    unicode_ostream &operator<<(const T &x)
    {
#if defined(_WIN32)
        std::ostringstream oss;
        oss << x;
        std::string output = oss.str();
        if (is_ascii(output))
        {
            (*m_ostrm) << x;
        }
        else
        {
            (*m_ostrm) << utf8_to_cp(output, m_target_cp);
        }
#else
        (*m_ostrm) << x;
#endif
        return *this;
    }
    unicode_ostream &operator<<(const std::string &x)
    {
#if defined(_WIN32)
        (*m_ostrm) << utf8_to_cp(x, m_target_cp);
#else
        (*m_ostrm) << x;
#endif
        return *this;
    }
    unicode_ostream &operator<<(const char *x)
    {
#if defined(_WIN32)
        (*m_ostrm) << utf8_to_cp(x, m_target_cp);
#else
        (*m_ostrm) << x;
#endif
        return *this;
    }
#ifdef __cpp_char8_t
    unicode_ostream &operator<<(const std::u8string &x)
    {
#if defined(_WIN32)
        (*m_ostrm) << char8_to_cp(x, m_target_cp);
#else
        (*m_ostrm) << char8_to_utf8(x);
#endif
        return *this;
    }
    unicode_ostream &operator<<(const char8_t *x)
    {
#if defined(_WIN32)
        (*m_ostrm) << char8_to_cp(x, m_target_cp);
#else
        (*m_ostrm) << char8_to_utf8(x);
#endif
        return *this;
    }
#endif
    unicode_ostream &operator<<(const std::u16string &x)
    {
#if defined(_WIN32)
        (*m_ostrm) << char16_to_cp(x, m_target_cp);
#else
        (*m_ostrm) << char16_to_utf8(x);
#endif
        return *this;
    }
    unicode_ostream &operator<<(const char16_t *x)
    {
#if defined(_WIN32)
        (*m_ostrm) << char16_to_cp(x, m_target_cp);
#else
        (*m_ostrm) << char16_to_utf8(x);
#endif
        return *this;
    }
    unicode_ostream &operator<<(const std::u32string &x)
    {
#if defined(_WIN32)
        (*m_ostrm) << char32_to_cp(x, m_target_cp);
#else
        (*m_ostrm) << char32_to_utf8(x);
#endif
        return *this;
    }
    unicode_ostream &operator<<(const char32_t *x)
    {
#if defined(_WIN32)
        (*m_ostrm) << char32_to_cp(x, m_target_cp);
#else
        (*m_ostrm) << char32_to_utf8(x);
#endif
        return *this;
    }
    unicode_ostream &operator<<(std::ostream &(*pf)(std::ostream &)) // For manipulators...
    {
        (*m_ostrm) << pf;
        return *this;
    }
    unicode_ostream &operator<<(std::basic_ios<char> &(*pf)(std::basic_ios<char> &)) // For manipulators...
    {
        (*m_ostrm) << pf;
        return *this;
    }
};

#if !defined(_WIN32)
#define GetConsoleCP() (0)
#define GetConsoleOutputCP() (0)
#endif

#define U8(X) ((const char *)u8##X)
#define WIDE(X) (L##X)

#endif /* STRCONV2_H */
