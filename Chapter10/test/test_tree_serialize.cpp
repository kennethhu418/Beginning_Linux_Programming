#include "util/util.h"
#include "../include/serialize.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h> 

const char* options_array = "hd:n:";

void print_help() {
    printf("-h\tshow this help\n");
    printf("-d\trandom degree to generate the tree\n");
    printf("-n\thow many iterations of test do you want\n");
}


int main(int argc, char **argv) {
   int option = -1;
   int exp_times = 1;
   int rand_degree = 10;
   int i = 0;
   const char* test_file = "/tmp/tree_serialize_test.data";
   TreeNode* root_org = NULL, *root_regenerated = NULL;
   FILE* fd = NULL;

   while(-1 != (option = getopt(argc, argv, options_array))) {
    switch(option) {
    case 'h':
        print_help();
        printf("\n");
        break;
    case 'd':
        rand_degree = atoi(optarg);
        break;
    case 'n':
        exp_times = atoi(optarg);
        break;
    case '?':
        printf("Invalid option: %c\n", (char)optopt);
        break;
    default:
        break;
    }
   }

   out_test();

/*
   for(i = 0; i < exp_times; ++i) {
    printf("============Starting %dth Experiment===============\n\n", i + 1);
    printf("\t\tGenerating Testing Tree...\n");
    TreeNode* root_org = generate_tree(rand_degree);

    printf("\t\tSerializing Tree\n");
    fd = fopen(test_file, "w");
    serialize(fd, root_org);
    fclose(fd);

    printf("\t\tDeserializing Tree\n");
    fd = fopen(test_file, "r");
    root_regenerated = deserialize(fd);
    fclose(fd);

    printf("\t\tVerifying deserialized tree...\n");
    assert(is_same_tree(root_org, root_regenerated));

    destroy_tree(root_org);
    destroy_tree(root_regenerated);
   }
*/
   printf("============Test Finished==============\n\n");
   return 0;
}
