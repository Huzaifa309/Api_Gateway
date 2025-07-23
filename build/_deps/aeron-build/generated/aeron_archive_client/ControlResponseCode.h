/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _AERON_ARCHIVE_CLIENT_CONTROLRESPONSECODE_CXX_H_
#define _AERON_ARCHIVE_CLIENT_CONTROLRESPONSECODE_CXX_H_

#if !defined(__STDC_LIMIT_MACROS)
#  define __STDC_LIMIT_MACROS 1
#endif

#include <cstdint>
#include <iomanip>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <sstream>
#include <string>

#define SBE_NULLVALUE_INT8 (std::numeric_limits<std::int8_t>::min)()
#define SBE_NULLVALUE_INT16 (std::numeric_limits<std::int16_t>::min)()
#define SBE_NULLVALUE_INT32 (std::numeric_limits<std::int32_t>::min)()
#define SBE_NULLVALUE_INT64 (std::numeric_limits<std::int64_t>::min)()
#define SBE_NULLVALUE_UINT8 (std::numeric_limits<std::uint8_t>::max)()
#define SBE_NULLVALUE_UINT16 (std::numeric_limits<std::uint16_t>::max)()
#define SBE_NULLVALUE_UINT32 (std::numeric_limits<std::uint32_t>::max)()
#define SBE_NULLVALUE_UINT64 (std::numeric_limits<std::uint64_t>::max)()

namespace aeron {
namespace archive {
namespace client {

class ControlResponseCode
{
public:
    enum Value
    {
        OK = INT32_C(0),
        ERROR = INT32_C(1),
        RECORDING_UNKNOWN = INT32_C(2),
        SUBSCRIPTION_UNKNOWN = INT32_C(3),
        NULL_VALUE = INT32_MIN
    };

    static ControlResponseCode::Value get(const std::int32_t value)
    {
        switch (value)
        {
            case INT32_C(0): return OK;
            case INT32_C(1): return ERROR;
            case INT32_C(2): return RECORDING_UNKNOWN;
            case INT32_C(3): return SUBSCRIPTION_UNKNOWN;
            case INT32_MIN: return NULL_VALUE;
        }

        throw std::runtime_error("unknown value for enum ControlResponseCode [E103]");
    }

    static const char *c_str(const ControlResponseCode::Value value)
    {
        switch (value)
        {
            case OK: return "OK";
            case ERROR: return "ERROR";
            case RECORDING_UNKNOWN: return "RECORDING_UNKNOWN";
            case SUBSCRIPTION_UNKNOWN: return "SUBSCRIPTION_UNKNOWN";
            case NULL_VALUE: return "NULL_VALUE";
        }

        throw std::runtime_error("unknown value for enum ControlResponseCode [E103]:");
    }

    template<typename CharT, typename Traits>
    friend std::basic_ostream<CharT, Traits> & operator << (
        std::basic_ostream<CharT, Traits> &os, ControlResponseCode::Value m)
    {
        return os << ControlResponseCode::c_str(m);
    }
};

}
}
}

#endif
