#ifndef TORREBIS_BENCODE_TORREBIS_BENCODE_H_
#define TORREBIS_BENCODE_TORREBIS_BENCODE_H_

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include <utility>


#if defined(__cplusplus) && __cplusplus >= 201703L
// C++ 17 or higher
#define TORREBIS_CONSTEXPR_IF if constexpr
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
    enum class DataTag : ::std::int8_t  {
        STR = 0, INT, LIST, DICT
    };

    enum MakeStr { make_str };
    enum MakeInt { make_int };
    enum MakeList { make_list };
    enum MakeDict { make_dict };

    namespace helpers {
        template<typename T>
        struct make_specifier_to_tag;

        template<> struct make_specifier_to_tag<MakeStr> : ::std::integral_constant<DataTag, DataTag::STR> {};
        template<> struct make_specifier_to_tag<MakeInt> : ::std::integral_constant<DataTag, DataTag::INT> {};
        template<> struct make_specifier_to_tag<MakeList> : ::std::integral_constant<DataTag, DataTag::LIST> {};
        template<> struct make_specifier_to_tag<MakeDict> : ::std::integral_constant<DataTag, DataTag::DICT> {};

        template<DataTag tag>
        struct tag_to_make_specifier;

        template<> struct tag_to_make_specifier<DataTag::STR> : ::std::integral_constant<MakeStr, make_str> {};
        template<> struct tag_to_make_specifier<DataTag::INT> : ::std::integral_constant<MakeInt, make_int> {};
        template<> struct tag_to_make_specifier<DataTag::LIST> : ::std::integral_constant<MakeList, make_list> {};
        template<> struct tag_to_make_specifier<DataTag::DICT> : ::std::integral_constant<MakeDict, make_dict> {};

        template<DataTag tag>
        using tag_to_make_specifier_t = typename tag_to_make_specifier<tag>::type;

#ifdef TORREBIS_VARIABLE_TEMPLATES
        template<typename T>
        constexpr DataTag make_specifier_to_tag_v = make_specifier_to_tag<T>::value;

        template<DataTag tag>
        constexpr tag_to_make_specifier_t<tag> tag_to_make_specifier_v = tag_to_make_specifier<tag>::value;
#endif


        namespace detail {
            template<DataTag tag, class StrType, class IntType, class ListType, class DictType>
            struct tag_to_type;

            template<class StrType, class IntType, class ListType, class DictType>
            struct tag_to_type<DataTag::STR, StrType, IntType, ListType, DictType> {
                using type = StrType;
            };

            template<class StrType, class IntType, class ListType, class DictType>
            struct tag_to_type<DataTag::INT, StrType, IntType, ListType, DictType> {
                using type = IntType;
            };

            template<class StrType, class IntType, class ListType, class DictType>
            struct tag_to_type<DataTag::LIST, StrType, IntType, ListType, DictType> {
                using type = ListType;
            };

            template<class StrType, class IntType, class ListType, class DictType>
            struct tag_to_type<DataTag::DICT, StrType, IntType, ListType, DictType> {
                using type = DictType;
            };

            template<DataTag tag, typename StrType, typename IntType, typename ListType, typename DictType, StrType StrValue, IntType IntValue, ListType ListValue, DictType DictValue>
            struct tag_to_value;

            template<typename StrType, typename IntType, typename ListType, typename DictType, StrType StrValue, IntType IntValue, ListType ListValue, DictType DictValue>
            struct tag_to_value<DataTag::STR, StrType, IntType, ListType, DictType, StrValue, IntValue, ListValue, DictValue> : ::std::integral_constant<StrType, StrValue> {};

            template<typename StrType, typename IntType, typename ListType, typename DictType, StrType StrValue, IntType IntValue, ListType ListValue, DictType DictValue>
            struct tag_to_value<DataTag::INT, StrType, IntType, ListType, DictType, StrValue, IntValue, ListValue, DictValue> : ::std::integral_constant<IntType, IntValue> {};

            template<typename StrType, typename IntType, typename ListType, typename DictType, StrType StrValue, IntType IntValue, ListType ListValue, DictType DictValue>
            struct tag_to_value<DataTag::LIST, StrType, IntType, ListType, DictType, StrValue, IntValue, ListValue, DictValue> : ::std::integral_constant<ListType, ListValue> {};

            template<typename StrType, typename IntType, typename ListType, typename DictType, StrType StrValue, IntType IntValue, ListType ListValue, DictType DictValue>
            struct tag_to_value<DataTag::DICT, StrType, IntType, ListType, DictType, StrValue, IntValue, ListValue, DictValue> : ::std::integral_constant<DictType, DictValue> {};


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

            template<typename T, typename... Args>
            struct is_any : ::std::false_type {};

            template<typename T, typename First>
            struct is_any<T, First> : ::std::is_same<T, First> {};

            template<typename T, typename First, typename... Args>
            struct is_any<T, First, Args...> :
                ::std::integral_constant<bool, ::std::is_same<T, First>::value || is_any<T, Args...>::value> {};

        }  // namespace detail
    }  // namespace helpers

    template<class StrType = ::std::string,
             class IntType = ::std::intmax_t,
             template<typename ValueType, typename...> class ListType = ::std::vector,
             bool dict_is_ordered = true,
             template<typename KeyType, typename ValueType, typename...> class DictType = ::std::map,
             template<typename T> class Allocator = ::std::allocator
             >
    class basic_data;

    namespace helpers { namespace detail {
        template<typename>
        struct is_basic_data : ::std::false_type {};

        template<class StrType, class IntType, template<typename, typename...> class ListType, bool dict_is_ordered, template<typename, typename, typename...> class DictType, template<typename> class Allocator>
        struct is_basic_data<basic_data<StrType, IntType, ListType, dict_is_ordered, DictType, Allocator>> : ::std::true_type {};

        template<typename BasicData>
        using enable_if_basic_data = typename ::std::enable_if<is_basic_data<BasicData>::value, BasicData>::type;
    } }  // namespace helpers::detail

    template<typename BasicData, typename T>
    BasicData bdecode(const T* const bencoded, const ::std::size_t size);

    template<typename BasicData>
    BasicData bdecode(const ::std::string& bencoded);

    template<typename BasicData, typename Iterator>
    BasicData bdecode(const Iterator begin, const Iterator end);

    template<typename BasicData, typename T>
    BasicData bdecode(const T* const begin, const T* const end);

    template<typename BasicData>
    ::std::string bencode(const BasicData& dat);

    template<typename BasicData>
    void bencode_to_stream(const BasicData& dat, ::std::ostream& stream);

    template<typename BasicData>
    class visitor;

    template<class StrType_ = ::std::string, template<typename T> class Allocator_ = ::std::allocator>
    class basic_path_el final {
    public:
        using StrType = StrType_;
        template<typename T>
        using Allocator = Allocator_<T>;

        using str_type = StrType;
        using str_ptr_type = ::std::shared_ptr<str_type>;
        using allocator_t = Allocator<str_type>;

        using index_type = ::std::size_t;
    private:
        DataTag tag_;  // Can only be DataTag::STR or DataTag::INT
        union KeyOrIndex {
            str_ptr_type key;
            index_type index;

            KeyOrIndex(const index_type i) noexcept : index(i) {}
            KeyOrIndex(const str_type k) : key(::std::allocate_shared<str_type, allocator_t>(allocator_t(), k)) {}
            KeyOrIndex(const str_ptr_type k) noexcept : key(k) {}
            KeyOrIndex() noexcept : key(nullptr) {}


            ~KeyOrIndex() noexcept {}  // basic_path_el class should handle this.

        } key_or_index_;
    public:
        basic_path_el(const std::size_t i) noexcept : tag_(DataTag::INT), key_or_index_(i) {}
        basic_path_el(const str_type k) : tag_(DataTag::STR), key_or_index_(k) {}
        basic_path_el(const str_ptr_type k) noexcept : tag_(DataTag::STR), key_or_index_(k) {}
        basic_path_el(const basic_path_el& other) noexcept : tag_(other.tag_) { if (tag_ == DataTag::STR) { key_or_index_.key = other.get_key(); } else { key_or_index_.index = other.get_index(); } }
        basic_path_el(const basic_path_el&& other) noexcept : tag_(other.tag_) { if (tag_ == DataTag::STR) { key_or_index_.key = other.get_key(); } else { key_or_index_.index = other.get_index(); } }
        basic_path_el& operator=(basic_path_el other) noexcept { if (&other != this) { swap(other); }; return *this; }
        index_type operator=(index_type other) noexcept { set_index(other); return other; }
        str_type& operator=(str_type other) noexcept { set_key(other); return other; }
        str_ptr_type operator=(str_ptr_type other) noexcept { set_key(other); return other; }
        bool operator==(const basic_path_el& other) const noexcept { if (&other == this) { return true; } if (is_key()) { return other.is_key() && get_key() == other.get_key(); } return other.is_index() && get_index() == other.get_index(); }
        bool operator!=(const basic_path_el& other) const noexcept { return !operator==(other); }
        void swap(basic_path_el& other) noexcept {
            using ::std::swap;
            if (is_key()) {
                if (other.is_key()) {
                    swap(key_or_index_.key, other.key_or_index_.key);
                } else {
                    index_type i = other.key_or_index_.index;
                    other.key_or_index_.key = key_or_index_.key;
                    key_or_index_.key = nullptr;
                    key_or_index_.index = i;
                    other.tag_ = DataTag::STR;
                    tag_ = DataTag::INT;
                }
            } else if (other.is_key()) {
                index_type i = key_or_index_.index;
                key_or_index_.key = other.key_or_index_.key;
                other.key_or_index_.key = nullptr;
                other.key_or_index_.index = i;
                tag_ = DataTag::STR;
                other.tag_ = DataTag::INT;
            } else {
                index_type i = key_or_index_.index;
                key_or_index_.index = other.key_or_index_.index;
                other.key_or_index_.index = i;
            }
        };

        bool is_key() const noexcept { return tag_ == DataTag::STR; }
        bool is_index() const noexcept { return tag_ == DataTag::INT; }

        str_ptr_type get_key() const noexcept { return is_key() ? key_or_index_.key : nullptr; }
        index_type get_index() const noexcept { return is_index() ? key_or_index_.index : static_cast<index_type>(-1); }

        void set_key(str_type k) { tag_ = DataTag::STR; key_or_index_.key = k; }
        void set_key(str_ptr_type k) noexcept { tag_ = DataTag::STR; key_or_index_.key = k; }
        void set_index(index_type i) noexcept {
            if (is_key()) {
                key_or_index_.key = nullptr;
                tag_ = DataTag::INT;
            }
            key_or_index_.index = i;
        }

        ~basic_path_el() noexcept {
            if (is_key()) {
                key_or_index_.key = nullptr;
            }
        }
    };

    using path_el = basic_path_el<>;


    /*
     * StrType: class of the string value.
     *     Requirements:
     *     Has a constructor StrType::StrType(char* begin, std::size_t length);
     *     Has an operator<(StrType& self, StrType& other);
     *     Has an operator<<(std::ostream& os, StrType& self);
     *
     * IntType: class of the int value (The cstdint values and regular int, long, etc. will work.)
     *     Requirements:
     *     Has a constructor so that `IntType(0)` will work.
     *     Can do (IntType) value / 10
     *     Can do (IntType) value < 1
     *     Can do (IntType) value == 0
     *     Can do (IntType) value + (int) other
     *     Can do (IntType) value * 10
     *     Has an operator<<(std::ostream& os, IntType& self); which will output:
     *         - "0" if self == 0.
     *         - "-" and operator<<(os, -self) if self < 0.
     *         - Base 10 representation of self if self > 0.
     *
     *     Should be an integral type, but can tolerate floats.
     *
     * ListType: Template class of the list value. Most iterators will work.
     *     Requirements:
     *     Takes 1 template argument of the type of the value.
     *     ListType<T>::value_type is T
     *     for (auto& i : (ListType<T>) value) decltype(i) is T;
     *     Can do ((ListType<T>) value).size()
     *     Is default constructible to an empty list.
     *     Has a push_back(T t) can take a T and add it to the list.
     *
     * bool dict_is_ordered: Set to `true` if the values in DictType instances are ordered by std::less.
     *
     * DictType: Template class of the dict value. std::map and std::unordered_map will work.
     *     Requirements:
     *     Takes 2 template arguments. The first is the key type (StrType) and the second is the value type.
     *     for (auto& i : (DictType<KT, VT>) value) { decltype(i.first) is KT; decltype(i.second) is VT; }
     *     Is default constructible to an empty dictionary.
     *     Has an operator[](KT t) which returns a VT&, constructing it if necessary.
     *
     * AllocatorType: An allocator, like std::allocator.
     */
    template<class StrType_, class IntType_,
             template<typename ValueType, typename...> class ListType_, bool dict_is_ordered_,
             template<typename KeyType, typename ValueType, typename...> class DictType_,
             template<typename T> class Allocator_
             >
    class basic_data final {
    private:
        DataTag tag_;
        ::std::shared_ptr<void> value_;

        template<typename IntType = IntType_>
        static ::std::size_t int_serialised_length(IntType i) noexcept {
            if (i == 0) {
                return 1;  // '0'
            }
            ::std::size_t negative = 0;
            if (i < 0) {
                negative = 1;
                i = -i;
            }
            ::std::size_t num_digits = 0;
            do {
                num_digits++;
                i /= 10;
            } while (!(i < 1 && static_cast<uint8_t>(i) == 0));
            return num_digits + negative;
        }

    public:
        static constexpr bool dict_is_ordered = dict_is_ordered_;

        using StrType = StrType_;
        using IntType = IntType_;
        template<typename ValueType>
        using ListType = ListType_<ValueType>;
        template<typename KeyType, typename ValueType>
        using DictType = DictType_<KeyType, ValueType>;
        template<typename T>
        using Allocator = Allocator_<T>;

        using str_type = StrType;
        using int_type = IntType;
        using list_type = ListType<basic_data>;
        using dict_type = DictType<str_type, basic_data>;

        using str_allocator_t = const Allocator<str_type>;
        using int_allocator_t = const Allocator<int_type>;
        using list_allocator_t = const Allocator<list_type>;
        using dict_allocator_t = const Allocator<dict_type>;

        /*
        static str_allocator_t str_allocator;
        static int_allocator_t int_allocator;
        static list_allocator_t list_allocator;
        static dict_allocator_t dict_allocator;
        */

        using str_ptr = ::std::shared_ptr<str_type>;
        using int_ptr = ::std::shared_ptr<int_type>;
        using list_ptr = ::std::shared_ptr<list_type>;
        using dict_ptr = ::std::shared_ptr<dict_type>;

        using const_str_ptr = ::std::shared_ptr<const str_type>;
        using const_int_ptr = ::std::shared_ptr<const int_type>;
        using const_list_ptr = ::std::shared_ptr<const list_type>;
        using const_dict_ptr = ::std::shared_ptr<const dict_type>;

        template<DataTag tag>
        using tag_to_type = typename helpers::detail::tag_to_type<tag, str_type, int_type, list_type, dict_type>::type;

        template<DataTag tag>
        using tag_to_allocator_t = typename helpers::detail::tag_to_type<tag, str_allocator_t, int_allocator_t, list_allocator_t, dict_allocator_t>::type;

        /*
        template<DataTag tag>
        using tag_to_allocator = helpers::detail::tag_to_value<tag, str_allocator_t&, int_allocator_t&, list_allocator_t&, dict_allocator_t&, str_allocator, int_allocator, list_allocator, dict_allocator>;

#ifdef TORREBIS_VARIABLE_TEMPLATES
        template<DataTag tag>
        static constexpr tag_to_allocator_t_v<tag>& tag_to_allocator_v = tag_to_allocator<tag>::value;
#endif
         */

        template<DataTag tag>
        using tag_to_ptr = typename helpers::detail::tag_to_type<tag, str_ptr, int_ptr, list_ptr, dict_ptr>::type;

        template<DataTag tag>
        using tag_to_const_ptr = typename helpers::detail::tag_to_type<tag, const_str_ptr, const_int_ptr, const_list_ptr, const_dict_ptr>::type;

        template<class type>
        using type_to_tag = typename helpers::detail::type_to_tag<type, str_type, int_type, list_type, dict_type>;

#ifdef TORREBIS_VARIABLE_TEMPLATES
        template<class type>
        static constexpr DataTag type_to_tag_v = type_to_tag<type>::value;
#endif

#define TORREBIS_TAG helpers::make_specifier_to_tag<make_specifier>::value
#define TORREBIS_TYPE tag_to_type<TORREBIS_TAG>
#define TORREBIS_ALLOCATOR_T tag_to_allocator_t<TORREBIS_TAG>

        template<typename make_specifier>
        basic_data(const make_specifier, const TORREBIS_TYPE value) :
            tag_(TORREBIS_TAG),
            value_(::std::allocate_shared<TORREBIS_TYPE, TORREBIS_ALLOCATOR_T>(TORREBIS_ALLOCATOR_T(), value)) {}
        template<typename make_specifier>
        basic_data(const make_specifier, tag_to_ptr<TORREBIS_TAG> value) : tag_(TORREBIS_TAG), value_(value) {}

        template<typename make_specifier, typename... Args>
        explicit basic_data(const make_specifier, Args... args) :
            tag_(TORREBIS_TAG),
            value_(::std::allocate_shared<TORREBIS_TYPE, TORREBIS_ALLOCATOR_T>(TORREBIS_ALLOCATOR_T(), TORREBIS_TYPE(args...))) {}

        template<typename T>
        explicit basic_data(const T value) :
            tag_(type_to_tag<T>::value),
            value_(::std::allocate_shared<T, tag_to_allocator_t<type_to_tag<T>::value>>(tag_to_allocator_t<type_to_tag<T>::value>(), value)) {}

        template<typename make_specifier, typename... Args>
        basic_data(const make_specifier, const tag_to_ptr<TORREBIS_TAG> value) noexcept :
            tag_(TORREBIS_TAG), value_(value) {}

        template<typename T>
        explicit basic_data(const ::std::shared_ptr<T> value) noexcept : tag_(type_to_tag<T>::value), value_(value) {}

#undef TORREBIS_TAG
#undef TORREBIS_TYPE
#undef TORREBIS_ALLOCATOR_T

        // Default constructor constructs an empty dictionary.
        basic_data() : tag_(DataTag::DICT), value_(::std::allocate_shared<dict_type, dict_allocator_t>(basic_data::dict_allocator_t())) {};

        basic_data(const basic_data& other) noexcept : tag_(other.tag_), value_(other.value_) {}
        basic_data(const basic_data&& other) noexcept : tag_(other.tag_), value_(other.value_) {}
        basic_data& operator=(const basic_data& other) noexcept { if (&other != this) { tag_ = other.tag_; value_ = other.value_; }; return *this; }
        basic_data& operator=(const basic_data&& other) noexcept { tag_ = other.tag_; value_ = other.value_; return *this; }
        bool operator==(const basic_data& other) const noexcept { if (&other == this) { return true; } return tag_ == other.tag_ && get<void>() == other.get<void>(); }
        bool operator!=(const basic_data& other) const noexcept { return !operator==(other); }
        void swap(basic_data& other) noexcept {
            using ::std::swap;
            DataTag other_tag = other.tag_;
            other.tag_ = tag_;
            tag_ = other_tag;
            swap(value_, other.value_);
        };

        DataTag get_tag() const noexcept {
            return tag_;
        }

        template<DataTag tag>
        tag_to_ptr<tag> get() const noexcept {
            if (tag == tag_) {
                return ::std::static_pointer_cast<tag_to_type<tag>>(value_);
            }
            return nullptr;
        }

        template<typename T>
        ::std::shared_ptr<T> get() const noexcept {
            TORREBIS_CONSTEXPR_IF (::std::is_same<T, void>::value) {
                return value_;
            }
            if (type_to_tag<T>::value == tag_) {
                return ::std::static_pointer_cast<T>(value_);
            }
            return nullptr;
        }

        ::std::shared_ptr<void> get(DataTag tag) const noexcept {
            if (tag == tag_) {
                return value_;
            }
            return nullptr;
        }

        str_ptr get_str() const noexcept { return get<DataTag::STR>(); }
        int_ptr get_int() const noexcept { return get<DataTag::INT>(); }
        list_ptr get_list() const noexcept { return get<DataTag::LIST>(); }
        dict_ptr get_dict() const noexcept { return get<DataTag::DICT>(); }

        template<DataTag tag>
        void set(const tag_to_type<tag> value) {
            if (tag_ == tag) {
                *get<tag>() = value;
            } else {
                value_ = nullptr;
                using allocator_t = tag_to_allocator_t<tag>;
                value_ = ::std::allocate_shared<tag_to_type<tag>, allocator_t>(allocator_t(), value);
            }
        }

        template<DataTag tag>
        void set(const tag_to_ptr<tag> value) noexcept {
            if (tag_ == tag) {
                get<tag>() = value;
            } else {
                get<void>() = ::std::static_pointer_cast<void>(value);
            }
        }

        template<typename T>
        void set(T value) { set<type_to_tag<T>::value>(value); }

        void set_str(const str_type s) { set<DataTag::STR>(s); }
        void set_str(const str_ptr s) noexcept { set<DataTag::STR>(s); }
        void set_int(const int_type i) { set<DataTag::INT>(i); }
        void set_int(const int_ptr i) noexcept { set<DataTag::INT>(i); }
        void set_list(const list_type l) { set<DataTag::LIST>(l); }
        void set_list(const list_ptr l) noexcept { set<DataTag::LIST>(l); }
        void set_dict(const dict_type d) { set<DataTag::DICT>(d); }
        void set_dict(const dict_ptr d) noexcept { set<DataTag::DICT>(d); }

        bool is_str() const noexcept { return tag_ == DataTag::STR; }
        bool is_int() const noexcept { return tag_ == DataTag::INT; }
        bool is_list() const noexcept { return tag_ == DataTag::LIST; }
        bool is_dict() const noexcept { return tag_ == DataTag::DICT; }

        static basic_data loads(const ::std::string& bencoded) {
            return ::torrebis::bencode::bdecode<basic_data>(bencoded);
        }

        template<typename T>
        static basic_data loads(const T* const bencoded, const ::std::size_t size) {
            return ::torrebis::bencode::bdecode<basic_data, T>(bencoded, size);
        }

        template<typename Iterator>
        static basic_data loads(const Iterator begin, const Iterator end) {
            return ::torrebis::bencode::bdecode<basic_data, Iterator>(begin, end);
        }

        template<typename T>
        static basic_data loads(const T* const begin, const T* const end) {
            return ::torrebis::bencode::bdecode<basic_data, T>(begin, end);
        }

        ::std::size_t serialised_length() const noexcept {
            switch (get_tag()) {
                case DataTag::STR: {
                    ::std::size_t size = get_str()->size();
                    return int_serialised_length<::std::size_t>(size) + 1 + size;
                }
                case DataTag::INT:
                    return static_cast<::std::size_t>(2) + int_serialised_length<int_type>(*get_int());
                case DataTag::LIST: {
                    ::std::size_t size = 2;
                    for (basic_data i : *get_list()) {
                        size += i.serialised_length();
                    }
                    return size;
                }
                case DataTag::DICT: {
                    ::std::size_t size = 2;
                    for (auto& i : *get_dict()) {
                        ::std::size_t string_size = i.first.size();
                        size += int_serialised_length<::std::size_t>(string_size) + static_cast<::std::size_t>(1) + string_size + i.second.serialised_length();
                    }
                    return size;
                }
                default:
                    return 0;
            }
        }

        ::std::string dumps() const {
            return bencode::bencode<basic_data>(*this);
        };

        void dump(::std::ostream& stream) const {
            return bencode::bencode_to_stream<basic_data>(*this, stream);
        }

        using path_el_type = basic_path_el<str_type, Allocator>;
        using visitor_type = visitor<basic_data>;

        template<typename T = ::std::initializer_list<const path_el_type>>
        ::std::pair<::std::size_t, const basic_data&> search_path(const T path) const {
            const basic_data& current = *this;
            ::std::size_t depth = 0;
            for (const path_el_type& i : path) {
                if (i.is_key()) {
                    if (is_dict()) {
                        auto it = get_dict()->find(*i.get_key());
                        if (it == get_dict()->end()) {
                            // key not in dictionary
                            goto end;
                        }
                        current = it->second;
                        ++depth;
                    } else {
                        goto end;
                    }
                } else if (is_list()) {
                    auto index = i.get_index();
                    if (index >= get_list()->size()) {
                        goto end;
                    }
                    current = (*get_list())[index];
                    ++depth;
                } else {
                    goto end;
                }
            }
end:
            return { depth, current };
        }

        template<typename T = ::std::initializer_list<const path_el_type>>
        ::std::pair<::std::size_t, basic_data&> search_path(const T path) {
            basic_data& current = *this;
            ::std::size_t depth = 0;
            for (const path_el_type& i : path) {
                if (i.is_key()) {
                    if (is_dict()) {
                        auto it = get_dict()->find(*i.get_key());
                        if (it == get_dict()->end()) {
                            // key not in dictionary
                            goto end;
                        }
                        current = it->second;
                        ++depth;
                    } else {
                        goto end;
                    }
                } else if (is_list()) {
                    auto index = i.get_index();
                    if (index >= get_list()->size()) {
                        goto end;
                    }
                    current = (*get_list())[index];
                    ++depth;
                } else {
                    goto end;
                }
            }
end:
            return { depth, current };
        }
    };

    using data = basic_data<>;

    template<typename BasicData = data, typename T>
    BasicData bdecode(const T* const bencoded, const ::std::size_t size);

    template<typename BasicData = data>
    BasicData bdecode(const ::std::string& bencoded) {
        return bdecode<BasicData, char>(bencoded.c_str(), bencoded.size());
    }

    template<typename BasicData = data, typename Iterator>
    BasicData bdecode(Iterator begin, const Iterator end);

    template<typename BasicData = data, typename T>
    BasicData bdecode(const T* const begin, const T* const end) {
        return bdecode<BasicData, T>(begin, end - begin);
    }

    template<typename BasicData = data>
    ::std::string bencode(const BasicData& dat);

    template<typename BasicData = data>
    void bencode_to_stream(const BasicData& dat, const ::std::ostream& stream);

    class decoding_error : ::std::exception {
    private:
        const char* what_;
    public:
        decoding_error() = delete;
        decoding_error(decoding_error& other) noexcept : what_(other.what_) {};
        decoding_error(decoding_error&& other) noexcept : what_(other.what_) {};
        explicit decoding_error(const char* what) noexcept : what_(what) {};

        const char* what() const noexcept final { return what_; }
    };

    template<typename BasicData>
    class visitor {
    public:
        using data_t = const helpers::detail::enable_if_basic_data<BasicData>&;
        using mutable_path_t = ::std::vector<typename BasicData::path_el_type>;
        using path_t = const mutable_path_t&;
        using str_type = const typename BasicData::const_str_ptr;
        using int_type = const typename BasicData::const_int_ptr;
        using list_type = const typename BasicData::const_list_ptr;
        using dict_type = const typename BasicData::const_dict_ptr;

        virtual void visit(const data_t dat) {
            // Start the visit with an empty path.
            return visit_any(dat, {});
        }
    protected:
        virtual void visit_any(const data_t dat, const path_t path) {
            switch (dat.get_tag()) {
                case DataTag::STR:
                    return visit_str(dat.get_str(), path);
                case DataTag::INT:
                    return visit_int(dat.get_int(), path);
                case DataTag::LIST:
                    return visit_list(dat.get_list(), path);
                case DataTag::DICT:
                    return visit_dict(dat.get_dict(), path);
                default:
                    return;
            }
        }

        virtual void visit_str(const str_type str, const path_t path) {
            // Do nothing.
            // Note: By default, visit_key uses this function to visit dictionary keys
            // with the final element of `path` set as a key, with a value of the index of the key.
        }


        virtual void visit_int(const int_type i, const path_t path) {
            // Do nothing.
        }

        virtual void visit_list(const list_type list, const path_t path) {
            // Visit all members with visit_item one level deeper.
            mutable_path_t new_path(path);
            ::std::size_t i = 0;
            new_path.push_back(0);
            for (const data_t& el : *list) {
                new_path[new_path.size() - 1] = i++;
                visit_item(el, new_path);
            }
        }

        virtual void visit_item(const data_t item, const path_t path) {
            // Just visit it with visit_any.
            return visit_any(item, path);
        }

        virtual void visit_dict(const dict_type dict, const path_t path) {
            // Visit:
            //    keys with nullptr at the end of the path.
            //    values with the key at the end of the path.
            mutable_path_t new_path(path);
            ::std::size_t i = 0;
            new_path.push_back(0);
            for (const auto& el : *dict) {
                data_t key = BasicData(make_str, el.first);
                visit_key(key, new_path);
                new_path[new_path.size() - 1] = key.get_str();
                visit_value(el.second, new_path);
                new_path[new_path.size() - 1] = ++i;
            }
        }

        virtual void visit_key(const data_t k, const path_t path) {
            return visit_any(k, path);
        }

        virtual void visit_value(const data_t value, const path_t path) {
            // Just visit it with visit_any.
            return visit_any(value, path);
        }
    };

    /*
     * Outputs some data to
     */
    template<typename BasicData>
    class print_visitor : public visitor<BasicData> {
    private:
        ::std::ostream& os_;
        const char indent_char_;
        const ::std::size_t indent_width_;
    public:
        using data_t = typename visitor<BasicData>::data_t;
        using path_t = typename visitor<BasicData>::path_t;
        using str_type = typename visitor<BasicData>::str_type;
        using int_type = typename visitor<BasicData>::int_type;
        using list_type = typename visitor<BasicData>::list_type;
        using dict_type = typename visitor<BasicData>::dict_type;

        explicit print_visitor(
            std::ostream& os,
            const char indent_char = ' ',
            const ::std::size_t indent_width = 4) :
            os_(os), indent_char_(indent_char), indent_width_(indent_width) {}
    protected:
        virtual void visit_str(const str_type str, const path_t path) {
            os_ << ::std::string("\"");
            for (const char c : *str) {
                if (' ' <= c && c <= '~') {
                    if (c == '\\' || c == '"') os_ << ::std::string("\\");
                    char s[2] = { c, '\0' };
                    os_ << ::std::string(s);
                } else {
                    os_ << ::std::string("\\x");
                    ::std::uint8_t x = static_cast<::std::uint8_t>(c);
                    char s[3] = { static_cast<char>('0' + (x >> 4)), static_cast<char>('0' + (x & 0xF)), '\0' };
                    os_ << ::std::string(s);
                }
            }
            os_ << ::std::string("\"");
        }

        virtual void visit_int(const int_type i, const path_t path) {
            os_ << *i;
        }

        virtual void visit_list(const list_type list, const path_t path) {
            os_ << ::std::string("[");
            visitor<BasicData>::visit_list(list, path);
            os_ << ::std::string("\n") << ::std::string(indent_width_ * path.size(), indent_char_) << ::std::string("]");
        }

        virtual void visit_item(const data_t item, const path_t path) {
            os_ << ((path[path.size() - 1] == 0) ? ::std::string("\n") : ::std::string(",\n"));
            os_ << ::std::string(indent_width_ * path.size(), indent_char_);
            visitor<BasicData>::visit_item(item, path);
        }

        virtual void visit_dict(const dict_type dict, const path_t path) {
            os_ << ::std::string("{");
            visitor<BasicData>::visit_dict(dict, path);
            os_ << ::std::string("\n") << ::std::string(indent_width_ * path.size(), indent_char_) << ::std::string("}");
        }

        virtual void visit_key(const data_t k, const path_t path) {
            os_ << ((path[path.size() - 1] == 0) ? ::std::string("\n") : ::std::string(",\n"));
            os_ << ::std::string(indent_width_ * path.size(), indent_char_);
            visitor<BasicData>::visit_key(k, path);
            os_ << ::std::string(": ");
        }
    };

} }  // namespace torrebis::bencode

#endif  // TORREBIS_BENCODE_TORREBIS_BENCODE_H_
