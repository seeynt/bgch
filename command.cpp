#include "command.h"

const int LENGTH = 1234;

parse_status command::parse(const char* str) {
    int pos = 0, result = 0;

    result = _process_type(str + pos);
    if (result < 0)
        return static_cast<parse_status>(result);
    pos += result;
    while (str[pos] == ' ' || str[pos] == '\t') ++pos;

    switch (type) {
    case command_type::quit:
        while (str[pos] == ' ' || str[pos] == '\t') ++pos;
        if (str[pos])
            return parse_status::syntax_error;
        return parse_status::success;
    case command_type::insert:
        result = _process_insert(str + pos);
        if (result < 0)
            return static_cast<parse_status>(result);
        pos += result;
        while (str[pos] == ' ' || str[pos] == '\t') ++pos;
        if (str[pos])
            return parse_status::syntax_error;
        return parse_status::success;
    case command_type::select:
        result = _process_order(str + pos);
        if (result < 0)
            return static_cast<parse_status>(result);
        pos += result;
        while (str[pos] == ' ' || str[pos] == '\t') ++pos;
        if (!strncmp(str + pos, "where", 5)) {
            pos += 5;
            result = _process_where(str + pos);
            if (result < 0)
                return static_cast<parse_status>(result);
            pos += result;
        }
        while (str[pos] == ' ' || str[pos] == '\t') ++pos;
        if (!strncmp(str + pos, "order", 5)) {
            pos += 5;
            while (str[pos] == ' ' || str[pos] == '\t') ++pos;
            if (!strncmp(str + pos, "by", 2)) {
                pos += 2;
                result = _process_order_by(str + pos);
                if (result < 0)
                    return static_cast<parse_status>(result);
                pos += result;
            }
            else
                return parse_status::syntax_error;
        }
        while (str[pos] == ' ' || str[pos] == '\t') ++pos;
        if (str[pos])
            return parse_status::syntax_error;
        return parse_status::success;
    case command_type::del:
        while (str[pos] == ' ' || str[pos] == '\t') ++pos;
        if (!strncmp(str + pos, "where", 5)) {
            pos += 5;
            result = _process_where(str + pos);
            if (result < 0)
                return static_cast<parse_status>(result);
            pos += result;
        }
        while (str[pos] == ' ' || str[pos] == '\t') ++pos;
        if (str[pos])
            return parse_status::syntax_error;
        return parse_status::success;
    case command_type::none:
        return parse_status::syntax_error;
    }

    return parse_status::success;
}

// returns number of processed symbols if successful, <0 if any error occured
int command::_process_type(const char* str) {
    int pos = 0;
    while (str[pos] == ' ' || str[pos] == '\t') ++pos;
    if (!str[pos])
        return static_cast<int>(parse_status::syntax_error);

    if (!strncmp(str + pos, "quit", 4)) {
        type = command_type::quit;
        pos += 4;
    }
    else if (!strncmp(str + pos, "insert", 6)) {
        type = command_type::insert;
        pos += 6;
    }
    else if (!strncmp(str + pos, "select", 6)) {
        type = command_type::select;
        pos += 6;
    }
    else if (!strncmp(str + pos, "delete", 6)) {
        type = command_type::del;
        pos += 6;
    }
    else
        return static_cast<int>(parse_status::syntax_error);

    return pos;
}

