#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing
#include <assert.h>
#include "ADTRecTree.h"
#include"common_types.h"
#include "ADTRecTree_utils.h"
int* create_int(int value) {
	int* p = malloc(sizeof(int));
	*p = value;
	return p;
}


void test_utils(){
	int*p=create_int(5);
	int*z=create_int(2);
	RecTree tree=rectree_create(p,NULL,NULL);
	RecTree tree2=rectree_get_subtree(tree,0);
	TEST_ASSERT(*(int*)rectree_value(tree2)==5);
	RecTree tree6=rectree_create(z,NULL,NULL);
	RecTree tree3=rectree_replace_subtree(tree,0,tree6);
	TEST_ASSERT(tree3==tree6);
	RecTree tree4=rectree_create(p,NULL,NULL);
	tree3=rectree_replace_subtree(tree3,1,tree4);
	tree2=rectree_get_subtree(tree3,1);
	TEST_ASSERT(tree2==tree4);
	RecTree tree5=rectree_create(NULL,NULL,NULL);
	TEST_ASSERT(rectree_get_subtree(tree5,0)==NULL);
	rectree_destroy(tree2);
	rectree_destroy(tree3);
	rectree_destroy(tree5);
	free(z);
	free(p);
}

TEST_LIST = {
	
{"rectree_utils",test_utils},
	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};