#ifndef LIST_H
#define LIST_H

#include "node.h"

class list {
    node* _head = nullptr;
    node* _tail = nullptr;
public:
    list() = default;
    ~list();
    node* head() { return _head; }
    void delete_node(node* tmp);
    bool push(const record& x);
    read_status read(const char* fname);
private:
    void _destroy();
};

#endif // LIST_H
