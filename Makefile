CC = g++ -std=c++14
FLAGS = -fsanitize=address -O3 -fstack-protector-all -g -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wwrite-strings -Wcast-align -Wno-long-long -Woverloaded-virtual -Wnon-virtual-dtor -Wcast-qual
LDFLAGS = -fsanitize=address

all: a.out
test: a.out
	cat commands.txt | ./a.out test_1e2.txt > "myres1e2.txt"
	cat commands.txt | ./a.out test_1e4.txt > "myres1e4.txt"
	cat commands.txt | ./a.out test_1e6.txt > "myres1e6.txt"

%.o : %.cpp
	$(CC) -c $(FLAGS) $<
%.out: %.o
	$(CC) $(FLAGS) $^ -o $@ $(LDFLAGS)

a.out: main.o command.o record.o list.o result_list.o
	$(CC) -o a.out $(FLAGS) main.o command.o record.o list.o result_list.o

main.o: main.cpp result_list.h result_node.h list.h node.h record.h command.h condition.h ordering.h operation.h command_type.h
command.o: command.cpp command.h result_list.h result_node.h list.h node.h record.h condition.h ordering.h operation.h command_type.h
record.o: record.cpp record.h condition.h ordering.h
list.o: list.cpp list.h node.h record.h condition.h ordering.h
result_list.o: result_list.cpp result_list.h result_node.h node.h list.h command.h record.h condition.h ordering.h operation.h command_type.h

clean:
	rm -rf *.o
	rm -rf *.out
