#ifndef NODE_H
#define NODE_H

#include "record.h"

class list;

class node : public record {
    node* _prev = nullptr;
    node* _next = nullptr;
public:
    node() = default;
    node(node&& n) : record(static_cast<record&&>(n)) {
        _next = n._next;
        _prev = n._prev;
        n._next = nullptr;
        n._prev = nullptr;
    }

    ~node() { _prev = _next = nullptr; }

    node* next() { return _next; }
    node* prev() { return _prev; }

    friend class list;
};

#endif // NODE_H
