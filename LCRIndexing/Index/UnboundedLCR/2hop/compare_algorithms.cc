#include <iostream>
#include <chrono>
#include <ctime>
#include <random>
#include <string>
#include <fstream>
#include <limits>
#include <unordered_set>
#include "compare_algorithms.h"
#include <queue>
#include <set>
#include <map>
#include <chrono>

using namespace std;



class Timer
{
public:
    Timer()
        : t1(res::zero())
        , t2(res::zero())
    {
        tic();
    }

    ~Timer()
    {}

    void tic()
    {
        t1 = clock::now();
    }

    void toc(const char* str)
    {
        t2 = clock::now();
        std::cout << str << " time: " 
            << std::chrono::duration_cast<res>(t2 - t1).count() / 1e3 << "ms." << std::endl;
    }

private:
    typedef std::chrono::high_resolution_clock clock;
    typedef std::chrono::microseconds res;

    clock::time_point t1;
    clock::time_point t2;
};



static const uint32_t INF = numeric_limits<uint32_t>::max();
typedef mt19937 RandomGenerator;
bool A_is_subset_of_B(set<LABEL_TYPE>& A, set<LABEL_TYPE>& B)
{
	if (includes(B.begin(), B.end(), A.begin(), A.end()))
	{
		return true;
	}
	return false;
}

struct ppl_pair_propagate {
	NODE_TYPE node;
	DIS_TYPE dis;
	set<LABEL_TYPE> labels;//cur
	ppl_pair_propagate() { ; }
	ppl_pair_propagate(NODE_TYPE node1, DIS_TYPE dis1)
	{
		node = node1;
		dis = dis1;
	}
	ppl_pair_propagate(NODE_TYPE node1, DIS_TYPE dis1, set<LABEL_TYPE> labels1)
	{
		node = node1;
		dis = dis1;
		for (auto label : labels1)
		{
			labels.insert(label);
		}
	}

	void set_values(NODE_TYPE node1, DIS_TYPE dis1)
	{
		node = node1;
		dis = dis1;
	}

	void set_values(NODE_TYPE node1, DIS_TYPE dis1, set<LABEL_TYPE> labels1)
	{
		node = node1;
		dis = dis1;
		labels.clear();
		for (auto label : labels1)
		{
			labels.insert(label);
		}
	}


	//need to refine the compare operator
	bool operator < (const ppl_pair_propagate&a) const
	{
		if (node < a.node)
		{
			return true;
		}
		else if (node > a.node)
		{
			return false;
		}
		if (dis < a.dis)
		{
			return true;
		}
		else if (dis == a.dis)
		{
			if (labels.size() == a.labels.size())
			{
				return labels < a.labels;
			}
			if (labels.size() < a.labels.size())
			{
				return true;
			}
			return false;
		}
		return false;
	}
	void add_labels(set<LABEL_TYPE> labels1)
	{
		for (auto label : labels1)
		{
			labels.insert(label);
		}
	}
};



struct ppl_pair {
	NODE_TYPE node;
	DIS_TYPE dis;
	set<LABEL_TYPE> labels;//cur
	ppl_pair() { ; }
	ppl_pair(NODE_TYPE node1, DIS_TYPE dis1)
	{
		node = node1;
		dis = dis1;
	}
	ppl_pair(NODE_TYPE node1, DIS_TYPE dis1, set<LABEL_TYPE> labels1)
	{
		node = node1;
		dis = dis1;
		for (auto label : labels1)
		{
			labels.insert(label);
		}
	}

	void set_values(NODE_TYPE node1, DIS_TYPE dis1)
	{
		node = node1;
		dis = dis1;
	}

	void set_values(NODE_TYPE node1, DIS_TYPE dis1, set<LABEL_TYPE> labels1)
	{
		node = node1;
		dis = dis1;
		labels.clear();
		for (auto label : labels1)
		{
			labels.insert(label);
		}
	}


	bool dominated_by_one_in_set(set<ppl_pair>& cmp_labels)
	{
		//if (cmp_labels.size() >= 3)
		//{
		//	cout << endl;
		//}
		if (cmp_labels.empty())
		{
			return false;
		}
		for (auto pair : cmp_labels)
		{
			if (pair.node != node)
			{
				continue;
			}
			if (pair.dis > dis)// cannot dominate
			{
				break;
			}
			if (A_is_subset_of_B(pair.labels, labels))
			{
				return true;
			}
		}
		return false;
	}
	bool dominated_by_single(ppl_pair& cmp)
	{
		if (dis < cmp.dis)
		{
			return false;
		}
		else if (A_is_subset_of_B(labels,cmp.labels) && labels != cmp.labels)
		{
			return false;
		}
		return true;
	}
	bool operator < (const ppl_pair&a) const
	{
		if (node < a.node)
		{
			return true;
		}
		else if (node > a.node)
		{
			return false;
		}
		if (dis < a.dis)
		{
			return true;
		}
		else if (dis == a.dis)
		{

			if (labels.size() < a.labels.size())
			{
				return true;
			}
			else if (labels.size() == a.labels.size())
			{
				return labels < a.labels;
			}
			return false;
		}
		return false;
	} 
	void add_labels(set<LABEL_TYPE> labels1)
	{
		for (auto label : labels1)
		{
			labels.insert(label);
		}
	}
};