// returns number of processed symbols if successful, <0 if any error occured
int command::_process_order(const char* str) {
    int pos = 0;
    while (str[pos] == ' ' || str[pos] == '\t') ++pos;
    if (!str[pos])
        return static_cast<int>(parse_status::syntax_error);

    if (str[pos] == '*') {
        order[0] = ordering::name;
        order[1] = ordering::phone;
        order[2] = ordering::group;
        ++pos;
    }
    else {
        int curr_items = 0;
        for (int i = 0; i < max_items; ++i)
            order[i] = ordering::none;

        bool expect_field = true;
        while (expect_field) {
            while (str[pos] == ' ' || str[pos] == '\t') ++pos;
            if (!strncmp(str + pos, "name", 4)) {
                pos += 4;
                for (int i = 0; i < curr_items; ++i)
                    if (order[i] == ordering::name)
                        return static_cast<int>(parse_status::same_field_twice);

                order[curr_items] = ordering::name;
                ++curr_items;
                expect_field = false;
            }
            else if (!strncmp(str + pos, "phone", 5)) {
                pos += 5;
                for (int i = 0; i < curr_items; ++i)
                    if (order[i] == ordering::phone)
                        return static_cast<int>(parse_status::same_field_twice);

                order[curr_items] = ordering::phone;
                ++curr_items;
                expect_field = false;
            }
            else if (!strncmp(str + pos, "group", 5)) {
                pos += 5;
                for (int i = 0; i < curr_items; ++i)
                    if (order[i] == ordering::group)
                        return static_cast<int>(parse_status::same_field_twice);

                order[curr_items] = ordering::group;
                ++curr_items;
                expect_field = false;
            }
            else
                return static_cast<int>(parse_status::syntax_error);

            while (str[pos] == ' ' || str[pos] == '\t') ++pos;
            if (str[pos] == ',')
                expect_field = true, ++pos;
        }
    }

    return pos;
}

// returns number of processed symbols if successful, <0 if any error occured
int command::_process_order_by(const char* str) {
    int pos = 0;
    while (str[pos] == ' ' || str[pos] == '\t') ++pos;
    if (!str[pos])
        return static_cast<int>(parse_status::syntax_error);

    int curr_items = 0;
    for (int i = 0; i < max_items; ++i)
        order_by[i] = ordering::none;

    bool expect_field = true;
    while (expect_field) {
        while (str[pos] == ' ' || str[pos] == '\t') ++pos;
        if (!strncmp(str + pos, "name", 4)) {
            pos += 4;
            for (int i = 0; i < curr_items; ++i)
                if (order_by[i] == ordering::name)
                    return static_cast<int>(parse_status::same_field_twice);

            order_by[curr_items] = ordering::name;
            ++curr_items;
            expect_field = false;
        }
        else if (!strncmp(str + pos, "phone", 5)) {
            pos += 5;
            for (int i = 0; i < curr_items; ++i)
                if (order_by[i] == ordering::phone)
                    return static_cast<int>(parse_status::same_field_twice);

            order_by[curr_items] = ordering::phone;
            ++curr_items;
            expect_field = false;
        }
        else if (!strncmp(str + pos, "group", 5)) {
            pos += 5;
            for (int i = 0; i < curr_items; ++i)
                if (order_by[i] == ordering::group)
                    return static_cast<int>(parse_status::same_field_twice);

            order_by[curr_items] = ordering::group;
            ++curr_items;
            expect_field = false;
        }
        else
            return static_cast<int>(parse_status::syntax_error);

        while (str[pos] == ' ' || str[pos] == '\t') ++pos;
        if (str[pos] == ',')
            expect_field = true, ++pos;
    }

    return pos;
}

// returns number of processed symbols if successful, <0 if any error occured
int command::_process_insert(const char* str) {
    int pos = 0, result = 0, phone = -0, group = 0;
    char name[LENGTH];

    while (str[pos] == ' ' || str[pos] == '\t') ++pos;
    if (str[pos] != '(')
        return static_cast<int>(parse_status::syntax_error);
    ++pos;

    result = _process_value(str + pos, "name", phone, name);
    if (result < 0)
        return result;
    pos += result;
    if (str[pos - 1] == ',') --pos;
    if (name[strlen(name) - 1] == ',')
        name[strlen(name) - 1] = '\0';

    while (str[pos] == ' ' || str[pos] == '\t') ++pos;
    if (str[pos] != ',')
        return static_cast<int>(parse_status::syntax_error);
    ++pos;

    result = _process_value(str + pos, "phone", phone, nullptr);
    if (result < 0)
        return result;
    pos += result;

    while (str[pos] == ' ' || str[pos] == '\t') ++pos;
    if (str[pos] != ',')
        return static_cast<int>(parse_status::syntax_error);
    ++pos;

    result = _process_value(str + pos, "group", group, nullptr);
    if (result < 0)
        return result;
    pos += result;

    while (str[pos] == ' ' || str[pos] == '\t') ++pos;
    if (str[pos] != ')')
        return static_cast<int>(parse_status::syntax_error);
    ++pos;

    if (!init(name, phone, group))
        return static_cast<int>(parse_status::memory);

    return pos;
}

