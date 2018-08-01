#include "torrebis_bencode.h"

#include <sstream>

namespace torrebis { namespace bencode {

    namespace {
        template<typename Iterator, typename OutInt>
        bool parse_int(Iterator begin, const Iterator end, OutInt &out_int, Iterator &out_end) {
            if (begin == end) {
                return false;
            }
            bool negative = false;
            if (*begin == '-') {
                negative = true;
                begin++;
                if (begin == end) {
                    return false;
                }
            } else if (static_cast<char>(*begin) == '0') {
                out_end = ++begin;
                out_int = OutInt(0);
                return true;
            }
            char first = static_cast<char>(*begin);
            if (!('1' <= first && first <= '9')) {
                return false;
            }
            OutInt i((negative ? -1 : +1) * static_cast<int>(first - '0'));
            for (begin++;
                 begin != end && ('0' <= static_cast<char>(*begin) && static_cast<char>(*begin) <= '9'); begin++) {
                i = i * 10 + ((negative ? -1 : +1) * static_cast<int>(static_cast<char>(*begin) - '0'));
            }
            out_int = ::std::move(i);
            out_end = begin;
            return true;
        }

        bool
        str_less_or_eq(const char *a_start, const char *const a_end, const char *b_start, const char *const b_end) {
            for (; a_start < a_end && b_start < b_end && *a_start == *b_end; a_start++, b_start++);
            if (a_start < a_end) return true;  // if (b_start < b_end) a == b else a < b
            if (b_start < b_end) return false;  // b > a
            return *a_start <= *b_start;
        }

        template<typename T, typename Int>
        bool
        read_string_fast(const T *const bencoded, ::std::size_t &start, const ::std::size_t size, const char *&s_start,
                         const char *&s_end) {
            if (start >= size || static_cast<char>(bencoded[start]) == '-') return false;
            Int length;
            const T* end;
            bool success = parse_int<const T*, Int>(bencoded + start, bencoded + size, length, end);
            if (!success || (start = static_cast<::std::size_t>(end - bencoded)) >= size ||
                static_cast<char>(bencoded[start++]) != ':') {
                return false;
            }
            ::std::size_t end_index = start + static_cast<::std::size_t>(length);
            if (end_index > size) {
                return false;
            }
            s_start = bencoded + start;
            s_end = bencoded + end_index;
            start = end_index;
            return true;
        }

        template<typename BasicData, typename T>
        BasicData bdecode_loop(const T *const bencoded, ::std::size_t &start, const ::std::size_t size) {
            if (start >= size) {
                throw decoding_error("Bencoded data ends early.");
            }
            switch (static_cast<char>(bencoded[start])) {
                case 'i': {
                    // Integer (iXXXe)
                    typename BasicData::int_type i;
                    const T *end;
                    bool success = parse_int<const T*, typename BasicData::int_type>(
                        static_cast<const char *>(bencoded + start + 1), bencoded + size, i, end);
                    if (!success || (start = static_cast<::std::size_t>(end - bencoded)) >= size ||
                        static_cast<char>(bencoded[start++]) != 'e') {
                        throw decoding_error("Expecting an integer in bencoded data.");
                    }
                    return BasicData(make_int, i);
                }
                case 'l': {
                    // List (lXXXe)
                    typename BasicData::list_type l;
                    start++;
                    while (true) {
                        if (start >= size) {
                            throw decoding_error("Expecting `e` to end list in bencoded data.");
                        }
                        if (static_cast<char>(bencoded[start]) == 'e') {
                            break;
                        }
                        l.push_back(bdecode_loop<BasicData, T>(bencoded, start, size));
                    }
                    start++;
                    return BasicData(make_list, l);
                }
                case 'd': {
                    // Dict (dSTRVALUEe)
                    using key_type = typename BasicData::str_type;
                    typename BasicData::dict_type d;
                    start++;
                    bool first = true;
                    const char *last_start;
                    const char *last_end;
                    while (true) {
                        if (start >= size) {
                            throw decoding_error("Expecting `d` to end dictionary in bencoded data.");
                        }
                        if (static_cast<char>(bencoded[start]) == 'e') {
                            break;
                        }
                        const char *k_start;
                        const char *k_end;
                        bool success = read_string_fast<T, typename BasicData::int_type>(bencoded, start, size, k_start,
                                                                                         k_end);
                        if (!success) {
                            throw decoding_error("Expecting string key for dictionary in bencoded data.");
                        }
                        if (first) {
                            first = false;
                        } else if (!str_less_or_eq(k_start, k_end, last_start, last_end)) {
                            throw decoding_error(
                                "Keys in dictionary must be strictly ascending in bencoded data.");
                        }
                        key_type key(k_start, static_cast<::std::size_t>(k_end - k_start));
                        d[key] = bdecode_loop<BasicData, T>(bencoded, start, size);
                        last_start = k_start;
                        last_end = k_end;
                    }
                    start++;
                    return BasicData(make_dict, d);
                }
                case '1': case '2': case '3':
                case '4': case '5': case '6':
                case '7': case '8': case '9': {
                    // string
                    const char *s_start;
                    const char *s_end;
                    bool success = read_string_fast<T, typename BasicData::int_type>(bencoded, start, size, s_start, s_end);
                    if (!success) {
                        throw decoding_error("Expecting string in bencoded data.");
                    }
                    using str_type = typename BasicData::str_type;
                    str_type s(s_start, static_cast<::std::size_t>(s_end - s_start));
                    return BasicData(make_str, s);
                }
                default:
                    throw decoding_error("Unknown value in bencoded data.");

            }
        }

