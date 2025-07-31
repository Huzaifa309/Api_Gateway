/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _MY_APP_MESSAGES_CHAR64STR_CXX_H_
#define _MY_APP_MESSAGES_CHAR64STR_CXX_H_

#if defined(SBE_HAVE_CMATH)
/* cmath needed for std::numeric_limits<double>::quiet_NaN() */
#  include <cmath>
#  define SBE_FLOAT_NAN std::numeric_limits<float>::quiet_NaN()
#  define SBE_DOUBLE_NAN std::numeric_limits<double>::quiet_NaN()
#else
/* math.h needed for NAN */
#  include <math.h>
#  define SBE_FLOAT_NAN NAN
#  define SBE_DOUBLE_NAN NAN
#endif

#if __cplusplus >= 201103L
#  define SBE_CONSTEXPR constexpr
#  define SBE_NOEXCEPT noexcept
#else
#  define SBE_CONSTEXPR
#  define SBE_NOEXCEPT
#endif

#if __cplusplus >= 201703L
#  include <string_view>
#  define SBE_NODISCARD [[nodiscard]]
#else
#  define SBE_NODISCARD
#endif

#if !defined(__STDC_LIMIT_MACROS)
#  define __STDC_LIMIT_MACROS 1
#endif

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>

#if defined(WIN32) || defined(_WIN32)
#  define SBE_BIG_ENDIAN_ENCODE_16(v) _byteswap_ushort(v)
#  define SBE_BIG_ENDIAN_ENCODE_32(v) _byteswap_ulong(v)
#  define SBE_BIG_ENDIAN_ENCODE_64(v) _byteswap_uint64(v)
#  define SBE_LITTLE_ENDIAN_ENCODE_16(v) (v)
#  define SBE_LITTLE_ENDIAN_ENCODE_32(v) (v)
#  define SBE_LITTLE_ENDIAN_ENCODE_64(v) (v)
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#  define SBE_BIG_ENDIAN_ENCODE_16(v) __builtin_bswap16(v)
#  define SBE_BIG_ENDIAN_ENCODE_32(v) __builtin_bswap32(v)
#  define SBE_BIG_ENDIAN_ENCODE_64(v) __builtin_bswap64(v)
#  define SBE_LITTLE_ENDIAN_ENCODE_16(v) (v)
#  define SBE_LITTLE_ENDIAN_ENCODE_32(v) (v)
#  define SBE_LITTLE_ENDIAN_ENCODE_64(v) (v)
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#  define SBE_LITTLE_ENDIAN_ENCODE_16(v) __builtin_bswap16(v)
#  define SBE_LITTLE_ENDIAN_ENCODE_32(v) __builtin_bswap32(v)
#  define SBE_LITTLE_ENDIAN_ENCODE_64(v) __builtin_bswap64(v)
#  define SBE_BIG_ENDIAN_ENCODE_16(v) (v)
#  define SBE_BIG_ENDIAN_ENCODE_32(v) (v)
#  define SBE_BIG_ENDIAN_ENCODE_64(v) (v)
#else
#  error "Byte Ordering of platform not determined. Set __BYTE_ORDER__ manually before including this file."
#endif

#if !defined(SBE_BOUNDS_CHECK_EXPECT)
#  if defined(SBE_NO_BOUNDS_CHECK)
#    define SBE_BOUNDS_CHECK_EXPECT(exp, c) (false)
#  elif defined(_MSC_VER)
#    define SBE_BOUNDS_CHECK_EXPECT(exp, c) (exp)
#  else 
#    define SBE_BOUNDS_CHECK_EXPECT(exp, c) (__builtin_expect(exp, c))
#  endif

#endif

#define SBE_NULLVALUE_INT8 (std::numeric_limits<std::int8_t>::min)()
#define SBE_NULLVALUE_INT16 (std::numeric_limits<std::int16_t>::min)()
#define SBE_NULLVALUE_INT32 (std::numeric_limits<std::int32_t>::min)()
#define SBE_NULLVALUE_INT64 (std::numeric_limits<std::int64_t>::min)()
#define SBE_NULLVALUE_UINT8 (std::numeric_limits<std::uint8_t>::max)()
#define SBE_NULLVALUE_UINT16 (std::numeric_limits<std::uint16_t>::max)()
#define SBE_NULLVALUE_UINT32 (std::numeric_limits<std::uint32_t>::max)()
#define SBE_NULLVALUE_UINT64 (std::numeric_limits<std::uint64_t>::max)()


namespace my {
namespace app {
namespace messages {

class Char64str
{
private:
    char *m_buffer = nullptr;
    std::uint64_t m_bufferLength = 0;
    std::uint64_t m_offset = 0;
    std::uint64_t m_actingVersion = 0;

public:
    enum MetaAttribute
    {
        EPOCH, TIME_UNIT, SEMANTIC_TYPE, PRESENCE
    };

    union sbe_float_as_uint_u
    {
        float fp_value;
        std::uint32_t uint_value;
    };

    union sbe_double_as_uint_u
    {
        double fp_value;
        std::uint64_t uint_value;
    };

    Char64str() = default;

    Char64str(
        char *buffer,
        const std::uint64_t offset,
        const std::uint64_t bufferLength,
        const std::uint64_t actingVersion) :
        m_buffer(buffer),
        m_bufferLength(bufferLength),
        m_offset(offset),
        m_actingVersion(actingVersion)
    {
        if (SBE_BOUNDS_CHECK_EXPECT(((m_offset + 64) > m_bufferLength), false))
        {
            throw std::runtime_error("buffer too short for flyweight [E107]");
        }
    }

    Char64str(
        char *buffer,
        const std::uint64_t bufferLength,
        const std::uint64_t actingVersion) :
        Char64str(buffer, 0, bufferLength, actingVersion)
    {
    }

