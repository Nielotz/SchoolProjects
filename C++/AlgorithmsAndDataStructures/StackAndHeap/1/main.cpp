#include <iostream>
#include <utility>
#include <cassert>

using namespace std;

class IndexError : public out_of_range {
public:
    explicit IndexError(const char *message) : out_of_range(message) {}
};

class StackError : public runtime_error {
public:
    explicit StackError(const char *message) : runtime_error(message) {}
};

class EmptyError : public StackError {
public:
    explicit EmptyError(const char *message) : StackError(message) {}
};

struct Node {
    Node *next = nullptr;
    int value = -1;

    explicit Node(int value) : value(value) {}

    Node() = default;
};

class PriorityNode : public Node {
public:
    size_t priority = -1;
    PriorityNode *next = nullptr;

    explicit PriorityNode(int value, size_t priority) : priority(priority) {
        this->value = value;
    }

    PriorityNode() = default;
};

class List {
    Node *first_node = nullptr;
public:
    List() = default;

    void insert(const int value, size_t pos) {
        if (pos == 0)
            return push_front(value);

        if (first_node == nullptr)
            throw IndexError("Cannot insert at non 0 position into empty list.");

        Node *previous_to_pos = first_node;  // At this point always equal not nullptr.

        // Traverse to one before {pos}.
        for (size_t i = 1; i < pos; i++) {
            if (previous_to_pos->next == nullptr)
                throw IndexError("Cannot insert. Insert position is out of list.");
            previous_to_pos = previous_to_pos->next;
        }
        Node *following = previous_to_pos->next;
        previous_to_pos->next = new Node(value);

        Node *&new_node = previous_to_pos->next;
        new_node->next = following;
    }

    void push_back(const Node &node) {
        if (this->first_node) {
            Node *new_node = this->first_node;
            while (new_node->next)
                new_node = new_node->next;
            new_node->next = new Node(node);
        } else
            this->first_node = new Node(node);
    }

    void push_back(const int value) {
        push_back(Node(value));
    }

    void push_front(const Node &node) {
        Node *head = this->first_node;
        this->first_node = new Node(node);
        this->first_node->next = head;
    }

    void push_front(const int value) {
        push_front(Node(value));
    }

    Node *pop_last() {
        if (this->first_node) {  // Check whether list being not empty.
            if (this->first_node->next) {  // Check whether list contains at least 2 elements.
                Node *second_to_last_node = this->first_node;
                while (second_to_last_node->next->next)
                    second_to_last_node = second_to_last_node->next;

                Node *last = second_to_last_node->next;
                second_to_last_node->next = nullptr;
                return last;

            } else {
                Node *last = this->first_node;
                this->first_node = nullptr;
                return last;
            }
        }
        throw EmptyError("Cannot pop from an empty list.");
    }

    Node *pop_front() {
        if (this->first_node == nullptr)
            throw EmptyError("Cannot pop front from empty list.");

        Node *head = this->first_node;
        this->first_node = this->first_node->next;
        return head;
    }

    void print() {
        cout << "List:" << endl;
        for (Node *node = this->first_node; node != nullptr; node = node->next)
            cout << node->value << " ";
        cout << endl;
    }

    ~List() {
        Node *node = first_node;
        while (node) {
            Node *next_node = node->next;
            delete node;
            node = next_node;
        }
    }
};

class PriorityList {
    PriorityNode *first_node = nullptr;
public:
    PriorityList() = default;

    void push(const int value, size_t priority) {
        if (first_node == nullptr || priority == 0)
            return this->push_front(value, priority);

        // There is only one element.
        if (first_node->next == nullptr) {
            if (priority > first_node->priority)
                push_back(value, priority);
            else
                push_front(value, priority);
            return;
        }

        PriorityNode *node = this->first_node;


        // Traverse to one before {priority + 1}.
        while (node->next != nullptr && priority > node->next->priority)
            node = node->next;

        PriorityNode *&previous_to_inserted = node;
        PriorityNode *&following_to_inserted = node->next;

        auto new_node = new PriorityNode(value, priority);

        new_node->next = following_to_inserted;
        previous_to_inserted->next = new_node;
        int x = 1;
    }

    void push_back(const PriorityNode &node) {
        if (this->first_node) {
            PriorityNode *new_node = this->first_node;
            while (new_node->next)
                new_node = new_node->next;
            new_node->next = new PriorityNode(node);
        } else
            this->first_node = new PriorityNode(node);
    }

    void push_back(const int value, const size_t priority) {
        push_back(PriorityNode(value, priority));
    }

    void push_front(const PriorityNode &node) {
        PriorityNode *head = this->first_node;
        this->first_node = new PriorityNode(node);
        this->first_node->next = head;
    }

    void push_front(const int value, const size_t priority) {
        push_front(PriorityNode(value, priority));
    }