// returns number of processed symbols if successful, <0 if any error occured
int command::_process_where(const char* str) {
    c_name = c_phone = c_group = condition::none;
    op = operation::none;

    int pos = 0, result = 0, phone = 0, group = 0;
    char name[LENGTH];
    bool expect_field = true;

    while (str[pos] == ' ' || str[pos] == '\t') ++pos;
    if (!str[pos])
        return static_cast<int>(parse_status::syntax_error);

    while (expect_field) {
        if (!strncmp(str + pos, "name", 4)) {
            pos += 4;
            if (c_name != condition::none)
                return static_cast<int>(parse_status::same_field_twice);
            result = _process_condition(str + pos, "name");
            if (result < 0)
                return result;
            pos += result;

            result = _process_value(str + pos, "name", phone, name);
            if (result < 0)
                return result;
            pos += result;
            expect_field = false;
        }
        else if (!strncmp(str + pos, "phone", 5)) {
            pos += 5;
            if (c_phone != condition::none)
                return static_cast<int>(parse_status::same_field_twice);
            result = _process_condition(str + pos, "phone");
            if (result < 0)
                return result;
            pos += result;

            result = _process_value(str + pos, "phone", phone, nullptr);
            if (result < 0)
                return result;
            pos += result;
            expect_field = false;
        }
        else if (!strncmp(str + pos, "group", 5)) {
            pos += 5;
            if (c_group != condition::none)
                return static_cast<int>(parse_status::same_field_twice);
            result = _process_condition(str + pos, "group");
            if (result < 0)
                return result;
            pos += result;

            result = _process_value(str + pos, "group", group, nullptr);
            if (result < 0)
                return result;
            pos += result;
            expect_field = false;
        }
        else
            return static_cast<int>(parse_status::syntax_error);

        while (str[pos] == ' ' || str[pos] == '\t') ++pos;

        if (!strncmp(str + pos, "and", 3)) {
            if (op == operation::lor)
                return static_cast<int>(parse_status::different_types_of_logical_operation);
            pos += 3, op = operation::land, expect_field = true;
        }
        else if (!strncmp(str + pos, "or", 2)) {
            if (strncmp(str + pos, "order", 5)) {
                if (op == operation::land)
                    return static_cast<int>(parse_status::different_types_of_logical_operation);
                pos += 3, op = operation::lor, expect_field = true;
            }
        }

        while (str[pos] == ' ' || str[pos] == '\t') ++pos;
    }

    if (!init((c_name == condition::none ? nullptr : name), phone, group))
        return static_cast<int>(parse_status::memory);
    if (c_name == condition::like || c_name == condition::nlike) {
        parse_status status = _create_pattern();
        if (status != parse_status::success)
            return static_cast<int>(status);
    }

    return pos;
}

