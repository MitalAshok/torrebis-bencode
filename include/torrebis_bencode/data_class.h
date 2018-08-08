#ifndef TORREBIS_BENCODE_DATA_CLASS_H_
#define TORREBIS_BENCODE_DATA_CLASS_H_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <type_traits>
#include <utility>
#include <ostream>

#ifdef TORREBIS_STD_ARRAY_HAS_DATA
#include <array>
#endif

#include "torrebis_bencode/helpers.h"
#include "torrebis_bencode/type_tags.h"
#include "torrebis_bencode/path_el.h"
#include "torrebis_bencode/errors.h"

namespace torrebis { namespace bencode {

// The following macros are used to define out-of-line functions

#define TORREBIS_BASIC_DATA_TEMPLATE_INNER \
    class StrType, class IntType, template<typename, typename...> class ListType, \
    bool dict_is_ordered, template<typename, typename, typename...> class DictType, \
    template<typename> class Allocator

#define TORREBIS_BASIC_DATA_TEMPLATE template<TORREBIS_BASIC_DATA_TEMPLATE_INNER>
#define TORREBIS_BASIC_DATA_TEMPLATE_TYPE_FORWARD StrType, IntType, ListType, dict_is_ordered, DictType, Allocator
#define TORREBIS_BASIC_DATA_TEMPLATE_TYPE basic_data<TORREBIS_BASIC_DATA_TEMPLATE_TYPE_FORWARD>
#define TORREBIS_BASIC_DATA_TEMPLATE_FORWARD(basic_data) (basic_data)::StrType, (basic_data)::IntType, (basic_data)::ListType, (basic_data)::dict_is_ordered, (basic_data)::DictType, (basic_data)::Allocator

    template<class StrType_ = ::std::string,
        class IntType_ = ::std::intmax_t,
        template<typename ValueType, typename...> class ListType_ = ::std::vector,
        bool dict_is_ordered_ = true,
        template<typename KeyType, typename ValueType, typename...> class DictType_ = ::std::map,
        template<typename T> class Allocator_ = ::std::allocator
    >
    class basic_data final {
    private:
        DataTag tag_;
        ::std::shared_ptr<void> value_;
    public:
        static constexpr bool dict_is_ordered = dict_is_ordered_;

        using StrType = StrType_;
        using IntType = IntType_;
        template<typename ValueType, typename... Args>
        using ListType = ListType_<ValueType, Args...>;
        template<typename KeyType, typename ValueType, typename... Args>
        using DictType = DictType_<KeyType, ValueType, Args...>;
        template<typename T>
        using Allocator = Allocator_<T>;

        using str_type = StrType;
        using int_type = IntType;
        using list_type = ListType<basic_data>;
        using dict_type = DictType<str_type, basic_data>;

        using const_str_type = const StrType;
        using const_int_type = const IntType;
        using const_list_type = const ListType<basic_data>;
        using const_dict_type = const DictType<str_type, basic_data>;

        using str_allocator_type = const Allocator<str_type>;
        using int_allocator_type = const Allocator<int_type>;
        using list_allocator_type = const Allocator<list_type>;
        using dict_allocator_type = const Allocator<dict_type>;

        using str_ptr = ::std::shared_ptr<str_type>;
        using int_ptr = ::std::shared_ptr<int_type>;
        using list_ptr = ::std::shared_ptr<list_type>;
        using dict_ptr = ::std::shared_ptr<dict_type>;
        using void_ptr = ::std::shared_ptr<void>;

        using const_str_ptr = const ::std::shared_ptr<const_str_type>;
        using const_int_ptr = const ::std::shared_ptr<const_int_type>;
        using const_list_ptr = const ::std::shared_ptr<const_list_type>;
        using const_dict_ptr = const ::std::shared_ptr<const_dict_type>;
        using const_void_ptr = const ::std::shared_ptr<const void>;

        template<DataTag tag>
        using tag_to_type = typename helpers::tag_to_type<tag, str_type, int_type, list_type, dict_type, void>;

        template<DataTag tag>
        using tag_to_type_t = typename tag_to_type<tag>::type;

        template<DataTag tag>
        using tag_to_const_type = typename helpers::tag_to_type<tag, const_str_type, const_int_type, const_list_type, const_dict_type, const void>;

        template<DataTag tag>
        using tag_to_const_type_t = typename tag_to_const_type<tag>::type;

        template<DataTag tag>
        using tag_to_allocator = typename helpers::itag_to_type<tag, str_allocator_type, int_allocator_type, list_allocator_type, dict_allocator_type>;

        template<DataTag tag>
        using tag_to_allocator_t = typename tag_to_allocator<tag>::type;

        template<DataTag tag>
        using tag_to_ptr = typename helpers::tag_to_type<tag, str_ptr, int_ptr, list_ptr, dict_ptr, void_ptr>;

        template<DataTag tag>
        using tag_to_ptr_t = typename tag_to_ptr<tag>::type;

        template<DataTag tag>
        using tag_to_const_ptr = typename helpers::tag_to_type<tag, const_str_ptr, const_int_ptr, const_list_ptr, const_dict_ptr, const_void_ptr>;

        template<DataTag tag>
        using tag_to_const_ptr_t = typename tag_to_const_ptr<tag>::type;

        template<class type>
        using type_to_tag = typename helpers::type_to_tag<type, str_type, int_type, list_type, dict_type>;

#ifdef TORREBIS_VARIABLE_TEMPLATES
        template<class type>
        static constexpr DataTag type_to_tag_v = type_to_tag<type>::value;
#endif

    private:
        template<typename MakeSpecifier>
        basic_data(const MakeSpecifier, tag_to_ptr_t<helpers::detail::make_specifier_to_tag<MakeSpecifier>::value> value) noexcept :
            tag_(helpers::detail::make_specifier_to_tag<MakeSpecifier>::value), value_(value) {}

    public:
        basic_data() noexcept : tag_(DataTag::UNINITIALISED), value_(nullptr) {}