class ppl_label_index {
public:
	map<NODE_TYPE,set<ppl_pair>> in_labels;
	map<NODE_TYPE, set<ppl_pair>> out_labels;
	void insert_in_label(ppl_pair& in_label)
	{
		auto in_exist = in_labels.find(in_label.node);
		set<ppl_pair> temp;
		temp.insert(in_label);
		if (in_exist == in_labels.end())//first time to insert
		{
			in_labels.insert(std::make_pair(in_label.node, temp));
		}
		else
		{
			in_exist->second.insert(in_label);
		}
	}
	void insert_out_label(ppl_pair& out_label)
	{
		auto out_exist = out_labels.find(out_label.node);
		set<ppl_pair> temp;
		temp.insert(out_label);
		if (out_exist == out_labels.end())//first time to insert
		{
			out_labels.insert(std::make_pair(out_label.node, temp));
		}
		else
		{
			out_exist->second.insert(out_label);
		}
	}

};

class hot_degree {
public:
	NODE_TYPE node;
	NODE_TYPE degree;
	hot_degree(NODE_TYPE a, NODE_TYPE b) :node(a), degree(b) {}
	bool operator < (const hot_degree &m)const {
		return degree > m.degree;
	}
	bool operator == (const hot_degree &m) const {
		return (node == m.node && degree == m.degree);
	}
};


struct ppl_LCR_pair {
	NODE_TYPE node;
	set<LABEL_TYPE> labels;//cur
	ppl_LCR_pair() { ; }
	ppl_LCR_pair(NODE_TYPE node1)
	{
		node = node1;
	}
	ppl_LCR_pair(NODE_TYPE node1, set<LABEL_TYPE> labels1)
	{
		node = node1;
		for (auto label : labels1)
		{
			labels.insert(label);
		}
	}

	void set_values(NODE_TYPE node1)
	{
		node = node1;
	}

	void set_values(NODE_TYPE node1,  set<LABEL_TYPE> labels1)
	{
		node = node1;
		labels.clear();
		for (auto label : labels1)
		{
			labels.insert(label);
		}
	}


	bool dominated_by_one_in_set(set<ppl_LCR_pair>& cmp_labels)
	{
		//if (cmp_labels.size() >= 3)
		//{
		//	cout << endl;
		//}
		if (cmp_labels.empty())
		{
			return false;
		}
		for (auto pair : cmp_labels)
		{
			if (pair.node != node)
			{
				continue;
			}
			if (A_is_subset_of_B(pair.labels, labels))
			{
				return true;
			}
		}
		return false;
	}
	bool dominated_by_single(ppl_LCR_pair& cmp)
	{
		if (A_is_subset_of_B(labels, cmp.labels) && labels != cmp.labels)
		{
			return false;
		}
		return true;
	}
	bool operator < (const ppl_LCR_pair&a) const
	{
		if (node < a.node)
		{
			return true;
		}
		else if (node > a.node)
		{
			return false;
		}
		if (labels.size() < a.labels.size())
		{
			return true;
		}
		else if (labels.size() == a.labels.size())
		{
			return labels < a.labels;
		}		
		return false;
	}
	void add_labels(set<LABEL_TYPE> labels1)
	{
		for (auto label : labels1)
		{
			labels.insert(label);
		}
	}
};


class ppl_LCR_index {
public:
	map<NODE_TYPE, set<ppl_LCR_pair>> in_labels;
	map<NODE_TYPE, set<ppl_LCR_pair>> out_labels;
	void insert_in_label(ppl_LCR_pair& in_label)
	{
		auto in_exist = in_labels.find(in_label.node);
		set<ppl_LCR_pair> temp;
		temp.insert(in_label);
		if (in_exist == in_labels.end())//first time to insert
		{
			in_labels.insert(std::make_pair(in_label.node, temp));
		}
		else
		{
			in_exist->second.insert(in_label);
		}
	}
	void insert_out_label(ppl_LCR_pair& out_label)
	{
		auto out_exist = out_labels.find(out_label.node);
		set<ppl_LCR_pair> temp;
		temp.insert(out_label);
		if (out_exist == out_labels.end())//first time to insert
		{
			out_labels.insert(std::make_pair(out_label.node, temp));
		}
		else
		{
			out_exist->second.insert(out_label);
		}
	}

};

class LCR_index
{
public:
	map<NODE_TYPE, ppl_LCR_index> index;// the value of vertex may be not continuous
	LCR_index() {};
	// judge_reachability_partial_index(set<NODE_TYPE>& indexed, NODE_TYPE src, NODE_TYPE dst, set<LABEL_TYPE>& labels)
	//{
	//	if (src == dst)
	//	{
	//		return true;
	//	}

