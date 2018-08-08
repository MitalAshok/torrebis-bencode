#ifndef TORREBIS_BENCODE_VISITORS_H_
#define TORREBIS_BENCODE_VISITORS_H_

#include <vector>

#include <torrebis_bencode/type_tags.h>
#include <torrebis_bencode/data_class.h>

namespace torrebis { namespace bencode {

    template<typename BasicData = data>
    class visitor {
    public:
        using data_t = const BasicData&;
        using path_t = const ::std::vector<typename BasicData::path_el_type>&;
        using str_type = const typename BasicData::const_str_ptr;
        using int_type = const typename BasicData::const_int_ptr;
        using list_type = const typename BasicData::const_list_ptr;
        using dict_type = const typename BasicData::const_dict_ptr;

        virtual void visit(const data_t dat) {
            // Start the visit with an empty path.
            typename ::std::remove_reference<path_t>::type path;
            visit_any(dat, path);
            return;
        }
    protected:
        virtual void visit_any(data_t dat, const path_t path) {
            switch (dat.get_tag()) {
                case DataTag::STR:
                    visit_str(dat.get_str(), path);
                    return;
                case DataTag::INT:
                    visit_int(dat.get_int(), path);
                    return;
                case DataTag::LIST:
                    visit_list(dat.get_list(), path);
                    return;
                case DataTag::DICT:
                    visit_dict(dat.get_dict(), path);
                    return;
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
            typename ::std::remove_const<typename ::std::remove_reference<path_t>::type>::type new_path(path);
            ::std::size_t i = 0;
            new_path.push_back(0);
            for (const data_t& el : *list) {
                new_path[new_path.size() - 1] = i++;
                visit_item(el, new_path);
            }
        }

        virtual void visit_item(data_t item, const path_t path) {
            // Just visit it with visit_any.
            return visit_any(item, path);
        }

        virtual void visit_dict(const dict_type dict, const path_t path) {
            // Visit:
            //    keys with nullptr at the end of the path.
            //    values with the key at the end of the path.
            typename ::std::remove_const<typename ::std::remove_reference<path_t>::type>::type new_path(path);
            ::std::size_t i = 0;
            new_path.push_back(0);
            for (const auto& el : *dict) {
                data_t key = BasicData::make_str(el.first);
                visit_key(key, new_path);
                new_path[new_path.size() - 1] = key.get_str();
                visit_value(el.second, new_path);
                new_path[new_path.size() - 1] = ++i;
            }
        }

        virtual void visit_key(data_t k, const path_t path) {
            visit_any(k, path);
            return;
        }

        virtual void visit_value(data_t value, const path_t path) {
            // Just visit it with visit_any.
            visit_any(value, path);
            return;
        }
    };

    /*
     * Outputs some data to a stream.
     */
    template<typename BasicData = data>
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

#endif  // TORREBIS_BENCODE_VISITORS_H_
