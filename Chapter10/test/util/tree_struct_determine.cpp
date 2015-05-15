#include "util.h"
#include <stdlib.h>

bool is_same_tree(TreeNode *root1, TreeNode *root2) {
    if(root1 == NULL && root2 == NULL)
        return true;
    if(root1 == NULL || root2 == NULL)
        return false;
    if(root1->data != root2->data)
        return false;
    return is_same_tree(root1->left, root2->left)
           && is_same_tree(root1->right, root2->right);
}

void destroy_tree(TreeNode* root) {
    if(root == NULL)
        return;
    destroy_tree(root->left);
    destroy_tree(root->right);
    free(root);
    return;
}