	//}
	bool construct_label_index(vector<label_edge>* adjacency_list, vector<label_edge>* adjacency_list_reverse, NODE_TYPE node_num)
	{
		//sort by node degree
		vector<hot_degree> hot_points;
		for (NODE_TYPE i = 0; i < node_num; i++)
		{
			NODE_TYPE temp_degree = adjacency_list[i].size() + adjacency_list_reverse[i].size();
			if (temp_degree != 0)
			{
				hot_degree hd1(i, temp_degree);
				hot_points.push_back(hd1);
			}
		}
		stable_sort(hot_points.begin(), hot_points.end(), less<hot_degree>());

		ppl_LCR_pair src(0);
		set<ppl_LCR_pair> cur_propagate;
		set<ppl_LCR_pair> cur_propagate_reverse;
		set<ppl_LCR_pair> temp_propagate;
		ppl_LCR_pair new_pair;
		ppl_LCR_pair new_pair_propagate;
		ppl_LCR_pair in_label;
		//ppl_label_index new_in_out;
		ppl_LCR_pair out_label;
		set<NODE_TYPE> already_indexed;
		bool ppl_start = false;
		//for (NODE_TYPE i = 1; i <= node_num; i++)
			//for (auto node_explore = hot_points.rbegin(); node_explore != hot_points.rend(); node_explore++)
			for (auto node_explore = hot_points.begin(); node_explore != hot_points.end(); node_explore++)
		{

			NODE_TYPE i = node_explore->node;
			src.node = i;
			if (i % 10 == 0)
			{
				cout << i << endl;
			}
			//BFS to construct the index
			cur_propagate.clear();
			cur_propagate_reverse.clear();


			//forward bfs
			cur_propagate.insert(src);
			set<NODE_TYPE> visited_index_nodes;
			while (!cur_propagate.empty())
			{
				temp_propagate.clear();
				for (auto iter1 = cur_propagate.begin(); iter1 != cur_propagate.end(); iter1++)
				{
					for (auto edge = adjacency_list[iter1->node].begin(); edge != adjacency_list[iter1->node].end(); edge++)
					{
						if (edge->node == 73)
						{
							//cout << 73 << endl;
						}
						if (already_indexed.find(edge->node) != already_indexed.end())
						{
							continue;
						}
						new_pair.set_values(edge->node,  iter1->labels);
						new_pair_propagate.set_values(edge->node, iter1->labels);

						new_pair.labels.insert(edge->label);
						if (ppl_start)
						{
							if (find_distance_ppl(i, edge->node, new_pair.labels) == 1)
							{
								continue;
							}
						}
						new_pair_propagate.labels.insert(edge->label);

						NODE_TYPE cur_node = edge->node;
						in_label.set_values(i, new_pair.labels);// store the start node
						auto exist_index = index.find(cur_node);
						if (cur_node == 38)
						{
							//cout << 38 << endl;
						}
						if (exist_index == index.end())//first time to insert index for node cur_node
						{
							ppl_LCR_index new_in_out;
							new_in_out.insert_in_label(in_label);
							index.insert(std::make_pair(cur_node, new_in_out));
							temp_propagate.insert(new_pair_propagate);
						}
						else
						{
							auto exist_in = exist_index->second.in_labels.find(in_label.node);
							if (exist_in == exist_index->second.in_labels.end())
							{
								exist_index->second.insert_in_label(in_label);
								temp_propagate.insert(new_pair_propagate);
								continue;
							}
							if (!in_label.dominated_by_one_in_set(exist_in->second))//explore conditions: not dominate by any one in in_labels
							{
								exist_index->second.insert_in_label(in_label);
								temp_propagate.insert(new_pair_propagate);
							}
						}
					}
				}
				cur_propagate = temp_propagate;
			}
			//when to stop 
			//reverse bfs
			cur_propagate_reverse.insert(src);
			while (!cur_propagate_reverse.empty())
			{
				temp_propagate.clear();
				for (auto iter1 = cur_propagate_reverse.begin(); iter1 != cur_propagate_reverse.end(); iter1++)
				{
					for (auto edge = adjacency_list_reverse[iter1->node].begin(); edge != adjacency_list_reverse[iter1->node].end(); edge++)
					{
						if (already_indexed.find(edge->node) != already_indexed.end())
						{
							continue;
						}
						new_pair.set_values(edge->node,  iter1->labels);
						new_pair_propagate.set_values(edge->node, iter1->labels);

						new_pair_propagate.labels.insert(edge->label);

						new_pair.labels.insert(edge->label);
						if (ppl_start)
						{
							if (find_distance_ppl(edge->node,i, new_pair.labels) == 1)
							{
								continue;
							}
						}



						NODE_TYPE cur_node = edge->node;
						out_label.set_values(i, new_pair.labels);// store the start node
						auto exist_index = index.find(cur_node);
						if (exist_index == index.end())//first time to insert index for node cur_node
						{
							ppl_LCR_index new_in_out;
							new_in_out.insert_out_label(out_label);
							index.insert(std::make_pair(cur_node, new_in_out));
							temp_propagate.insert(new_pair_propagate);
						}
						else
						{
							auto exist_out = exist_index->second.out_labels.find(out_label.node);
							if (exist_out == exist_index->second.out_labels.end())
							{
								exist_index->second.insert_out_label(out_label);
								temp_propagate.insert(new_pair_propagate);
								continue;
							}
							if (!out_label.dominated_by_one_in_set(exist_out->second))//explore conditions: not dominate by any one in in_labels
							{
								exist_index->second.insert_out_label(out_label);
								temp_propagate.insert(new_pair_propagate);
							}
						}
					}
				}
				cur_propagate_reverse = temp_propagate;
			}
			already_indexed.insert(i);
			ppl_start = true;
		}
		return true;
	}
	uint32_t find_mindis_with_labels_constraint(set<ppl_LCR_pair>& pairs, set<LABEL_TYPE>& labels)
	{
		//int result = INF;
		for (auto pair : pairs)
		{
			if (A_is_subset_of_B(pair.labels, labels))
			{
				return 1;//the set ppl_pair is sorted by dis firstly
			}
		}
		return INF;
	}
	uint32_t find_distance_ppl(NODE_TYPE src, NODE_TYPE dst, set<LABEL_TYPE>& labels)
	{
		if (src == dst)
		{
			return 0;
		}
		uint32_t result = INF;
		auto index_dst = index.find(dst);
		auto index_src = index.find(src);
		if (index_dst == index.end() || index_src == index.end())//no index for this query
		{
			return INF;
		}
		else
		{
			auto in_exist = index_dst->second.in_labels.find(src);
			if (in_exist == index_dst->second.in_labels.end())//no index for src dst
			{
				//return INF;
			}
			else {
				for (auto pair : in_exist->second)
				{
					if (pair.node != src)
					{
						continue;
					}
					if (A_is_subset_of_B(pair.labels, labels))
					{
						return 1;
					}
				}
			}

			auto out_exist = index_src->second.out_labels.find(dst);
			if (out_exist == index_src->second.out_labels.end())//no index for src dst
			{
				//return INF;
			}
			else {
				for (auto pair : out_exist->second)
				{
					if (pair.node != dst)
					{
						continue;
					}
					if (A_is_subset_of_B(pair.labels, labels))
					{
						return 1;
					}
				}
			}




			for (auto iter1 = index_dst->second.in_labels.begin(); iter1 != index_dst->second.in_labels.end(); iter1++)
			{
				auto iter2 = index_src->second.out_labels.find(iter1->first);
				if (iter2 == index_src->second.out_labels.end())
				{
					continue;
				}
				
				uint32_t dis1 = find_mindis_with_labels_constraint(iter1->second, labels);
				if (dis1 == INF)
				{
					continue;
				}
				uint32_t dis2 = find_mindis_with_labels_constraint(iter2->second, labels);
				if (dis1 == INF || dis2 == INF)
				{
					continue;
				}
				else {
					return 1;
				}

			}
			//return INF;
		}
		return result;
	}
};



