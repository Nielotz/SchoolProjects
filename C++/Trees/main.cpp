#include <iostream>
#include <string>

using namespace std;

class KeyError : public out_of_range {
public:
    explicit KeyError(const char *message) : out_of_range(message) {}
};

class BinarySearchTreeNode {
public:
    BinarySearchTreeNode *left = nullptr;
    BinarySearchTreeNode *right = nullptr;
    BinarySearchTreeNode *parent = nullptr;
    int key = -1;

    BinarySearchTreeNode(BinarySearchTreeNode *parent, int key) : parent(parent), key(key) {}

    BinarySearchTreeNode() = default;

};

class BinarySearchTree {
    BinarySearchTreeNode *root = nullptr;

public:
    BinarySearchTree() = default;

    void add_key(const int key) {
        BinarySearchTreeNode **address_of_node = &(this->root);
        while (*address_of_node != nullptr) {
            BinarySearchTreeNode *&node = *address_of_node;
            if (key >= node->key)
                address_of_node = &(node->right);
            else
                address_of_node = &(node->left);
        }
        *address_of_node = new BinarySearchTreeNode(*child, key);
    }

    void remove_key(const int key) {
    }

    BinarySearchTreeNode *find_key(const int key){
        BinarySearchTreeNode *node = this->root;
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
    /*
     * Recursively print node - horizontal tree, with root at the left.
     *
     * spacing - how many spaces put in front of parent
     */
    void _print_node(size_t deep_level, size_t spacing, BinarySearchTreeNode *node) {
        if (node->right != nullptr) {
            _print_node(deep_level + 1, spacing, node->right);
            cout << string(deep_level * spacing, ' ') << node->key << endl;

        } else
            cout << string(deep_level * spacing, ' ') << node->key << endl;
        if (node->left != nullptr)
            _print_node(deep_level + 1, spacing, node->left);
    }
};

int main() {
    BinarySearchTree binary_search_tree;

    for (auto i: {5, 10, 15, 4, 9, 14, 6, 11, 17})
        binary_search_tree.add_key(i);

    binary_search_tree.print(2);
    cout << endl;
    return 0;
}
