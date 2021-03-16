#ifndef RESULT_NODE_H
#define RESULT_NODE_H

#include "node.h"

class result_list;

class result_node {
    node* _data = nullptr;
    result_node* _prev = nullptr;
    result_node* _next = nullptr;
public:
    result_node() = default;
    ~result_node() { _prev = _next = nullptr; }

    friend class result_list;
};


#endif // RESULT_NODE_H