class pruned_landmark_label
{
public:
	map<NODE_TYPE, ppl_label_index> index;// the value of vertex may be not continuous
	pruned_landmark_label() {};
	
	bool construct_label_index(vector<label_edge>* adjacency_list, vector<label_edge>* adjacency_list_reverse, NODE_TYPE node_num)
	{ 
		//sort by node degree
		vector<hot_degree> hot_points;
		for (NODE_TYPE i = 0; i < node_num; i++)
		{
			NODE_TYPE temp_degree = adjacency_list[i].size() + adjacency_list_reverse[i].size();
			if (temp_degree != 0)
			{
				hot_degree hd1(i, temp_degree);
				hot_points.push_back(hd1);
			}
		}
		stable_sort(hot_points.begin(), hot_points.end(), less<hot_degree>());

		ppl_pair_propagate src(0, 0);
		set<ppl_pair_propagate> cur_propagate;
		set<ppl_pair_propagate> cur_propagate_reverse;
		set<ppl_pair_propagate> temp_propagate;
		ppl_pair new_pair;
		ppl_pair_propagate new_pair_propagate;
		ppl_pair in_label;
		//ppl_label_index new_in_out;
		ppl_pair out_label;
		set<NODE_TYPE> already_indexed;
		for (NODE_TYPE i = 1; i <= node_num; i++)
		//for (auto node_explore = hot_points.rbegin(); node_explore != hot_points.rend(); node_explore++)
		//for (auto node_explore = hot_points.begin(); node_explore != hot_points.end(); node_explore++)
		{
			
			//NODE_TYPE i = node_explore->node;
			src.node = i;
			if (i%10 == 0)
			{
				cout << i << endl;
			}
			//BFS to construct the index
			cur_propagate.clear();
			cur_propagate_reverse.clear();
			
			
			//forward bfs
			cur_propagate.insert(src);
			set<NODE_TYPE> visited_index_nodes;
			while (!cur_propagate.empty()) 
			{
				temp_propagate.clear();
				for (auto iter1 = cur_propagate.begin(); iter1 != cur_propagate.end(); iter1++)
				{
					for (auto edge = adjacency_list[iter1->node].begin(); edge != adjacency_list[iter1->node].end(); edge++)
					{
						if (edge->node == 73)
						{
							//cout << 73 << endl;
						}
						if (already_indexed.find(edge->node) != already_indexed.end())
						{
							continue;
						}
						new_pair.set_values(edge->node, iter1->dis, iter1->labels);
						new_pair_propagate.set_values(edge->node, iter1->dis, iter1->labels);
						new_pair.dis += edge->weight;
						new_pair.labels.insert(edge->label);
						
						new_pair_propagate.dis += edge->weight;
						new_pair_propagate.labels.insert(edge->label);

						NODE_TYPE cur_node = edge->node;
						in_label.set_values(i, new_pair.dis, new_pair.labels);// store the start node
						auto exist_index = index.find(cur_node);
						if (cur_node == 38)
						{
							//cout << 38 << endl;
						}
						if (exist_index == index.end())//first time to insert index for node cur_node
						{
							ppl_label_index new_in_out;
							new_in_out.insert_in_label(in_label);
							index.insert(std::make_pair(cur_node, new_in_out));
							temp_propagate.insert(new_pair_propagate);
						}
						else
						{
							auto exist_in = exist_index->second.in_labels.find(in_label.node);
							if (exist_in == exist_index->second.in_labels.end())
							{
								exist_index->second.insert_in_label(in_label);
								temp_propagate.insert(new_pair_propagate);
								continue;
							}
							if (!in_label.dominated_by_one_in_set(exist_in->second))//explore conditions: not dominate by any one in in_labels
							{
								exist_index->second.insert_in_label(in_label);
								temp_propagate.insert(new_pair_propagate);
							}
						}
					}
				}
				cur_propagate = temp_propagate;
			}
			//when to stop 
			//reverse bfs
			cur_propagate_reverse.insert(src);
			while (!cur_propagate_reverse.empty())
			{
				temp_propagate.clear();
				for (auto iter1 = cur_propagate_reverse.begin(); iter1 != cur_propagate_reverse.end(); iter1++)
				{
					for (auto edge = adjacency_list_reverse[iter1->node].begin(); edge != adjacency_list_reverse[iter1->node].end(); edge++)
					{
						if (already_indexed.find(edge->node) != already_indexed.end())
						{
							continue;
						}
						new_pair.set_values(edge->node, iter1->dis, iter1->labels);
						new_pair_propagate.set_values(edge->node, iter1->dis, iter1->labels);

						new_pair_propagate.dis += edge->weight;//should equal to 1,otherwise, it may cause some errors
						new_pair_propagate.labels.insert(edge->label);

						new_pair.dis += edge->weight;//should equal to 1,otherwise, it may cause some errors
						new_pair.labels.insert(edge->label);
						NODE_TYPE cur_node = edge->node;
						out_label.set_values(i, new_pair.dis, new_pair.labels);// store the start node
						auto exist_index = index.find(cur_node);
						if (exist_index == index.end())//first time to insert index for node cur_node
						{
							ppl_label_index new_in_out;
							new_in_out.insert_out_label(out_label);
							index.insert(std::make_pair(cur_node, new_in_out));
							temp_propagate.insert(new_pair_propagate);
						}
						else
						{
							auto exist_out = exist_index->second.out_labels.find(out_label.node);
							if (exist_out == exist_index->second.out_labels.end())
							{
								exist_index->second.insert_out_label(out_label);
								temp_propagate.insert(new_pair_propagate);
								continue;
							}
							if (!out_label.dominated_by_one_in_set(exist_out->second))//explore conditions: not dominate by any one in in_labels
							{
								exist_index->second.insert_out_label(out_label);
								temp_propagate.insert(new_pair_propagate);
							}
						}
					}
				}
				cur_propagate_reverse = temp_propagate;
			}
			already_indexed.insert(i);
		}
		return true;
	}
	uint32_t find_mindis_with_labels_constraint(set<ppl_pair>& pairs, set<LABEL_TYPE>& labels)
	{
		//int result = INF;
		for (auto pair : pairs)
		{
			//if(pair.node != )
			if (A_is_subset_of_B(pair.labels, labels))
			{
				return pair.dis;//the set ppl_pair is sorted by dis firstly
			}
		}
		return INF;
	}
	uint32_t find_distance_ppl(NODE_TYPE src, NODE_TYPE dst, set<LABEL_TYPE>& labels)
	{
		if (src == dst)
		{
			return 0;
		}
		uint32_t result = INF;
		auto index_dst = index.find(dst);
		auto index_src = index.find(src);
		if (index_dst == index.end() || index_src == index.end())//no index for this query
		{
			return INF;
		}
		else
		{
			auto in_exist = index_dst->second.in_labels.find(src);
			if (in_exist == index_dst->second.in_labels.end())//no index for src dst
			{
				//return INF;
			}
			else {
				for (auto pair : in_exist->second)
				{
					if (pair.node != src)
					{
						continue;
					}
					if (A_is_subset_of_B(pair.labels, labels))
					{
						if (pair.dis < result) {
							result = pair.dis;
						}
					}
				}
			}

			auto out_exist = index_src->second.out_labels.find(dst);
			if (out_exist == index_src->second.out_labels.end())//no index for src dst
			{
				//return INF;
			}
			else {
				for (auto pair : out_exist->second)
				{
					if (pair.node != dst)
					{
						continue;
					}
					if (A_is_subset_of_B(pair.labels, labels))
					{
						if (pair.dis < result) {
							result = pair.dis;
						}
					}
				}
			}




			for (auto iter1 = index_dst->second.in_labels.begin(); iter1 != index_dst->second.in_labels.end(); iter1++)
			{
				auto iter2 = index_src->second.out_labels.find(iter1->first);
				if (iter2 == index_src->second.out_labels.end())
				{
					continue;
				}
				uint32_t dis1 = find_mindis_with_labels_constraint(iter1->second, labels);
				uint32_t dis2 = find_mindis_with_labels_constraint(iter2->second, labels);
				if (dis1 == INF || dis2 == INF)
				{
					continue;
				}
				if (dis1 + dis2 < result)
				{
					result = dis1 + dis2;
				}

			}
			//return INF;
		}
		return result;
	}
};

