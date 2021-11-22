#include <iostream>
#include <string>

using namespace std;

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
        if (this->root == nullptr)
            this->root = new BinarySearchTreeNode(nullptr, key);
        else {
            BinarySearchTreeNode *node = this->root;
            while (true) {
                if (key >= node->key) {
                    if (node->right != nullptr)
                        node = node->right;
                    else {
                        node->right = new BinarySearchTreeNode(node, key);
                        cout << "Dodano right: " << key << endl;
                        break;
                    }
                } else {
                    if (node->left != nullptr)
                        node = node->left;
                    else {
                        node->left = new BinarySearchTreeNode(node, key);
                        cout << "Dodano left: " << key << endl;
                        break;
                    }
                }
            }
        }
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
            // Go to the right children.
            _print_node(deep_level + 1, spacing, node->right);
            cout << string(deep_level * spacing, ' ') << node->key << endl;

        } else {
            // Print parent.
            cout << string(deep_level * spacing, ' ') << node->key << endl;
        }
        if (node->left != nullptr) {
            _print_node(deep_level + 1, spacing, node->left);
        }
    }
};

int main() {
    BinarySearchTree binary_search_tree;

    for (auto i : {5, 10, 15, 4, 9, 14, 6, 11, 16})
        binary_search_tree.add_key(i);

    binary_search_tree.print(2);
    cout << endl;
    return 0;
}
