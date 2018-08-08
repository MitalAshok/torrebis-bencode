# torrebis::bencode

A C++11 library for dealing with bencoded data. Version 1.0.0

To install, either just include `single_include/torrebis_bencode.h`, or make a shared library object (`.so` or `.dll`) by running cmake.

`namespace bencode = ::torrebis::bencode` is recommended if you do not use the full `torrebis` library, as only
the `bencode` namespace is defined inside `torrebis` here.

The class `bencode::data` is the main type.

Use `data::load(std::string)`, `data::load(const char*, std::size_t size)`, `data::load(Iterator begin, Iterator end)` or
`data::load(std::istream&)` to load data from a string / iterable into a `data` instance. Also accepts
`signed` / `unsigned char`, `std::uint8_t` and `std::int8_t`.

Constructing new values programmatically can be done using the `make_TYPE` static methods.

```C++
namespace bencode = ::torrebis::bencode;

bencode::data my_str = bencode::data::make_str("String");
bencode::data my_int = bencode::data::make_int(1);
bencode::data my_list = bencode::data::make_list({
    bencode::data::make_int(1),
    bencode::data::make_int(2)
});
bencode::data my_dict = bencode::data::make_dict({
    { "String (Not bencode::data)", bencode::data::make_str("arbitrary bencode::data") },
    { "Because keys are always strings", bencode::data::make_int(3) }
});

// All the `make_TYPE` functions pass their arguments to the constructors
// for the types backing them
// (by default, std::string, std::intmax_t, std::vector and std::map)
```

Accessing values is done with type tags. `bencode::DataTag` is an enum with 4 values:
`bencode::DataTag::STR`, `bencode::DataTag::INT`, `bencode::DataTag::LIST` and
`bencode::DataTag::DICT`.

Call `DataTag data::get_tag()` to get the current tag (And use in a switch statement for unknown values).
You can also use the for helper functions, `bool data::is_str()`, `bool data::is_int()`, `bool data::is_list()` and
`bool data::is_dict()`.

Use `std::shared_ptr<T> get<DataTag>()` to get a pointer to a value. It will be `nullptr` if the type is incorrect,
but you can safely dereference it if the tag was correct.

```C++
std::ifstream ifs("test.torrent", std::ios::in | std::ios::binary)
bencode::data my_data = bencode::data::load(ifs);

// It should be a dict. Can do one of:

bencode::data::dict_ptr_type dp = my_data.get_dict();  // or `my_data.get<bencode::DataTag::DICT>()`
if (d != nullptr) { bencode::data::dict_type& d = *dp;  /* do stuff with d */ }

// or

std::string s = read_file("test.torrent");
bencode::data my_data = bencode::data::load(s);

if (my_data.is_dict()) {  // or `my_data.get_tag() == bencode::DataTag::DICT` or `my_data.is<bencode::DataTag::DICT>`.
    bencode::data::dict_type& d = *my_data.get_dict();
    // Do stuff with d
}
```

Using tags is useful for generic functions. There is also a tag version of the `make` methods, `basic_data make<DataTag>(Args... args)`.

