#ifndef TORREBIS_BENCODE_ERRORS_H_
#define TORREBIS_BENCODE_ERRORS_H_

namespace torrebis { namespace bencode {
    class decoding_error final : ::std::exception {
    private:
        const char* what_;
    public:
        decoding_error() = delete;
        decoding_error(decoding_error& other) noexcept : what_(other.what_) {};
        decoding_error(decoding_error&& other) noexcept : what_(other.what_) {};
        /* not explicit */ decoding_error(const char* what) noexcept : what_(what) {};

        const char* what() const noexcept final { return what_; }
        decoding_error copy() const noexcept { return decoding_error(what_); }

        bool operator==(decoding_error&& other) const noexcept { return what_ == other.what_; }
        bool operator!=(decoding_error&& other) const noexcept { return what_ != other.what_; }

        decoding_error operator()() { return decoding_error(what_); }
    };

    namespace errors {
        const char* const EARLY_END = "bencoded data ends early.";
        const decoding_error early_end_error = decoding_error(EARLY_END);

        const char* const EXPECTING_VALUE = "expecting value in bencoded data.";
        const decoding_error expecting_value_error = decoding_error(EXPECTING_VALUE);

        const char* const EXPECTING_INT = "expecting int in bencoded data.";
        const decoding_error expecting_int_error = decoding_error(EXPECTING_INT);

        const char* const KEY_ORDER = "keys in bencoded data must be strictly ascending.";
        const decoding_error key_order_error = decoding_error(KEY_ORDER);

        const char* const UNKNOWN_VALUE = "unknown value in bencoded data.";
        const decoding_error unknown_value_error = decoding_error(UNKNOWN_VALUE);

        const char* const INTEGER_OVERFLOW = "integer overflow when reading bencoded integer.";
        const decoding_error integer_overflow_error = decoding_error(INTEGER_OVERFLOW);

        const char* const CUT_OFF_STR = "string cut off in bencoded data.";
        const decoding_error cut_off_str_error = decoding_error(CUT_OFF_STR);

        const char* const BAD_STREAM = "read stream is bad for bencoded data.";
        const decoding_error bad_stream_error = decoding_error(BAD_STREAM);
    }  // namespace torrebis::bencode::errors
} }  // namespace torrebis::bencode

#endif  // TORREBIS_BENCODE_ERRORS_H_
