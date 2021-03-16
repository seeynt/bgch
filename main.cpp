#include <stdio.h>
#include "command.h"
#include "record.h"
#include "result_list.h"

const int LENGTH = 1234;

// -1 - error
// -2 - quit
// >=0 - length of result
int process_command(const char* str, list& db, result_list& result, FILE* fout) {
    command c;
    parse_status p_status = c.parse(str);
    if (p_status != parse_status::success) {
        switch (p_status) {
        case parse_status::condition_is_not_compatible_with_field:
            printf("Condition is not compatible with field in command \"%s\"\n", str);
            break;
        case parse_status::different_types_of_logical_operation:
            printf("Different types of logical operation in command \"%s\"\n", str);
            break;
        case parse_status::memory:
            printf("Memory error occured processing command \"%s\"\n", str);
            break;
        case parse_status::same_field_twice:
            printf("Same field mentioned twice in command \"%s\"\n", str);
            break;
        case parse_status::syntax_error:
            printf("Syntax error in command \"%s\"\n", str);
            break;
        case parse_status::only_one_quote:
            printf("Unpaired quote in command \"%s\"\n", str);
            break;
        case parse_status::incorrect_search_pattern:
            printf("Incorrect search pattern in command \"%s\"\n", str);
            break;
        default:
            printf("Unknown error occured in command \"%s\"\n", str);
        }
        return -1;
    }

    int c_result = c.apply(db, result, fout);
    if (c_result < 0) {
        command_status c_status = static_cast<command_status>(c_result);
        switch (c_status) {
        case command_status::quit:
            return -2;
        case command_status::memory:
            printf("Memory error occured applying command \"%s\"\n", str);
            return -1;
        default:
            printf("Unknown error occured applying command \"%s\"\n", str);
            return -1;
        }
    }

    return c_result;
}

int process_stream(FILE* fin, FILE* fout, list& db) {
    result_list result;
    char command_ready[LENGTH]; // for command
    char read_buffer[LENGTH]; // for reading
    char buffer[LENGTH]; // to glue strings into one command
    int pos = 0; // position to glue
    int cnt = 0, curr_cnt = 0;

    command_ready[0] = '\0';
    buffer[0] = '\0';

    while (fgets(read_buffer, LENGTH, fin)) {
        char* start = read_buffer;
        char* semicolon = strchr(read_buffer, ';');
        while (semicolon) {
            strncpy(buffer + pos, start, int(semicolon - start));
            buffer[pos + int(semicolon - start)] = '\0';
            strcpy(command_ready, buffer);
            pos = 0;

            curr_cnt = process_command(command_ready, db, result, fout);
            if (curr_cnt == -2)
                return cnt;
            if (curr_cnt == -1)
                return -1;
            cnt += curr_cnt;

            start = semicolon + 1;
            semicolon = strchr(start, ';');
        }
        if (!semicolon) {
            strcpy(buffer + pos, start);
            pos += strlen(start);
            if (buffer[pos - 1] == '\n')
                buffer[pos - 1] = ' ';
        }
    }

    return cnt;
}

int main(int argc, char* argv[]) {
    //processing args
    if (argc != 2) {
        printf("Usage: a.out a.txt\n");
        return -1;
    }

    list db;
    read_status status = db.read(argv[1]);
    if (status != read_status::success) {
        switch (status) {
        case read_status::file_error:
            printf("Cannot open the file %s\n", argv[1]);
            break;
        case read_status::format:
            printf("Cannot read the file %s\n", argv[1]);
            break;
        case read_status::eof:
            printf("Unexpected eof reading the file %s\n", argv[1]);
            break;
        case read_status::memory:
            printf("Memory error occured reading the file %s\n", argv[1]);
            break;
        default:
            printf("Unknown error processing the file %s\n", argv[1]);
        }
        return -1;
    }

    double timer = clock();
    int result = process_stream(stdin, stdout, db);

    timer = (clock() - timer) / CLOCKS_PER_SEC;
    printf("%s : Result = %d Elapsed = %.2f\n", argv[0], (result < 0 ? -1 : result), timer);

    return 0;
}