// returns number of processed symbols if successsful, <0 if any error occured
int command::_process_condition(const char* str, const char* field) {
    int pos = 0;
    while (str[pos] == ' ' || str[pos] == '\t') ++pos;

    if (!str[pos])
        return static_cast<int>(parse_status::syntax_error);

    if (!strcmp(field, "name")) {
        if (!strncmp(str + pos, "=", 1))
            c_name = condition::eq, pos += 1;
        else if (!strncmp(str + pos, "<>", 2))
            c_name = condition::ne, pos += 2;
        else if (!strncmp(str + pos, "<=", 2))
            c_name = condition::le, pos += 2;
        else if (!strncmp(str + pos, ">=", 2))
            c_name = condition::ge, pos += 2;
        else if (!strncmp(str + pos, "<", 1))
            c_name = condition::lt, pos += 1;
        else if (!strncmp(str + pos, ">", 1))
            c_name = condition::gt, pos += 1;
        else if (!strncmp(str + pos, "like", 4))
            c_name = condition::like, pos += 4;
        else {
            if (strncmp(str + pos, "not", 3))
                return static_cast<int>(parse_status::syntax_error);
            pos += 3;
            while (str[pos] == ' ' || str[pos] == '\t') ++pos;
            if (!strncmp(str + pos, "like", 4))
                c_name = condition::nlike, pos += 4;
            else
                return static_cast<int>(parse_status::syntax_error);
        }
    }
    else if (!strcmp(field, "phone")) {
        if (!strncmp(str + pos, "=", 1))
            c_phone = condition::eq, pos += 1;
        else if (!strncmp(str + pos, "<>", 2))
            c_phone = condition::ne, pos += 2;
        else if (!strncmp(str + pos, "<=", 2))
            c_phone = condition::le, pos += 2;
        else if (!strncmp(str + pos, ">=", 2))
            c_phone = condition::ge, pos += 2;
        else if (!strncmp(str + pos, "<", 1))
            c_phone = condition::lt, pos += 1;
        else if (!strncmp(str + pos, ">", 1))
            c_phone = condition::gt, pos += 1;
        else if (!strncmp(str + pos, "like", 4))
            return static_cast<int>(parse_status::condition_is_not_compatible_with_field);
        else {
            if (strncmp(str + pos, "not", 3))
                return static_cast<int>(parse_status::syntax_error);
            pos += 3;
            while (str[pos] == ' ' || str[pos] == '\t') ++pos;
            if (!strncmp(str + pos, "like", 4))
                return static_cast<int>(parse_status::condition_is_not_compatible_with_field);
            else
                return static_cast<int>(parse_status::syntax_error);
        }
    }
    else if (!strcmp(field, "group")) {
        if (!strncmp(str + pos, "=", 1))
            c_group = condition::eq, pos += 1;
        else if (!strncmp(str + pos, "<>", 2))
            c_group = condition::ne, pos += 2;
        else if (!strncmp(str + pos, "<=", 2))
            c_group = condition::le, pos += 2;
        else if (!strncmp(str + pos, ">=", 2))
            c_group = condition::ge, pos += 2;
        else if (!strncmp(str + pos, "<", 1))
            c_group = condition::lt, pos += 1;
        else if (!strncmp(str + pos, ">", 1))
            c_group = condition::gt, pos += 1;
        else if (!strncmp(str + pos, "like", 4))
            return static_cast<int>(parse_status::condition_is_not_compatible_with_field);
        else {
            if (strncmp(str + pos, "not", 3))
                return static_cast<int>(parse_status::syntax_error);
            pos += 3;
            while (str[pos] == ' ' || str[pos] == '\t') ++pos;
            if (!strncmp(str + pos, "like", 4))
                return static_cast<int>(parse_status::condition_is_not_compatible_with_field);
            else
                return static_cast<int>(parse_status::syntax_error);
        }
    }

    return pos;
}

int command::_process_value(const char* str, const char* field, int& value, char* string_value) {
    int pos = 0;
    int ql = 0, qr = 0, n = 0;
    while (str[pos] == ' ' || str[pos] == '\t') ++pos;

    if (!str[pos])
        return static_cast<int>(parse_status::syntax_error);

    if (str[pos] == '"') {
        ql = pos, qr = pos + 1;
        while (str[qr] && str[qr] != '"') ++qr;
        if (!str[qr])
            return static_cast<int>(parse_status::only_one_quote);

        if (!strcmp(field, "name")) {
            if (!string_value)
                return static_cast<int>(parse_status::memory);
            strncpy(string_value, str + ql + 1, qr - ql - 1);
            string_value[qr - ql - 1] = '\0';
        }
        else if (!strcmp(field, "phone") || !strcmp(field, "group")) {
            if (sscanf(str + ql + 1, "%d%n", &value, &n) != 1 || n != qr - ql - 1)
                return static_cast<int>(parse_status::syntax_error);
        }

        return qr + 1;
    }

    if (!strcmp(field, "name")) {
        if (sscanf(str + pos, "%s%n", string_value, &n) != 1)
            return static_cast<int>(parse_status::syntax_error);
    }
    else if (!strcmp(field, "phone") || !strcmp(field, "group")) {
        if (sscanf(str + pos, "%d%n", &value, &n) != 1)
            return static_cast<int>(parse_status::syntax_error);
    }

    return pos + n;
}

