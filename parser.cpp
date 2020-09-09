#include "parser.hpp"

#include <map>
#include <sstream>
#include <string>
#include <vector>

class json_integer : public json_value {
    int value;

public:
    json_integer(int v)
        : value(v) {};

    json_type type() const override { return json_type::integer; };

    int int_value() const override { return value; };

    json_ref item_at(int) const override { throw std::logic_error("Undefined"); };

    json_ref item_at(const std::string&) const override { throw std::logic_error("Undefined"); };

    int length() const noexcept override { return 0; };
};

class json_array : public json_value {
    std::vector<json_ptr> value;

public:
    json_array(std::vector<json_ptr> v)
        : value(std::move(v)) {};

    json_type type() const override { return json_type::array; };

    int int_value() const override { throw std::logic_error("Undefined"); };

    json_ref item_at(int i) const override { return *value.at(i); };

    json_ref item_at(const std::string&) const override { throw std::logic_error("Undefined"); };

    int length() const noexcept override { return value.size(); };
};

class json_object : public json_value {
    std::map<std::string, json_ptr> value;

public:
    json_object(std::map<std::string, json_ptr> v)
        : value(std::move(v)) {};

    json_type type() const override { return json_type::object; };

    int int_value() const override { throw std::logic_error("Undefined"); };

    json_ref item_at(int i) const override
    {
        if (i < 0 || size_t(i) >= value.size())
            throw std::out_of_range("Value doesn't exist");

        auto ptr = value.begin();
        std::advance(ptr, i);
        return *ptr->second;
    };

    json_ref item_at(const std::string& s) const override { return *value.at(s); };

    int length() const noexcept override { return value.size(); };
};

// Forward declaration
json_ptr get_value(std::istringstream&);

inline void parse_blank(std::istringstream& input)
{
    while (std::isspace(input.peek()))
        input.get();
}

inline void expect(std::istringstream& input, char c, const char* message)
{
    if (input.peek() != c)
        throw json_error(message);
    input.get();
}

int parse_integer(std::istringstream& input)
{
    bool positive = true;
    int result = 0;

    if (input.peek() == '0') {
        input.get();
        if (std::isdigit(input.peek()))
            throw json_error("Number starting with 0");
        return result;
    }

    if (input.peek() == '-') {
        positive = false;
        input.get();
        if (input.peek() == '0' || !std::isdigit(input.peek()))
            throw json_error("Number isn't valid");
    }

    while (std::isdigit(input.peek())) {
        result *= 10;
        result += input.get() - '0';
    }

    return positive ? result : -result;
}

std::vector<json_ptr> parse_array(std::istringstream& input)
{
    // Skip the opening bracket
    input.get();

    if (input.peek() == ']') {
        input.get();
        return {};
    }

    std::vector<json_ptr> result;

    while (true) {
        result.emplace_back(get_value(input));

        if (input.peek() == ']') {
            input.get();
            return result;
        }
        if (input.peek() != ',')
            throw json_error("Malformed array");

        input.get();
    }
}

std::pair<std::string, json_ptr> get_kvpair(std::istringstream& input)
{
    parse_blank(input);

    if (!std::isalpha(input.peek()))
        throw json_error("Malformed key");

    std::string key;
    while (std::isalpha(input.peek()))
        key.push_back(input.get());

    parse_blank(input);

    expect(input, ':', "Key-Value pair doesn't contain :");

    return { key, get_value(input) };
}

std::map<std::string, json_ptr> parse_object(std::istringstream& input)
{
    input.get();

    if (input.peek() == '}') {
        input.get();
        return {};
    }

    std::map<std::string, json_ptr> result;

    while (true) {
        auto key_value = get_kvpair(input);
        if (result.count(key_value.first) == 1)
            throw json_error("Object contains duplicate key");

        result.insert(std::move(key_value));

        if (input.peek() == '}') {
            input.get();
            return result;
        }

        if (input.peek() != ',')
            throw json_error("Object isn't comma separated");

        input.get();
    }
}

json_ptr get_integer(std::istringstream& input) { return std::make_unique<json_integer>(parse_integer(input)); }
json_ptr get_array(std::istringstream& input) { return std::make_unique<json_array>(parse_array(input)); }
json_ptr get_object(std::istringstream& input) { return std::make_unique<json_object>(parse_object(input)); }

json_ptr get_value(std::istringstream& input)
{
    json_ptr result;

    parse_blank(input);

    char c = input.peek();
    if (c == '-' || std::isdigit(c))
        result = get_integer(input);
    else if (c == '[')
        result = get_array(input);
    else if (c == '{')
        result = get_object(input);
    else
        throw json_error("Unrecognized value");

    parse_blank(input);

    return result;
}

json_ptr json_parse(const std::string& s)
{
    std::istringstream input(s);

    parse_blank(input);
    json_ptr result = get_value(input);
    parse_blank(input);

    if (!input.eof())
        throw json_error("Malformed input string");

    return result;
}