        basic_data(const basic_data& other) noexcept : tag_(other.tag_), value_(other.value_) {}
        basic_data(const basic_data&& other) noexcept : tag_(other.tag_), value_(other.value_) {}
        basic_data& operator=(const basic_data& other) noexcept { if (&other != this) { tag_ = other.tag_; value_ = other.value_; }; return *this; }
        basic_data& operator=(const basic_data&& other) noexcept { tag_ = other.tag_; value_ = other.value_; return *this; }
        bool operator==(const basic_data& other) const noexcept { return tag_ == other.tag_ && value_ == other.value_; }
        bool operator!=(const basic_data& other) const noexcept { return !operator==(other); }
        void swap(basic_data& other) noexcept {
            DataTag other_tag = other.tag_;
            other.tag_ = tag_;
            tag_ = other_tag;
            ::std::swap(value_, other.value_);
        };
        explicit operator bool() const noexcept { return tag_ != DataTag::UNINITIALISED; }
        bool operator!() const noexcept { return tag_ == DataTag::UNINITIALISED; }

        DataTag get_tag() const noexcept {
            return tag_;
        }

        template<DataTag tag>
        bool is() const noexcept { return tag == tag_; }
        template<typename T>
        bool is() const noexcept { return type_to_tag<T>::value == tag_; }
        bool is(DataTag tag) const noexcept { return tag == tag_; }

        bool is_valid() const noexcept { return tag_ != DataTag::UNINITIALISED; }
        bool is_str() const noexcept { return tag_ == DataTag::STR; }
        bool is_int() const noexcept { return tag_ == DataTag::INT; }
        bool is_list() const noexcept { return tag_ == DataTag::LIST; }
        bool is_dict() const noexcept { return tag_ == DataTag::DICT; }

        template<DataTag tag>
        tag_to_ptr_t<tag> get() noexcept {
            return (tag == DataTag::UNINITIALISED || tag == tag_)
                   ? ::std::static_pointer_cast<tag_to_type_t<tag>>(value_)
                   : nullptr;
        }

        template<DataTag tag>
        tag_to_const_ptr_t<tag> get() const noexcept {
            return (tag == DataTag::UNINITIALISED || tag == tag_)
                   ? ::std::static_pointer_cast<tag_to_const_type_t<tag>>(value_)
                   : nullptr;
        }

        template<typename T>
        ::std::shared_ptr<T> get() noexcept {
            TORREBIS_CONSTEXPR_IF (helpers::detail::is_same_decayed<T, void>::value) {
                return value_;
            }
            if (type_to_tag<T>::value == tag_) {
                return ::std::static_pointer_cast<T>(value_);
            }
            return nullptr;
        }

        template<typename T>
        const ::std::shared_ptr<const T> get() const noexcept {
            TORREBIS_CONSTEXPR_IF (helpers::detail::is_same_decayed<T, void>::value) {
                return value_;
            }
            if (type_to_tag<T>::value == tag_) {
                return ::std::static_pointer_cast<const T>(value_);
            }
            return nullptr;
        }

        void_ptr get(DataTag tag) noexcept {
            return (tag == DataTag::UNINITIALISED || tag == tag_) ? value_ : nullptr;
        }
        const_void_ptr get(DataTag tag) const noexcept {
            return (tag == DataTag::UNINITIALISED || tag == tag_) ? ::std::static_pointer_cast<const void>(value_) : nullptr;
        }

        str_ptr get_str() noexcept { return get<DataTag::STR>(); }
        const_str_ptr get_str() const noexcept { return get<DataTag::STR>(); }
        int_ptr get_int() noexcept { return get<DataTag::INT>(); }
        const_int_ptr get_int() const noexcept { return get<DataTag::INT>(); }
        list_ptr get_list() noexcept { return get<DataTag::LIST>(); }
        const_list_ptr get_list() const noexcept { return get<DataTag::LIST>(); }
        dict_ptr get_dict() noexcept { return get<DataTag::DICT>(); }
        const_dict_ptr get_dict() const noexcept { return get<DataTag::DICT>(); }
        void_ptr get_any() noexcept { return get<DataTag::UNINITIALISED>(); }
        const_void_ptr get_any() const noexcept { return get<DataTag::UNINITIALISED>(); }

        template<DataTag tag>
        void set(const tag_to_type<tag> value) {
            using allocator_t = tag_to_allocator_t<tag>;
            value_ = ::std::allocate_shared<tag_to_type_t<tag>, allocator_t>(allocator_t(), value);
        }

        template<DataTag tag>
        void set(const tag_to_ptr<tag> value) noexcept {
            if (value == nullptr || tag == DataTag::UNINITIALISED) {
                tag_ = DataTag::UNINITIALISED;
                value_ = nullptr;
                return;
            }

            using type = tag_to_type<tag>;
            if (tag != tag_) {
                tag_ = tag;
                value = nullptr;
            }
            ::std::static_pointer_cast<type>(value_) = value;
        }

        void set(const DataTag tag, const ::std::shared_ptr<void> value) noexcept {
            if (value == nullptr || tag == DataTag::UNINITIALISED) {
                tag_ = DataTag::UNINITIALISED;
                value_ = nullptr;
                return;
            }
            if (tag != tag_) {
                tag_ = tag;
            }
            value_ = value;
        }

        template<typename T>
        void set(T value) { set<type_to_tag<T>::value>(value); }

        template<DataTag tag, typename... Args>
        void set(Args... args) {
            using type = tag_to_type<tag>;
            using allocator_t = tag_to_allocator_t<tag>;
            value_ = ::std::allocate_shared<type, allocator_t>(allocator_t(), type(args...));
        }

        void set_str(const str_type s) { set<DataTag::STR>(s); }
        void set_str(const str_ptr s) noexcept { set<DataTag::STR>(s); }
        template<typename... Args> void set_str(Args... args) { set<DataTag::STR>(args...); }
        void set_int(const int_type i) { set<DataTag::INT>(i); }
        void set_int(const int_ptr i) noexcept { set<DataTag::INT>(i); }
        template<typename... Args> void set_int(Args... args) { set<DataTag::INT>(args...); }
        void set_list(const list_type l) { set<DataTag::LIST>(l); }
        void set_list(const list_ptr l) noexcept { set<DataTag::LIST>(l); }
        template<typename... Args> void set_list(Args... args) { set<DataTag::LIST>(args...); }
        void set_dict(const dict_type d) { set<DataTag::DICT>(d); }
        void set_dict(const dict_ptr d) noexcept { set<DataTag::DICT>(d); }
        template<typename... Args> void set_dict(Args... args) { set<DataTag::DICT>(args...); }