    PriorityNode *pop_last() {
        if (this->first_node) {  // Check whether list being not empty.
            if (this->first_node->next) {  // Check whether list contains at least 2 elements.
                PriorityNode *second_to_last_node = this->first_node;
                while (second_to_last_node->next->next)
                    second_to_last_node = second_to_last_node->next;

                PriorityNode *last = second_to_last_node->next;
                second_to_last_node->next = nullptr;
                return last;

            } else {
                PriorityNode *last = this->first_node;
                this->first_node = nullptr;
                return last;
            }
        }
        throw EmptyError("Cannot pop from an empty list.");
    }

    PriorityNode *pop_front() {
        if (this->first_node == nullptr)
            throw EmptyError("Cannot pop front from empty list.");

        PriorityNode *head = this->first_node;
        this->first_node = this->first_node->next;
        return head;
    }

    void print() {
        cout << "List:" << endl;
        for (PriorityNode *node = this->first_node; node != nullptr; node = node->next)
            cout << "\tPriority: " << node->priority << " value:" << node->value << " " << endl;
        cout << endl;
    }

    ~PriorityList() {
        PriorityNode *node = first_node;
        while (node != nullptr) {
            PriorityNode *next_node = node->next;
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
protected:
    List list;

public:
    Node *pop_back() {
        return list.pop_last();
    }

    Node *pop_front() {
        return list.pop_front();
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

class PriorityQueue {
    PriorityList list;
public:
    //! Insert Node with value at given priority.
    //!
    //! Highest priority is 0, node will be inserted at first possible position.
    //! \param value
    //! \param priority size_t
    void push(const int value, size_t priority) {
        list.push(value, priority);
    }

    PriorityNode *pop_front() {
        return list.pop_front();
    }

    PriorityNode *pop_back() {
        return list.pop_last();
    }

public:
    void push(const PriorityNode &node) {
        list.push_back(node);
    }
};

void test_list() {
    // I am lazy :/

    List list;  // []
    list.insert(0, 0);  // [0]
    list.insert(1, 0);  // [1, 0]
    list.insert(2, 1);  // [1, 2, 0]
    list.insert(3, 3);  // [1, 2, 0, 3]

    Node *node;
    for (int i: {1, 2, 0, 3}) {
        node = list.pop_front();
        assert(node->value == i);
        delete node;
    }
}

void test_priority_list() {
    // I am lazy :/

    PriorityList list;            // []
    list.push(0, 1);  // [0:1] - test basic push
    list.push(1, 1);  // [1:1, 0:1] - test same priority
    list.push(2, 2);  // [1:1, 0:1, 2:1] - test lower priority
    list.push(3, 0);  // [3:0, 1:1, 0:1, 2:1] - test higher priority

    PriorityNode *node;
    for (int i: {3, 1, 0, 2}) {
        node = list.pop_front();
        assert(node->value == i);
        delete node;
    }
}

void test_queue() {
    Queue queue;
    Node *node;

    queue.push_front(1);
    node = queue.pop_front();
    assert(node->value == 1);
    delete node;

    queue.push_back(1);
    node = queue.pop_back();
    assert(node->value == 1);
    delete node;

    queue.push_back(2);
    queue.push_front(1);

    node = queue.pop_front();
    assert(node->value == 1);
    delete node;
    queue.push_front(3);
    node = queue.pop_back();
    assert(node->value == 2);
    delete node;
    node = queue.pop_back();
    assert(node->value == 3);
    delete node;

}

void test_priority_queue() {
    PriorityQueue queue;
    PriorityNode *node;

    // Test push, pop_front for 1 item.
    queue.push(1, 0);
    node = queue.pop_front();
    assert(node->value == 1);
    delete node;

    // Test push, pop_back for 1 item.
    queue.push(1, 0);
    node = queue.pop_back();
    assert(node->value == 1);
    delete node;

    // Test push for 2 priorities in order.
    queue.push(1, 0);
    queue.push(2, 1);

    for (int i: {1, 2}) {
        node = queue.pop_front();
        assert(node->value == i);
        delete node;
    }

    // Test push for 2 priorities not in order.
    queue.push(2, 1);
    queue.push(1, 0);

    for (int i: {1, 2}) {
        node = queue.pop_front();
        assert(node->value == i);
        delete node;
    }

    // Test same priority.
    queue.push(1, 0);
    queue.push(2, 0);
    queue.push(3, 0);

    for (int i: {3, 2, 1}) {
        node = queue.pop_front();
        assert(node->value == i);
        delete node;
    }
}

void test_stack() {
    Stack stack;
    stack.push(1);
    stack.push(2);
    stack.push(3);

    Node *node;
    for (int i: {3, 2, 1}) {
        node = stack.pop();
        assert(node->value == i);
        delete node;
    }
}

void test_all() {
    test_stack();
    test_queue();
    test_list();
    test_priority_list();
    test_priority_queue();
}


int main() {
    test_all();

    return 0;
}
