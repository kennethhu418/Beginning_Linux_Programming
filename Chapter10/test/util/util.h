#include <stdbool.h>
#include "../../include/tree.h"

#ifndef __RANDOM_TREE__
#define __RANDOM_TREE__


#define MAX_RANDOM_TREE_NODE 100000

TreeNode* generate_tree(unsigned int random_degree);

bool is_same_tree(TreeNode* root1, TreeNode* root2);

void destroy_tree(TreeNode* root);
#endif
