#include "result_list.h"

result_list::~result_list() {
    _destroy();
}

void result_list::_destroy() {
    result_node *curr, *next;
    for (curr = _head; curr; curr = next) {
        next = curr->_next;
        delete curr;
    }
    _head = nullptr;
}

bool result_list::push(node* data) {
    result_node* tmp = new result_node();
    if (!tmp)
        return false;
    tmp->_data = data;

    if (!_head)
        _head = _tail = tmp;
    else {
        _tail->_next = tmp;
        tmp->_prev = _tail;
        _tail = tmp;
    }

    return true;
}

int result_list::length() const {
    int len = 0;
    result_node *curr;
    for (curr = _head; curr; curr = curr->_next, ++len);
    return len;
}

void result_list::clear_data(list& db) {
    result_node* curr;
    for (curr = _head; curr; curr = curr->_next)
        db.delete_node(curr->_data);
    _destroy();
}

void result_list::clear() {
    _destroy();
}

void result_list::print(const ordering order[], FILE* fp) const {
    result_node* curr;
    for (curr = _head; curr; curr = curr->_next)
        curr->_data->print(order, fp);
}

void result_list::sort(const ordering order[], int max_items) {
    if (!_head)
        return;

    int length = 0;
    result_node *a, *b, *sorted_head = nullptr, *sorted_tail = nullptr, *curr;

    for (curr = _head; curr; curr = curr->_next)
        ++length;

    for (int i = 1; i < length; i *= 2) {
        while (_head) {
            a = curr = _head;
            for (int j = 0; curr && j < i - 1; ++j)
                curr = curr->_next;
            if (curr) {
                _head = curr->_next;
                curr->_next = nullptr;
            }
            else
                _head = nullptr;

            b = curr = _head;
            for (int j = 0; curr && j < i - 1; ++j)
                curr = curr->_next;
            if (curr) {
                _head = curr->_next;
                curr->_next = nullptr;
            }
            else
                _head = nullptr;

            _merge(a, b, order, max_items);

            if (sorted_tail)
                sorted_tail->_next = a;
            else
                sorted_head = a;
            sorted_tail = b;
        }

        _head = sorted_head;
        sorted_head = sorted_tail = nullptr;
    }
}

//a - start of c, b - end of c
void result_list::_merge(result_node*& a, result_node*& b, const ordering order[], int max_items) {
    result_node* head = nullptr, *tail = nullptr;
    while (a && b) {
        if (_compare(*a, *b, order, max_items)) {
            if (tail)
                tail->_next = a;
            else
                head = a;
            tail = a;
            a = a->_next;
        }
        else {
            if (tail)
                tail->_next = b;
            else
                head = b;
            tail = b;
            b = b->_next;
        }
    }

    if (a) {
        if (tail)
            tail->_next = a;
        else
            head = a;
        while (a->_next)
            a = a->_next;
        tail = a;
    }

    if (b) {
        if (tail)
            tail->_next = b;
        else
            head = b;
        while (b->_next)
            b = b->_next;
        tail = b;
    }

    tail->_next = nullptr;
    a = head;
    b = tail;
}

// a < b
bool result_list::_compare(const result_node& a, const result_node& b, const ordering order[], int max_items) const {
    for (int i = 0; i < max_items; ++i) {
        switch (order[i]) {
        case ordering::name:
            if (a._data->compare_name(condition::lt, *(b._data)))
                return true;
            if (a._data->compare_name(condition::gt, *(b._data)))
                return false;
            break;
        case ordering::phone:
            if (a._data->compare_phone(condition::lt, *(b._data)))
                return true;
            if (a._data->compare_phone(condition::gt, *(b._data)))
                return false;
            break;
        case ordering::group:
            if (a._data->compare_group(condition::lt, *(b._data)))
                return true;
            if (a._data->compare_group(condition::gt, *(b._data)))
                return false;
            break;
        case ordering::none:
            continue;
        }
    }
    return true;
}