class full_landmark_index
{
public:
	unordered_map<NODE_TYPE, ppl_label_index> index;// the value of vertex may be not continuous
	full_landmark_index() {};

	bool construct_label_index(vector<label_edge>* adjacency_list, vector<label_edge>* adjacency_list_reverse, NODE_TYPE node_num)
	{
		//sort by node degree
		vector<hot_degree> hot_points;
		for (NODE_TYPE i = 0; i < node_num; i++)
		{
			NODE_TYPE temp_degree = adjacency_list[i].size() + adjacency_list_reverse[i].size();
			if (temp_degree != 0)
			{
				hot_degree hd1(i, temp_degree);
				hot_points.push_back(hd1);
			}
		}
		stable_sort(hot_points.begin(), hot_points.end(), less<hot_degree>());

		ppl_pair_propagate src(0, 0);
		set<ppl_pair_propagate> cur_propagate;
		set<ppl_pair_propagate> cur_propagate_reverse;
		set<ppl_pair_propagate> temp_propagate;
		ppl_pair new_pair;
		ppl_pair_propagate new_pair_propagate;
		ppl_pair in_label;
		//ppl_label_index new_in_out;
		ppl_pair out_label;
		set<NODE_TYPE> already_indexed;
		//for (NODE_TYPE i = 1; i <= node_num; i++)
		//for (auto node_explore = hot_points.rbegin(); node_explore != hot_points.rend(); node_explore++)
		for (auto node_explore = hot_points.begin(); node_explore != hot_points.end(); node_explore++)
		{

			NODE_TYPE i = node_explore->node;
			src.node = i;
			if (i % 10 == 0)
			{
				cout << i << endl;
			}
			//BFS to construct the index
			cur_propagate.clear();
			cur_propagate_reverse.clear();


			//forward bfs
			cur_propagate.insert(src);
			while (!cur_propagate.empty())
			{
				temp_propagate.clear();
				for (auto iter1 = cur_propagate.begin(); iter1 != cur_propagate.end(); iter1++)
				{
					for (auto edge = adjacency_list[iter1->node].begin(); edge != adjacency_list[iter1->node].end(); edge++)
					{
						if (edge->node == 73)
						{
							//cout << 73 << endl;
						}
						new_pair.set_values(edge->node, iter1->dis, iter1->labels);
						new_pair_propagate.set_values(edge->node, iter1->dis, iter1->labels);
						new_pair.dis += edge->weight;
						new_pair.labels.insert(edge->label);

						new_pair_propagate.dis += edge->weight;
						new_pair_propagate.labels.insert(edge->label);

						NODE_TYPE cur_node = edge->node;
						in_label.set_values(i, new_pair.dis, new_pair.labels);// store the start node
						auto exist_index = index.find(cur_node);
						if (cur_node == 38)
						{
							//cout << 38 << endl;
						}
						if (exist_index == index.end())//first time to insert index for node cur_node
						{
							ppl_label_index new_in_out;
							new_in_out.insert_in_label(in_label);
							index.insert(std::make_pair(cur_node, new_in_out));
							temp_propagate.insert(new_pair_propagate);
						}
						else
						{
							auto exist_in = exist_index->second.in_labels.find(in_label.node);
							if (exist_in == exist_index->second.in_labels.end())
							{
								exist_index->second.insert_in_label(in_label);
								temp_propagate.insert(new_pair_propagate);
								continue;
							}
							if (!in_label.dominated_by_one_in_set(exist_in->second))//explore conditions: not dominate by any one in in_labels
							{
								exist_index->second.insert_in_label(in_label);
								temp_propagate.insert(new_pair_propagate);
							}
						}
					}
				}
				cur_propagate = temp_propagate;
			}
			//when to stop 
			//reverse bfs
			cur_propagate_reverse.insert(src);
			while (!cur_propagate_reverse.empty())
			{
				temp_propagate.clear();
				for (auto iter1 = cur_propagate_reverse.begin(); iter1 != cur_propagate_reverse.end(); iter1++)
				{
					for (auto edge = adjacency_list_reverse[iter1->node].begin(); edge != adjacency_list_reverse[iter1->node].end(); edge++)
					{
						new_pair.set_values(edge->node, iter1->dis, iter1->labels);
						new_pair_propagate.set_values(edge->node, iter1->dis, iter1->labels);

						new_pair_propagate.dis += edge->weight;//should equal to 1,otherwise, it may cause some errors
						new_pair_propagate.labels.insert(edge->label);

						new_pair.dis += edge->weight;//should equal to 1,otherwise, it may cause some errors
						new_pair.labels.insert(edge->label);
						NODE_TYPE cur_node = edge->node;
						out_label.set_values(i, new_pair.dis, new_pair.labels);// store the start node
						auto exist_index = index.find(cur_node);
						if (exist_index == index.end())//first time to insert index for node cur_node
						{
							ppl_label_index new_in_out;
							new_in_out.insert_out_label(out_label);
							index.insert(std::make_pair(cur_node, new_in_out));
							temp_propagate.insert(new_pair_propagate);
						}
						else
						{
							auto exist_out = exist_index->second.out_labels.find(out_label.node);
							if (exist_out == exist_index->second.out_labels.end())
							{
								exist_index->second.insert_out_label(out_label);
								temp_propagate.insert(new_pair_propagate);
								continue;
							}
							if (!out_label.dominated_by_one_in_set(exist_out->second))//explore conditions: not dominate by any one in in_labels
							{
								exist_index->second.insert_out_label(out_label);
								temp_propagate.insert(new_pair_propagate);
							}
						}
					}
				}
				cur_propagate_reverse = temp_propagate;
			}
			already_indexed.insert(i);
		}
		return true;
	}
	uint32_t find_mindis_with_labels_constraint(set<ppl_pair>& pairs, set<LABEL_TYPE>& labels)
	{
		//int result = INF;
		for (auto pair : pairs)
		{
			if (A_is_subset_of_B(pair.labels, labels))
			{
				return pair.dis;//the set ppl_pair is sorted by dis firstly
			}
		}
		return INF;
	}
	uint32_t find_distance_ppl(NODE_TYPE src, NODE_TYPE dst, set<LABEL_TYPE>& labels)
	{
		if (src == dst)
		{
			return 0;
		}
		uint32_t result = INF;
		auto index_dst = index.find(dst);
		auto index_src = index.find(src);
		if (index_dst == index.end() || index_src == index.end())//no index for this query
		{
			return INF;
		}
		else
		{
			auto in_exist = index_dst->second.in_labels.find(src);
			if (in_exist == index_dst->second.in_labels.end())//no index for src dst
			{
				//return INF;
			}
			else {
				for (auto pair : in_exist->second)
				{
					if (pair.node != src)
					{
						continue;
					}
					if (A_is_subset_of_B(pair.labels, labels))
					{
						if (pair.dis < result) {
							result = pair.dis;
						}
					}
				}
			}
			for (auto iter1 = index_dst->second.in_labels.begin(); iter1 != index_dst->second.in_labels.end(); iter1++)
			{
				auto iter2 = index_src->second.out_labels.find(iter1->first);
				if (iter2 == index_src->second.out_labels.end())
				{
					continue;
				}
				uint32_t dis1 = find_mindis_with_labels_constraint(iter1->second, labels);
				uint32_t dis2 = find_mindis_with_labels_constraint(iter2->second, labels);
				if (dis1 == INF || dis2 == INF)
				{
					continue;
				}
				if (dis1 + dis2 < result)
				{
					result = dis1 + dis2;
				}

			}
			//return INF;
		}
		return result;
	}
};