        template<typename Iterator, typename Int>
        bool read_string(Iterator& begin, const Iterator& end, ::std::string out_string) {
/*            if (start >= size || static_cast<char>(bencoded[start]) == '-') return false;
            Int length;
            const T* end;
            bool success = parse_int<T*, Int>(bencoded + start, bencoded + size, length, end);
            if (!success || (start = static_cast<::std::size_t>(end - bencoded)) >= size ||
                static_cast<char>(bencoded[start++]) != ':') {
                return false;
            }
            ::std::size_t end_index = start + static_cast<::std::size_t>(length);
            if (end_index > size) {
                return false;
            }
            s_start = bencoded + start;
            s_end = bencoded + end_index;
            start = end_index;
            return true; */
            if (begin == end || static_cast<char>(*begin) == '-') return false;
            Int length_;
            const Iterator* int_end;
            bool success = parse_int<Iterator, Int>(begin, end, length_, int_end);
            if (!success || (begin = int_end) == end || static_cast<char>(*begin++) != ':')  {
                return false;
            }
            ::std::size_t length = static_cast<::std::size_t>(length_);
            out_string.reserve(length);
            for (; length-- > 0; ) {
                if (begin == end) {
                    return false;
                }
                out_string.push_back(*begin++);
            }
            return true;
        }

        template<typename BasicData, typename Iterator>
        BasicData bdecode_iterator_loop(Iterator& begin, const Iterator& end) {
            if (begin == end) {
                throw decoding_error("Bencoded data ends early.");
            }
            switch (static_cast<char>(*begin)) {
                case 'i': {
                    // Integer (iXXXe)
                    typename BasicData::int_type i;
                    const Iterator int_end;
                    bool success = parse_int<Iterator, typename BasicData::int_type>(
                        begin, end, i, int_end);
                    if (!success || (begin = int_end) == end ||
                        static_cast<char>(*(begin++)) != 'e') {
                        throw decoding_error("Expecting an integer in bencoded data.");
                    }
                    return BasicData(make_int, i);
                }
                case 'l': {
                    // List (lXXXe)
                    typename BasicData::list_type l;
                    begin++;
                    while (true) {
                        if (begin == end) {
                            throw decoding_error("Expecting `e` to end list in bencoded data.");
                        }
                        if (static_cast<char>(*begin) == 'e') {
                            break;
                        }
                        l.push_back(bdecode_iterator_loop<BasicData, Iterator>(begin, end));
                    }
                    begin++;
                    return BasicData(make_list, l);
                }
                case 'd': {
                    // Dict (dSTRVALUEe)
                    using key_type = typename BasicData::str_type;
                    typename BasicData::dict_type d;
                    begin++;
                    bool first = true;
                    ::std::string last;
                    while (true) {
                        if (begin == end) {
                            throw decoding_error("Expecting `d` to end dictionary in bencoded data.");
                        }
                        if (static_cast<char>(*begin) == 'e') {
                            break;
                        }
                        ::std::string key;
                        bool success = read_string<Iterator, typename BasicData::int_type>(begin, end, key);
                        if (!success) {
                            throw decoding_error("Expecting string key for dictionary in bencoded data.");
                        }
                        if (first) {
                            first = false;
                        } else if (key <= last) {
                            throw decoding_error(
                                "Keys in dictionary must be strictly ascending in bencoded data.");
                        }
                        TORREBIS_CONSTEXPR_IF (::std::is_same<key_type, ::std::string>::value) {
                            d[key] = bdecode_iterator_loop<BasicData, Iterator>(begin, end);
                        } else {
                            key_type real_key(key.c_str(), key.size());
                            d[real_key] = bdecode_iterator_loop<BasicData, Iterator>(begin, end);
                        }
                        last = key;
                    }
                    begin++;
                    return BasicData(make_dict, d);
                }
                case '1': case '2': case '3':
                case '4': case '5': case '6':
                case '7': case '8': case '9': {
                    // string
                    ::std::string s;
                    bool success = read_string<Iterator, typename BasicData::int_type>(begin, end, s);
                    if (!success) {
                        throw decoding_error("Expecting string in bencoded data.");
                    }
                    using str_type = typename BasicData::str_type;
                    TORREBIS_CONSTEXPR_IF (std::is_same<str_type, ::std::string>::value) {
                        return BasicData(make_str, s);
                    }
                    str_type real_s(s.c_str(), s.size());
                    return BasicData(make_str, s);
                }
                default:
                    throw decoding_error("Unknown value in bencoded data.");

            }
        }

