#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing
#include <assert.h>
#include"common_types.h"
#include "ADTCompTree.h"
int* create_int(int value) {
	int* p = malloc(sizeof(int));
	*p = value;
	return p;
}

void test_create(){
	int*p=create_int(5);
	CompTree tree=comp_tree_create(p,NULL,NULL);
	TEST_ASSERT(*(int*)comp_tree_value(tree)==(5));
	free(p);
	int *y=create_int(3);
	CompTree tree2=comp_tree_create(y,tree,NULL);
	TEST_ASSERT(*(int*)comp_tree_value(tree2)==3);
	free(y);
	int *z=create_int(2);
	CompTree tree3=comp_tree_create(z,tree2,tree);
	TEST_ASSERT(*(int*)comp_tree_value(tree3)==2);
	free(z);
	comp_tree_destroy(tree);
	comp_tree_destroy(tree2);
	comp_tree_destroy(tree3);
}


void test_size(){
	int*p=create_int(5);
	CompTree tree=comp_tree_create(p,NULL,NULL);
	int count=0;
	count=(comp_tree_size(tree));
	TEST_ASSERT(count==1);
	free(p);
	int *y=create_int(3);
	CompTree tree5=comp_tree_create(y,tree,NULL);
	free(y);
	count=comp_tree_size(tree5);
	TEST_CHECK(count==2);
	int *z=create_int(3);
	CompTree tree3=comp_tree_create(z,tree5,NULL);
	count=comp_tree_size(tree3);
	TEST_CHECK(comp_tree_size(tree3)==3);
	CompTree tree4=comp_tree_create(NULL,NULL,NULL);
	TEST_CHECK(comp_tree_size(tree4)==0);
	free(z);
	comp_tree_destroy(tree);
	comp_tree_destroy(tree3);
	comp_tree_destroy(tree5);
}


void test_value(){
	int*p=create_int(5);
	CompTree tree=comp_tree_create(p,NULL,NULL);
	TEST_ASSERT(*(int*)comp_tree_value(tree)==5);
	CompTree tree2=comp_tree_create(NULL,NULL,NULL);
	TEST_ASSERT(comp_tree_value(tree2)==NULL);
	free(p);
	comp_tree_destroy(tree);
}

void test_left_right(){
	int*p=create_int(5);
	CompTree tree=comp_tree_create(p,NULL,NULL);
    CompTree  tree2=comp_tree_create(p,tree,tree);
	TEST_ASSERT(comp_tree_left(tree2)==tree);
	TEST_ASSERT(comp_tree_right(tree2)==tree);
	TEST_ASSERT(comp_tree_right(tree)==NULL);
	TEST_ASSERT(comp_tree_left(tree)==NULL);
	free(p);
	comp_tree_destroy(tree);
	comp_tree_destroy(tree2);
}

void test_insert_remove(){
	int*p=create_int(5);
	int*z=create_int(2);
	CompTree tree=comp_tree_create(NULL,NULL,NULL);
	CompTree tree2=comptree_insert_last(tree,p);
	CompTree tree3=comp_tree_get_subtree(tree2,0);
	TEST_ASSERT(tree3==tree2);
	CompTree tree4=comp_tree_create(z,NULL,NULL);
	CompTree tree5=comptree_insert_last(tree4,z);
	CompTree tree6=comp_tree_get_subtree(tree5,1);
	TEST_ASSERT(*(int*)comp_tree_value(tree6)==2);
	CompTree tree7=comptree_remove_last(tree6);
	TEST_ASSERT(tree7==NULL);
	CompTree tree8=comptree_remove_last(tree7);
	TEST_ASSERT(tree8==NULL);
	comp_tree_destroy(tree3);
	comp_tree_destroy(tree5);
	free(p);
	free(z);
}

TEST_LIST = {
	{ "comptree_create", test_create },
	{ "comptree_size", test_size },
	{"comptree_value",test_value},
	{"comptree_left_right",test_left_right},
	{"comptree_instert_remove",test_insert_remove},
	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};