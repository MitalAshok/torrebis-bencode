#ifndef TORREBIS_BENCODE_TYPE_TAGS_H_
#define TORREBIS_BENCODE_TYPE_TAGS_H_

#include <cstdint>
#include <type_traits>


namespace torrebis { namespace bencode {
    enum class DataTag : ::std::int8_t {
        STR = 0,
        INT = 1,
        LIST = 2,
        DICT = 3,

        UNINITIALISED = -1
    };


    namespace helpers {
        namespace detail {
            enum MakeStr {
                make_str = static_cast<::std::int8_t>(0)
            };
            enum MakeInt {
                make_int = static_cast<::std::int8_t>(1)
            };
            enum MakeList {
                make_list = static_cast<::std::int8_t>(2)
            };
            enum MakeDict {
                make_dict = static_cast<::std::int8_t>(3)
            };

            template<typename T>
            struct make_specifier_to_tag;

            template<>
            struct make_specifier_to_tag<MakeStr> : ::std::integral_constant<DataTag, DataTag::STR> {
            };
            template<>
            struct make_specifier_to_tag<MakeInt> : ::std::integral_constant<DataTag, DataTag::INT> {
            };
            template<>
            struct make_specifier_to_tag<MakeList> : ::std::integral_constant<DataTag, DataTag::LIST> {
            };
            template<>
            struct make_specifier_to_tag<MakeDict> : ::std::integral_constant<DataTag, DataTag::DICT> {
            };

            template<DataTag tag>
            struct tag_to_make_specifier;

            template<>
            struct tag_to_make_specifier<DataTag::STR> : ::std::integral_constant<MakeStr, make_str> {
            };
            template<>
            struct tag_to_make_specifier<DataTag::INT> : ::std::integral_constant<MakeInt, make_int> {
            };
            template<>
            struct tag_to_make_specifier<DataTag::LIST> : ::std::integral_constant<MakeList, make_list> {
            };
            template<>
            struct tag_to_make_specifier<DataTag::DICT> : ::std::integral_constant<MakeDict, make_dict> {
            };
        }  // namespace torrebis::bencode::helpers::detail  (Private)

        template<DataTag tag, class StrType, class IntType, class ListType, class DictType, class UninitialisedType>
        struct tag_to_type;

        template<class StrType, class IntType, class ListType, class DictType, class UninitialisedType>
        struct tag_to_type<DataTag::STR, StrType, IntType, ListType, DictType, UninitialisedType> {
            using type = StrType;
        };

        template<class StrType, class IntType, class ListType, class DictType, class UninitialisedType>
        struct tag_to_type<DataTag::INT, StrType, IntType, ListType, DictType, UninitialisedType> {
            using type = IntType;
        };

        template<class StrType, class IntType, class ListType, class DictType, class UninitialisedType>
        struct tag_to_type<DataTag::LIST, StrType, IntType, ListType, DictType, UninitialisedType> {
            using type = ListType;
        };

        template<class StrType, class IntType, class ListType, class DictType, class UninitialisedType>
        struct tag_to_type<DataTag::DICT, StrType, IntType, ListType, DictType, UninitialisedType> {
            using type = DictType;
        };

        template<class StrType, class IntType, class ListType, class DictType, class UninitialisedType>
        struct tag_to_type<DataTag::UNINITIALISED, StrType, IntType, ListType, DictType, UninitialisedType> {
            using type = UninitialisedType;
        };

        template<DataTag tag, class StrType, class IntType, class ListType, class DictType, class UninitialisedType>
        using tag_to_type_t = typename tag_to_type<tag, StrType, IntType, ListType, DictType, UninitialisedType>::type;

        template<DataTag tag, class StrType, class IntType, class ListType, class DictType>
        struct itag_to_type;

        template<class StrType, class IntType, class ListType, class DictType>
        struct itag_to_type<DataTag::STR, StrType, IntType, ListType, DictType> {
            using type = StrType;
        };

        template<class StrType, class IntType, class ListType, class DictType>
        struct itag_to_type<DataTag::INT, StrType, IntType, ListType, DictType> {
            using type = IntType;
        };