struct cmp_int_pair {
	bool operator()(pair<uint32_t, uint32_t> a, pair<uint32_t, uint32_t> b) {
		if (a.first == b.first)	return a.second>b.second;
		return a.first>b.first;
	}
};

void load_label_graph(string filename, long node_num, vector<label_edge>* adjacency_list, vector<label_edge>* adjacency_list_reverse)
{
	ifstream input(filename);
	if (!input.is_open())
	{
		cerr << "Input file cannot be opened!" << endl;
		return;
		//return index;
	}
	int label;
	NODE_TYPE src, dst;// , label;
	DIS_TYPE weight;
	unordered_map<uint32_t, unordered_set<uint32_t> > other_hosts_map;
	map<std::pair<NODE_TYPE,NODE_TYPE>, bool > remove_duplicate;
	string str;
	for (int i = 1; i <= 9; i++)
	{
		getline(input,str);
	}
	weight = 1;
	while (input >> src >> dst >> label)
	{
		//label++;
		if (weight > 1)
		{
			cout << src << "\t" << dst << "\t" << weight << endl;
		}
		auto iter = remove_duplicate.find(std::make_pair(src, dst));
		if (iter == remove_duplicate.end())//remove duplicate edges
		{
			remove_duplicate.insert(std::make_pair(std::make_pair(src, dst), true));
			label_edge temp;
			label_edge temp_reverse;
			temp.label = label;
			temp.node = dst;
			temp.weight = weight;
			temp_reverse.label = label;
			temp_reverse.node = src;
			temp_reverse.weight = weight;
			adjacency_list[src].push_back(temp);
			adjacency_list_reverse[dst].push_back(temp_reverse);
		}
	}
}




