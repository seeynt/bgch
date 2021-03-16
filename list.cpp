#include "list.h"

list::~list() {
    _destroy();
}

void list::_destroy() {
    node *curr, *next;
    for (curr = _head; curr; curr = next) {
        next = curr->_next;
        delete curr;
    }
    _head = nullptr;
}

void list::delete_node(node* tmp) {
    if (!tmp)
        return;

    if (!tmp->_prev) {
        _head = tmp->_next;
        if (tmp->_next)
            tmp->_next->_prev = nullptr;
    }
    else {
        tmp->_prev->_next = tmp->_next;
        if (tmp->_next)
            tmp->_next->_prev = tmp->_prev;
    }

    delete tmp;
}

bool list::push(const record& x) {
    node* tmp = new node();
    if (!tmp)
        return false;
    if (!tmp->init(x.get_name(), x.get_phone(), x.get_group()))
        return false;

    if (!_head)
        _head = _tail = tmp;
    else {
        _tail->_next = tmp;
        tmp->_prev = _tail;
        _tail = tmp;
    }

    return true;
}

read_status list::read(const char* fname) {
    FILE* fp = fopen(fname, "r");
    if (!fp)
        return read_status::file_error;

    if (!_head)
        _destroy();

    _head = new node();
    if (!_head) {
        fclose(fp);
        return read_status::memory;
    }

    read_status curr_status = _head->read(fp);
    if (curr_status != read_status::success) {
        delete _head;
        _head = nullptr;
        if (feof(fp)) {
            fclose(fp);
            return read_status::success;
        }
        fclose(fp);
        return curr_status;
    }

    node *curr = _head, tmp;
    _tail = _head;

    while (true) {
        curr_status = tmp.read(fp);
        if (curr_status != read_status::success)
            break;

        curr = new node(static_cast<node&&>(tmp));
        if (!curr) {
            _destroy();
            fclose(fp);
            return read_status::memory;
        }

        _tail->_next = curr;
        curr->_prev = _tail;
        _tail = curr;
    }

    if (!feof(fp)) {
        _destroy();
        fclose(fp);
        return curr_status;
    }

    return read_status::success;
}
