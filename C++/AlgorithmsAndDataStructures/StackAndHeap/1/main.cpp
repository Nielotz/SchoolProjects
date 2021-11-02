#include <iostream>
#include <fstream>
#include <utility>
#include <string>

using namespace std;

class StackError : public std::runtime_error {
public:
    explicit StackError(const char *message) : std::runtime_error(message) {}
};

class EmptyError : public StackError {
public:
    explicit EmptyError(const char *message) : StackError(message) {}
};

class Node {
public:
    explicit Node(int value) : value(value) {}

    Node() = default;

public:
    Node *next = nullptr;
    int value = -1;
};

class List {
    Node *first = nullptr;
public:

    List() = default;

    void push_back(const Node &node) {
        if (first) {
            Node *new_node = first;
            while (new_node->next)
                new_node = new_node->next;
            new_node->next = new Node(node);
        } else
            first = new Node(node);
    }

    void remove_first() {
        Node *next = first->next;
        delete first;
        first = next;
    }

    Node *pop_last() {
        if (first) {  // Check whether list being not empty.
            if (first->next) {  // Check whether list contains at least 2 elements.
                Node *second_to_last_node = first;
                while (second_to_last_node->next->next)
                    second_to_last_node = second_to_last_node->next;
                
                Node *last = second_to_last_node->next;
                second_to_last_node->next = nullptr;
                return last;

            } else {
                Node *last = first;
                first = nullptr;
                return last;
            }
        }
        throw EmptyError("Cannot pop from an empty list.");
    }

    ~List() {
        Node *node = first;
        while (node) {
            Node *next_node = node->next;
            delete node;
            node = next_node;
        }
    }
};

/*
void read_ranking_from_file(List &ranking) {
    ifstream input_data("data.txt");

    for (string input_line; getline(input_data, input_line);) {
        // cout << "Reading line: " << input_line << endl;

        string strings[6];
        int i = 0;
        size_t last_space_pos = 0;
        size_t space_pos = input_line.find(' ');

        while (space_pos != std::string::npos) {
            strings[i] = input_line.substr(last_space_pos, space_pos - last_space_pos);
            // cout << "\tstrings[" << i << "]: " << strings[i] << endl;

            last_space_pos = space_pos + 1;  // Recompense space.
            space_pos = input_line.find(' ', last_space_pos);

            i++;
        }
        strings[i] = input_line.substr(last_space_pos, space_pos - last_space_pos);
        // cout << "\tstrings[" << i << "]: " << strings[i] << endl;


        if (i == 6)  // Contain second name.
            ranking.push_back(
                    new List::Node(strings[0], strings[1],
                                        strings[2],
                                        stoi(strings[3]),
                                        stoi(strings[4])));
        else
            ranking.push_back(
                    new List::Node(strings[0], "",
                                        strings[2],
                                        stoi(strings[3]),
                                        stoi(strings[4])));
    }
}*/

class Stack {
    List list;

public:
    Node *pop() {
        return list.pop_last();
    }

    void push(Node &node) {
        list.push_back(node);
    }

    void push(int value) {
        list.push_back(Node(value));
    }

};

int main() {
    Stack stack;
    stack.push(1);
    stack.push(2);
    stack.push(3);
    stack.push(4);
    stack.push(5);
    while (1) {
        Node *node = stack.pop();
        cout << node->value;
        delete node;
    }

    return 0;
}