        template<DataTag tag>
        static basic_data make(const tag_to_type<tag> value) {
            using allocator_t = tag_to_allocator_t<tag>;
            return basic_data(
                helpers::detail::tag_to_make_specifier<tag>::value,
                ::std::allocate_shared<tag_to_type_t<tag>, allocator_t>(allocator_t(), value)
            );
        }

        template<DataTag tag>
        static basic_data make(const tag_to_ptr<tag> value) noexcept {
            if (value == nullptr || tag == DataTag::UNINITIALISED) {
                return basic_data();
            }

            return basic_data(
                helpers::detail::tag_to_make_specifier<tag>::value,
                value
            );
        }

        static basic_data make(const DataTag tag, const ::std::shared_ptr<void> value) noexcept {
            if (value == nullptr || tag == DataTag::UNINITIALISED) {
                return basic_data();
            }
            switch (tag) {
                case DataTag::STR:
                    return basic_data(make_str, value);
                case DataTag::INT:
                    return basic_data(make_int, value);
                case DataTag::LIST:
                    return basic_data(make_list, value);
                case DataTag::DICT:
                    return basic_data(make_dict, value);
                default:
                    return basic_data();
            }
        }

        template<typename T>
        static basic_data make(T value) { return make<type_to_tag<T>::value>(value); }

        template<DataTag tag, typename... Args>
        static basic_data make(Args... args) {
            using type = tag_to_type_t<tag>;
            using allocator_t = tag_to_allocator_t<tag>;
            return basic_data(
                helpers::detail::tag_to_make_specifier<tag>::value,
                ::std::allocate_shared<type, allocator_t>(allocator_t(), type(args...))
            );
        }

        static basic_data make_str(const str_type s) { return make<DataTag::STR>(s); }
        static basic_data make_str(const str_ptr s) noexcept { return make<DataTag::STR>(s); }
        template<typename... Args> static basic_data make_str(Args... args) { return make<DataTag::STR>(args...); }
        static basic_data make_int(const int_type i) { return make<DataTag::INT>(i); }
        static basic_data make_int(const int_ptr i) noexcept { return make<DataTag::INT>(i); }
        template<typename... Args> static basic_data make_int(Args... args) { return make<DataTag::INT>(args...); }
        static basic_data make_list(const list_type l) { return make<DataTag::LIST>(l); }
        static basic_data make_list(const list_ptr l) noexcept { return make<DataTag::LIST>(l); }
        template<typename... Args> static basic_data make_list(Args... args) { return make<DataTag::LIST>(args...); }
        static basic_data make_dict(const dict_type d) { return make<DataTag::DICT>(d); }
        static basic_data make_dict(const dict_ptr d) noexcept { return make<DataTag::DICT>(d); }
        template<typename... Args> static basic_data make_dict(Args... args) { return make<DataTag::DICT>(args...); }

        void reset() noexcept { tag_ = DataTag::UNINITIALISED; value_ = nullptr; }

        ~basic_data() noexcept { tag_ = DataTag::UNINITIALISED; value_ = nullptr; }

        using path_el_type = basic_path_el<str_type>;

        template<class... PathEls, class = typename ::std::enable_if<helpers::detail::all_convertible<path_el_type, PathEls...>::value>::type>
        const basic_data* get_path(PathEls... path) const {
            const basic_data* result = this;
            const path_el_type path_arr[sizeof...(path)] = { path... };
            for (::std::size_t i = 0; i < sizeof...(path); ++i) {
                const path_el_type path_i(path_arr[i]);
                if (path_i.is_key()) {
                    if (!result->is_dict()) {
                        return nullptr;
                    }
                    dict_type& dict = result->get_dict();
                    auto search = dict.find(*path_i.get_key());
                    if (search == dict.end()) {
                        return nullptr;
                    }
                    result = &search->second;
                } else if (path_i.is_index()) {
                    if (!result->is_list()) {
                        return nullptr;
                    }
                    list_type& list = result->get_list();
                    if (path_i.get_index() >= list.size()) {
                        return nullptr;
                    }
                    result = &list.at(path_i.get_index());
                } else {
                    return nullptr;
                }
            }
            return result;
        }

        template<class... PathEls, class = typename ::std::enable_if<helpers::detail::all_convertible<path_el_type, PathEls...>::value>::type>
        basic_data* get_path(PathEls... path) {
            basic_data* result = this;
            const path_el_type path_arr[sizeof...(path)] = { path_el_type(path)... };
            for (::std::size_t i = 0; i < sizeof...(path); ++i) {
                const path_el_type path_i(path_arr[i]);
                if (path_i.is_key()) {
                    if (!result->is_dict()) {
                        return nullptr;
                    }
                    dict_type& dict = *result->get_dict();
                    auto search = dict.find(*path_i.get_key());
                    if (search == dict.end()) {
                        return nullptr;
                    }
                    result = &search->second;
                } else if (path_i.is_index()) {
                    if (!result->is_list()) {
                        return nullptr;
                    }
                    list_type& list = *result->get_list();
                    if (path_i.get_index() >= list.size()) {
                        return nullptr;
                    }
                    result = &list.at(path_i.get_index());
                } else {
                    return nullptr;
                }
            }
            return result;
        }

        template<class T>  // Where T is an iterable of basic_data::path_el_type
        const basic_data* get_path(T path) const {
            const basic_data& result = *this;
            for (const auto& el : path) {
                path_el_type path_i(el);
                if (path_i.is_key()) {
                    if (!result.is_dict()) {
                        return nullptr;
                    }
                    dict_type& dict = *result.get_dict();
                    auto search = dict.find(*path_i.get_key());
                    if (search == dict.end()) {
                        return nullptr;
                    }
                    result = search->second;
                } else if (path_i.is_index()) {
                    if (!result.is_list()) {
                        return nullptr;
                    }
                    list_type& list = *result.get_list();
                    if (path_i.get_index() >= list.size()) {
                        return nullptr;
                    }
                    result = list[path_i.get_index()];
                } else {
                    return nullptr;
                }
            }
            return &result;
        }