    Char64str(
        char *buffer,
        const std::uint64_t bufferLength) :
        Char64str(buffer, 0, bufferLength, sbeSchemaVersion())
    {
    }

    Char64str &wrap(
        char *buffer,
        const std::uint64_t offset,
        const std::uint64_t actingVersion,
        const std::uint64_t bufferLength)
    {
        return *this = Char64str(buffer, offset, bufferLength, actingVersion);
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t encodedLength() SBE_NOEXCEPT
    {
        return 64;
    }

    SBE_NODISCARD std::uint64_t offset() const SBE_NOEXCEPT
    {
        return m_offset;
    }

    SBE_NODISCARD const char *buffer() const SBE_NOEXCEPT
    {
        return m_buffer;
    }

    SBE_NODISCARD char *buffer() SBE_NOEXCEPT
    {
        return m_buffer;
    }

    SBE_NODISCARD std::uint64_t bufferLength() const SBE_NOEXCEPT
    {
        return m_bufferLength;
    }

    SBE_NODISCARD std::uint64_t actingVersion() const SBE_NOEXCEPT
    {
        return m_actingVersion;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t sbeSchemaId() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(1);
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t sbeSchemaVersion() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(1);
    }

    SBE_NODISCARD static const char *charValMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t charValId() SBE_NOEXCEPT
    {
        return -1;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t charValSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool charValInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t charValEncodingOffset() SBE_NOEXCEPT
    {
        return 0;
    }

    static SBE_CONSTEXPR char charValNullValue() SBE_NOEXCEPT
    {
        return static_cast<char>(0);
    }

    static SBE_CONSTEXPR char charValMinValue() SBE_NOEXCEPT
    {
        return static_cast<char>(32);
    }

    static SBE_CONSTEXPR char charValMaxValue() SBE_NOEXCEPT
    {
        return static_cast<char>(126);
    }

    static SBE_CONSTEXPR std::size_t charValEncodingLength() SBE_NOEXCEPT
    {
        return 64;
    }

    static SBE_CONSTEXPR std::uint64_t charValLength() SBE_NOEXCEPT
    {
        return 64;
    }

    SBE_NODISCARD const char *charVal() const SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 0;
    }

    SBE_NODISCARD char *charVal() SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 0;
    }

    SBE_NODISCARD char charVal(const std::uint64_t index) const
    {
        if (index >= 64)
        {
            throw std::runtime_error("index out of range for charVal [E104]");
        }

        char val;
        std::memcpy(&val, m_buffer + m_offset + 0 + (index * 1), sizeof(char));
        return (val);
    }

    Char64str &charVal(const std::uint64_t index, const char value)
    {
        if (index >= 64)
        {
            throw std::runtime_error("index out of range for charVal [E105]");
        }

        char val = (value);
        std::memcpy(m_buffer + m_offset + 0 + (index * 1), &val, sizeof(char));
        return *this;
    }

    std::uint64_t getCharVal(char *const dst, const std::uint64_t length) const
    {
        if (length > 64)
        {
            throw std::runtime_error("length too large for getCharVal [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 0, sizeof(char) * static_cast<std::size_t>(length));
        return length;
    }

    Char64str &putCharVal(const char *const src) SBE_NOEXCEPT
    {
        std::memcpy(m_buffer + m_offset + 0, src, sizeof(char) * 64);
        return *this;
    }

    SBE_NODISCARD std::string getCharValAsString() const
    {
        const char *buffer = m_buffer + m_offset + 0;
        std::size_t length = 0;

        for (; length < 64 && *(buffer + length) != '\0'; ++length);
        std::string result(buffer, length);

        return result;
    }

    std::string getCharValAsJsonEscapedString()
    {
        std::ostringstream oss;
        std::string s = getCharValAsString();

        for (const auto c : s)
        {
            switch (c)
            {
                case '"': oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;

                default:
                    if ('\x00' <= c && c <= '\x1f')
                    {
                        oss << "\\u" << std::hex << std::setw(4)
                            << std::setfill('0') << (int)(c);
                    }
                    else
                    {
                        oss << c;
                    }
            }
        }

        return oss.str();
    }

    #if __cplusplus >= 201703L
    SBE_NODISCARD std::string_view getCharValAsStringView() const SBE_NOEXCEPT
    {
        const char *buffer = m_buffer + m_offset + 0;
        std::size_t length = 0;

        for (; length < 64 && *(buffer + length) != '\0'; ++length);
        std::string_view result(buffer, length);

        return result;
    }
    #endif

    #if __cplusplus >= 201703L
    Char64str &putCharVal(const std::string_view str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 64)
        {
            throw std::runtime_error("string too large for putCharVal [E106]");
        }

        std::memcpy(m_buffer + m_offset + 0, str.data(), srcLength);
        for (std::size_t start = srcLength; start < 64; ++start)
        {
            m_buffer[m_offset + 0 + start] = 0;
        }

        return *this;
    }
    #else
    Char64str &putCharVal(const std::string &str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 64)
        {
            throw std::runtime_error("string too large for putCharVal [E106]");
        }

        std::memcpy(m_buffer + m_offset + 0, str.c_str(), srcLength);
        for (std::size_t start = srcLength; start < 64; ++start)
        {
            m_buffer[m_offset + 0 + start] = 0;
        }

        return *this;
    }
    #endif

template<typename CharT, typename Traits>
friend std::basic_ostream<CharT, Traits> & operator << (
    std::basic_ostream<CharT, Traits> &builder, Char64str &writer)
{
    builder << '{';
    builder << R"("charVal": )";
    builder << '"' <<
        writer.getCharValAsJsonEscapedString().c_str() << '"';

    builder << '}';

    return builder;
}

};

}
}
}

#endif