There are multiple helper templates in the `bencode::data` class to access types related to values. All of
these have a template of `<DataTag tag>`.

 - `tag_to_type`: `str_type`, `int_type`, `list_type` or `dict_type`.
   These are the types that were given in the template of `basic_data` (See [Customising types](#customising-types)).
   `list_type` and `dict_type` have their templates filled in with value types.
 - `tag_to_allocator_t`: `str_allocator_t`, `int_allocator_t`, `list_allocator_t` or `dict_allocator_t`.
   Just the `Allocator` template parameter filled with the type.
   
 - `tag_to_ptr`: `str_ptr`, `int_ptr`, `list_ptr` or `dict_ptr`. The return types of the `get` functions.
 - `tag_to_const_ptr`: A pointer to a const-qualified value, same as above. (`const_str_ptr`, etc.)
 - `type_to_tag`: A reverse map of `type_to_tag`, so the syntax is `type_to_tag<typename T>::value`.

For example:

```C++
template<DataTag tag>
void do_something_with_arbitrary_data(bencode::data dat) {
    typename bencode::data::tag_to_ptr<tag> value = dat.get<tag>();
    // Although in this case you can just do `auto value = dat.get<tag>();`.

    // For example, check something with the type.
    if /* constexpr */ (has_property<bencode::data::tag_to_type<tag>>::value) {
        fast_operation(value);
    } else {
        slow_operation(value);
    }
}
```

## Paths

A quick way to access deep into some bencoded data without doing:

```C++
bencode::data dat = bencode::data::load(file);

// Want dat["info"]["pieces"]["files"][0]["length"]
bool found = false;
if (dat.is_dict()) {
    auto& d = *dat.get_dict();
    auto i = d.find("info");
    if (i != d.end()) {
        dat = i.second;  // original_dat["info"]
        if (dat.is_dict()) {
            d = *dat.get_dict();
            i = d.find("pieces");
            if (i != d.end()) {
                dat = i.second;  // original_dat["info"]["pieces"]
                if (dat.is_dict()) {
                    d = *dat.get_dict();
                    i = d.find("files");
                    if (i != d.end()) {
                        dat = i.second;  // original_dat["info"]["pieces"]["files"]
                        if (dat.is_list()) {
                            auto& l = *dat.get_list();
                            if (l.size() > 0) {
                                dat = l[0];  // original_dat["info"]["pieces"]["files"][0]
                                if (dat.is_dict()) {
                                    d = *dat.get_dict();
                                    i = d.find("length");
                                    if (i != d.end()) {
                                        dat = i.second;  // original_dat["info"]["pieces"]["files"][0]["length"]
                                        // Done! Finally.
                                        found = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

if (!found) { throw error; }
// Do something with dat.
```

you can do it without 10 levels of indentation with paths.

For the standard `data` class, `bencode::path_el` can be used.
For a specilisation of `basic_data`, use `basic_data<...>::path_el_type`.

Use `data::get_path(<iterable of path_el_type>)` or `data::get_path(path_el_type...)`.
It returns a pointer to a single `data` if found, otherwise a `nullptr`.

`data::try_path` has the same signature,
but returns `size_t depth`, `data last`, where `depth` is how deep into the path it got, and `last` is the last `data` before the path
could be continued (for example, because the data was the wrong type, the list was too short or
the specified key did not exist.)

```C++
using p = bencode::data::path_el_type;

bencode::data dat = bencode::data::load(file);

// Same as above. Want dat["info"]["pieces"]["files"][0]["length"]

auto found = dat.get_path("info", "pieces", "files", 0U, "length");
if (found.first == nullptr) { throw error; }
dat = *found;
// Do something with dat.


// Could also do something like
auto found = dat.try_path("info", "pieces", "files", 0U, "length");
if (found.first != 5) {
    if (found.first == 2) {
        dat = found.second;  // dat is original_dat["info"]["pieces"].
        // Do something with a different value.
    } else {
        throw error;
    }
}
```

## Customising types

`torrebis::bencode::data` is a template specilisation of `torrebis::bencode::basic_data` with all values set to the
default. The template parameters are:

 - `class StrType`: The class of string values. (Binary strings. Defaults to `std::string`.)
 - `class IntType`: The class of integer values. Should be a signed integral. Defaults to `std::intmax_t`.
 - `template<typename ValueType> class ListType`: Template class of list values. `ListType<T>` has value type `T`.
   Defaults to `std::vector`.
 - `bool dict_is_ordered`: Set to `true` if the values in `DictType` instances are sorted. Currently, if this is
   `false`, the class will not work, so you must use `std::map` or similar (as opposed to `std::unordered_map`.)
 - `template<typename KeyType, typename ValueType> class DictType`: Template class of dictionary values.
   Defaults to `std::map`.
 - `template<typename T> class AllocatorType`: An allocator. Defaults to `std::allocator`.

So you can just use:

```C++
using my_data_t = torrebis::bencode::basic_data<template arguments...>;
```

## Visitors

`visitor` is a class that has a number of virtual functions used in traversing some data.
It is a template class that takes a `basic_data` type as the first argument.

Only override methods that need to be overridden. You can still use `bencode::visitor<BasicData>.original_method(args...)`
to have the default behaviour happen. Here is a boiler plate of a visitor:

```C++
template<typename BasicData>
class print_visitor : public visitor<BasicData> {
private:
    // private members here

public:
    // useful types
    using data_t = typename visitor<BasicData>::data_t;
    using path_t = typename visitor<BasicData>::path_t;
    // These are all shared pointers to the corrosponding BasicData type (So BasicData::const_TYPE_ptr)
    using str_type = typename visitor<BasicData>::str_type;
    using int_type = typename visitor<BasicData>::int_type;
    using list_type = typename visitor<BasicData>::list_type;
    using dict_type = typename visitor<BasicData>::dict_type;

    print_visitor() : /* constructor */ {}

    // The public function to start visiting some nodes.
    // By default calls `visit_any(dat, empty_path)`.
    virtual void visit(const data_t dat);
protected:
    // Called every time a new object is encountered. (Including dictionary keys). Defaults to calling
    // the correct `visit_TYPE` function.
    virtual void visit_any(const data_t dat, const path_t path);

    // Called for every str (including keys).
    virtual void visit_str(const str_type str, const path_t path);

    // Called for every int.
    virtual void visit_int(const int_type i, const path_t path);

    // Called for every list. By default, just calls `visit_item` on every item after adding the index to `path`.
    virtual void visit_list(const list_type list, const path_t path);

    // Called for every list item. By default, just calls `visit_any`.
    virtual void visit_item(const data_t item, const path_t path);

    // Called for every dict. By default, calls `visit_key` with the last item in the path being the
    // index of the key and `visit_value` with the last item in the path being the key.
    virtual void visit_dict(const dict_type dict, const path_t path);

    // Called for every dict key. By default, calls `visit_any`, which calls `visit_str`.
    virtual void visit_key(const data_t k, const path_t path);

    // Called for every dict value. By default, calls `visit_any`.
    virtual void visit_value(const data_t value, const path_t path);
}
```

You can find the depth of the current object by calling `path.size()`.

One visitor implementation is defined: `print_visitor`. It outputs the bencoded data as JSON as
if it were encoded in latin-1 to an ostream (e.g.: `print_visitor<bencode::data>(std::cout).visit(my_data);`).

`print_visitor`'s constructor takes three arguments. `print_visitor::print_visitor(std::ostream& os, const char indent_char = ' ', const std::size_t indent_width = 4)`.
`os` is the output `std::ostream`. `indent_char` is the character to indent with. `indent_width` is how much to indent by.
For example, `print_visitor<bencode::data>(std::cout, '\t', 1)` to use 1 tab per indent instead of the default 4 spaces.
