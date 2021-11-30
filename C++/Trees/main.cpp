#include <iostream>
#include <string>

using namespace std;

class KeyError : public out_of_range {
public:
    explicit KeyError(const char *message) : out_of_range(message) {}
};

class BinarySearchTree {
public:
    class Node {
    public:
        Node *left = nullptr;
        Node *right = nullptr;
        Node *parent = nullptr;
        int key = -1;

        Node(Node *parent, int key) : parent(parent), key(key) {}

        explicit Node(int key) : key(key) {}

        Node() = default;
    };

    BinarySearchTree() = default;

    void add_key(const int key) {
        BinarySearchTree::Node **pointer_to_parent = nullptr;
        BinarySearchTree::Node **pointer_to_child = &(this->root);
        while (*pointer_to_child != nullptr) {
            pointer_to_parent = pointer_to_child;

            BinarySearchTree::Node *&parent = *pointer_to_parent;
            if (key >= parent->key)
                pointer_to_child = &(parent->right);
            else
                pointer_to_child = &(parent->left);
        }
        BinarySearchTree::Node *&target_node = *pointer_to_child;
        if (pointer_to_parent != nullptr)
        {
            BinarySearchTree::Node *&target_parent = *pointer_to_parent;
            target_node = new BinarySearchTree::Node(target_parent, key);
        }
        else
            target_node = new BinarySearchTree::Node(key);
    }

    void remove_key(const int key) {
        BinarySearchTree::Node *node_to_remove = this->find(key);
        BinarySearchTree::Node **parent_child_to_remove;
        if (node_to_remove->parent->key >= key)
            parent_child_to_remove = &node_to_remove->parent->right;
        else
            parent_child_to_remove = &node_to_remove->parent->left;

        if (node_to_remove->left == nullptr && node_to_remove->right == nullptr) {
            delete node_to_remove;
            *parent_child_to_remove = nullptr;
        } else if (node_to_remove->left != nullptr || node_to_remove->right != nullptr) {
            if (node_to_remove->left != nullptr) {
                node_to_remove->left->parent = *parent_child_to_remove;
                *parent_child_to_remove = node_to_remove->left;
                delete node_to_remove;
            } else {
                *parent_child_to_remove = node_to_remove->right;
                delete node_to_remove;
            }
        } else if (node_to_remove->left != nullptr && node_to_remove->right != nullptr) {
            BinarySearchTree::Node *smallest_key_node = find_smallest_key(node_to_remove);
            smallest_key_node->parent = *parent_child_to_remove;
            *parent_child_to_remove = smallest_key_node;
            delete node_to_remove;
        }
    }

    BinarySearchTree::Node *find(const int key) {
        BinarySearchTree::Node *node = this->root;
        while (node != nullptr) {
            if (key > node->key)
                node = node->right;
            else if (key != node->key)
                node = node->left;
            else
                return node;
        }
        throw KeyError("Key has not been found in the tree.");
    }

    /*
     * Print tree horizontally, with root at the left.
     *
     * Tree is "knocked over" to the left.
     * */
    void print(size_t spacing) {
        if (this->root != nullptr)
            this->_print_node(0, spacing, this->root);
    }

private:
    BinarySearchTree::Node *root = nullptr;

    /*
     * Recursively print node - horizontal tree, with root at the left.
     *
     * spacing - how many spaces put in front of parent
     */
    void _print_node(size_t deep_level, size_t spacing, BinarySearchTree::Node *node) {
        if (node->right != nullptr) {
            _print_node(deep_level + 1, spacing, node->right);
            cout << string(deep_level * spacing, ' ') << node->key << endl;

        } else
            cout << string(deep_level * spacing, ' ') << node->key << endl;
        if (node->left != nullptr)
            _print_node(deep_level + 1, spacing, node->left);
    }

    static BinarySearchTree::Node *find_smallest_key(Node *parent) {
        BinarySearchTree::Node *node = parent;
        while (node->left != nullptr)
            node = node->left;
        return node;
    }
};

int main() {
    BinarySearchTree binary_search_tree;

    for (auto i: {5, 10, 15, 4, 9, 14, 6, 11, 17}) {
        binary_search_tree.add_key(i);
        // binary_search_tree.print(2);
    }

    // binary_search_tree.remove_key(4);
    binary_search_tree.print(2);
    cout << endl;
    return 0;
}
