#ifndef TORREBIS_BENCODE_PATH_EL_H_
#define TORREBIS_BENCODE_PATH_EL_H_

#include <string>
#include <memory>
#include <type_traits>

#include "torrebis_bencode/type_tags.h"


namespace torrebis { namespace bencode {
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

#ifdef TORREBIS_HAS_IS_NOTHROW_CONVERTIBLE
        template<class T>
        basic_path_el(
            const typename ::std::enable_if<::std::is_convertible<T, index_type>::value && !::std::is_nothrow_convertible<T, index_type>::value, T>::type i
        ) : tag_(DataTag::INT), key_or_index_(static_cast<::std::size_t>(i)) {}

        template<class T>
        basic_path_el(
            const typename ::std::enable_if<::std::is_nothrow_convertible<T, index_type>::value, T>::type i
        ) noexcept : tag_(DataTag::INT), key_or_index_(static_cast<::std::size_t>(i)) {}
#else
        template<class T>
        basic_path_el(
            const typename ::std::enable_if<::std::is_convertible<T, index_type>::value && !::std::is_integral<T>::value, T>::type i
        ) : tag_(DataTag::INT), key_or_index_(static_cast<::std::size_t>(i)) {}

        template<class T>
        basic_path_el(
            const typename ::std::enable_if<::std::is_integral<T>::value, T>::type i
        ) noexcept : tag_(DataTag::INT), key_or_index_(static_cast<::std::size_t>(i)) {}
#endif

        basic_path_el(const index_type i) noexcept : tag_(DataTag::INT), key_or_index_(i) {}

        basic_path_el(const str_type k) : tag_(DataTag::STR), key_or_index_(k) {}
        basic_path_el(const ::std::shared_ptr<const str_type> k) : tag_(DataTag::STR), key_or_index_(k ? str_type(*k) : nullptr) {}
        basic_path_el(const str_ptr_type k) noexcept : tag_(DataTag::STR), key_or_index_(k) {}
        basic_path_el(const basic_path_el& other) noexcept : tag_(other.tag_) { if (tag_ == DataTag::STR) { key_or_index_.key = other.get_key(); } else { key_or_index_.index = other.get_index(); } }
        basic_path_el(const basic_path_el&& other) noexcept : tag_(other.tag_) { if (tag_ == DataTag::STR) { key_or_index_.key = other.get_key(); } else { key_or_index_.index = other.get_index(); } }
        template<typename... Args, class = typename helpers::detail::path_el_str_constructor_enable_if<Args...>::type>
        basic_path_el(const Args... args) : tag_(DataTag::STR), key_or_index_(str_type(args...)) {}
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

        bool is_key() const noexcept { return tag_ == DataTag::STR && key_or_index_.key != nullptr; }
        bool is_index() const noexcept { return tag_ == DataTag::INT; }

        str_ptr_type get_key() const noexcept { return is_key() ? key_or_index_.key : nullptr; }
        index_type get_index() const noexcept { return is_index() ? key_or_index_.index : static_cast<index_type>(-1); }

        void set_key(str_type k) { tag_ = DataTag::STR; key_or_index_.key = k; }
        void set_key(str_ptr_type k) noexcept {
            if (is_index()) {
                key_or_index_.index = 0;
            }
            tag_ = DataTag::STR;
            key_or_index_.key = k;
        }
        void set_index(index_type i) noexcept {
            if (is_key()) {
                key_or_index_.key = nullptr;
            }
            tag_ = DataTag::INT;
            key_or_index_.index = i;
        }

        ~basic_path_el() noexcept {
            if (is_key()) {
                key_or_index_.key = nullptr;
            } else if (is_index()) {
                key_or_index_.index = static_cast<index_type>(-1);
            }
            tag_ = DataTag::UNINITIALISED;
        }
    };

    using path_el = basic_path_el<>;
} }  // namespace torrebis::bencode

#endif  // TORREBIS_BENCODE_PATH_EL_H_
