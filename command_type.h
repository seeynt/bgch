#ifndef COMMAND_TYPE_H
#define COMMAND_TYPE_H

enum class command_type {
    none,   // uninitialized
    quit,   // end of input stream
    insert, // add record
    select, // find by conditions specified
    del,    // delete record
};

#endif // COMMAND_TYPE_H
