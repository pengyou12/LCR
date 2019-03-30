#include <iostream>
#include <chrono>
#include <ctime>
#include <random>
#include <string>
#include <fstream>
#include <limits>
#include <unordered_set>
#include "compare_algorithms.h"
#include <set>



#include <queue>

using namespace std;

static const uint32_t INF = numeric_limits<uint32_t>::max();
typedef mt19937 RandomGenerator;

bool A_is_subsetB(set<LABEL_TYPE>& A, set<LABEL_TYPE>& B)
{
	if (includes(B.begin(), B.end(), A.begin(), A.end()))
	{
		return true;
	}
	return false;
}


int main(int argc, char** argv)
{
	//vector<LABEL_TYPE> A{ 4 };
	//cout << A.[0] << endl;
	//A.erase
	//set<LABEL_TYPE> B{ 2};
	//if (A_is_subsetB(A, B))
	//{
	//	cout << "is subset" << endl;
	//}
	//system("pause");
	test_edp();
	//test_dfs_with_label(982, 1933);
	system("pause");
	return 0;
}



