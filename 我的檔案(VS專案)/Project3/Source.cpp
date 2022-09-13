#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <csignal>
#define MAX_NUM_OF_VERTICES 82200
#pragma warning (disable: 4996)
using namespace std;



vector<set<int>>edges_from_;
set<int>vertex_in_G;
//vector<int>degree;內建在edges_from_的每個元素(set)的size函數
vector<int>coreness;
vector<int> Find_coreness(const int, vector<set<int>>, set<int>);

vector<int>vertex_with_needed_coreness;
int biggest_coreness = 0;
int num_of_coreK_I_need_to_find = 0;
vector<int>temp_clique;
vector<int>final_clique;
vector<int> Find_clique();
bool check_to_see_whether_OK_to_add(const int);

void signalHandler(int signum) { 
	sort(temp_clique.begin(), temp_clique.end());
	
	string output_Clique_txt_name = "clique.txt";//clique
	
	ofstream output_clique_txt_fh;
	output_clique_txt_fh.open(output_Clique_txt_name, std::ofstream::trunc);
	for (const auto &v : temp_clique) {
		output_clique_txt_fh << v << endl;
	}
	output_clique_txt_fh.close();




	exit(signum); 
}




int main(int argc, char *argv[]) {
	signal(SIGINT, signalHandler);


	edges_from_.resize(MAX_NUM_OF_VERTICES);
	coreness.resize(MAX_NUM_OF_VERTICES, 0);

	string input_txt_name = argv[1]; //Open_TestCase //SampleInput
	string output_Kcore_txt_name = "kcore.txt";
	string output_Clique_txt_name = "clique.txt";

	ifstream input_fh(input_txt_name);
	string temp_line;
	while (getline(input_fh, temp_line)) {
		int a = 0, b = 0;
		int idx = 0;
		for (; idx < temp_line.length() && isdigit(temp_line[idx]); idx++) {
			a *= 10;
			a += temp_line[idx] - '0';
		}
		while (!isdigit(temp_line[idx]))idx++;
		for (; idx < temp_line.length() && isdigit(temp_line[idx]); idx++) {
			b *= 10;
			b += temp_line[idx] - '0';
		}

		if (a == b)continue;

		edges_from_[a].insert(b);
		edges_from_[b].insert(a);

		vertex_in_G.insert(a);
		vertex_in_G.insert(b);
	}
	//以上在處理題目給定的資訊


	//以下求K-core並且輸出
	int k;	//cin >> k;
	k = stoi(argv[2]);
	vertex_with_needed_coreness = Find_coreness(k, edges_from_, vertex_in_G);

	ofstream output_coreness_txt_fh;
	output_coreness_txt_fh.open(output_Kcore_txt_name);
	for (int i = 0; i < MAX_NUM_OF_VERTICES; i++) {
		if (coreness[i] >= k) output_coreness_txt_fh << i << " " << coreness[i] << endl;
	}
	output_coreness_txt_fh.close();
	//以上求K-core並且輸出



	//以下求出Clique並且輸出
	num_of_coreK_I_need_to_find = biggest_coreness + 1;//如果biggest_coreness是3，我就需要找剛剛好4個vertex
	if (vertex_with_needed_coreness.size() == vertex_in_G.size()) { //小心如果題目陷阱出k超到大，導致你Kcore那邊一次性把所有vertex刪除導致這裡會找不到正確的clique
		vector<int> new_vertex_with_needed_coreness = Find_coreness(2, edges_from_, vertex_in_G);//那就自己再跑一次KCore

		if (new_vertex_with_needed_coreness == vertex_with_needed_coreness && new_vertex_with_needed_coreness.size() % 2 != 0) {
			num_of_coreK_I_need_to_find = biggest_coreness;
		}
		else {
			vertex_with_needed_coreness = new_vertex_with_needed_coreness;
		}
	}

	final_clique = Find_clique();
	ofstream output_clique_txt_fh;
	output_clique_txt_fh.open(output_Clique_txt_name);
	for (const auto &v : final_clique) {
		output_clique_txt_fh << v << endl;
	}
	output_clique_txt_fh.close();
	//以上求出Clique並且輸出




	//system("pause");
}






vector<int> Find_coreness(const int k, vector<set<int>>edges_from_, set<int>vertex_in_G) {
	int num_of_vertices_still_unused = vertex_in_G.size();
	vector<bool> vertex_still_valid; vertex_still_valid.resize(MAX_NUM_OF_VERTICES, false);
	for (const auto &v : vertex_in_G) vertex_still_valid[v] = true;

	vector<int> vertex_deleted_during_this_degree;
	for (int cur_degree = k; num_of_vertices_still_unused; cur_degree++) {
		vertex_deleted_during_this_degree.clear();

		bool flag_prev_iteration_did_nothing = false;
		while (!flag_prev_iteration_did_nothing) {
			flag_prev_iteration_did_nothing = true;

			for (auto it = vertex_in_G.begin(); it != vertex_in_G.end(); ++it) {
				auto v_start = *it;
				if (!vertex_still_valid[v_start])continue;

				if (edges_from_[v_start].size() < cur_degree) {
					vertex_still_valid[v_start] = false;
					num_of_vertices_still_unused--;
					vertex_deleted_during_this_degree.emplace_back(v_start);

					for (const auto destination : edges_from_[v_start]) edges_from_[destination].erase(edges_from_[destination].find(v_start));
					edges_from_[v_start].clear();

					flag_prev_iteration_did_nothing = false;
				}
			}
		}

		for (const auto &v : vertex_deleted_during_this_degree) {
			coreness[v] = cur_degree - 1;
			biggest_coreness = cur_degree - 1;
		}
	}

	return vertex_deleted_during_this_degree;
}


vector<int> Find_clique() {

	int first_element_captured = vertex_with_needed_coreness.front();
	temp_clique.emplace_back(first_element_captured);

	int count = 1;
	for (int i = 1/*第一個元素我已經用了*/; i < vertex_with_needed_coreness.size() && count < num_of_coreK_I_need_to_find; i++) {
		if (check_to_see_whether_OK_to_add(vertex_with_needed_coreness[i])) {
			temp_clique.emplace_back(vertex_with_needed_coreness[i]);
			count++;
		}
	}

	//if (count != num_of_coreK_I_need_to_find)cout << "Not Found!!!!!" << endl;
	/*else*/ sort(temp_clique.begin(), temp_clique.end());

	return temp_clique;//不會用到
}


bool check_to_see_whether_OK_to_add(const int num) {
	for (const auto &v_in_cur_temp_clique : temp_clique) {
		if (edges_from_[v_in_cur_temp_clique].find(num) == edges_from_[v_in_cur_temp_clique].end()) return false;
	}

	return true;
}