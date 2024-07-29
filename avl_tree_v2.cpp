// Sequence Binary Tree - AVL Tree
#include <cassert>
#include <cstdint>
#include <iostream>

typedef struct AVLNode {
    uint32_t height = 0;
    uint32_t size = 1;
    int32_t val = -1;
    AVLNode* parent = nullptr;
    AVLNode* left = nullptr;
    AVLNode* right = nullptr;
} AVLNode;

uint32_t get_height(AVLNode* node) {
    if (node != nullptr)
        return node->height;
    return 0;
}

uint32_t get_size(AVLNode* node) {
    if (node != nullptr)
        return node->size;
    return 0;
}

uint32_t compute_skew(AVLNode* node) {
    assert(node != nullptr && "Compute skew has a nullptr node");
    return get_height(node->left) - get_height(node->right);
}

typedef struct AVLTree {
    AVLNode* root = nullptr;
} AVLTree;

AVLNode* init_AVLNode(int32_t val) {
    AVLNode* node = new AVLNode();
    node->val = val;
    return node;
}

// Updates size along the spine
// Here changing the size update to add left + right + 1, 
//      instead of just +1, which doesn't work for rotations.
void update_augments(AVLNode* naya) {
    if (naya->parent == nullptr)
        return;
    AVLNode* cur = naya->parent;
    while (cur != nullptr) {
        // cur->size += 1;
        cur->size = get_size(cur->left) + get_size(cur->right) + 1;
        cur->height = std::max(get_height(cur->left), get_height(cur->right)) + 1;
        cur = cur->parent;
    }
}

AVLNode* subtree_at(AVLNode* node, uint32_t idx) {
    uint32_t soize = node->left->size;
    if (idx < soize)
        return subtree_at(node->left, idx);
    else if (idx > soize)
        return subtree_at(node->right, idx - soize - 1);
    return node;
}

void insert_node(AVLTree* tree, int32_t val, uint32_t idx) {
    AVLNode* naya = init_AVLNode(val);
    AVLNode* cur = nullptr;
    AVLNode* temp = tree->root;
    while (temp != nullptr) {
        cur = temp; // stores the to be parent
        if (idx < cur->size) {
            temp = temp->left;
        } else {
            temp = temp->right;
            idx = idx - cur->size - 1;
        }
    }
    naya->parent = cur;
    if (cur == nullptr)
        tree->root = naya;
    else if (idx < cur->size)
        cur->left = naya;
    else
        cur->right = naya;

    update_augments(naya);
}

void transplant(AVLTree* tree, AVLNode* original, AVLNode* naya) {
    if (original == tree->root)
        tree->root = naya;
    else if (original->parent->left == original)
        original->parent->left = naya;
    else if (original->parent->right == original)
        original->parent->right = naya;
    if (naya != nullptr)
        naya->parent = original->parent;
}

// rot
//      : True for right rotate
//      : False for left rotate
void rotate(AVLTree* tree, AVLNode* node, bool rot) {
    // right rotate
    if (rot == true) {
        // left child definitely exists
        AVLNode* rep_node = node->left;
        node->left = rep_node->right;
        rep_node->right = node;
        transplant(tree, node, rep_node);
        node->parent = rep_node;
    }
    // left rotate
    else if (rot == false) {
        AVLNode* rep_node = node->right;
        node->right = rep_node->left;
        rep_node->left = node;
        transplant(tree, node, rep_node);
        node->parent = rep_node;
    }
    update_augments(node);
}

int main() {
    AVLTree* tree = new AVLTree();
    insert_node(tree, 1, 0);
    insert_node(tree, 2, 1);
    insert_node(tree, 3, 2);
    rotate(tree, tree->root, false);
    insert_node(tree, 4, 3);
    insert_node(tree, 5, 4);
    rotate(tree, tree->root, false);
    printf("Boom\n");
    printf("The root is %d\n", tree->root->val);
    std::cout << subtree_at(tree->root, 2)->val << std::endl;
}
