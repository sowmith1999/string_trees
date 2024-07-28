// Simple AVL Tree. github.com/sowmith1999

// What does the node have?
// What operations does it support?
// What internal operations are needed?

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>

typedef struct AVLNode {
    uint32_t height;
    uint32_t count; // because this is a sequence based tree
    uint32_t key;
    uint32_t value;
    AVLNode* parent;
    AVLNode* left;
    AVLNode* right;
} AVLNode;

typedef struct AVLTree {
    AVLNode* root;
} AVLTree;
// init function
AVLNode* init_AVLNode(uint32_t key, uint32_t value) {
    AVLNode* root = (AVLNode*)malloc(sizeof(AVLNode));
    root->height = 0;
    root->count = 1;
    root->key = key;
    root->value = value;
    root->parent = nullptr;
    root->left = nullptr;
    root->right = nullptr;
    return root;
}

uint32_t get_height(AVLNode* node) {
    if (node != nullptr)
        return node->height;
    return 0;
}

uint32_t get_count(AVLNode* node) {
    if (node != nullptr)
        return node->count;
    return 0;
}

void transplant(AVLTree* tree, AVLNode* original, AVLNode* naya) {
    if (original == tree->root) {
        tree->root = naya;
    } else if (original->parent->left == original) {
        original->parent->left = naya;
    } else if (original->parent->right == original) {
        original->parent->right = naya;
    }
    if (naya != nullptr)
        naya->parent = original->parent;
}

bool is_leftchild(AVLNode* node) {
    if (node->parent == nullptr)
        return false;
    if (node->parent->left == node)
        return true;
    return false;
}

AVLNode* get_leftmost(AVLNode* node) {
    if (node->left == nullptr) {
        return node;
    }
    return get_leftmost(node->left);
}

// two cases
// 1) should have a right child, and succ is left most node of right sub-tree
// 2) Is the left child of the parent
// else: No successors
AVLNode* get_succ(AVLNode* node) {
    if (node->right != nullptr)
        return get_leftmost(node->right);
    else if (is_leftchild(node))
        return node->parent;
    assert(false && "Node doesn't have any successors");
}

AVLNode* find(AVLNode* root, uint32_t key) {
    if (root != nullptr) {
        if (key < root->key)
            return find(root->left, key);
        else if (key > root->key)
            return find(root->right, key);
    }
    assert((root == nullptr || root->key == key) && "Shouldn't be possible in find");
    return root;
}

// inorder left -> root -> right
void traverse_AVLNode(AVLNode* tree) {
    if (tree->left != nullptr) {
        traverse_AVLNode(tree->left);
    }
    std::cout << " , " << tree->key << " : " << tree->value;
    if (tree->right != nullptr) {
        traverse_AVLNode(tree->right);
    }
}

// In a sequence tree, this'd take the index to store it at, and we'd use the same log
// ic but with the count, not the actual key value
void insert_node(AVLNode* root, uint32_t key, uint32_t value) {
    assert(root != nullptr && "root passed to insert is null");
    if (root->key == key)
        root->value = value;

    if (root->key > key) {
        if (root->left != nullptr)
            insert_node(root->left, key, value);
        else {
            AVLNode* node = init_AVLNode(key, value);
            node->parent = root;
            root->left = node;
        }
    } else if (root->key < key) {
        if (root->right != nullptr)
            insert_node(root->right, key, value);
        else {
            AVLNode* node = init_AVLNode(key, value);
            node->parent = root;
            root->right = node;
        }
    }
    root->height = std::max(get_height(root->left), get_height(root->right)) + 1;
    root->count = get_count(root->left) + get_count(root->right) + 1;
}

void delete_node(AVLTree* tree, uint32_t key) {
    AVLNode* node = find(tree->root, key);
    if (node == nullptr)
        return;

    // leaf case
    if (node->left == nullptr && node->right == nullptr) {
        transplant(tree, node, nullptr);
        // one child cases
    } else if (node->left != nullptr && node->right == nullptr) {
        transplant(tree, node, node->left);
    } else if (node->left == nullptr && node->right != nullptr) {
        transplant(tree, node, node->right);
    }
    // Two child cases
    else if (node->left != nullptr) {
        AVLNode* succ = get_succ(node);
        if (succ == node->right) {
            assert(node->right->left == nullptr && "This doesn't satisfy the c in 12.4 in CLRS");
            transplant(tree, node, succ);
            succ->left = node->left;
            succ->left->parent = succ;
            assert((node->right->parent == node->parent) && "Parents haven't been replaced correctly");
            assert((node->right->left == node->left) && "left node hasn't be replaced right");
            assert((succ->left->parent == succ) && "parent problem");
        } else if (succ != node->right) {
            transplant(tree, succ, succ->right);
            transplant(tree, node, succ);
            succ->right = node->right;
            succ->left = node->left;
            succ->right->parent = succ;
            succ->left->parent = succ;
            assert((succ->right->parent == succ) && "parent pointers are not set on right");
            assert((succ->left->parent == succ) && "parents pointers are not set on left");
        }
    }
    free(node);
}

void delete_AVLNode(AVLNode* node) {
    if (node == nullptr)
        return;
    if (node->left != nullptr)
        delete_AVLNode(node->left);
    if (node->right != nullptr)
        delete_AVLNode(node->right);
    free(node);
}

// check if the parents pointers are right
// In future add the check for height and count as well.
void sanitize_AVL(AVLNode* node) {
    if (node->left != nullptr) {
        assert((node->left->parent == node) && "Left parent pointer");
        sanitize_AVL(node->left);
    }
    if (node->right != nullptr) {
        assert((node->right->parent == node) && "Right parent pointer");
        sanitize_AVL(node->right);
    }
}

int main() {
    AVLTree* tree = (AVLTree*)malloc(sizeof(AVLTree));

    tree->root = init_AVLNode(6, 6);
    insert_node(tree->root, 1, 1);
    insert_node(tree->root, 4, 4);
    insert_node(tree->root, 0, 0);
    insert_node(tree->root, 2, 2);
    insert_node(tree->root, 3, 3);
    insert_node(tree->root, 5, 5);
    printf("traverse1:\n");
    traverse_AVLNode(tree->root);
    printf("\n");
    delete_node(tree, 1);
    sanitize_AVL(tree->root);
    printf("traverse2:\n");
    traverse_AVLNode(tree->root);
    printf("\n");
    insert_node(tree->root, 24, 24);
    printf("traverse3:\n");
    traverse_AVLNode(tree->root);
    printf("\n");
    delete_AVLNode(tree->root);
    return 0;
}

// rotate (right and left)

// concat

// insert

// split

//
