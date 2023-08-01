#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing
#include <assert.h>
#include "ADTRecTree.h"
#include"common_types.h"
int* create_int(int value) {
	int* p = malloc(sizeof(int));
	*p = value;
	return p;
}


void test_create(){
	int*p=create_int(5);
	RecTree tree=rectree_create(p,NULL,NULL);
	TEST_ASSERT(*(int*)rectree_value(tree)==(5));
	free(p);
	int *y=create_int(3);
	RecTree tree2=rectree_create(y,tree,NULL);
	TEST_ASSERT(*(int*)rectree_value(tree2)==3);
	free(y);
	int *z=create_int(2);
	RecTree tree3=rectree_create(z,tree2,tree);
	TEST_ASSERT(*(int*)rectree_value(tree3)==2);
	free(z);
	rectree_destroy(tree);
	rectree_destroy(tree2);
	rectree_destroy(tree3);
}


void test_size(){
	int*p=create_int(5);
	RecTree tree=rectree_create(p,NULL,NULL);
	int count=0;
	count=(rectree_size(tree));
	TEST_ASSERT(count==1);
	free(p);
	int *y=create_int(3);
	RecTree tree5=rectree_create(y,tree,NULL);
	free(y);
	count=rectree_size(tree5);
	TEST_CHECK(count==2);
	int *z=create_int(3);
	RecTree tree3=rectree_create(z,tree5,NULL);
	count=rectree_size(tree3);
	TEST_CHECK(rectree_size(tree3)==3);
	free(z);
	rectree_destroy(tree);
	rectree_destroy(tree3);
	rectree_destroy(tree5);
}


void test_value(){
	int*p=create_int(5);
	RecTree tree=rectree_create(p,NULL,NULL);
	TEST_ASSERT(*(int*)rectree_value(tree)==5);
	free(p);
	rectree_destroy(tree);
}


void test_left_right(){
	int*p=create_int(5);
	RecTree tree=rectree_create(p,NULL,NULL);
    RecTree tree2=rectree_create(p,tree,tree);
	TEST_ASSERT(rectree_left(tree2)==tree);
	TEST_ASSERT(rectree_right(tree2)==tree);
	TEST_ASSERT(rectree_right(tree)==NULL);
	TEST_ASSERT(rectree_left(tree)==NULL);
	free(p);
	rectree_destroy(tree);
	rectree_destroy(tree2);
}


TEST_LIST = {
	{ "rectree_create", test_create },
		{ "rectree_size", test_size },
{"rectree_value",test_value},
{"rectree_left_right",test_left_right},
	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};