/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _SBE_APPHEADER_CXX_H_
#define _SBE_APPHEADER_CXX_H_

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
#  if !defined(SBE_USE_STRING_VIEW)
#    define SBE_USE_STRING_VIEW 1
#  endif
#else
#  define SBE_NODISCARD
#endif

#if __cplusplus >= 202002L
#  include <span>
#  if !defined(SBE_USE_SPAN)
#    define SBE_USE_SPAN 1
#  endif
#endif

#if !defined(__STDC_LIMIT_MACROS)
#  define __STDC_LIMIT_MACROS 1
#endif

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <limits>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

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
#  error \
      "Byte Ordering of platform not determined. Set __BYTE_ORDER__ manually before including this file."
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

#define SBE_FLOAT_NAN std::numeric_limits<float>::quiet_NaN()
#define SBE_DOUBLE_NAN std::numeric_limits<double>::quiet_NaN()
#define SBE_NULLVALUE_INT8 (std::numeric_limits<std::int8_t>::min)()
#define SBE_NULLVALUE_INT16 (std::numeric_limits<std::int16_t>::min)()
#define SBE_NULLVALUE_INT32 (std::numeric_limits<std::int32_t>::min)()
#define SBE_NULLVALUE_INT64 (std::numeric_limits<std::int64_t>::min)()
#define SBE_NULLVALUE_UINT8 (std::numeric_limits<std::uint8_t>::max)()
#define SBE_NULLVALUE_UINT16 (std::numeric_limits<std::uint16_t>::max)()
#define SBE_NULLVALUE_UINT32 (std::numeric_limits<std::uint32_t>::max)()
#define SBE_NULLVALUE_UINT64 (std::numeric_limits<std::uint64_t>::max)()

namespace SBE {

class AppHeader {
   private:
    char *m_buffer = nullptr;
    std::uint64_t m_bufferLength = 0;
    std::uint64_t m_offset = 0;
    std::uint64_t m_actingVersion = 0;

   public:
    enum MetaAttribute { EPOCH, TIME_UNIT, SEMANTIC_TYPE, PRESENCE };

    union sbe_float_as_uint_u {
        float fp_value;
        std::uint32_t uint_value;
    };

    union sbe_double_as_uint_u {
        double fp_value;
        std::uint64_t uint_value;
    };

    AppHeader() = default;

    AppHeader(char *buffer, const std::uint64_t offset,
              const std::uint64_t bufferLength,
              const std::uint64_t actingVersion)
        : m_buffer(buffer),
          m_bufferLength(bufferLength),
          m_offset(offset),
          m_actingVersion(actingVersion) {
        if (SBE_BOUNDS_CHECK_EXPECT(((m_offset + 288) > m_bufferLength),
                                    false)) {
            throw std::runtime_error("buffer too short for flyweight [E107]");
        }
    }

    AppHeader(char *buffer, const std::uint64_t bufferLength,
              const std::uint64_t actingVersion)
        : AppHeader(buffer, 0, bufferLength, actingVersion) {}

    AppHeader(char *buffer, const std::uint64_t bufferLength)
        : AppHeader(buffer, 0, bufferLength, sbeSchemaVersion()) {}