        template<class T>
        basic_data* get_path(T path) {
            basic_data& result = *this;
            for (const auto& el : path) {
                path_el_type path_i(el);
                if (path_i.is_key()) {
                    if (!result.is_dict()) {
                        return nullptr;
                    }
                    dict_type& dict = *result.get_dict();
                    auto search = dict.find(*path_i.get_key());
                    if (search == dict.end()) {
                        return nullptr;
                    }
                    result = search->second;
                } else if (path_i.is_index()) {
                    if (!result.is_list()) {
                        return nullptr;
                    }
                    list_type& list = *result.get_list();
                    if (path_i.get_index() >= list.size()) {
                        return nullptr;
                    }
                    result = list[path_i.get_index()];
                } else {
                    return nullptr;
                }
            }
            return &result;
        }

        template<class... PathEls, class = typename ::std::enable_if<helpers::detail::all_convertible<path_el_type, PathEls...>::value>::type>
        const ::std::pair<const ::std::size_t, const basic_data&> try_path(PathEls... path) const {
            const basic_data& result = *this;
            ::std::size_t i = 0;
            const path_el_type path_arr[sizeof...(path)] = { path... };
            for (i = 0; i < sizeof...(path); ++i) {
                const path_el_type path_i(path_arr[i]);
                if (path_i.is_key()) {
                    if (!result.is_dict()) {
                        break;
                    }
                    dict_type& dict = *result.get_dict();
                    auto search = dict.find(*path_i.get_key());
                    if (search == dict.end()) {
                        break;
                    }
                    result = search->second;
                } else if (path_i.is_index()) {
                    if (!result.is_list()) {
                        break;
                    }
                    list_type& list = *result.get_list();
                    if (path_i.get_index() >= list.size()) {
                        break;
                    }
                    result = list[path_i.get_index()];
                } else {
                    break;
                }
            }
            return { i, result };
        }

        template<class... PathEls, class = typename ::std::enable_if<helpers::detail::all_convertible<path_el_type, PathEls...>::value>::type>
        const ::std::pair<const ::std::size_t, basic_data&> try_path(PathEls... path) {
            basic_data& result = *this;
            ::std::size_t i = 0;
            const path_el_type path_arr[sizeof...(path)] = { path... };
            for (i = 0; i < sizeof...(path); ++i) {
                const path_el_type path_i(path_arr[i]);
                if (path_i.is_key()) {
                    if (!result.is_dict()) {
                        break;
                    }
                    dict_type& dict = *result.get_dict();
                    auto search = dict.find(*path_i.get_key());
                    if (search == dict.end()) {
                        break;
                    }
                    result = search->second;
                } else if (path_i.is_index()) {
                    if (!result.is_list()) {
                        break;
                    }
                    list_type& list = *result.get_list();
                    if (path_i.get_index() >= list.size()) {
                        break;
                    }
                    result = list[path_i.get_index()];
                } else {
                    break;
                }
            }
            return { i, result };
        }

        template<typename T>
        ::std::pair<const ::std::size_t, const basic_data&> try_path(T path) const {
            const basic_data& result = *this;
            ::std::size_t i = 0;
            for (auto& el : path) {
                ++i;
                path_el_type path_i(el);
                if (path_i.is_key()) {
                    if (!result.is_dict()) {
                        break;
                    }
                    dict_type& dict = *result.get_dict();
                    auto search = dict.find(*path_i.get_key());
                    if (search == dict.end()) {
                        break;
                    }
                    result = search->second;
                } else if (path_i.is_index()) {
                    if (!result.is_list()) {
                        break;
                    }
                    list_type& list = *result.get_list();
                    if (path_i.get_index() >= list.size()) {
                        break;
                    }
                    result = list[path_i.get_index()];
                } else {
                    break;
                }
            }
            return { i, result };
        }

        template<typename T>
        ::std::pair<const ::std::size_t, basic_data&> try_path(T path) {
            basic_data& result = *this;
            ::std::size_t i = 0;
            for (auto& el : path) {
                ++i;
                path_el_type path_i(el);
                if (path_i.is_key()) {
                    if (!result.is_dict()) {
                        break;
                    }
                    dict_type& dict = *result.get_dict();
                    auto search = dict.find(*path_i.get_key());
                    if (search == dict.end()) {
                        break;
                    }
                    result = search->second;
                } else if (path_i.is_index()) {
                    if (!result.is_list()) {
                        break;
                    }
                    list_type& list = *result.get_list();
                    if (path_i.get_index() >= list.size()) {
                        break;
                    }
                    result = list[path_i.get_index()];
                } else {
                    break;
                }
            }
            return { i, result };
        }

        // Serialisation
    private:
        template<typename IntType>
        static ::std::size_t serialised_int_size(IntType i) {
            if (i == 0) {
                return 1;
            }
            ::std::size_t size = 0;
            if (i < 0) {
                i = -i;
                ++size;
            }
            do {
                i /= 10;
                ++size;
            } while (i != 0);
            return size;
        }

        template<bool reserve, typename IntType, typename T, typename Traits = ::std::char_traits<T>>
        static void int_to_string(IntType i, ::std::basic_string<T, Traits>& out_str) {
            TORREBIS_CONSTEXPR_IF (helpers::detail::is_any<IntType,
                int, long, long long, unsigned, unsigned long, unsigned long long>::value) {
                TORREBIS_CONSTEXPR_IF (helpers::detail::is_same_decayed<T, char>::value) {
                    out_str.append(::std::to_string(i));
                    return;
                }
                ::std::string s = ::std::to_string(i);
                TORREBIS_CONSTEXPR_IF (reserve) {
                    ::std::size_t reservation = out_str.size() + s.size();
                    if (out_str.capacity() < reservation) {
                        out_str.reserve(reservation);
                    }
                }
                for (const char c : s) {
                    s.push_back(static_cast<T>(c));
                }
                return;
            }
            if (i == 0) {
                out_str.push_back(static_cast<T>("0"));
                return;
            }
            TORREBIS_CONSTEXPR_IF (reserve) {
                ::std::size_t reservation = out_str.size() + serialised_int_size<IntType>(i);
                if (out_str.capacity() < reservation) {
                    out_str.reserve(reservation);
                }
            }
            if (i < 0) {
                i = -i;
                out_str.push_back(static_cast<T>('-'));
            }
            do {
                IntType mod = i % 10;
                i /= 10;
                out_str.push_back(static_cast<T>(static_cast<char>('0' + static_cast<int>(mod))));
            } while (i != 0);
        }