int command::apply(list& db, result_list& result, FILE *fp) const {
    //print(fp);
    if (!result.empty())
        result.clear();

    node* curr;
    switch (type) {
    case command_type::quit:
        return static_cast<int>(command_status::quit);
    case command_type::insert:
        if (!db.push(*this))
            return static_cast<int>(command_status::memory);
        return static_cast<int>(command_status::success);
    case command_type::select:
        for (curr = db.head(); curr; curr = curr->next()) {
            if (_apply_conditions(*curr))
                if (!result.push(curr))
                    return static_cast<int>(command_status::memory);
        }
        for (int i = 0; i < max_items; ++i) {
            if (order_by[i] != ordering::none) {
                result.sort(order_by, max_items);
                break;
            }
        }
        result.print(order, fp);
        return result.length();
    case command_type::del:
        for (curr = db.head(); curr; curr = curr->next()) {
            if (_apply_conditions(*curr))
                if (!result.push(curr))
                    return static_cast<int>(command_status::memory);
        }
        result.clear_data(db);
        return static_cast<int>(command_status::success);
    default:
        return static_cast<int>(command_status::success);
    }

    return static_cast<int>(command_status::success);
}

bool command::_apply_conditions(const record& x) const {
    bool result_name = true, result_phone = true, result_group = true;
    if (op == operation::lor)
        result_name = false, result_phone = false, result_group = false;

    if (c_name != condition::none) {
        switch (c_name) {
        case condition::like:
            result_name = _compare_pattern(x.get_name(), _pattern.get());
            break;
        case condition::nlike:
            result_name = !_compare_pattern(x.get_name(), _pattern.get());
            break;
        default:
            result_name = x.compare_name(c_name, *this);
        }
    }

    if (c_phone != condition::none)
        result_phone = x.compare_phone(c_phone, *this);

    if (c_group != condition::none)
        result_group = x.compare_group(c_group, *this);

    if (op == operation::lor)
        return result_name || result_phone || result_group;
    return result_name && result_phone && result_group;
}

// 0 << 16 + code of char - char
// 1 << 16 - _
// 2 << 16 - %
// (3 << 16) + (m << 8) + n - [m-n]
// (4 << 16) + (m << 8) + n - [^m-n]
parse_status command::_create_pattern() {
    int mask[LENGTH];
    int j = 0;

    const char* str = get_name();

    if (!str)
        return parse_status::memory;

    for (int i = 0; str[i]; ++i) {
        mask[i] = 0;
        if (str[i] == '\\') {
            if (!str[i + 1])
                return parse_status::incorrect_search_pattern;
            if (str[i + 1] == '\\' || str[i + 1] == '_' || str[i + 1] == '%'
                    || str[i + 1] == '[' || str[i + 1] == ']' || str[i + 1] == '^') {
                mask[j] = (unsigned char)str[i + 1];
                ++j, ++i;
                continue;
            }
            else
                return parse_status::incorrect_search_pattern;
        }
        if (str[i] == '_') {
            mask[j] = 1 << 16;
        }
        else if (str[i] == '%') {
            if (j != 0 && (mask[j - 1] >> 16) == 2)
                continue;
            mask[j] = 2 << 16;
        }
        else if (str[i] == '[') {
            if (!str[i + 1])
                return parse_status::incorrect_search_pattern;
            if (str[i + 1] != '^') {
                if (!str[i + 2] || !str[i + 3] || !str[i + 4] || str[i + 2] != '-' || str[i + 4] != ']')
                    return parse_status::incorrect_search_pattern;
                mask[j] = (3 << 16) + ((unsigned char)str[i + 1] << 8) + (unsigned char)str[i + 3];
                i += 4;
            }
            else  {
                if (!str[i + 2] || !str[i + 3] || !str[i + 4] || !str[i + 5] ||
                        str[i + 3] != '-' || str[i + 5] != ']')
                    return parse_status::incorrect_search_pattern;
                mask[j] = (4 << 16) + ((unsigned char)str[i + 2] << 8) + (unsigned char)str[i + 4];
                i += 5;
            }
        }
        else
            mask[j] = (unsigned char)str[i];
        ++j;
    }
    mask[j] = 0;

    _pattern = std::make_unique<int[]>(j + 1);
    if (!_pattern)
        return parse_status::memory;

    for (int i = 0; i <= j; ++i)
        _pattern.get()[i] = mask[i];

    return parse_status::success;
}