int test_dfs_with_label(NODE_TYPE src,NODE_TYPE dst, unordered_set<LABEL_TYPE> labels, int node_num, string filename, vector<label_edge>* adjacency_list, vector<label_edge>* adjacency_list_reverse)
{
	//long node_num = 100;
	//load the graph file
	//string filename = "youtube_subset.txt_small";

	//query by using the dijkstra algorithm
	unordered_map<NODE_TYPE, NODE_TYPE> preVertex;
	unordered_map<uint32_t, uint32_t> distances; //node,distance currently using a map because ID is not contiguous
	distances[src] = 0;
	//priority_queue<pair<uint32_t, uint32_t>, vector<pair<uint32_t, uint32_t> >, greater<pair<uint32_t, uint32_t> > > dj_q;
	priority_queue<pair<uint32_t, uint32_t>, vector<pair<uint32_t, uint32_t> >, cmp_int_pair > dj_q;//distance and node id

	dj_q.emplace(0, src);
	while (!dj_q.empty())
	{
		auto v = dj_q.top();
		dj_q.pop();
		auto it = distances.find(v.second);
		if (it != distances.end() && v.first == it->second) // ignore redundant entries in PQ
		{

			// loop over the outgoing edges
			for (auto edge = adjacency_list[v.second].begin(); edge != adjacency_list[v.second].end(); edge++)
			{
				if (labels.find(edge->label) == labels.end()) //invalid labe
				{
					continue;
				}
				uint32_t new_weight = v.first + edge->weight;
				auto preUpdate = preVertex.find(edge->node);
				if (preUpdate  == preVertex.end())
				{//first time to insert
					preVertex.insert(std::make_pair(edge->node, v.second));
				}
				else {
					preUpdate->second = edge->node;
				}
				auto it = distances.find(edge->node);
				if (it == distances.end() || it->second > new_weight)
				{
					distances[edge->node] = new_weight;
					dj_q.emplace(new_weight, edge->node);
				}
			}
		}
	}
	//output the result path
	auto res = distances.find(dst);
	if (res == distances.end())
	{
		cout << src << " cannot reach " << dst << endl;
		return INF;
	}
	else {
		cout << "distance : " << res->second << endl;
	}
	if (res->second == INF)
		return INF;
	return 1;
	//return res->second;
}



