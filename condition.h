#ifndef CONDITION_H
#define CONDITION_H

enum class condition {
    none, // not specified
    eq,   // equal
    ne,   // not equal
    lt,   // less than
    gt,   // less than
    le,   // less equal
    ge,   // great equal
    like, // strings only: match pattern
    nlike, // strings only: not match pattern
};

#endif // CONDITION_H