bool command::_compare_pattern(const char* str, const int* pattern) const {
    int i = 0;
    char m, n;

    if (!str || !pattern)
        return false;

    for (i = 0; str[i] && pattern[i]; ++i) {
        //printf("%d %d %d %d %d\n", str[i], pattern[i], pattern[i] >> 16, char((pattern[i] >> 8) % (1 << 8)), char(pattern[i] % (1 << 8)));
        if (pattern[i] >> 16 == 1)
            continue;
        else if (pattern[i] >> 16 == 2) {
            int k = -1;
            do {
                ++k;
                if (_compare_pattern(str + i + k, pattern + i + 1))
                    return true;
            } while (str[i + k]);

            return false;
        }
        else if (pattern[i] >> 16 == 3) {
            m = char((pattern[i] >> 8) % (1 << 8));
            n = char(pattern[i] % (1 << 8));

            if (!(m <= str[i] && str[i] <= n))
                return false;
        }
        else if (pattern[i] >> 16 == 4) {
            m = char((pattern[i] >> 8) % (1 << 8));
            n = char(pattern[i] % (1 << 8));

            if (m <= str[i] && str[i] <= n)
                return false;
        }
        else {
            if (str[i] != char(pattern[i] % (1 << 8)))
                return false;
        }
    }

    int j = i;
    while (pattern[j] >> 16 == 2)
        ++j;

    if (str[i] || pattern[j])
        return false;

    return true;
}

void command::print(FILE* fp) const {
    fprintf(fp, "Command_type: ");
    switch (type) {
    case command_type::quit:
        fprintf(fp, "quit\n");
        break;
    case command_type::insert:
        fprintf(fp, "insert\n");
        break;
    case command_type::select:
        fprintf(fp, "select\n");
        break;
    case command_type::del:
        fprintf(fp, "delete\n");
        break;
    case command_type::none:
        fprintf(fp, "none\n");
        break;
    }

    fprintf(fp, "Logical operation: ");
    switch (op) {
    case operation::land:
        fprintf(fp, "&&\n");
        break;
    case operation::lor:
        fprintf(fp, "||\n");
        break;
    case operation::none:
        fprintf(fp, "none\n");
        break;
    }

    fprintf(fp, "Name condition: ");
    switch (c_name) {
    case condition::none:
        fprintf(fp, "none\n");
        break;
    case condition::eq:
        fprintf(fp, "eq\n");
        break;
    case condition::ne:
        fprintf(fp, "ne\n");
        break;
    case condition::lt:
        fprintf(fp, "lt\n");
        break;
    case condition::gt:
        fprintf(fp, "gt\n");
        break;
    case condition::le:
        fprintf(fp, "le\n");
        break;
    case condition::ge:
        fprintf(fp, "ge\n");
        break;
    case condition::like:
        fprintf(fp, "like\n");
        break;
    case condition::nlike:
        fprintf(fp, "nlike\n");
        break;
    }

    fprintf(fp, "Phone condition: ");
    switch (c_phone) {
    case condition::none:
        fprintf(fp, "none\n");
        break;
    case condition::eq:
        fprintf(fp, "eq\n");
        break;
    case condition::ne:
        fprintf(fp, "ne\n");
        break;
    case condition::lt:
        fprintf(fp, "lt\n");
        break;
    case condition::gt:
        fprintf(fp, "gt\n");
        break;
    case condition::le:
        fprintf(fp, "le\n");
        break;
    case condition::ge:
        fprintf(fp, "ge\n");
        break;
    case condition::like:
        fprintf(fp, "like\n");
        break;
    case condition::nlike:
        fprintf(fp, "nlike\n");
        break;
    }

    fprintf(fp, "Group condition: ");
    switch (c_group) {
    case condition::none:
        fprintf(fp, "none\n");
        break;
    case condition::eq:
        fprintf(fp, "eq\n");
        break;
    case condition::ne:
        fprintf(fp, "ne\n");
        break;
    case condition::lt:
        fprintf(fp, "lt\n");
        break;
    case condition::gt:
        fprintf(fp, "gt\n");
        break;
    case condition::le:
        fprintf(fp, "le\n");
        break;
    case condition::ge:
        fprintf(fp, "ge\n");
        break;
    case condition::like:
        fprintf(fp, "like\n");
        break;
    case condition::nlike:
        fprintf(fp, "nlike\n");
        break;
    }
}
