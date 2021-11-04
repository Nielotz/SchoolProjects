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

    void push_back(const int value) {
        push_back(Node(value));
    }

    void push_front(const Node &node) {
        Node *head = first;
        first = new Node(node);
        first->next = head;
    }

    void push_front(const int value) {
        push_front(Node(value));
    }

    void remove_first() {
        if (first)
            throw EmptyError("Cannot remove first element from empty list.");

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

    Node *pop_front() {
        if (first)
            throw EmptyError("Cannot pop front from empty list.");

        Node *head = first;
        first = first->next;
        return head;
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

class Stack {
    List list;

public:
    Node *pop() {
        return list.pop_last();
    }

    void push(const Node &node) {
        list.push_back(node);
    }

    void push(const int value) {
        list.push_back(value);
    }

};

class Queue {
    List list;

public:
    Node *pop_back() {
        return list.pop_last();
    }

    Node *pop_front() {
        return list.remove_first();
    }

    void push_back(const Node &node) {
        list.push_back(node);
    }

    void push_back(const int value) {
        list.push_back(value);
    }

    void push_front(const Node &node) {
        list.push_front(node);
    }

    void push_front(const int value) {
        list.push_front(value);
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