    public:
        ::std::size_t serialised_size() const {
            switch (get_tag()) {
                case DataTag::STR: {
                    ::std::size_t str_size = get_str()->size();
                    return serialised_int_size<::std::size_t>(str_size) + 1 + str_size;
                }
                case DataTag::INT:
                    return 2 + serialised_int_size<int_type>(*get_int());
                case DataTag::LIST: {
                    ::std::size_t size = 2;
                    for (const basic_data& i : *get_list()) {
                        size += i.serialised_size();
                    }
                    return size;
                }
                case DataTag::DICT: {
                    ::std::size_t size = 2;
                    for (const auto& i : *get_dict()) {
                        ::std::size_t key_size = i.first.size();
                        size += serialised_int_size<::std::size_t>(key_size) + 1 + key_size + i.second.serialised_size();
                    }
                    return size;
                }
                default:
                    return 0;
            }
        }

        template<class T = char, class Traits = ::std::char_traits<T>>
        ::std::basic_string<T, Traits> serialise() const {
            ::std::basic_string<T, Traits> s;
            s.reserve(serialised_size());
            append_serialisation(s);
            return s;
        }

    private:
        template<class T, class Traits>
        void append_serialisation_loop(::std::basic_string<T, Traits>& s) const {
            switch (get_tag()) {
                case DataTag::STR: {
                    const str_type& ss = *get_str();
                    int_to_string<false, ::std::size_t, T, Traits>(ss.size(), s);
                    s.push_back(static_cast<T>(':'));
                    TORREBIS_CONSTEXPR_IF (helpers::detail::is_same_decayed<str_type, ::std::basic_string<T, Traits>>::value) {
                        s.append(ss);
                    } else {
                        for (const auto byte : ss) {
                            s.push_back(static_cast<T>(static_cast<char>(ss)));
                        }
                    }
                    return;
                }
                case DataTag::INT:
                    s.push_back(static_cast<T>('i'));
                    int_to_string<false, int_type, T, Traits>(*get_int(), s);
                    s.push_back(static_cast<T>('e'));
                    return;
                case DataTag::LIST:
                    s.push_back(static_cast<T>('l'));
                    for (const basic_data& i : *get_list()) {
                        i.append_serialisation_loop<T, Traits>(s);
                    }
                    s.push_back(static_cast<T>('e'));
                    return;
                case DataTag::DICT:
                    s.push_back(static_cast<T>('d'));
                    for (const auto& i : *get_dict()) {
                        const str_type& key = i.first;
                        int_to_string<false, ::std::size_t, T, Traits>(key.size(), s);
                        s.push_back(static_cast<T>(':'));
                        TORREBIS_CONSTEXPR_IF (helpers::detail::is_same_decayed<str_type, ::std::basic_string<T, Traits>>::value) {
                            s.append(key);
                        } else {
                            for (const auto byte : key) {
                                s.push_back(static_cast<T>(static_cast<char>(byte)));
                            }
                        }
                        i.second.template append_serialisation_loop<T, Traits>(s);
                    }
                    s.push_back(static_cast<T>('e'));
                    return;
                default:
                    return;
            }
        }

    public:
        template<class T = char, class Traits = ::std::char_traits<T>>
        void append_serialisation(::std::basic_string<T, Traits>& s) const {
            const ::std::size_t reservation = s.size() + serialised_size();
            if (reservation > s.capacity()) {
                s.reserve(reservation);
            }
            append_serialisation_loop<T, Traits>(s);
        }

        template<class T = char, class Traits = std::char_traits<T>>
        void serialise(::std::basic_ostream<T, Traits>& os) const {
            switch (get_tag()) {
                case DataTag::STR: {
                    const str_type& ss = *get_str();
                    ::std::basic_string<T, Traits> size_str;
                    int_to_string<true, ::std::size_t, T, Traits>(ss.size(), size_str);
                    os.write(size_str.data(), size_str.size());
                    os.put(static_cast<T>(':'));
                    TORREBIS_CONSTEXPR_IF (helpers::detail::has_char_t<T, str_type>::value) {
                        os.write(ss.data(), ss.size());
                    } else {
                        for (const auto byte : ss) {
                            os.put(static_cast<T>(static_cast<char>(byte)));
                        }
                    }
                    return;
                }
                case DataTag::INT: {
                    os.put(static_cast<T>('i'));
                    ::std::basic_string<T, Traits> int_str;
                    int_to_string<true, int_type, T, Traits>(*get_int(), int_str);
                    os.write(int_str.data(), int_str.size());
                    os.put(static_cast<T>('e'));
                    return;
                }
                case DataTag::LIST:
                    os.put(static_cast<T>('l'));
                    for (const basic_data& i : *get_list()) {
                        i.serialise<T, Traits>(os);
                    }
                    os.put(static_cast<T>('e'));
                    return;
                case DataTag::DICT:
                    os.put(static_cast<T>('d'));
                    for (const ::std::pair<const str_type, const basic_data>& i : *get_dict()) {
                        const str_type& key = i.first;
                        ::std::basic_string<T, Traits> key_size_str;
                        int_to_string<true, ::std::size_t, T, Traits>(key.size(), key_size_str);
                        os.write(key_size_str.data(), key_size_str.size());
                        os.put(static_cast<T>(':'));
                        TORREBIS_CONSTEXPR_IF (helpers::detail::has_char_t<T, str_type>::value) {
                            os.write(key.data(), key.size());
                        } else {
                            for (const auto byte : key) {
                                os.put(static_cast<T>(static_cast<char>(byte)));
                            }
                        }
                        i.second.template serialise<T, Traits>(os);
                    }
                    os.put(static_cast<T>('e'));
                    return;
                default:
                    return;
            }
        }

