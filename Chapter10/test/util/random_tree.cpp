#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "util.h"


static TreeNode* generate_tree_internal(unsigned int, unsigned int);


TreeNode* generate_tree(unsigned int random_degree) {
    if(random_degree < 2)
        return NULL;

    unsigned int node_count = 0;
    return generate_tree_internal(random_degree, node_count);
}

static TreeNode* generate_tree_internal(unsigned int random_degree, unsigned int node_count) {
    if(node_count > MAX_RANDOM_TREE_NODE || rand() % random_degree == 0)
        return NULL;

    TreeNode* root = (TreeNode*)malloc(sizeof(TreeNode));
    ++node_count;
    root->data = rand();
    root->left = generate_tree_internal(random_degree, node_count);
    root->right = generate_tree_internal(random_degree, node_count);
    return root;
}
