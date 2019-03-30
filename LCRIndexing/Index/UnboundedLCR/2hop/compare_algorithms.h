#ifndef COMPARE_ALGORITHMS_H_
#define COMPARE_ALGORITHMS_H_
#include <stdint.h>
#include <algorithm>
#define NODE_TYPE uint32_t
#define DIS_TYPE uint32_t
#define LABEL_TYPE uint32_t
struct label_edge {
	int label;
	NODE_TYPE node;
	DIS_TYPE weight;
};
#include <string>
#include <set>
#include <unordered_set>
#include "algorithms.h"


using namespace std;
int test_edp();
void load_label_graph(string filename, long node_num, vector<label_edge>* adjacency_list, vector<label_edge>* adjacency_list_reverse);
int test_dfs_with_label(NODE_TYPE src, NODE_TYPE dst, unordered_set<LABEL_TYPE> labels, int node_num, string filename, vector<label_edge>* adjacency_list, vector<label_edge>* adjacency_list_reverse);
bool A_is_subset_of_B(set<LABEL_TYPE>& A, set<LABEL_TYPE>& B);


#endif
