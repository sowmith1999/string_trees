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
    return get_height(node->right) - get_height(node->left);
}

bool is_leftchild(AVLNode* node) {
    if (node->parent == nullptr)
        return false;
    if (node->parent->left == node)
        return true;
    return false;
}

AVLNode* get_leftmost(AVLNode* node) {
    if (node->left == nullptr)
        return node;
    return get_leftmost(node->left);
}

AVLNode* get_succ(AVLNode* node) {
    if (node->right != nullptr)
        return get_leftmost(node->right);
    else if (is_leftchild(node))
        return node->parent;
    assert(false && "Node doesn't have any successors");
}

void sanitize(AVLNode* node) {
    if (node->left != nullptr) {
        assert((node->left->parent == node) && "Left parent pointer issue");
        sanitize(node->left);
    }
    if (node->right != nullptr) {
        assert((node->right->parent == node) && "Right parent pointer issue");
        sanitize(node->right);
    }
}

void traversal(AVLNode* tree) {
    if (tree->left != nullptr)
        traversal(tree->left);
    std::cout << " , " << tree->val;
    if (tree->right != nullptr) {
        traversal(tree->right);
    }
}

typedef struct AVLTree {
    AVLNode* root = nullptr;
} AVLTree;

AVLNode* init_AVLNode(int32_t val) {
    AVLNode* node = new AVLNode();
    node->val = val;
    return node;
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
// Updates size along the spine
// Here changing the size update to add left + right + 1,
//      instead of just +1, which doesn't work for rotations.
void update_augments(AVLNode* naya) {
    // if (naya->parent == nullptr)
    //     return;
    AVLNode* cur = naya;
    while (cur != nullptr) {
        // cur->size += 1;
        cur->size = get_size(cur->left) + get_size(cur->right) + 1;
        cur->height = std::max(get_height(cur->left), get_height(cur->right)) + 1;
        cur = cur->parent;
    }
}

AVLNode* subtree_at(AVLNode* node, uint32_t idx) {
    // assert((node != nullptr) && "Index is out of bounds, node is null");
    // assert((get_size(node)>=idx) && "Index is out of bounds, idx is too big");
    if (node == nullptr || idx > get_size(node))
        return nullptr;
    AVLNode* cur = node;
    AVLNode* par = nullptr;
    while (idx > 0) {
        uint32_t soize = get_size(cur->left);
        if (idx < soize) {
            par = cur;
            cur = cur->left;
        } else if (idx > soize) {
            par = cur;
            cur = cur->right;
            idx = idx - soize - 1;
        } else
            return cur;
    }
    return par;
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
        if (node->left != nullptr)
            node->left->parent = node;
        rep_node->right = node;
        transplant(tree, node, rep_node);
        node->parent = rep_node;
    }
    // left rotate
    else if (rot == false) {
        AVLNode* rep_node = node->right;
        node->right = rep_node->left;
        if (node->right != nullptr)
            node->right->parent = node;
        rep_node->left = node;
        transplant(tree, node, rep_node);
        node->parent = rep_node;
    }
    update_augments(node);
    sanitize(tree->root);
}

// simple, check the skew, if it is in {+1, 0, -1}
// Move up to the parent, if not do rotations based on the skew.
void rebalance(AVLTree* tree, AVLNode* node) {
    assert((node != nullptr) && "Node passed to rebalance is nullptr");
    int32_t skew = compute_skew(node);
    if (skew == 2) {
        uint32_t rc_skew = compute_skew(node->right);
        if (rc_skew == 0 || rc_skew == 1) {
            rotate(tree, node, false);
        } else if (rc_skew == -1) {
            rotate(tree, node->right, true);
            rotate(tree, node, false);
        }
    } else if (skew == -2) {
        uint32_t lc_skew = compute_skew(node->left);
        if (lc_skew == 0 || lc_skew == 1) {
            rotate(tree, node, true);
        } else if (lc_skew == -1) {
            rotate(tree, node->left, false);
            rotate(tree, node, true);
        }
    }
    int32_t new_skew = compute_skew(node);
    assert((abs(new_skew) <= 1) && "skew is still more than 1 after rebalance");
    if (tree->root != node)
        return rebalance(tree, node->parent);
    return;
}

void insert_first(AVLTree* tree, AVLNode* cur_root, AVLNode* naya) {
    assert((cur_root != nullptr) && "sub tree passed is nullptr");
    if (cur_root->left == nullptr) {
        cur_root->left = naya;
        naya->parent = cur_root;
    } else
        insert_first(tree, cur_root->left, naya);
}

void insert_last(AVLTree* tree, AVLNode* cur_root, AVLNode* naya) {
    assert((cur_root != nullptr) && "sub tree passed is nullptr");
    if (cur_root->right == nullptr) {
        cur_root->right = naya;
        naya->parent = cur_root;
    } else
        insert_last(tree, cur_root->right, naya);
}

void insert_node(AVLTree* tree, int32_t val, uint32_t idx) {
    AVLNode* naya = init_AVLNode(val);
    AVLNode* parent = subtree_at(tree->root, idx);
    if (parent == nullptr)
        tree->root = naya;
    else if (parent->right == nullptr) {
        parent->right = naya;
        naya->parent = parent;
    } else
        insert_first(tree, parent->right, naya);
    update_augments(naya);
    rebalance(tree, naya);
}

void delete_node(AVLTree* tree, uint32_t idx) {
    AVLNode* node = subtree_at(tree->root, idx);

    // leaf case
    if (node->left == nullptr && node->right == nullptr)
        transplant(tree, node, nullptr);
    else if (node->left != nullptr && node->right == nullptr)
        transplant(tree, node, node->left);
    else if (node->left == nullptr && node->right != nullptr)
        transplant(tree, node, node->right);
    // Two child cases
    else if (node->left != nullptr) {
        AVLNode* succ = get_succ(node);
        if (succ == node->right) {
            assert(node->right->left == nullptr && "successor shouldn't have a left child");
            transplant(tree, node, succ);
            succ->left = node->left;
            succ->left->parent = succ;
            assert((succ->parent == node->parent) && "succ parent isn't right");
        } else if (succ != node->right) {
            transplant(tree, succ, succ->right);
            assert(succ->right->parent == succ->parent && "succ.right parent is not set");
            transplant(tree, node, succ);
            succ->right = node->right;
            succ->left = node->left;
            succ->right->parent = succ;
            succ->left->parent = succ;
        }
        update_augments(succ);
        rebalance(tree, succ);
        free(node);
    }
}

int main() {
    AVLTree* tree = new AVLTree();
    insert_node(tree, 1, 0);
    insert_node(tree, 2, 1);
    insert_node(tree, 3, 2);
    insert_node(tree, 4, 3);
    insert_node(tree, 5, 4);
    insert_node(tree, 6, 5);
    insert_node(tree, 7, 6);
    traversal(tree->root);
    std::cout << std::endl;
    delete_node(tree, 2);
    traversal(tree->root);
    std::cout << std::endl;
    insert_node(tree, 12, 3);
    traversal(tree->root);
    std::cout << std::endl;
    printf("Boom\n");
    printf("The root is %d\n", tree->root->val);
    std::cout << subtree_at(tree->root, 2)->val << std::endl;
}