    AppHeader &wrap(char *buffer, const std::uint64_t offset,
                    const std::uint64_t actingVersion,
                    const std::uint64_t bufferLength) {
        m_buffer = buffer;
        m_bufferLength = bufferLength;
        m_offset = offset;
        m_actingVersion = actingVersion;

        if (SBE_BOUNDS_CHECK_EXPECT(((m_offset + 288) > m_bufferLength),
                                    false)) {
            throw std::runtime_error("buffer too short for flyweight [E107]");
        }

        return *this;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t encodedLength()
        SBE_NOEXCEPT {
        return 288;
    }

    SBE_NODISCARD std::uint64_t offset() const SBE_NOEXCEPT { return m_offset; }

    SBE_NODISCARD const char *buffer() const SBE_NOEXCEPT { return m_buffer; }

    SBE_NODISCARD char *buffer() SBE_NOEXCEPT { return m_buffer; }

    SBE_NODISCARD std::uint64_t bufferLength() const SBE_NOEXCEPT {
        return m_bufferLength;
    }

    SBE_NODISCARD std::uint64_t actingVersion() const SBE_NOEXCEPT {
        return m_actingVersion;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t sbeSchemaId()
        SBE_NOEXCEPT {
        return static_cast<std::uint16_t>(1);
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t sbeSchemaVersion()
        SBE_NOEXCEPT {
        return static_cast<std::uint16_t>(1);
    }

    SBE_NODISCARD static const char *versionMetaAttribute(
        const MetaAttribute metaAttribute) SBE_NOEXCEPT {
        switch (metaAttribute) {
            case MetaAttribute::PRESENCE:
                return "required";
            default:
                return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t versionId() SBE_NOEXCEPT { return -1; }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t versionSinceVersion()
        SBE_NOEXCEPT {
        return 0;
    }

    SBE_NODISCARD bool versionInActingVersion() SBE_NOEXCEPT { return true; }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t versionEncodingOffset()
        SBE_NOEXCEPT {
        return 0;
    }

    static SBE_CONSTEXPR std::uint16_t versionNullValue() SBE_NOEXCEPT {
        return SBE_NULLVALUE_UINT16;
    }

    static SBE_CONSTEXPR std::uint16_t versionMinValue() SBE_NOEXCEPT {
        return static_cast<std::uint16_t>(0);
    }

    static SBE_CONSTEXPR std::uint16_t versionMaxValue() SBE_NOEXCEPT {
        return static_cast<std::uint16_t>(65534);
    }

    static SBE_CONSTEXPR std::size_t versionEncodingLength() SBE_NOEXCEPT {
        return 2;
    }

    SBE_NODISCARD std::uint16_t version() const SBE_NOEXCEPT {
        std::uint16_t val;
        std::memcpy(&val, m_buffer + m_offset + 0, sizeof(std::uint16_t));
        return SBE_LITTLE_ENDIAN_ENCODE_16(val);
    }

    AppHeader &version(const std::uint16_t value) SBE_NOEXCEPT {
        std::uint16_t val = SBE_LITTLE_ENDIAN_ENCODE_16(value);
        std::memcpy(m_buffer + m_offset + 0, &val, sizeof(std::uint16_t));
        return *this;
    }

    SBE_NODISCARD static const char *messageTypeMetaAttribute(
        const MetaAttribute metaAttribute) SBE_NOEXCEPT {
        switch (metaAttribute) {
            case MetaAttribute::PRESENCE:
                return "required";
            default:
                return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t messageTypeId() SBE_NOEXCEPT {
        return -1;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t messageTypeSinceVersion()
        SBE_NOEXCEPT {
        return 0;
    }

    SBE_NODISCARD bool messageTypeInActingVersion() SBE_NOEXCEPT {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t messageTypeEncodingOffset()
        SBE_NOEXCEPT {
        return 2;
    }

    static SBE_CONSTEXPR char messageTypeNullValue() SBE_NOEXCEPT {
        return static_cast<char>(0);
    }

    static SBE_CONSTEXPR char messageTypeMinValue() SBE_NOEXCEPT {
        return static_cast<char>(32);
    }

    static SBE_CONSTEXPR char messageTypeMaxValue() SBE_NOEXCEPT {
        return static_cast<char>(126);
    }

    static SBE_CONSTEXPR std::size_t messageTypeEncodingLength() SBE_NOEXCEPT {
        return 16;
    }

    static SBE_CONSTEXPR std::uint64_t messageTypeLength() SBE_NOEXCEPT {
        return 16;
    }

    SBE_NODISCARD const char *messageType() const SBE_NOEXCEPT {
        return m_buffer + m_offset + 2;
    }

    SBE_NODISCARD char *messageType() SBE_NOEXCEPT {
        return m_buffer + m_offset + 2;
    }

    SBE_NODISCARD char messageType(const std::uint64_t index) const {
        if (index >= 16) {
            throw std::runtime_error(
                "index out of range for messageType [E104]");
        }

        char val;
        std::memcpy(&val, m_buffer + m_offset + 2 + (index * 1), sizeof(char));
        return (val);
    }

    AppHeader &messageType(const std::uint64_t index, const char value) {
        if (index >= 16) {
            throw std::runtime_error(
                "index out of range for messageType [E105]");
        }

        char val = (value);
        std::memcpy(m_buffer + m_offset + 2 + (index * 1), &val, sizeof(char));
        return *this;
    }

    std::uint64_t getMessageType(char *const dst,
                                 const std::uint64_t length) const {
        if (length > 16) {
            throw std::runtime_error(
                "length too large for getMessageType [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 2,
                    sizeof(char) * static_cast<std::size_t>(length));
        return length;
    }

#ifdef SBE_USE_SPAN
    SBE_NODISCARD std::span<const char> getMessageTypeAsSpan() const
        SBE_NOEXCEPT {
        const char *buffer = m_buffer + m_offset + 2;
        return std::span<const char>(reinterpret_cast<const char *>(buffer),
                                     16);
    }
#endif

#ifdef SBE_USE_SPAN
    template <std::size_t N>
    AppHeader &putMessageType(std::span<const char, N> src) SBE_NOEXCEPT {
        static_assert(N <= 16, "array too large for putMessageType");

        std::memcpy(m_buffer + m_offset + 2, src.data(), sizeof(char) * N);
        for (std::size_t start = N; start < 16; ++start) {
            m_buffer[m_offset + 2 + start] = 0;
        }

        return *this;
    }
#endif

#ifdef SBE_USE_SPAN
    template <typename T>
    AppHeader &putMessageType(T &&src) SBE_NOEXCEPT
        requires(std::is_pointer_v<std::remove_reference_t<T>> &&
                 !std::is_array_v<std::remove_reference_t<T>>)
#else
    AppHeader &putMessageType(const char *const src) SBE_NOEXCEPT
#endif
    {
        std::memcpy(m_buffer + m_offset + 2, src, sizeof(char) * 16);
        return *this;
    }

#ifdef SBE_USE_SPAN
    template <std::size_t N>
    AppHeader &putMessageType(const char (&src)[N]) SBE_NOEXCEPT {
        return putMessageType(std::span<const char, N>(src));
    }
#endif

    SBE_NODISCARD std::string getMessageTypeAsString() const {
        const char *buffer = m_buffer + m_offset + 2;
        std::size_t length = 0;

        for (; length < 16 && *(buffer + length) != '\0'; ++length);
        std::string result(buffer, length);

        return result;
    }

    std::string getMessageTypeAsJsonEscapedString() {
        std::ostringstream oss;
        std::string s = getMessageTypeAsString();

        for (const auto c : s) {
            switch (c) {
                case '"':
                    oss << "\\\"";
                    break;
                case '\\':
                    oss << "\\\\";
                    break;
                case '\b':
                    oss << "\\b";
                    break;
                case '\f':
                    oss << "\\f";
                    break;
                case '\n':
                    oss << "\\n";
                    break;
                case '\r':
                    oss << "\\r";
                    break;
                case '\t':
                    oss << "\\t";
                    break;

                default:
                    if ('\x00' <= c && c <= '\x1f') {
                        oss << "\\u" << std::hex << std::setw(4)
                            << std::setfill('0') << (int)(c);
                    } else {
                        oss << c;
                    }
            }
        }

        return oss.str();
    }

#ifdef SBE_USE_STRING_VIEW
    SBE_NODISCARD std::string_view getMessageTypeAsStringView() const
        SBE_NOEXCEPT {
        const char *buffer = m_buffer + m_offset + 2;
        std::size_t length = 0;

        for (; length < 16 && *(buffer + length) != '\0'; ++length);
        std::string_view result(buffer, length);

        return result;
    }
#endif

#ifdef SBE_USE_STRING_VIEW
    AppHeader &putMessageType(const std::string_view str) {
        const std::size_t srcLength = str.length();
        if (srcLength > 16) {
            throw std::runtime_error(
                "string too large for putMessageType [E106]");
        }

        std::memcpy(m_buffer + m_offset + 2, str.data(), srcLength);
        for (std::size_t start = srcLength; start < 16; ++start) {
            m_buffer[m_offset + 2 + start] = 0;
        }

        return *this;
    }
#else
    AppHeader &putMessageType(const std::string &str) {
        const std::size_t srcLength = str.length();
        if (srcLength > 16) {
            throw std::runtime_error(
                "string too large for putMessageType [E106]");
        }

        std::memcpy(m_buffer + m_offset + 2, str.c_str(), srcLength);
        for (std::size_t start = srcLength; start < 16; ++start) {
            m_buffer[m_offset + 2 + start] = 0;
        }

        return *this;
    }
#endif

    SBE_NODISCARD static const char *messageIdMetaAttribute(
        const MetaAttribute metaAttribute) SBE_NOEXCEPT {
        switch (metaAttribute) {
            case MetaAttribute::PRESENCE:
                return "required";
            default:
                return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t messageIdId() SBE_NOEXCEPT { return -1; }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t messageIdSinceVersion()
        SBE_NOEXCEPT {
        return 0;
    }

    SBE_NODISCARD bool messageIdInActingVersion() SBE_NOEXCEPT { return true; }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t messageIdEncodingOffset()
        SBE_NOEXCEPT {
        return 18;
    }

    static SBE_CONSTEXPR char messageIdNullValue() SBE_NOEXCEPT {
        return static_cast<char>(0);
    }

    static SBE_CONSTEXPR char messageIdMinValue() SBE_NOEXCEPT {
        return static_cast<char>(32);
    }

    static SBE_CONSTEXPR char messageIdMaxValue() SBE_NOEXCEPT {
        return static_cast<char>(126);
    }

    static SBE_CONSTEXPR std::size_t messageIdEncodingLength() SBE_NOEXCEPT {
        return 32;
    }

    static SBE_CONSTEXPR std::uint64_t messageIdLength() SBE_NOEXCEPT {
        return 32;
    }

    SBE_NODISCARD const char *messageId() const SBE_NOEXCEPT {
        return m_buffer + m_offset + 18;
    }

    SBE_NODISCARD char *messageId() SBE_NOEXCEPT {
        return m_buffer + m_offset + 18;
    }

    SBE_NODISCARD char messageId(const std::uint64_t index) const {
        if (index >= 32) {
            throw std::runtime_error("index out of range for messageId [E104]");
        }

        char val;
        std::memcpy(&val, m_buffer + m_offset + 18 + (index * 1), sizeof(char));
        return (val);
    }

    AppHeader &messageId(const std::uint64_t index, const char value) {
        if (index >= 32) {
            throw std::runtime_error("index out of range for messageId [E105]");
        }

        char val = (value);
        std::memcpy(m_buffer + m_offset + 18 + (index * 1), &val, sizeof(char));
        return *this;
    }

    std::uint64_t getMessageId(char *const dst,
                               const std::uint64_t length) const {
        if (length > 32) {
            throw std::runtime_error(
                "length too large for getMessageId [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 18,
                    sizeof(char) * static_cast<std::size_t>(length));
        return length;
    }

#ifdef SBE_USE_SPAN
    SBE_NODISCARD std::span<const char> getMessageIdAsSpan() const
        SBE_NOEXCEPT {
        const char *buffer = m_buffer + m_offset + 18;
        return std::span<const char>(reinterpret_cast<const char *>(buffer),
                                     32);
    }
#endif

#ifdef SBE_USE_SPAN
    template <std::size_t N>
    AppHeader &putMessageId(std::span<const char, N> src) SBE_NOEXCEPT {
        static_assert(N <= 32, "array too large for putMessageId");

        std::memcpy(m_buffer + m_offset + 18, src.data(), sizeof(char) * N);
        for (std::size_t start = N; start < 32; ++start) {
            m_buffer[m_offset + 18 + start] = 0;
        }

        return *this;
    }
#endif

#ifdef SBE_USE_SPAN
    template <typename T>
    AppHeader &putMessageId(T &&src) SBE_NOEXCEPT
        requires(std::is_pointer_v<std::remove_reference_t<T>> &&
                 !std::is_array_v<std::remove_reference_t<T>>)
#else
    AppHeader &putMessageId(const char *const src) SBE_NOEXCEPT
#endif
    {
        std::memcpy(m_buffer + m_offset + 18, src, sizeof(char) * 32);
        return *this;
    }

#ifdef SBE_USE_SPAN
    template <std::size_t N>
    AppHeader &putMessageId(const char (&src)[N]) SBE_NOEXCEPT {
        return putMessageId(std::span<const char, N>(src));
    }
#endif

    SBE_NODISCARD std::string getMessageIdAsString() const {
        const char *buffer = m_buffer + m_offset + 18;
        std::size_t length = 0;

        for (; length < 32 && *(buffer + length) != '\0'; ++length);
        std::string result(buffer, length);

        return result;
    }

    std::string getMessageIdAsJsonEscapedString() {
        std::ostringstream oss;
        std::string s = getMessageIdAsString();

        for (const auto c : s) {
            switch (c) {
                case '"':
                    oss << "\\\"";
                    break;
                case '\\':
                    oss << "\\\\";
                    break;
                case '\b':
                    oss << "\\b";
                    break;
                case '\f':
                    oss << "\\f";
                    break;
                case '\n':
                    oss << "\\n";
                    break;
                case '\r':
                    oss << "\\r";
                    break;
                case '\t':
                    oss << "\\t";
                    break;

                default:
                    if ('\x00' <= c && c <= '\x1f') {
                        oss << "\\u" << std::hex << std::setw(4)
                            << std::setfill('0') << (int)(c);
                    } else {
                        oss << c;
                    }
            }
        }

        return oss.str();
    }

#ifdef SBE_USE_STRING_VIEW
    SBE_NODISCARD std::string_view getMessageIdAsStringView() const
        SBE_NOEXCEPT {
        const char *buffer = m_buffer + m_offset + 18;
        std::size_t length = 0;

        for (; length < 32 && *(buffer + length) != '\0'; ++length);
        std::string_view result(buffer, length);

        return result;
    }
#endif

#ifdef SBE_USE_STRING_VIEW
    AppHeader &putMessageId(const std::string_view str) {
        const std::size_t srcLength = str.length();
        if (srcLength > 32) {
            throw std::runtime_error(
                "string too large for putMessageId [E106]");
        }

        std::memcpy(m_buffer + m_offset + 18, str.data(), srcLength);
        for (std::size_t start = srcLength; start < 32; ++start) {
            m_buffer[m_offset + 18 + start] = 0;
        }

        return *this;
    }
#else
    AppHeader &putMessageId(const std::string &str) {
        const std::size_t srcLength = str.length();
        if (srcLength > 32) {
            throw std::runtime_error(
                "string too large for putMessageId [E106]");
        }

        std::memcpy(m_buffer + m_offset + 18, str.c_str(), srcLength);
        for (std::size_t start = srcLength; start < 32; ++start) {
            m_buffer[m_offset + 18 + start] = 0;
        }

        return *this;
    }
#endif

    SBE_NODISCARD static const char *messageCodeMetaAttribute(
        const MetaAttribute metaAttribute) SBE_NOEXCEPT {
        switch (metaAttribute) {
            case MetaAttribute::PRESENCE:
                return "required";
            default:
                return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t messageCodeId() SBE_NOEXCEPT {
        return -1;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t messageCodeSinceVersion()
        SBE_NOEXCEPT {
        return 0;
    }

    SBE_NODISCARD bool messageCodeInActingVersion() SBE_NOEXCEPT {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t messageCodeEncodingOffset()
        SBE_NOEXCEPT {
        return 50;
    }

    static SBE_CONSTEXPR char messageCodeNullValue() SBE_NOEXCEPT {
        return static_cast<char>(0);
    }

    static SBE_CONSTEXPR char messageCodeMinValue() SBE_NOEXCEPT {
        return static_cast<char>(32);
    }

    static SBE_CONSTEXPR char messageCodeMaxValue() SBE_NOEXCEPT {
        return static_cast<char>(126);
    }

    static SBE_CONSTEXPR std::size_t messageCodeEncodingLength() SBE_NOEXCEPT {
        return 16;
    }

    static SBE_CONSTEXPR std::uint64_t messageCodeLength() SBE_NOEXCEPT {
        return 16;
    }

    SBE_NODISCARD const char *messageCode() const SBE_NOEXCEPT {
        return m_buffer + m_offset + 50;
    }

    SBE_NODISCARD char *messageCode() SBE_NOEXCEPT {
        return m_buffer + m_offset + 50;
    }

    SBE_NODISCARD char messageCode(const std::uint64_t index) const {
        if (index >= 16) {
            throw std::runtime_error(
                "index out of range for messageCode [E104]");
        }

        char val;
        std::memcpy(&val, m_buffer + m_offset + 50 + (index * 1), sizeof(char));
        return (val);
    }

    AppHeader &messageCode(const std::uint64_t index, const char value) {
        if (index >= 16) {
            throw std::runtime_error(
                "index out of range for messageCode [E105]");
        }

        char val = (value);
        std::memcpy(m_buffer + m_offset + 50 + (index * 1), &val, sizeof(char));
        return *this;
    }

    std::uint64_t getMessageCode(char *const dst,
                                 const std::uint64_t length) const {
        if (length > 16) {
            throw std::runtime_error(
                "length too large for getMessageCode [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 50,
                    sizeof(char) * static_cast<std::size_t>(length));
        return length;
    }

#ifdef SBE_USE_SPAN
    SBE_NODISCARD std::span<const char> getMessageCodeAsSpan() const
        SBE_NOEXCEPT {
        const char *buffer = m_buffer + m_offset + 50;
        return std::span<const char>(reinterpret_cast<const char *>(buffer),
                                     16);
    }
#endif

#ifdef SBE_USE_SPAN
    template <std::size_t N>
    AppHeader &putMessageCode(std::span<const char, N> src) SBE_NOEXCEPT {
        static_assert(N <= 16, "array too large for putMessageCode");

        std::memcpy(m_buffer + m_offset + 50, src.data(), sizeof(char) * N);
        for (std::size_t start = N; start < 16; ++start) {
            m_buffer[m_offset + 50 + start] = 0;
        }

        return *this;
    }
#endif

#ifdef SBE_USE_SPAN
    template <typename T>
    AppHeader &putMessageCode(T &&src) SBE_NOEXCEPT
        requires(std::is_pointer_v<std::remove_reference_t<T>> &&
                 !std::is_array_v<std::remove_reference_t<T>>)
#else
    AppHeader &putMessageCode(const char *const src) SBE_NOEXCEPT
#endif
    {
        std::memcpy(m_buffer + m_offset + 50, src, sizeof(char) * 16);
        return *this;
    }

#ifdef SBE_USE_SPAN
    template <std::size_t N>
    AppHeader &putMessageCode(const char (&src)[N]) SBE_NOEXCEPT {
        return putMessageCode(std::span<const char, N>(src));
    }
#endif

    SBE_NODISCARD std::string getMessageCodeAsString() const {
        const char *buffer = m_buffer + m_offset + 50;
        std::size_t length = 0;

        for (; length < 16 && *(buffer + length) != '\0'; ++length);
        std::string result(buffer, length);

        return result;
    }

    std::string getMessageCodeAsJsonEscapedString() {
        std::ostringstream oss;
        std::string s = getMessageCodeAsString();

        for (const auto c : s) {
            switch (c) {
                case '"':
                    oss << "\\\"";
                    break;
                case '\\':
                    oss << "\\\\";
                    break;
                case '\b':
                    oss << "\\b";
                    break;
                case '\f':
                    oss << "\\f";
                    break;
                case '\n':
                    oss << "\\n";
                    break;
                case '\r':
                    oss << "\\r";
                    break;
                case '\t':
                    oss << "\\t";
                    break;

                default:
                    if ('\x00' <= c && c <= '\x1f') {
                        oss << "\\u" << std::hex << std::setw(4)
                            << std::setfill('0') << (int)(c);
                    } else {
                        oss << c;
                    }
            }
        }

        return oss.str();
    }

#ifdef SBE_USE_STRING_VIEW
    SBE_NODISCARD std::string_view getMessageCodeAsStringView() const
        SBE_NOEXCEPT {
        const char *buffer = m_buffer + m_offset + 50;
        std::size_t length = 0;

        for (; length < 16 && *(buffer + length) != '\0'; ++length);
        std::string_view result(buffer, length);

        return result;
    }
#endif

#ifdef SBE_USE_STRING_VIEW
    AppHeader &putMessageCode(const std::string_view str) {
        const std::size_t srcLength = str.length();
        if (srcLength > 16) {
            throw std::runtime_error(
                "string too large for putMessageCode [E106]");
        }

        std::memcpy(m_buffer + m_offset + 50, str.data(), srcLength);
        for (std::size_t start = srcLength; start < 16; ++start) {
            m_buffer[m_offset + 50 + start] = 0;
        }

        return *this;
    }
#else
    AppHeader &putMessageCode(const std::string &str) {
        const std::size_t srcLength = str.length();
        if (srcLength > 16) {
            throw std::runtime_error(
                "string too large for putMessageCode [E106]");
        }

        std::memcpy(m_buffer + m_offset + 50, str.c_str(), srcLength);
        for (std::size_t start = srcLength; start < 16; ++start) {
            m_buffer[m_offset + 50 + start] = 0;
        }

        return *this;
    }
#endif

    SBE_NODISCARD static const char *sequenceMetaAttribute(
        const MetaAttribute metaAttribute) SBE_NOEXCEPT {
        switch (metaAttribute) {
            case MetaAttribute::PRESENCE:
                return "required";
            default:
                return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t sequenceId() SBE_NOEXCEPT { return -1; }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t sequenceSinceVersion()
        SBE_NOEXCEPT {
        return 0;
    }

    SBE_NODISCARD bool sequenceInActingVersion() SBE_NOEXCEPT { return true; }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t sequenceEncodingOffset()
        SBE_NOEXCEPT {
        return 66;
    }

    static SBE_CONSTEXPR std::uint32_t sequenceNullValue() SBE_NOEXCEPT {
        return SBE_NULLVALUE_UINT32;
    }

    static SBE_CONSTEXPR std::uint32_t sequenceMinValue() SBE_NOEXCEPT {
        return UINT32_C(0x0);
    }

    static SBE_CONSTEXPR std::uint32_t sequenceMaxValue() SBE_NOEXCEPT {
        return UINT32_C(0xfffffffe);
    }

    static SBE_CONSTEXPR std::size_t sequenceEncodingLength() SBE_NOEXCEPT {
        return 4;
    }

    SBE_NODISCARD std::uint32_t sequence() const SBE_NOEXCEPT {
        std::uint32_t val;
        std::memcpy(&val, m_buffer + m_offset + 66, sizeof(std::uint32_t));
        return SBE_LITTLE_ENDIAN_ENCODE_32(val);
    }

    AppHeader &sequence(const std::uint32_t value) SBE_NOEXCEPT {
        std::uint32_t val = SBE_LITTLE_ENDIAN_ENCODE_32(value);
        std::memcpy(m_buffer + m_offset + 66, &val, sizeof(std::uint32_t));
        return *this;
    }

    SBE_NODISCARD static const char *timestampMetaAttribute(
        const MetaAttribute metaAttribute) SBE_NOEXCEPT {
        switch (metaAttribute) {
            case MetaAttribute::PRESENCE:
                return "required";
            default:
                return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t timestampId() SBE_NOEXCEPT { return -1; }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t timestampSinceVersion()
        SBE_NOEXCEPT {
        return 0;
    }

    SBE_NODISCARD bool timestampInActingVersion() SBE_NOEXCEPT { return true; }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t timestampEncodingOffset()
        SBE_NOEXCEPT {
        return 70;
    }

    static SBE_CONSTEXPR std::uint64_t timestampNullValue() SBE_NOEXCEPT {
        return SBE_NULLVALUE_UINT64;
    }

    static SBE_CONSTEXPR std::uint64_t timestampMinValue() SBE_NOEXCEPT {
        return UINT64_C(0x0);
    }

    static SBE_CONSTEXPR std::uint64_t timestampMaxValue() SBE_NOEXCEPT {
        return UINT64_C(0xfffffffffffffffe);
    }

    static SBE_CONSTEXPR std::size_t timestampEncodingLength() SBE_NOEXCEPT {
        return 8;
    }

    SBE_NODISCARD std::uint64_t timestamp() const SBE_NOEXCEPT {
        std::uint64_t val;
        std::memcpy(&val, m_buffer + m_offset + 70, sizeof(std::uint64_t));
        return SBE_LITTLE_ENDIAN_ENCODE_64(val);
    }

    AppHeader &timestamp(const std::uint64_t value) SBE_NOEXCEPT {
        std::uint64_t val = SBE_LITTLE_ENDIAN_ENCODE_64(value);
        std::memcpy(m_buffer + m_offset + 70, &val, sizeof(std::uint64_t));
        return *this;
    }

    SBE_NODISCARD static const char *statusCodeMetaAttribute(
        const MetaAttribute metaAttribute) SBE_NOEXCEPT {
        switch (metaAttribute) {
            case MetaAttribute::PRESENCE:
                return "required";
            default:
                return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t statusCodeId() SBE_NOEXCEPT {
        return -1;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t statusCodeSinceVersion()
        SBE_NOEXCEPT {
        return 0;
    }

    SBE_NODISCARD bool statusCodeInActingVersion() SBE_NOEXCEPT { return true; }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t statusCodeEncodingOffset()
        SBE_NOEXCEPT {
        return 78;
    }

    static SBE_CONSTEXPR std::uint16_t statusCodeNullValue() SBE_NOEXCEPT {
        return SBE_NULLVALUE_UINT16;
    }

    static SBE_CONSTEXPR std::uint16_t statusCodeMinValue() SBE_NOEXCEPT {
        return static_cast<std::uint16_t>(0);
    }

    static SBE_CONSTEXPR std::uint16_t statusCodeMaxValue() SBE_NOEXCEPT {
        return static_cast<std::uint16_t>(65534);
    }

    static SBE_CONSTEXPR std::size_t statusCodeEncodingLength() SBE_NOEXCEPT {
        return 2;
    }

    SBE_NODISCARD std::uint16_t statusCode() const SBE_NOEXCEPT {
        std::uint16_t val;
        std::memcpy(&val, m_buffer + m_offset + 78, sizeof(std::uint16_t));
        return SBE_LITTLE_ENDIAN_ENCODE_16(val);
    }

    AppHeader &statusCode(const std::uint16_t value) SBE_NOEXCEPT {
        std::uint16_t val = SBE_LITTLE_ENDIAN_ENCODE_16(value);
        std::memcpy(m_buffer + m_offset + 78, &val, sizeof(std::uint16_t));
        return *this;
    }

    SBE_NODISCARD static const char *deviceIdMetaAttribute(
        const MetaAttribute metaAttribute) SBE_NOEXCEPT {
        switch (metaAttribute) {
            case MetaAttribute::PRESENCE:
                return "required";
            default:
                return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t deviceIdId() SBE_NOEXCEPT { return -1; }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t deviceIdSinceVersion()
        SBE_NOEXCEPT {
        return 0;
    }

    SBE_NODISCARD bool deviceIdInActingVersion() SBE_NOEXCEPT { return true; }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t deviceIdEncodingOffset()
        SBE_NOEXCEPT {
        return 80;
    }

    static SBE_CONSTEXPR char deviceIdNullValue() SBE_NOEXCEPT {
        return static_cast<char>(0);
    }

    static SBE_CONSTEXPR char deviceIdMinValue() SBE_NOEXCEPT {
        return static_cast<char>(32);
    }

    static SBE_CONSTEXPR char deviceIdMaxValue() SBE_NOEXCEPT {
        return static_cast<char>(126);
    }

    static SBE_CONSTEXPR std::size_t deviceIdEncodingLength() SBE_NOEXCEPT {
        return 32;
    }

    static SBE_CONSTEXPR std::uint64_t deviceIdLength() SBE_NOEXCEPT {
        return 32;
    }

    SBE_NODISCARD const char *deviceId() const SBE_NOEXCEPT {
        return m_buffer + m_offset + 80;
    }

    SBE_NODISCARD char *deviceId() SBE_NOEXCEPT {
        return m_buffer + m_offset + 80;
    }

    SBE_NODISCARD char deviceId(const std::uint64_t index) const {
        if (index >= 32) {
            throw std::runtime_error("index out of range for deviceId [E104]");
        }

        char val;
        std::memcpy(&val, m_buffer + m_offset + 80 + (index * 1), sizeof(char));
        return (val);
    }

    AppHeader &deviceId(const std::uint64_t index, const char value) {
        if (index >= 32) {
            throw std::runtime_error("index out of range for deviceId [E105]");
        }

        char val = (value);
        std::memcpy(m_buffer + m_offset + 80 + (index * 1), &val, sizeof(char));
        return *this;
    }

    std::uint64_t getDeviceId(char *const dst,
                              const std::uint64_t length) const {
        if (length > 32) {
            throw std::runtime_error("length too large for getDeviceId [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 80,
                    sizeof(char) * static_cast<std::size_t>(length));
        return length;
    }

#ifdef SBE_USE_SPAN
    SBE_NODISCARD std::span<const char> getDeviceIdAsSpan() const SBE_NOEXCEPT {
        const char *buffer = m_buffer + m_offset + 80;
        return std::span<const char>(reinterpret_cast<const char *>(buffer),
                                     32);
    }
#endif

#ifdef SBE_USE_SPAN
    template <std::size_t N>
    AppHeader &putDeviceId(std::span<const char, N> src) SBE_NOEXCEPT {
        static_assert(N <= 32, "array too large for putDeviceId");

        std::memcpy(m_buffer + m_offset + 80, src.data(), sizeof(char) * N);
        for (std::size_t start = N; start < 32; ++start) {
            m_buffer[m_offset + 80 + start] = 0;
        }

        return *this;
    }
#endif

#ifdef SBE_USE_SPAN
    template <typename T>
    AppHeader &putDeviceId(T &&src) SBE_NOEXCEPT
        requires(std::is_pointer_v<std::remove_reference_t<T>> &&
                 !std::is_array_v<std::remove_reference_t<T>>)
#else
    AppHeader &putDeviceId(const char *const src) SBE_NOEXCEPT
#endif
    {
        std::memcpy(m_buffer + m_offset + 80, src, sizeof(char) * 32);
        return *this;
    }

#ifdef SBE_USE_SPAN
    template <std::size_t N>
    AppHeader &putDeviceId(const char (&src)[N]) SBE_NOEXCEPT {
        return putDeviceId(std::span<const char, N>(src));
    }
#endif

    SBE_NODISCARD std::string getDeviceIdAsString() const {
        const char *buffer = m_buffer + m_offset + 80;
        std::size_t length = 0;

        for (; length < 32 && *(buffer + length) != '\0'; ++length);
        std::string result(buffer, length);

        return result;
    }

    std::string getDeviceIdAsJsonEscapedString() {
        std::ostringstream oss;
        std::string s = getDeviceIdAsString();

        for (const auto c : s) {
            switch (c) {
                case '"':
                    oss << "\\\"";
                    break;
                case '\\':
                    oss << "\\\\";
                    break;
                case '\b':
                    oss << "\\b";
                    break;
                case '\f':
                    oss << "\\f";
                    break;
                case '\n':
                    oss << "\\n";
                    break;
                case '\r':
                    oss << "\\r";
                    break;
                case '\t':
                    oss << "\\t";
                    break;

                default:
                    if ('\x00' <= c && c <= '\x1f') {
                        oss << "\\u" << std::hex << std::setw(4)
                            << std::setfill('0') << (int)(c);
                    } else {
                        oss << c;
                    }
            }
        }

        return oss.str();
    }

#ifdef SBE_USE_STRING_VIEW
    SBE_NODISCARD std::string_view getDeviceIdAsStringView() const
        SBE_NOEXCEPT {
        const char *buffer = m_buffer + m_offset + 80;
        std::size_t length = 0;

        for (; length < 32 && *(buffer + length) != '\0'; ++length);
        std::string_view result(buffer, length);

        return result;
    }
#endif

#ifdef SBE_USE_STRING_VIEW
    AppHeader &putDeviceId(const std::string_view str) {
        const std::size_t srcLength = str.length();
        if (srcLength > 32) {
            throw std::runtime_error("string too large for putDeviceId [E106]");
        }

        std::memcpy(m_buffer + m_offset + 80, str.data(), srcLength);
        for (std::size_t start = srcLength; start < 32; ++start) {
            m_buffer[m_offset + 80 + start] = 0;
        }

        return *this;
    }
#else
    AppHeader &putDeviceId(const std::string &str) {
        const std::size_t srcLength = str.length();
        if (srcLength > 32) {
            throw std::runtime_error("string too large for putDeviceId [E106]");
        }

        std::memcpy(m_buffer + m_offset + 80, str.c_str(), srcLength);
        for (std::size_t start = srcLength; start < 32; ++start) {
            m_buffer[m_offset + 80 + start] = 0;
        }

        return *this;
    }
#endif

    SBE_NODISCARD static const char *deviceNameMetaAttribute(
        const MetaAttribute metaAttribute) SBE_NOEXCEPT {
        switch (metaAttribute) {
            case MetaAttribute::PRESENCE:
                return "required";
            default:
                return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t deviceNameId() SBE_NOEXCEPT {
        return -1;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t deviceNameSinceVersion()
        SBE_NOEXCEPT {
        return 0;
    }

    SBE_NODISCARD bool deviceNameInActingVersion() SBE_NOEXCEPT { return true; }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t deviceNameEncodingOffset()
        SBE_NOEXCEPT {
        return 112;
    }

    static SBE_CONSTEXPR char deviceNameNullValue() SBE_NOEXCEPT {
        return static_cast<char>(0);
    }

    static SBE_CONSTEXPR char deviceNameMinValue() SBE_NOEXCEPT {
        return static_cast<char>(32);
    }

    static SBE_CONSTEXPR char deviceNameMaxValue() SBE_NOEXCEPT {
        return static_cast<char>(126);
    }

    static SBE_CONSTEXPR std::size_t deviceNameEncodingLength() SBE_NOEXCEPT {
        return 64;
    }

    static SBE_CONSTEXPR std::uint64_t deviceNameLength() SBE_NOEXCEPT {
        return 64;
    }

    SBE_NODISCARD const char *deviceName() const SBE_NOEXCEPT {
        return m_buffer + m_offset + 112;
    }

    SBE_NODISCARD char *deviceName() SBE_NOEXCEPT {
        return m_buffer + m_offset + 112;
    }

    SBE_NODISCARD char deviceName(const std::uint64_t index) const {
        if (index >= 64) {
            throw std::runtime_error(
                "index out of range for deviceName [E104]");
        }

        char val;
        std::memcpy(&val, m_buffer + m_offset + 112 + (index * 1),
                    sizeof(char));
        return (val);
    }

    AppHeader &deviceName(const std::uint64_t index, const char value) {
        if (index >= 64) {
            throw std::runtime_error(
                "index out of range for deviceName [E105]");
        }

        char val = (value);
        std::memcpy(m_buffer + m_offset + 112 + (index * 1), &val,
                    sizeof(char));
        return *this;
    }

    std::uint64_t getDeviceName(char *const dst,
                                const std::uint64_t length) const {
        if (length > 64) {
            throw std::runtime_error(
                "length too large for getDeviceName [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 112,
                    sizeof(char) * static_cast<std::size_t>(length));
        return length;
    }

#ifdef SBE_USE_SPAN
    SBE_NODISCARD std::span<const char> getDeviceNameAsSpan() const
        SBE_NOEXCEPT {
        const char *buffer = m_buffer + m_offset + 112;
        return std::span<const char>(reinterpret_cast<const char *>(buffer),
                                     64);
    }
#endif

#ifdef SBE_USE_SPAN
    template <std::size_t N>
    AppHeader &putDeviceName(std::span<const char, N> src) SBE_NOEXCEPT {
        static_assert(N <= 64, "array too large for putDeviceName");

        std::memcpy(m_buffer + m_offset + 112, src.data(), sizeof(char) * N);
        for (std::size_t start = N; start < 64; ++start) {
            m_buffer[m_offset + 112 + start] = 0;
        }

        return *this;
    }
#endif

#ifdef SBE_USE_SPAN
    template <typename T>
    AppHeader &putDeviceName(T &&src) SBE_NOEXCEPT
        requires(std::is_pointer_v<std::remove_reference_t<T>> &&
                 !std::is_array_v<std::remove_reference_t<T>>)
#else
    AppHeader &putDeviceName(const char *const src) SBE_NOEXCEPT
#endif
    {
        std::memcpy(m_buffer + m_offset + 112, src, sizeof(char) * 64);
        return *this;
    }

#ifdef SBE_USE_SPAN
    template <std::size_t N>
    AppHeader &putDeviceName(const char (&src)[N]) SBE_NOEXCEPT {
        return putDeviceName(std::span<const char, N>(src));
    }
#endif

    SBE_NODISCARD std::string getDeviceNameAsString() const {
        const char *buffer = m_buffer + m_offset + 112;
        std::size_t length = 0;

        for (; length < 64 && *(buffer + length) != '\0'; ++length);
        std::string result(buffer, length);

        return result;
    }

    std::string getDeviceNameAsJsonEscapedString() {
        std::ostringstream oss;
        std::string s = getDeviceNameAsString();

        for (const auto c : s) {
            switch (c) {
                case '"':
                    oss << "\\\"";
                    break;
                case '\\':
                    oss << "\\\\";
                    break;
                case '\b':
                    oss << "\\b";
                    break;
                case '\f':
                    oss << "\\f";
                    break;
                case '\n':
                    oss << "\\n";
                    break;
                case '\r':
                    oss << "\\r";
                    break;
                case '\t':
                    oss << "\\t";
                    break;

                default:
                    if ('\x00' <= c && c <= '\x1f') {
                        oss << "\\u" << std::hex << std::setw(4)
                            << std::setfill('0') << (int)(c);
                    } else {
                        oss << c;
                    }
            }
        }

        return oss.str();
    }

#ifdef SBE_USE_STRING_VIEW
    SBE_NODISCARD std::string_view getDeviceNameAsStringView() const
        SBE_NOEXCEPT {
        const char *buffer = m_buffer + m_offset + 112;
        std::size_t length = 0;

        for (; length < 64 && *(buffer + length) != '\0'; ++length);
        std::string_view result(buffer, length);

        return result;
    }
#endif

#ifdef SBE_USE_STRING_VIEW
    AppHeader &putDeviceName(const std::string_view str) {
        const std::size_t srcLength = str.length();
        if (srcLength > 64) {
            throw std::runtime_error(
                "string too large for putDeviceName [E106]");
        }

        std::memcpy(m_buffer + m_offset + 112, str.data(), srcLength);
        for (std::size_t start = srcLength; start < 64; ++start) {
            m_buffer[m_offset + 112 + start] = 0;
        }

        return *this;
    }
#else
    AppHeader &putDeviceName(const std::string &str) {
        const std::size_t srcLength = str.length();
        if (srcLength > 64) {
            throw std::runtime_error(
                "string too large for putDeviceName [E106]");
        }

        std::memcpy(m_buffer + m_offset + 112, str.c_str(), srcLength);
        for (std::size_t start = srcLength; start < 64; ++start) {
            m_buffer[m_offset + 112 + start] = 0;
        }

        return *this;
    }
#endif

    SBE_NODISCARD static const char *deviceIpMetaAttribute(
        const MetaAttribute metaAttribute) SBE_NOEXCEPT {
        switch (metaAttribute) {
            case MetaAttribute::PRESENCE:
                return "required";
            default:
                return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t deviceIpId() SBE_NOEXCEPT { return -1; }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t deviceIpSinceVersion()
        SBE_NOEXCEPT {
        return 0;
    }

    SBE_NODISCARD bool deviceIpInActingVersion() SBE_NOEXCEPT { return true; }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t deviceIpEncodingOffset()
        SBE_NOEXCEPT {
        return 176;
    }

    static SBE_CONSTEXPR char deviceIpNullValue() SBE_NOEXCEPT {
        return static_cast<char>(0);
    }

    static SBE_CONSTEXPR char deviceIpMinValue() SBE_NOEXCEPT {
        return static_cast<char>(32);
    }

    static SBE_CONSTEXPR char deviceIpMaxValue() SBE_NOEXCEPT {
        return static_cast<char>(126);
    }

    static SBE_CONSTEXPR std::size_t deviceIpEncodingLength() SBE_NOEXCEPT {
        return 48;
    }

    static SBE_CONSTEXPR std::uint64_t deviceIpLength() SBE_NOEXCEPT {
        return 48;
    }

    SBE_NODISCARD const char *deviceIp() const SBE_NOEXCEPT {
        return m_buffer + m_offset + 176;
    }

    SBE_NODISCARD char *deviceIp() SBE_NOEXCEPT {
        return m_buffer + m_offset + 176;
    }

    SBE_NODISCARD char deviceIp(const std::uint64_t index) const {
        if (index >= 48) {
            throw std::runtime_error("index out of range for deviceIp [E104]");
        }

        char val;
        std::memcpy(&val, m_buffer + m_offset + 176 + (index * 1),
                    sizeof(char));
        return (val);
    }

    AppHeader &deviceIp(const std::uint64_t index, const char value) {
        if (index >= 48) {
            throw std::runtime_error("index out of range for deviceIp [E105]");
        }

        char val = (value);
        std::memcpy(m_buffer + m_offset + 176 + (index * 1), &val,
                    sizeof(char));
        return *this;
    }

    std::uint64_t getDeviceIp(char *const dst,
                              const std::uint64_t length) const {
        if (length > 48) {
            throw std::runtime_error("length too large for getDeviceIp [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 176,
                    sizeof(char) * static_cast<std::size_t>(length));
        return length;
    }

#ifdef SBE_USE_SPAN
    SBE_NODISCARD std::span<const char> getDeviceIpAsSpan() const SBE_NOEXCEPT {
        const char *buffer = m_buffer + m_offset + 176;
        return std::span<const char>(reinterpret_cast<const char *>(buffer),
                                     48);
    }
#endif

#ifdef SBE_USE_SPAN
    template <std::size_t N>
    AppHeader &putDeviceIp(std::span<const char, N> src) SBE_NOEXCEPT {
        static_assert(N <= 48, "array too large for putDeviceIp");

        std::memcpy(m_buffer + m_offset + 176, src.data(), sizeof(char) * N);
        for (std::size_t start = N; start < 48; ++start) {
            m_buffer[m_offset + 176 + start] = 0;
        }

        return *this;
    }
#endif

#ifdef SBE_USE_SPAN
    template <typename T>
    AppHeader &putDeviceIp(T &&src) SBE_NOEXCEPT
        requires(std::is_pointer_v<std::remove_reference_t<T>> &&
                 !std::is_array_v<std::remove_reference_t<T>>)
#else
    AppHeader &putDeviceIp(const char *const src) SBE_NOEXCEPT
#endif
    {
        std::memcpy(m_buffer + m_offset + 176, src, sizeof(char) * 48);
        return *this;
    }

#ifdef SBE_USE_SPAN
    template <std::size_t N>
    AppHeader &putDeviceIp(const char (&src)[N]) SBE_NOEXCEPT {
        return putDeviceIp(std::span<const char, N>(src));
    }
#endif

    SBE_NODISCARD std::string getDeviceIpAsString() const {
        const char *buffer = m_buffer + m_offset + 176;
        std::size_t length = 0;

        for (; length < 48 && *(buffer + length) != '\0'; ++length);
        std::string result(buffer, length);

        return result;
    }

    std::string getDeviceIpAsJsonEscapedString() {
        std::ostringstream oss;
        std::string s = getDeviceIpAsString();

        for (const auto c : s) {
            switch (c) {
                case '"':
                    oss << "\\\"";
                    break;
                case '\\':
                    oss << "\\\\";
                    break;
                case '\b':
                    oss << "\\b";
                    break;
                case '\f':
                    oss << "\\f";
                    break;
                case '\n':
                    oss << "\\n";
                    break;
                case '\r':
                    oss << "\\r";
                    break;
                case '\t':
                    oss << "\\t";
                    break;

                default:
                    if ('\x00' <= c && c <= '\x1f') {
                        oss << "\\u" << std::hex << std::setw(4)
                            << std::setfill('0') << (int)(c);
                    } else {
                        oss << c;
                    }
            }
        }

        return oss.str();
    }

#ifdef SBE_USE_STRING_VIEW
    SBE_NODISCARD std::string_view getDeviceIpAsStringView() const
        SBE_NOEXCEPT {
        const char *buffer = m_buffer + m_offset + 176;
        std::size_t length = 0;

        for (; length < 48 && *(buffer + length) != '\0'; ++length);
        std::string_view result(buffer, length);

        return result;
    }
#endif

#ifdef SBE_USE_STRING_VIEW
    AppHeader &putDeviceIp(const std::string_view str) {
        const std::size_t srcLength = str.length();
        if (srcLength > 48) {
            throw std::runtime_error("string too large for putDeviceIp [E106]");
        }

        std::memcpy(m_buffer + m_offset + 176, str.data(), srcLength);
        for (std::size_t start = srcLength; start < 48; ++start) {
            m_buffer[m_offset + 176 + start] = 0;
        }

        return *this;
    }
#else
    AppHeader &putDeviceIp(const std::string &str) {
        const std::size_t srcLength = str.length();
        if (srcLength > 48) {
            throw std::runtime_error("string too large for putDeviceIp [E106]");
        }

        std::memcpy(m_buffer + m_offset + 176, str.c_str(), srcLength);
        for (std::size_t start = srcLength; start < 48; ++start) {
            m_buffer[m_offset + 176 + start] = 0;
        }

        return *this;
    }
#endif

    SBE_NODISCARD static const char *locationMetaAttribute(
        const MetaAttribute metaAttribute) SBE_NOEXCEPT {
        switch (metaAttribute) {
            case MetaAttribute::PRESENCE:
                return "required";
            default:
                return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t locationId() SBE_NOEXCEPT { return -1; }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t locationSinceVersion()
        SBE_NOEXCEPT {
        return 0;
    }

    SBE_NODISCARD bool locationInActingVersion() SBE_NOEXCEPT { return true; }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t locationEncodingOffset()
        SBE_NOEXCEPT {
        return 224;
    }

    static SBE_CONSTEXPR char locationNullValue() SBE_NOEXCEPT {
        return static_cast<char>(0);
    }

    static SBE_CONSTEXPR char locationMinValue() SBE_NOEXCEPT {
        return static_cast<char>(32);
    }

    static SBE_CONSTEXPR char locationMaxValue() SBE_NOEXCEPT {
        return static_cast<char>(126);
    }

    static SBE_CONSTEXPR std::size_t locationEncodingLength() SBE_NOEXCEPT {
        return 64;
    }

    static SBE_CONSTEXPR std::uint64_t locationLength() SBE_NOEXCEPT {
        return 64;
    }

    SBE_NODISCARD const char *location() const SBE_NOEXCEPT {
        return m_buffer + m_offset + 224;
    }

    SBE_NODISCARD char *location() SBE_NOEXCEPT {
        return m_buffer + m_offset + 224;
    }

    SBE_NODISCARD char location(const std::uint64_t index) const {
        if (index >= 64) {
            throw std::runtime_error("index out of range for location [E104]");
        }

        char val;
        std::memcpy(&val, m_buffer + m_offset + 224 + (index * 1),
                    sizeof(char));
        return (val);
    }

    AppHeader &location(const std::uint64_t index, const char value) {
        if (index >= 64) {
            throw std::runtime_error("index out of range for location [E105]");
        }

        char val = (value);
        std::memcpy(m_buffer + m_offset + 224 + (index * 1), &val,
                    sizeof(char));
        return *this;
    }

    std::uint64_t getLocation(char *const dst,
                              const std::uint64_t length) const {
        if (length > 64) {
            throw std::runtime_error("length too large for getLocation [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 224,
                    sizeof(char) * static_cast<std::size_t>(length));
        return length;
    }

#ifdef SBE_USE_SPAN
    SBE_NODISCARD std::span<const char> getLocationAsSpan() const SBE_NOEXCEPT {
        const char *buffer = m_buffer + m_offset + 224;
        return std::span<const char>(reinterpret_cast<const char *>(buffer),
                                     64);
    }
#endif

#ifdef SBE_USE_SPAN
    template <std::size_t N>
    AppHeader &putLocation(std::span<const char, N> src) SBE_NOEXCEPT {
        static_assert(N <= 64, "array too large for putLocation");

        std::memcpy(m_buffer + m_offset + 224, src.data(), sizeof(char) * N);
        for (std::size_t start = N; start < 64; ++start) {
            m_buffer[m_offset + 224 + start] = 0;
        }

        return *this;
    }
#endif

#ifdef SBE_USE_SPAN
    template <typename T>
    AppHeader &putLocation(T &&src) SBE_NOEXCEPT
        requires(std::is_pointer_v<std::remove_reference_t<T>> &&
                 !std::is_array_v<std::remove_reference_t<T>>)
#else
    AppHeader &putLocation(const char *const src) SBE_NOEXCEPT
#endif
    {
        std::memcpy(m_buffer + m_offset + 224, src, sizeof(char) * 64);
        return *this;
    }

#ifdef SBE_USE_SPAN
    template <std::size_t N>
    AppHeader &putLocation(const char (&src)[N]) SBE_NOEXCEPT {
        return putLocation(std::span<const char, N>(src));
    }
#endif

    SBE_NODISCARD std::string getLocationAsString() const {
        const char *buffer = m_buffer + m_offset + 224;
        std::size_t length = 0;

        for (; length < 64 && *(buffer + length) != '\0'; ++length);
        std::string result(buffer, length);

        return result;
    }

    std::string getLocationAsJsonEscapedString() {
        std::ostringstream oss;
        std::string s = getLocationAsString();

        for (const auto c : s) {
            switch (c) {
                case '"':
                    oss << "\\\"";
                    break;
                case '\\':
                    oss << "\\\\";
                    break;
                case '\b':
                    oss << "\\b";
                    break;
                case '\f':
                    oss << "\\f";
                    break;
                case '\n':
                    oss << "\\n";
                    break;
                case '\r':
                    oss << "\\r";
                    break;
                case '\t':
                    oss << "\\t";
                    break;

                default:
                    if ('\x00' <= c && c <= '\x1f') {
                        oss << "\\u" << std::hex << std::setw(4)
                            << std::setfill('0') << (int)(c);
                    } else {
                        oss << c;
                    }
            }
        }

        return oss.str();
    }

#ifdef SBE_USE_STRING_VIEW
    SBE_NODISCARD std::string_view getLocationAsStringView() const
        SBE_NOEXCEPT {
        const char *buffer = m_buffer + m_offset + 224;
        std::size_t length = 0;

        for (; length < 64 && *(buffer + length) != '\0'; ++length);
        std::string_view result(buffer, length);

        return result;
    }
#endif

#ifdef SBE_USE_STRING_VIEW
    AppHeader &putLocation(const std::string_view str) {
        const std::size_t srcLength = str.length();
        if (srcLength > 64) {
            throw std::runtime_error("string too large for putLocation [E106]");
        }

        std::memcpy(m_buffer + m_offset + 224, str.data(), srcLength);
        for (std::size_t start = srcLength; start < 64; ++start) {
            m_buffer[m_offset + 224 + start] = 0;
        }

        return *this;
    }
#else
    AppHeader &putLocation(const std::string &str) {
        const std::size_t srcLength = str.length();
        if (srcLength > 64) {
            throw std::runtime_error("string too large for putLocation [E106]");
        }

        std::memcpy(m_buffer + m_offset + 224, str.c_str(), srcLength);
        for (std::size_t start = srcLength; start < 64; ++start) {
            m_buffer[m_offset + 224 + start] = 0;
        }

        return *this;
    }
#endif

    template <typename CharT, typename Traits>
    friend std::basic_ostream<CharT, Traits> &operator<<(
        std::basic_ostream<CharT, Traits> &builder, AppHeader &writer) {
        builder << '{';
        builder << R"("version": )";
        builder << +writer.version();

        builder << ", ";
        builder << R"("messageType": )";
        builder << '"' << writer.getMessageTypeAsJsonEscapedString().c_str()
                << '"';

        builder << ", ";
        builder << R"("messageId": )";
        builder << '"' << writer.getMessageIdAsJsonEscapedString().c_str()
                << '"';

        builder << ", ";
        builder << R"("messageCode": )";
        builder << '"' << writer.getMessageCodeAsJsonEscapedString().c_str()
                << '"';

        builder << ", ";
        builder << R"("sequence": )";
        builder << +writer.sequence();

        builder << ", ";
        builder << R"("timestamp": )";
        builder << +writer.timestamp();

        builder << ", ";
        builder << R"("statusCode": )";
        builder << +writer.statusCode();

        builder << ", ";
        builder << R"("deviceId": )";
        builder << '"' << writer.getDeviceIdAsJsonEscapedString().c_str()
                << '"';

        builder << ", ";
        builder << R"("deviceName": )";
        builder << '"' << writer.getDeviceNameAsJsonEscapedString().c_str()
                << '"';

        builder << ", ";
        builder << R"("deviceIp": )";
        builder << '"' << writer.getDeviceIpAsJsonEscapedString().c_str()
                << '"';

        builder << ", ";
        builder << R"("location": )";
        builder << '"' << writer.getLocationAsJsonEscapedString().c_str()
                << '"';

        builder << '}';

        return builder;
    }
};

}  // namespace SBE

#endif