int test_edp()
{
	ofstream al1, al2, runtime_file;
	al1.open("EDP_result");
	al2.open("ours_result");
	runtime_file.open("runtime");
	runtime_file << "src" << " \t " << "dst" << "\t" << "edp ms" << "\t" << "ours ms" << endl;

	//if (argc != 4)
	//{
	//	cout << "Usage: ./edp /path/to/graph num_of_labels num_of_vertices" << endl;
	//	exit(-1);
	//}
	uint32_t num_of_labels = 8;// atoi(argv[2]);
	uint32_t num_of_vertices = 1500000;// 2000;// atoi(argv[3]);100000 for twitter
	uint32_t num_of_queries = 1000;

	uint32_t node_num = num_of_vertices;
	vector<label_edge>* adjacency_list = new vector<label_edge>[num_of_vertices + 1];
	vector<label_edge>* adjacency_list_reverse = new vector<label_edge>[num_of_vertices + 1];


	cout << "----Stage 1: initialization----" << endl;
	//string input(argv[1]);
	//string input("youtube_subset.txt");
	//string input("Wiki-Vote.txt_randint");
	//string input("twitter_combined.txtreoder_randint");
	string input("V1000kD5L8exp.edge");
	
	load_label_graph(input, num_of_vertices, adjacency_list, adjacency_list_reverse);
	auto index_ppl_start = chrono::high_resolution_clock::now();
	
	//vector<label_edge>* adjacency_list = new vector<label_edge>[node_num + 1];
	//vector<label_edge>* adjacency_list_reverse = new vector<label_edge>[node_num + 1];
	//load_label_graph(input, node_num, adjacency_list, adjacency_list_reverse);


	//choose which index to construct
	LCR_index ppl_index;//
	//end of construction

	ppl_index.construct_label_index(adjacency_list, adjacency_list_reverse, num_of_vertices);
	auto index_ppl_end = chrono::high_resolution_clock::now();
	cout << "Created index ppl" << endl;
	
	auto index_edp_start = chrono::high_resolution_clock::now();
	//auto index = RunAlgorithmOne(input, num_of_labels);
	auto index_edp_end = chrono::high_resolution_clock::now();
	cout << "Created index edp" << endl;

	

	auto diff_edp = chrono::duration_cast<chrono::milliseconds>(index_edp_end - index_edp_start);
	auto diff_ppl = chrono::duration_cast<chrono::milliseconds>(index_ppl_end - index_ppl_start);
	cout << "edp index:\t" << diff_edp.count() << "\t: ppl index:\t" << diff_ppl.count() << endl;

	runtime_file << "edp index:\t" << diff_edp.count() << "\t: ppl index:\t" << diff_ppl.count() << endl;



	cout << "----Stage 2: query processing----" << endl;
	RandomGenerator rng;
	rng.seed(random_device()());
	uniform_int_distribution<uint32_t> label_dist(0, num_of_labels);
	uniform_int_distribution<uint32_t> vertex_dist(0, num_of_vertices);

	unordered_set<LABEL_TYPE> labels;
	set<LABEL_TYPE> temp_labels;

	for (uint32_t iter = 1; iter <= num_of_queries; ++iter)
	{
		// generate query labels
		labels.clear();
		//unordered_set<uint32_t> labels_32;

		while (labels.size() < num_of_labels / 2)
		{
			labels.insert(label_dist(rng));
			//labels_32.insert(label_dist(rng));
		}
//#ifndef NDEBUG
		for (LABEL_TYPE label : labels)
			cout << "label: " << label << endl;
//#endif

		// run query
		cout << "query num: " << iter << endl;
		/*uint32_t src;
		do {
			src = vertex_dist(rng);
		} while (index.GetMinPr(src, labels) == INF);*/
		uint32_t src = vertex_dist(rng);
		uint32_t dst = vertex_dist(rng);

		//test 58,73,(3,5)  1
		//src = 74;
		//dst = 100;
		//labels.clear();
		//labels.insert(0);
		//labels.insert(4);


		cout << "src: " << src << ", dst: " << dst << endl;

		auto t1 = chrono::high_resolution_clock::now();
		uint32_t cost = test_dfs_with_label(src, dst, labels, node_num, input,adjacency_list,adjacency_list_reverse);
		//uint32_t cost = RunAlgorithmTwo(index, src, dst, labels);
		auto t2 = chrono::high_resolution_clock::now();
		temp_labels.clear();
		//set<uint32_t> temp_labels_32;

		for (auto label : labels)
		{
			temp_labels.insert(label);
			//temp_labels_32.insert(label);
		}
		uint32_t cost2 = ppl_index.find_distance_ppl(src, dst, temp_labels);
		//uint32_t cost2 = 0;
		//uint32_t cost2 = test_dfs_with_label(src, dst, labels);
		auto t3 = chrono::high_resolution_clock::now();

		if (cost == INF)
			cout << "Cannot find a route" << endl;
		else
		{
			cout << "al1 Finished, cost is " << cost << " from " << src << " to " << dst << " with labels ";
			//for (auto&& l : labels)
				//cout << l << " ";
			//cout << endl;
		}

		if (cost2 == INF)
			cout << "Cannot find a route" << endl;
		else
		{
			cout << "al2 Finished, cost is " << cost2 << " from " << src << " to " << dst << " with labels ";
			//for (auto&& l : labels)
			//	cout << l << " ";
			//cout << endl;
		}



		uint32_t total_entries = 0;
		/*for (uint32_t l = 0; l < num_of_labels; ++l)
			total_entries += index.GetPartition(l).GetCostSize();*/
		cout << "Now there are " << total_entries << " entries in index" << endl;
		cout << iter << " " << total_entries << endl;

		auto diff = chrono::duration_cast<chrono::microseconds>(t2 - t1);
		auto diff2 = chrono::duration_cast<chrono::microseconds>(t3 - t2);
		runtime_file << src << " \t " << dst << "\t" << diff.count()/1e3 << "ms for dfs\t" << diff2.count()/1e3 <<"ms for 2hop" <<endl;
		if (cost != INF) 
		{
			al1 << src << "\t" << dst << "\t" << cost << "\t cost" << endl;

			//al1 << src << "\t" << dst << "\t";
			//al1  << cost << "\t cost \t";
			//for (auto&& l : labels)
			//	al1 << l << ",";
			//al1 << endl;
		}
		else
		{
			al1 << src << "\t" << dst << "\t" << "INF" << "\t cost" << endl;

			//al1 << src << "\t" << dst << "\t";
			//al1 << "INF" << "\t cost\t";
			//for (auto&& l : labels)
			//	al1 << l << ",";
			//al1 << endl;

		}
		if (cost2 != INF)
		{
			al2 << src << "\t" << dst << "\t" << cost2 << "\t cost" << endl;
		}
		else
		{
			al2 << src << "\t" << dst << "\t" << "INF" << "\t cost" << endl;

		}

		cout << "Time taken to run the query: " << diff.count()/1e3 << "ms" << endl;
		cout << "Time taken by pure Dijkstra algorithm to run the query: " << diff2.count()/1e3 << "ms" << endl;

		cout << "========================================" << endl;
	}
	al1.close();
	al2.close();
	runtime_file.close();
	//system("pause");
	return 0;
}
