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
        if (pointer_to_parent != nullptr) {
            BinarySearchTree::Node *&target_parent = *pointer_to_parent;
            target_node = new BinarySearchTree::Node(target_parent, key);
        } else
            target_node = new BinarySearchTree::Node(key);
    }

    void remove_key(const int key) {
        BinarySearchTree::Node **pointer_to_node_pointer_to_remove = this->find(key);
        BinarySearchTree::Node *& node_pointer_to_remove = *pointer_to_node_pointer_to_remove;

        BinarySearchTree::Node *&left_child_of_node_to_remove = node_pointer_to_remove->left;
        BinarySearchTree::Node *&right_child_of_node_to_remove = node_pointer_to_remove->right;

        // When no children.
        if (left_child_of_node_to_remove == nullptr && right_child_of_node_to_remove == nullptr) {
            delete node_pointer_to_remove;
            node_pointer_to_remove = nullptr;
        }
        // When two children.
        else if (left_child_of_node_to_remove != nullptr && right_child_of_node_to_remove != nullptr) {
            BinarySearchTree::Node **pointer_to_pointer_to_smallest_key_node = find_smallest_key(&node_pointer_to_remove);
            BinarySearchTree::Node *& smallest_key_node = *pointer_to_pointer_to_smallest_key_node;
            // What if (*parents_pointer_to_child_to_remove)->parent == nullptr?
            smallest_key_node->parent = node_pointer_to_remove->parent;
            smallest_key_node = nullptr;
        }
        // When one child.
        else if (left_child_of_node_to_remove != nullptr || right_child_of_node_to_remove != nullptr) {
            // When there is a left child.
            if (left_child_of_node_to_remove != nullptr) {
                // Swap node parent
                left_child_of_node_to_remove->parent = node_pointer_to_remove->parent;
                // Delete node.
                BinarySearchTree::Node *temp = left_child_of_node_to_remove;
                delete node_pointer_to_remove;
                // Swap node child
                node_pointer_to_remove = temp;
            }
            // When right child.
            else {
                // Swap node parent
                right_child_of_node_to_remove->parent = node_pointer_to_remove->parent;

                BinarySearchTree::Node *temp = left_child_of_node_to_remove;

                // Delete node.
                delete node_pointer_to_remove;

                // Swap node child
                node_pointer_to_remove = right_child_of_node_to_remove;
            }
        }
    }

    BinarySearchTree::Node **find(const int key) {
        if (this->root == nullptr)
            throw KeyError("Cannot search in an empty tree.");

        BinarySearchTree::Node **pointer_to_node_pointer = &(this->root);
        while ((*pointer_to_node_pointer) != nullptr) {
            if (key > (*pointer_to_node_pointer)->key)
                pointer_to_node_pointer = &(*pointer_to_node_pointer)->right;
            else if (key != (*pointer_to_node_pointer)->key)
                pointer_to_node_pointer = &(*pointer_to_node_pointer)->left;
            else
                return pointer_to_node_pointer;
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

    //! Find smallest key.
    //! \param parent pointer to a pointer of node parent
    //! \return pointer to a pointer of node found node
    static BinarySearchTree::Node **find_smallest_key(Node **parent) {
        BinarySearchTree::Node **pointer_of_node = parent;
        while ((*pointer_of_node)->left != nullptr)
            (*pointer_of_node) = (*pointer_of_node)->left;
        return pointer_of_node;
    }
};

int main() {
    BinarySearchTree binary_search_tree;

    for (auto i: {5, 10, 15, 4, 9, 14, 6, 11, 17}) {
        binary_search_tree.add_key(i);
        // binary_search_tree.print(2);
    }

    binary_search_tree.remove_key(15);
    binary_search_tree.print(2);
    cout << endl;
    return 0;
}