        template<typename IntType>
        ::std::string int_to_string(IntType i) {
            TORREBIS_CONSTEXPR_IF (helpers::detail::is_any<IntType, int, long, long long, unsigned, unsigned long long>::value) {
                return ::std::to_string(i);
            }
            TORREBIS_CONSTEXPR_IF (helpers::detail::is_any<IntType, float, double, long double>::value) {
                return ::std::to_string(static_cast<intmax_t>(i));
            }
            ::std::ostringstream o;
            o << i;
            return o.str();
        }

        template<typename BasicData>
        void serialise_to_string(const BasicData &data, ::std::string &bencoded) {
            switch (data.get_tag()) {
                case DataTag::STR: {
                    typename BasicData::str_type &str = *data.get_str();
                    bencoded += int_to_string(str.size());
                    bencoded.push_back(':');
                    bencoded += str;
                    return;
                }
                case DataTag::INT:
                    bencoded.push_back('i');
                    bencoded += int_to_string(*data.get_int());
                    bencoded.push_back('e');
                    return;
                case DataTag::LIST:
                    bencoded.push_back('l');
                    for (auto &i : *data.get_list()) {
                        serialise_to_string(i, bencoded);
                    }
                    bencoded.push_back('e');
                    return;
                case DataTag::DICT:
                    bencoded.push_back('d');
                    for (auto &i : *data.get_dict()) {
                        const typename BasicData::str_type &key = i.first;
                        bencoded += int_to_string(key.size());
                        bencoded.push_back(':');
                        bencoded += key;
                        serialise_to_string(i.second, bencoded);
                    }
                    bencoded.push_back('e');
                    return;
                default:
                    return;
            }
        }

        template<typename BasicData>
        void serialise_to_stream(const BasicData &data, ::std::ostream &bencoded) {
            switch (data.get_tag()) {
                case DataTag::STR: {
                    typename BasicData::str_type &str = *data.get_str();
                    bencoded << int_to_string(str.size());
                    bencoded << ::std::string(":");
                    bencoded << str;
                    return;
                }
                case DataTag::INT:
                    bencoded << ::std::string("i");
                    bencoded << int_to_string(*data.get_int());
                    bencoded << ::std::string("e");
                    return;
                case DataTag::LIST:
                    bencoded << ::std::string("l");
                    for (auto &i : *data.get_list()) {
                        serialise_to_stream(i, bencoded);
                    }
                    bencoded << ::std::string("e");
                    return;
                case DataTag::DICT:
                    bencoded << ::std::string("d");
                    for (auto &i : *data.get_dict()) {
                        const typename BasicData::str_type &key = i.first;
                        bencoded << int_to_string(key.size());
                        bencoded << ::std::string(":");
                        bencoded += key;
                        serialise_to_stream(i.second, bencoded);
                    }
                    bencoded << ::std::string("e");
                    return;
                default:
                    return;
            }
        }
    }

    template<typename BasicData, typename T>
    BasicData bdecode(const T *const bencoded, const ::std::size_t size) {
        static_assert(
            helpers::detail::is_any<T, char, signed char, unsigned char, ::std::uint8_t, ::std::int8_t>::value,
            "Must bdecode a pointer to a byte-like value");
        static_assert(helpers::detail::is_basic_data<BasicData>::value,
                      "Must bdecode<torrebis::bencode::basic_data<...>>.");

        if (bencoded == nullptr) {
            throw decoding_error("bdecode(nullptr) called");
        }

        ::std::size_t start = 0;
        BasicData dat(bdecode_loop<BasicData, T>(bencoded, start, size));
        if (start != size) {
            throw decoding_error("bencoded data ends early.");
        }
        return dat;
    }

    template<typename BasicData, typename Iterator>
    BasicData bdecode(Iterator begin, const Iterator end) {
        static_assert(
            helpers::detail::is_any<typename ::std::iterator_traits<Iterator>::value_type, char, signed char, unsigned char, ::std::uint8_t, ::std::int8_t>::value,
            "Must bdecode an iterable to a byte-like value");
        static_assert(helpers::detail::is_basic_data<BasicData>::value,
                      "Must bdecode<::torrebis::bencode::basic_data<...>>.");

        bdecode_iterator_loop(begin, end);
        if (begin != end) {
            throw decoding_error("bencoded data ends early.");
        }

        return BasicData(make_int, 0);
    }

    template<typename BasicData>
    ::std::string bencode(const BasicData &dat) {
        static_assert(helpers::detail::is_basic_data<BasicData>::value,
                      "Must bencode<::torrebis::bencode::basic_data<...>>");
        ::std::string bencoded;
        bencoded.reserve(dat.serialised_length());
        serialise_to_string(dat, bencoded);
        return bencoded;
    }

    template<typename BasicData>
    void bencode_to_stream(const BasicData dat, ::std::ostream &stream) {
        static_assert(helpers::detail::is_basic_data<BasicData>::value,
                      "Must bencode<::torrebis::bencode::basic_data<...>>");
        serialise_to_stream(dat, stream);
    }

} }  // namespace torrebis::bencode
