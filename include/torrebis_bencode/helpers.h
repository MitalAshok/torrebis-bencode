// Various macros to enable stuff for newer C++ versions

#ifndef TORREBIS_BENCODE_HELPERS_H_
#define TORREBIS_BENCODE_HELPERS_H_

#include <type_traits>
#include <cstddef>
#include <cstring>
#include <algorithm>
#include <string>
#include <vector>


// TODO: Update value of __cplusplus for 2020
#if defined(__cplusplus) && __cplusplus >= 202000L
// C++ 20 or higher
#define TORREBIS_HAS_IS_NOTHROW_CONVERTIBLE
#else

#endif

#if defined(__cplusplus) && __cplusplus >= 201703L
// C++ 17 or higher
#define TORREBIS_CONSTEXPR_IF if constexpr
#define TORREBIS_STD_ARRAY_HAS_DATA

#include <array>

#else
#define TORREBIS_CONSTEXPR_IF if
#endif

#if defined(__cplusplus) && __cplusplus >= 201402L
// C++ 14 or higher
#define TORREBIS_VARIABLE_TEMPLATES
#else

#endif

#if defined(__cplusplus) && __cplusplus >= 201103L
// C++ 11 or higher (Required)

#else

#endif

#if !defined(__cplusplus) || __cplusplus < 201103L
// Less than C++ 11 or undetected
#endif

namespace torrebis { namespace bencode {
    namespace helpers {
        namespace detail {
            template<class To, class... From>
            struct all_convertible;

            template<class To>
            struct all_convertible<To> : ::std::true_type {};

            template<class To, class From, class... Rest>
            struct all_convertible<To, From, Rest...> : ::std::integral_constant<bool,
                ::std::is_convertible<From, To>::value && all_convertible<To, Rest...>::value
            > {};

            template<typename T>
            struct remove_cv_ref {
                using type = typename ::std::remove_cv<typename ::std::remove_reference<T>::type>::type;
            };

            template<class T, class U>
            struct is_same_decayed : ::std::integral_constant<bool, ::std::is_same<
                typename remove_cv_ref<T>::type, typename remove_cv_ref<U>::type
            >::value> {};

            template<class T, class... All>
            struct all_same;

            template<class T>
            struct all_same<T> : ::std::true_type {};

            template<class T, class U, class... Rest>
            struct all_same<T, U, Rest...> : ::std::integral_constant<bool, is_same_decayed<T, U>::value && all_same<T, Rest...>::value> {};

            template<typename T, typename... All>
            struct is_any;

            template<typename T>
            struct is_any<T> : ::std::false_type {};

            template<typename T, typename U, typename... Rest>
            struct is_any<T, U, Rest...> : ::std::integral_constant<bool, is_same_decayed<T, U>::value || is_any<T, Rest...>::value> {};

            template<typename T>
            struct is_byte : ::std::integral_constant<bool, is_any<T,
                char, unsigned char, signed char,
                ::std::uint8_t, ::std::int8_t
            >::value> {};

            template<class T = char>
            static bool key_less_than(const T* const str_a, const ::std::size_t a_size, const T* const str_b, ::std::size_t b_size) {
                int cmp = ::std::memcmp(str_a, str_b, ::std::min(a_size, b_size));
                if (cmp == 0) {
                    return a_size < b_size;
                }
                return cmp < 0;
            }

            template<class... Args>
            struct path_el_str_constructor_enable_if;

            template<>
            struct path_el_str_constructor_enable_if<> : ::std::enable_if<false> {};

            template<class T, class U, class...Args>
            struct path_el_str_constructor_enable_if<T, U, Args...> : ::std::enable_if<true> {};

            template<class T>
            struct path_el_str_constructor_enable_if<T> : ::std::enable_if<!::std::is_convertible<T, ::std::size_t>::value> {};

            template<class CharT, class StringT>
            struct has_char_t : ::std::false_type {};

            template<class CharT, class CharTraits>
            struct has_char_t<CharT, ::std::basic_string<CharT, CharTraits>> : ::std::true_type {};

            template<class CharT, class... Rest>
            struct has_char_t<CharT, ::std::vector<CharT, Rest...>> : ::std::true_type {};

#ifdef TORREBIS_STD_ARRAY_HAS_DATA
            template<class CharT, ::std::size_t N>
            struct has_char_t<CharT, ::std::array<CharT, N>> : ::std::true_type {};
#endif


        }  // namespace torrebis::bencode::helpers::detail  (Private)
    }  // namespace torrebis::bencode::helpers
} }  // namespace torrebis::bencode

#endif  // TORREBIS_BENCODE_HELPERS_H_
