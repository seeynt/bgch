#ifndef RECORD_H
#define RECORD_H

#include <memory>
#include <stdio.h>
#include <cstring>
#include "condition.h"
#include "ordering.h"

enum class read_status {
    success = 0,
    eof = -1,
    format = -2,
    memory = -3,
    file_error = -4,
};

class record {
    std::unique_ptr<char[]> _name = nullptr;
    int _phone = 0;
    int _group = 0;
public:
    record() = default;
    ~record() = default;

    char* get_name() const { return _name.get(); }
    int get_phone() const { return _phone; }
    int get_group() const { return _group; }

    bool init(const char *name, int phone, int group);

    // Allow as return value for functions
    record(record&& x) = default; // move constructor

    // Assignement move operator
    record& operator=(record&& x) = default;

    // Prohibit pass by value
    // (it is default when move constructor is declared)
    record(const record &x) = delete;

    // Prohibit assignement operator
    // (it is default when move constructor is declared)
    record& operator=(const record&) = delete;

    // Check condition ’x’ for field ’name’ for ’this’ and ’y’
    bool compare_name(condition x, const record& y) const;

    // Check condition ’x’ for field ’phone’ for ’this’ and ’y’
    bool compare_phone(condition x, const record& y) const;

    // Check condition ’x’ for field ’group’ for ’this’ and ’y’
    bool compare_group(condition x, const record& y) const;

    void print(const ordering order[] = nullptr, FILE* fp = stdout) const;
    read_status read(FILE *fp = stdin);
};

#endif // RECORD_H