        // Deserialisation of pointers / arrays
    private:
        template<typename IntType, typename T>
        static IntType pointer_read_int(const T* const begin, ::std::size_t& i, const ::std::size_t size, const char end, const char* const first) {
            char first_char;
            if (first == nullptr) {
                if (i >= size) {
                    throw errors::expecting_int_error.copy();
                }
                first_char = static_cast<char>(begin[i++]);
            } else {
                first_char = *first;
            }
            if (first_char == '0') {
                if (i >= size || static_cast<char>(begin[i++]) != end) {
                    throw errors::expecting_int_error.copy();
                }
                return IntType(0);
            }
            int sign = +1;
            if (first_char == '-') {
                sign = -1;
                if (i >= size) {
                    throw errors::expecting_int_error.copy();
                }
                first_char = static_cast<char>(begin[i++]);
            }
            if (!('1' <= first_char && first_char <= '9')) {
                throw errors::expecting_int_error.copy();
            }
            IntType res = IntType(static_cast<int>(first_char - '0'));
            while (true) {
                if (i >= size) {
                    throw errors::expecting_int_error.copy();
                }
                const char next_char = static_cast<char>(begin[i++]);
                if (!('0' <= next_char && next_char <= '9')) {
                    if (next_char == end) {
                        break;
                    }
                    throw errors::expecting_int_error.copy();
                }
                IntType previous_res = res;
                res = res * IntType(static_cast<int>(sign * 10)) + IntType(static_cast<int>(sign * static_cast<int>(next_char - '0')));
                if ((previous_res < res) != (sign > 0)) {
                    throw errors::integer_overflow_error.copy();
                }
            }
            return res;
        }

        static ::std::pair<const char*, ::std::size_t> pointer_read_str_fast(const char* const begin, ::std::size_t& i, const ::std::size_t size, const char* const first) {
            ::std::size_t str_size = pointer_read_int<::std::size_t, char>(begin, i, size, ':', first);
            const char* str_begin = begin + i;
            i += str_size;
            if (i > size) {
                throw errors::cut_off_str_error.copy();
            }
            return  { str_begin, str_size };
        }

        template<typename T>
        static void pointer_read_str_slow(const T* const begin, ::std::size_t& i, const ::std::size_t size, const char* first, ::std::string& out_str) {
            ::std::size_t str_size = pointer_read_int<::std::size_t, T>(begin, i, size, ':', first);
            if (i + str_size > size) {
                throw errors::cut_off_str_error.copy();
            }
            out_str.reserve(str_size);
            for (; i < i + str_size; ++i) {
                out_str.push_back(static_cast<char>(begin[i]));
            }
        }

        template<typename T>
        static basic_data pointer_bdecode_loop(
            const T* const begin, ::std::size_t& i, const ::std::size_t size
        ) {
            if (i >= size) {
                throw errors::expecting_value_error.copy();
            }
            switch (static_cast<char>(begin[i])) {
                case 'i': {
                    // int "ixxxe"
                    ++i;
                    return make_int(pointer_read_int<int_type, T>(begin, i, size, 'e', nullptr));
                }
                case 'l': {
                    // list "lxxxe"
                    ++i;
                    list_type list;
                    while (true) {
                        if (i >= size) {
                            throw errors::expecting_value_error.copy();
                        }
                        if (static_cast<char>(begin[i]) == 'e') {
                            break;
                        }
                        list.push_back(pointer_bdecode_loop<T>(begin, i, size));
                    }
                    ++i;
                    return make_list(list);
                }
                case 'd': {
                    // dict "dxxxe"
                    ++i;
                    dict_type dict;
                    const char* last_key_begin = nullptr;
                    ::std::size_t last_key_size;
                    while (true) {
                        if (i >= size) {
                            throw errors::expecting_value_error.copy();
                        }
                        if (static_cast<char>(begin[i]) == 'e') {
                            break;
                        }
                        const char* key_begin;
                        ::std::size_t key_size;
                        TORREBIS_CONSTEXPR_IF (helpers::detail::is_same_decayed<T, char>::value) {
                            auto begin_size = pointer_read_str_fast(begin, i, size, nullptr);
                            key_begin = begin_size.first;
                            key_size = begin_size.second;
                            if (last_key_begin != nullptr) {
                                // TODO
                                if (!helpers::detail::key_less_than(last_key_begin, last_key_size, key_begin, key_size)) {
                                    throw errors::key_order_error.copy();
                                }
                            }
                            last_key_begin = key_begin;
                            last_key_size = key_size;
                            str_type key(key_begin, key_size);
                            dict[key] = pointer_bdecode_loop<T>(begin, i, size);
                        } else {
                            ::std::string key;
                            pointer_read_str_slow<T>(begin, i, size, nullptr, key);
                            key_begin = key.data();
                            key_size = key.size();
                            if (last_key_begin != nullptr) {
                                if (!helpers::detail::key_less_than(last_key_begin, last_key_size, key_begin, key_size)) {
                                    throw errors::key_order_error.copy();
                                }
                            }
                            last_key_begin = key_begin;
                            last_key_size = key_size;
                            TORREBIS_CONSTEXPR_IF (helpers::detail::is_same_decayed<str_type, ::std::string>::value) {
                                dict[key] = pointer_bdecode_loop<T>(begin, i, size);
                            } else {
                                str_type real_key(key_begin, key_size);
                                dict[real_key] = pointer_bdecode_loop<T>(begin, i, size);
                            }
                        }
                    }
                    ++i;
                    return make_dict(dict);
                }
                case '1': case '2': case '3':
                case '4': case '5': case '6':
                case '7': case '8': case '9': {
                    // str "yy:xxxx"
                    TORREBIS_CONSTEXPR_IF (helpers::detail::is_same_decayed<T, char>::value) {
                        auto begin_size = pointer_read_str_fast(begin, i, size, &begin[i++]);
                        const char* str_begin = begin_size.first;
                        ::std::size_t str_size = begin_size.second;
                        return make_str(str_type(str_begin, str_size));
                    }
                    ::std::string s;
                    pointer_read_str_slow<T>(begin, i, size, &begin[i++], s);
                    TORREBIS_CONSTEXPR_IF (helpers::detail::is_same_decayed<str_type, ::std::string>::value) {
                        return make_str(s);
                    }
                    return make_str(str_type(s.data(), s.size()));
                }
                default:
                    throw errors::unknown_value_error.copy();
            }
        }

    public:
        template<typename T>
        static basic_data load(const T* const begin, const ::std::size_t size) {
            static_assert(helpers::detail::is_byte<T>::value, "Must be a string of a byte-like value (char, unsigned char, signed char, std::uint8_t, std::int8_t)");
            ::std::size_t i = 0;
            auto parsed = pointer_bdecode_loop(begin, i, size);
            if (i != size) {
                throw errors::early_end_error.copy();
            }
            return parsed;
        }

