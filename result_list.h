#ifndef RESULT_LIST_H
#define RESULT_LIST_H

#include "ordering.h"
#include "list.h"
#include "result_node.h"

class result_list {
    result_node* _head = nullptr;
    result_node* _tail = nullptr;
public:
    result_list() = default;
    ~result_list();
    result_node* head() { return _head; }
    bool push(node* data);
    bool empty() const { return !_head; }
    int length() const;
    void print(const ordering order[] = nullptr, FILE* fp = stdout) const;
    void sort(const ordering order[], int max_items);
    void clear_data(list& db);
    void clear();
private:
    void _destroy();
    void _merge(result_node*& a, result_node*& b, const ordering order[], int max_items);
    bool _compare(const result_node& a, const result_node& b, const ordering order[], int max_items) const;
};

#endif // RESULT_LIST_H