        template<class StrType, class IntType, class ListType, class DictType>
        struct itag_to_type<DataTag::LIST, StrType, IntType, ListType, DictType> {
            using type = ListType;
        };

        template<class StrType, class IntType, class ListType, class DictType>
        struct itag_to_type<DataTag::DICT, StrType, IntType, ListType, DictType> {
            using type = DictType;
        };

        template<DataTag tag, class StrType, class IntType, class ListType, class DictType>
        using itag_to_type_t = typename itag_to_type<tag, StrType, IntType, ListType, DictType>::type;

        template<DataTag tag, typename StrType, StrType StrValue, typename IntType, IntType IntValue, typename ListType, ListType ListValue, typename DictType, DictType DictValue>
        struct tag_to_value;

        template<typename StrType, StrType StrValue, typename IntType, IntType IntValue, typename ListType, ListType ListValue, typename DictType, DictType DictValue>
        struct tag_to_value<DataTag::STR, StrType, StrValue, IntType, IntValue, ListType, ListValue, DictType, DictValue> : ::std::integral_constant<StrType, StrValue> {};

        template<typename StrType, StrType StrValue, typename IntType, IntType IntValue, typename ListType, ListType ListValue, typename DictType, DictType DictValue>
        struct tag_to_value<DataTag::INT, StrType, StrValue, IntType, IntValue, ListType, ListValue, DictType, DictValue> : ::std::integral_constant<IntType, IntValue> {};

        template<typename StrType, StrType StrValue, typename IntType, IntType IntValue, typename ListType, ListType ListValue, typename DictType, DictType DictValue>
        struct tag_to_value<DataTag::LIST, StrType, StrValue, IntType, IntValue, ListType, ListValue, DictType, DictValue> : ::std::integral_constant<ListType, ListValue> {};

        template<typename StrType, StrType StrValue, typename IntType, IntType IntValue, typename ListType, ListType ListValue, typename DictType, DictType DictValue>
        struct tag_to_value<DataTag::DICT, StrType, StrValue, IntType, IntValue, ListType, ListValue, DictType, DictValue> : ::std::integral_constant<DictType, DictValue> {};


        template<class T, class StrType, class IntType, class ListType, class DictType>
        struct type_to_tag;

        template<class StrType, class IntType, class ListType, class DictType>
        struct type_to_tag<StrType, StrType, IntType, ListType, DictType> : ::std::integral_constant<DataTag, DataTag::STR> {};

        template<class StrType, class IntType, class ListType, class DictType>
        struct type_to_tag<IntType, StrType, IntType, ListType, DictType> : ::std::integral_constant<DataTag, DataTag::INT> {};

        template<class StrType, class IntType, class ListType, class DictType>
        struct type_to_tag<ListType, StrType, IntType, ListType, DictType> : ::std::integral_constant<DataTag, DataTag::LIST> {};

        template<class StrType, class IntType, class ListType, class DictType>
        struct type_to_tag<DictType, StrType, IntType, ListType, DictType> : ::std::integral_constant<DataTag, DataTag::DICT> {};

        template<class StrType, class IntType, class ListType, class DictType>
        struct type_to_tag<void, StrType, IntType, ListType, DictType> : ::std::integral_constant<DataTag, DataTag::UNINITIALISED> {};

#ifdef TORREBIS_VARIABLE_TEMPLATES
        template<DataTag tag, typename StrType, StrType StrValue, typename IntType, IntType IntValue, typename ListType, ListType ListValue, typename DictType, DictType DictValue>
        constexpr
        typename tag_to_value<tag, StrType, StrValue, IntType, IntValue, ListType, ListValue, DictType, DictValue>::type
        tag_to_value_v = tag_to_value<tag, StrType, StrValue, IntType, IntValue, ListType, ListValue, DictType, DictValue>::value;

        template<class T, class StrType, class IntType, class ListType, class DictType>
        constexpr DataTag type_to_tag_v = type_to_tag<T, StrType, IntType, ListType, DictType>::value;
#endif

    }  // namespace ::torrebis::bencode::helpers

} }  // namespace ::torrebis::bencode

#endif  // TORREBIS_BENCODE_TYPE_TAGS_H_