        template<typename T>
        static basic_data load(const T* const begin, const T* const end) {
            return load<T>(begin, static_cast<::std::size_t>(end - begin));
        }

        template<typename T, typename... Rest>
        static basic_data load(const ::std::basic_string<T, Rest...>& s) {
            return load<T>(s.data(), s.size());
        }

        template<typename T, typename... Rest>
        static basic_data load(const ::std::vector<T, Rest...>& v) {
            return load<T>(v.data(), v.size());
        }

#ifdef TORREBIS_STD_ARRAY_HAS_DATA
        template<typename T, ::std::size_t N = 0>
        static basic_data load(const ::std::array<T, N>& a) {
            return load<T>(a.data(), a.size());
        }
#endif
        // Deserialisation of iterators
    private:

        template<typename IntType, typename Iterator>
        static IntType array_read_int(Iterator& it, const Iterator& end, const char end_char, const char* const first) {
            char first_char;
            if (first == nullptr) {
                if (it == end) {
                    throw errors::expecting_int_error.copy();
                }
                first_char = static_cast<char>(*it);
                ++it;
            } else {
                first_char = *first;
            }
            if (first_char == '0') {
                if (it == end || static_cast<char>(*it) != end_char) {
                    throw errors::expecting_int_error.copy();
                }
                ++it;
                return IntType(0);
            }
            int sign = +1;
            if (first_char == '-') {
                sign = -1;
                if (it == end) {
                    throw errors::expecting_int_error.copy();
                }
                first_char = static_cast<char>(*it);
                ++it;
            }
            if (!('1' <= first_char && first_char <= '9')) {
                throw errors::expecting_int_error.copy();
            }
            IntType res = IntType(static_cast<int>(first_char - '0'));
            while (true) {
                if (it == end) {
                    throw errors::expecting_int_error.copy();
                }
                const char next_char = static_cast<char>(*it);
                ++it;
                if (!('0' <= next_char && next_char <= '9')) {
                    if (next_char == end_char) {
                        break;
                    }
                    throw errors::expecting_int_error.copy();
                }
                IntType previous_res = res;
                res = res * IntType(static_cast<int>(sign * 10)) + IntType(static_cast<int>(sign * static_cast<int>(next_char - '0')));
                if ((previous_res < res) != (sign > 0)) {
                    throw errors::integer_overflow_error.copy();
                }
            }
            return res;
        }

        template<typename Iterator>
        static void array_read_str(Iterator& it, const Iterator& end, const char* first, ::std::string& out_str) {
            ::std::size_t str_size = array_read_int<::std::size_t, Iterator>(it, end, ':', first);
            out_str.reserve(str_size);
            for (; str_size-- > 0;) {
                if (it == end) {
                    throw errors::cut_off_str_error.copy();
                }
                out_str.push_back(static_cast<char>(*it));
                ++it;
            }
        }

        template<typename Iterator>
        static basic_data array_bdecode_loop(Iterator& it, const Iterator& end) {
            if (it == end) {
                throw errors::expecting_value_error.copy();
            }
            switch (static_cast<char>(*it)) {
                case 'i':
                    // int "ixxxe"
                    ++it;
                    return make_int(array_read_int<int_type, Iterator>(it, end, 'e', nullptr));
                case 'l': {
                    // list "lxxxe"
                    ++it;
                    list_type list;
                    while (true) {
                        if (it == end) {
                            throw errors::expecting_value_error.copy();
                        }
                        if (static_cast<char>(*it) == 'e') {
                            break;
                        }
                        list.push_back(array_bdecode_loop<Iterator>(it, end));
                    }
                    ++it;
                    return make_list(list);
                }
                case 'd': {
                    // dict "dxxxe"
                    ++it;
                    dict_type dict;
                    const char* last_key_begin = nullptr;
                    ::std::size_t last_key_size;
                    while (true) {
                        if (it == end) {
                            throw errors::expecting_value_error.copy();
                        }
                        if (static_cast<char>(*it) == 'e') {
                            break;
                        }
                        const char* key_begin;
                        ::std::size_t key_size;
                        ::std::string key;
                        array_read_str<Iterator>(it, end, nullptr, key);
                        key_begin = key.data();
                        key_size = key.size();
                        if (last_key_begin != nullptr) {
                            if (!helpers::detail::key_less_than(last_key_begin, last_key_size, key_begin, key_size)) {
                                throw errors::key_order_error.copy();
                            }
                        }
                        last_key_begin = key_begin;
                        last_key_size = key_size;
                        TORREBIS_CONSTEXPR_IF (helpers::detail::is_same_decayed<str_type, ::std::string>::value) {
                            dict[key] = array_bdecode_loop<Iterator>(it, end);
                        } else {
                            str_type real_key(key_begin, key_size);
                            dict[real_key] = array_bdecode_loop<Iterator>(it, end);
                        }
                    }
                    ++it;
                    return make_dict(dict);
                }
                case '1': case '2': case '3':
                case '4': case '5': case '6':
                case '7': case '8': case '9': {
                    // str "yy:xxxx"
                    ::std::string s;
                    array_read_str<Iterator>(it, end, nullptr, s);
                    TORREBIS_CONSTEXPR_IF (helpers::detail::is_same_decayed<str_type, ::std::string>::value) {
                        return make_str(s);
                    }
                    return make_str(str_type(s.data(), s.size()));
                }
                default:
                    throw errors::unknown_value_error.copy();
            }
        }

    public:

        template<typename Iterator>
        static basic_data load(Iterator it, const Iterator& end) {
            auto parsed = array_bdecode_loop<Iterator>(it, end);
            if (it != end) {
                throw errors::early_end_error.copy();
            }
            return parsed;
        }

        // Deserialisation of istreams

    private:

