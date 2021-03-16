#ifndef COMMAND_H
#define COMMAND_H

#include "record.h"
#include "operation.h"
#include "command_type.h"
#include "result_list.h"
#include <memory>

enum class parse_status {
    success = 0,
    same_field_twice = -1,
    different_types_of_logical_operation = -2,
    memory = -3,
    syntax_error = -4,
    condition_is_not_compatible_with_field = -5,
    only_one_quote = -6,
    incorrect_search_pattern = -7,
};

enum class command_status {
    success = 0, // for insert, delete ect
    memory = -1,
    quit = -2,
};

class command : public record {
    static const int max_items = 3;

    command_type type = command_type::none;
    condition c_name  = condition::none;
    condition c_phone = condition::none;
    condition c_group = condition::none;
    operation op = operation::none;
    ordering order[max_items] = { };
    ordering order_by[max_items] = { };

    std::unique_ptr<int[]> _pattern = nullptr;
public:
    command() = default;
    ~command() = default;

    // Convert string command to data structure
    // Example 1: "phone = 1234567" parsed to
    //   (command::phone = 1234567, command::c_phone = condition::eq)
    //   other fields are unspecified
    // Example 2: "name like St%" parsed to
    //   (command::name = "St%", command::c_name = condition::like)
    //   other fields are unspecified
    parse_status parse(const char* str);

    // Print parsed structure
    void print(FILE* fp = stdout) const;

    // Apply command, return application result and list for record ’x’
    int apply(list& db, result_list& result, FILE* fp = stdout) const;
private:
    // Treats string as a pattern, inits record
    parse_status _create_pattern();
    bool _compare_pattern(const char* str, const int* pattern) const;
    // returns number of processed symbols if successful, <0 if any error occured
    int _process_condition(const char* str, const char* field);
    int _process_value(const char* str, const char* field, int& value, char* string_value);
    int _process_type(const char* str);
    int _process_order(const char* str);
    int _process_order_by(const char* str);
    int _process_where(const char* str);
    int _process_insert(const char* str);

    // Apply conditions, return comparision result for record ’x’
    bool _apply_conditions(const record& x) const;
};

#endif // COMMAND_H
