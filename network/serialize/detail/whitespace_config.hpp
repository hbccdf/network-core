#pragma once
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/info_parser.hpp>

#ifndef CUSTOM_WHITESPACE_TABLE

#ifndef WHITESPACE_TAB
#define WHITESPACE_TAB 1
#endif

#ifndef WHITESPACE_CR
#define WHITESPACE_CR 1
#endif

#ifndef WHITESPACE_LF
#define WHITESPACE_LF 1
#endif

#ifndef WHITESPACE_SPACE
#define WHITESPACE_SPACE 1
#endif

namespace boost {
    namespace property_tree {
        namespace detail {
            namespace rapidxml {
                namespace internal
                {
                    // Whitespace (space \n \r \t)
                    template<>
                    const unsigned char lookup_tables<0>::lookup_whitespace[256] =
                    {
                        // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
                        0,  0,  0,  0,  0,  0,  0,  0,  0,  WHITESPACE_TAB,  WHITESPACE_LF,  0,  0,  WHITESPACE_CR,  0,  0,  // 0
                        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 1
                        WHITESPACE_SPACE,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 2
                        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 3
                        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 4
                        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 5
                        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 6
                        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 7
                        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 8
                        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 9
                        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // A
                        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // B
                        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // C
                        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // D
                        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // E
                        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   // F
                    };
                }
            }
        }
    }
}

#else
namespace boost {
    namespace property_tree {
        namespace detail {
            namespace rapidxml {
                namespace internal
                {
                    CUSTOM_WHITESPACE_TABLE
                }
            }
        }
    }
}
#endif