        template<typename IntType, class T, class Traits>
        static IntType stream_read_int(::std::basic_istream<T, Traits>& is, const char end_char, const char* const first) {
            T first_byte;
            if (first == nullptr) {
                if (!is.read(&first_byte, 1)) {
                    throw errors::expecting_int_error.copy();
                }
            } else {
                first_byte = static_cast<T>(*first);
            }
            if (first_byte == static_cast<T>('0')) {
                if (!is.read(&first_byte, 1) || static_cast<char>(first_byte) != end_char) {
                    throw errors::expecting_int_error.copy();
                }
                return IntType(0);
            }
            int sign = +1;
            if (first_byte == static_cast<T>('-')) {
                sign = -1;
                if (!is.read(&first_byte, 1)) {
                    throw errors::expecting_int_error.copy();
                }
            }
            if (!(static_cast<T>('1') <= first_byte && first_byte <= static_cast<T>('9'))) {
                throw errors::expecting_int_error.copy();
            }
            IntType res = IntType(static_cast<int>(static_cast<char>(first_byte) - '0'));
            while (true) {
                T next_byte;
                if (!is.read(&next_byte, 1)) {
                    throw errors::expecting_int_error.copy();
                }

                if (!(static_cast<T>('0') <= next_byte && next_byte <= static_cast<T>('9'))) {
                    if (static_cast<char>(next_byte) == end_char) {
                        break;
                    }
                    throw errors::expecting_int_error.copy();
                }
                IntType previous_res = res;
                res = res * IntType(static_cast<int>(sign * 10)) + IntType(static_cast<int>(sign * static_cast<int>(static_cast<char>(next_byte) - '0')));
                if ((previous_res < res) != (sign > 0)) {
                    throw errors::integer_overflow_error.copy();
                }
            }
            return res;
        }

        template<class T, class Traits>
        static ::std::basic_string<T, Traits> stream_read_str(::std::basic_istream<T, Traits>& is, const char* first) {
            ::std::size_t str_size = stream_read_int<::std::size_t, T, Traits>(is, ':', first);
            ::std::basic_string<T, Traits> out_str(str_size, static_cast<T>('\0'));
            if (!is.read(&out_str[0], static_cast<::std::streamsize>(str_size))) {
                throw errors::cut_off_str_error.copy();
            }
            return out_str;
        }

        template<class T, class Traits>
        static basic_data stream_bdecode_loop(::std::basic_istream<T, Traits>& is, T* first) {
            if (!is) {
                throw errors::bad_stream_error.copy();
            }
            T start_byte;
            if (first == nullptr) {
                if (!is.read(&start_byte, 1)) {
                    throw errors::expecting_value_error.copy();
                }
            } else {
                start_byte = *first;
            }
            switch (static_cast<char>(start_byte)) {
                case 'i':
                    // int "ixxxe"
                    return make_int(stream_read_int<int_type, T, Traits>(is, 'e', nullptr));
                case 'l': {
                    // list "lxxxe"
                    list_type list;
                    while (true) {
                        T end_byte;
                        if (!is.read(&end_byte, 1)) {
                            throw errors::expecting_value_error.copy();
                        }
                        if (static_cast<char>(end_byte) == 'e') {
                            break;
                        }
                        list.push_back(stream_bdecode_loop<T, Traits>(is, &end_byte));
                    }
                    return make_list(list);
                }
                case 'd': {
                    // dict "dxxxe"
                    dict_type dict;
                    const char* last_key_begin = nullptr;
                    ::std::size_t last_key_size;
                    while (true) {
                        T end_byte;
                        if (!is.read(&end_byte, 1)) {
                            throw errors::expecting_value_error.copy();
                        }
                        if (static_cast<char>(end_byte) == 'e') {
                            break;
                        }
                        const char* key_begin;
                        ::std::size_t key_size;
                        ::std::basic_string<T, Traits> key = stream_read_str<T, Traits>(is, &end_byte);
                        key_begin = key.data();
                        key_size = key.size();
                        if (last_key_begin != nullptr) {
                            if (!helpers::detail::key_less_than<T>(last_key_begin, last_key_size, key_begin, key_size)) {
                                throw errors::key_order_error.copy();
                            }
                        }
                        last_key_begin = key_begin;
                        last_key_size = key_size;
                        TORREBIS_CONSTEXPR_IF (helpers::detail::is_same_decayed<::std::basic_string<T, Traits>, str_type>::value) {
                            dict[key] = stream_bdecode_loop<T, Traits>(is, nullptr);
                        } else TORREBIS_CONSTEXPR_IF (helpers::detail::has_char_t<T, str_type>::value) {
                            str_type real_key(key_begin, key_size);
                            dict[real_key] = stream_bdecode_loop<T, Traits>(is, nullptr);
                        } else {
                            str_type real_key(key_begin, key_size);
                            dict[real_key] = stream_bdecode_loop<T, Traits>(is, nullptr);
                        }
                    }
                    return make_dict(dict);
                }
                case '1': case '2': case '3':
                case '4': case '5': case '6':
                case '7': case '8': case '9': {
                    // str "yy:xxxx"
                    ::std::basic_string<T, Traits> s = stream_read_str<T, Traits>(is, &start_byte);
                    TORREBIS_CONSTEXPR_IF (helpers::detail::is_same_decayed<str_type, ::std::basic_string<T, Traits>>::value) {
                        return make_str(s);
                    }
                    return make_str(str_type(s.data(), s.size()));
                }
                default:
                    throw errors::unknown_value_error.copy();
            }
        }

    public:
        template<class T = char, class Traits = ::std::char_traits<T>>
        static basic_data load(::std::basic_istream<T, Traits>& is) {
            auto parsed = stream_bdecode_loop<T, Traits>(is, nullptr);
            if (!is) {
                throw errors::bad_stream_error.copy();
            }
            if (is.peek() != Traits::eof()) {
                throw errors::early_end_error.copy();
            }
            return parsed;
        }

    };

    using data = basic_data<>;

    namespace helpers { namespace detail {
        template<class T>
        struct is_basic_data_ : ::std::false_type {};

        TORREBIS_BASIC_DATA_TEMPLATE
        struct is_basic_data_<TORREBIS_BASIC_DATA_TEMPLATE_TYPE> : ::std::true_type {};

        template<class T>
        struct is_basic_data : is_basic_data_<typename remove_cv_ref<T>::type> {};
    } }  // namespace torrebis::bencode::helpers::detail

} }  // namespace torrebis::bencode

#endif  // TORREBIS_BENCODE_DATA_CLASS_H_
