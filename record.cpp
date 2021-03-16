#include "record.h"

const int LENGTH = 1234;

bool record::init(const char *name, int phone, int group) {
    _phone = phone;
    _group = group;

    if (name) {
        _name = std::make_unique<char[]>(strlen(name) + 1);
        if (!_name)
            return false;
        strcpy(_name.get(), name);
    } else
        _name = nullptr;
    return true;
}

void record::print(const ordering order[], FILE *fp) const {
    const int max_items = 3;
    const ordering default_ordering[max_items]
            = { ordering::name, ordering::phone, ordering::group };
    const ordering *p = (order ? order : default_ordering);

    for (int i = 0; i < max_items; i++)
        switch (p[i]) {
        case ordering::name:
            fprintf(fp, " %s", _name.get());
            break;
        case ordering::phone:
            fprintf(fp, " %d", _phone);
            break;
        case ordering::group:
            fprintf(fp, " %d", _group);
            break;
        case ordering::none:
            continue;
        }
    fprintf(fp, "\n");
}

read_status record::read(FILE* fp) {
    char buf[LENGTH];
    int phone, group;

    _name = nullptr;

    if (!fp)
        return read_status::file_error;

    if (fscanf(fp, "%s%d%d", buf, &phone, &group) != 3) {
        if (feof(fp))
            return read_status::eof;
        return read_status::format;
    }
    if (!init(buf, phone, group))
        return read_status::memory;
    return read_status::success;
}

// Check condition ’x’ for field ’name’ for ’this’ and ’y’
bool record::compare_name(condition x, const record& y) const {
    switch (x) {
    case condition::none: // not specified
        return true; // unspecified opeation is true
    case condition::eq:   // equal
        return strcmp(_name.get(), y._name.get()) == 0;
    case condition::ne:   // not equal
        return strcmp(_name.get(), y._name.get()) != 0;
    case condition::lt:   // less than
        return strcmp(_name.get(), y._name.get()) < 0;
    case condition::gt:   // greater than
        return strcmp(_name.get(), y._name.get()) > 0;
    case condition::le:   // less equal
        return strcmp(_name.get(), y._name.get()) <= 0;
    case condition::ge:   // great equal
        return strcmp(_name.get(), y._name.get()) >= 0;
    case condition::like: // commands only: match pattern
        return false;
    case condition::nlike: // commands only: not match pattern
        return false;
    }
    return false;
}

// Check condition ’x’ for field ’phone’ for ’this’ and ’y’
bool record::compare_phone(condition x, const record& y) const {
    switch (x) {
    case condition::none: // not specified
        return true; // unspecified opeation is true
    case condition::eq:   // equal
        return _phone == y._phone;
    case condition::ne:   // not equal
        return _phone != y._phone;
    case condition::lt:   // less than
        return _phone < y._phone;
    case condition::gt:   // greater than
        return _phone > y._phone;
    case condition::le:   // less equal
        return _phone <= y._phone;
    case condition::ge:   // great equal
        return _phone >= y._phone;
    case condition::like: // commands only: match pattern
        return false; // cannot be used for phone
    case condition::nlike: // commands only: not match pattern
        return false;
    }
    return false;
}

// Check condition ’x’ for field ’group’ for ’this’ and ’y’
bool record::compare_group(condition x, const record& y) const {
    switch (x) {
    case condition::none: // not specified
        return true; // unspecified opeation is true
    case condition::eq:   // equal
        return _group == y._group;
    case condition::ne:   // not equal
        return _group != y._group;
    case condition::lt:   // less than
        return _group < y._group;
    case condition::gt:   // greater than
        return _group > y._group;
    case condition::le:   // less equal
        return _group <= y._group;
    case condition::ge:   // great equal
        return _group >= y._group;
    case condition::like: // commands only: match pattern
        return false; // cannot be used for phone
    case condition::nlike: // commands only: not match pattern
        return false;
    }
    return false;
}

