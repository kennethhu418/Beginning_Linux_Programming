#ifndef __SERIALIZE_H__
#define __SERIALIZE_H__

#include <stdio.h>
#include "tree.h"
void serialize(FILE* outfile, TreeNode *root);

TreeNode *deserialize(FILE *infile);

void out_test();
#endif
