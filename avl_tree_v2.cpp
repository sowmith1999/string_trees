// Sequence Binary Tree - AVL Tree
// stores strings, one string is one unit, characters don't exist
#include <cstdint>
#include <string>

typedef struct AVLNode {
    uint32_t height = 0;
    uint32_t size = 1;
    std::string* val_str = nullptr;
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

typedef struct AVLTree {
    AVLNode* root = nullptr;
} AVLTree;

AVLNode* init_AVLNode(std::string* val_str) {
    AVLNode* node = new AVLNode();
    node->val_str = val_str;
    return node;
}

// Updates size along the spine
// Can just do a simple +1 along the spine, I think
void update_augments(AVLNode* naya) {
    if (naya->parent == nullptr)
        return;
    AVLNode* cur = naya->parent;
    while (cur != nullptr) {
        cur->size += 1;
        cur->height = std::max(get_height(cur->left), get_height(cur->right)) + 1;
        cur = cur->parent;
    }
}

void insert_node(AVLTree* tree, const std::string& str, uint32_t idx) {
    std::string* val_str = new std::string(str);
    AVLNode* naya = init_AVLNode(val_str);
    AVLNode* cur = nullptr;
    AVLNode* temp = tree->root;
    while (temp != nullptr) {
        cur = temp; // stores the to be parent
        if (idx < cur->size)
            temp = temp->left;
        else
            temp = temp->right;
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

int main() {
    AVLTree* tree = new AVLTree();
    insert_node(tree, "Hello World!", 0);
    insert_node(tree, "well", 0);
    insert_node(tree, "beaut", 0);
}
