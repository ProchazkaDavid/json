#include <memory>
#include <stdexcept>

struct json_value;

using json_ptr = std::unique_ptr<json_value>;
using json_ref = const json_value&;

json_ptr json_parse(const std::string&);

enum class json_type {
    integer,
    array,
    object,
};

using json_error = std::runtime_error;

struct json_value {
    virtual json_type type() const = 0;
    virtual int int_value() const = 0;
    virtual json_ref item_at(int) const = 0;
    virtual json_ref item_at(const std::string&) const = 0;
    virtual int length() const noexcept = 0;
    virtual ~json_value() = default;
};
