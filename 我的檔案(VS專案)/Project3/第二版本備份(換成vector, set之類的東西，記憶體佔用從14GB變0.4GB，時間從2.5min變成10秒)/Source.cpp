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
//vector<int>degree;���ئbedges_from_���C�Ӥ���(set)��size���
vector<int>coreness;
vector<int> Find_coreness(const int, vector<set<int>>, set<int>);

vector<int>vertex_with_needed_coreness;
int biggest_coreness = 0;
int num_of_coreK_I_need_to_find = 0;
vector<int>clique;
vector<int> Find_clique();

void signalHandler(int signum) {exit(signum);}




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
	//�H�W�b�B�z�D�ص��w����T


	//�H�U�DK-core�åB��X
	int k;	//cin >> k;
	k = stoi(argv[2]);
	vertex_with_needed_coreness = Find_coreness(k, edges_from_, vertex_in_G);

	ofstream output_coreness_txt_fh;
	output_coreness_txt_fh.open(output_Kcore_txt_name);
	for (int i = 0; i < MAX_NUM_OF_VERTICES; i++) {
		if (coreness[i] >= k) output_coreness_txt_fh << i << " " << coreness[i] << endl;
	}
	output_coreness_txt_fh.close();
	//�H�W�DK-core�åB��X



	//�H�U�D�XClique�åB��X
	num_of_coreK_I_need_to_find = biggest_coreness + 1;//�p�Gbiggest_coreness�O3�A�ڴN�ݭn����n4��vertex
	if (vertex_with_needed_coreness.size()==vertex_in_G.size()) { //�p�ߦp�G�D�س����Xk�W��j�A�ɭP�AKcore����@���ʧ�Ҧ�vertex�R���ɭP�o�̷|�䤣�쥿�T��clique
		vertex_with_needed_coreness = Find_coreness(2, edges_from_, vertex_in_G);//���N�ۤv�A�]�@��KCore
	}

	clique = Find_clique();
	ofstream output_clique_txt_fh;
	output_clique_txt_fh.open(output_Clique_txt_name);
	for (const auto &v : clique) {
		output_clique_txt_fh << v << endl;
	}
	output_clique_txt_fh.close();
	//�H�W�D�XClique�åB��X





	system("pause");
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

			for (auto it = vertex_in_G.begin(); it != vertex_in_G.end();++it) {
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

		for(const auto &v:vertex_deleted_during_this_degree) {
			coreness[v] = cur_degree - 1;
			biggest_coreness = cur_degree - 1;
		}
	}

	return vertex_deleted_during_this_degree;
}


vector<int> Find_clique() {
	vector <int>clique;

	int first_element_captured = vertex_with_needed_coreness.front();
	clique.emplace_back(first_element_captured);

	int count = 1;
	for (int i = 1/*�Ĥ@�Ӥ����ڤw�g�ΤF*/; i < vertex_with_needed_coreness.size() && count < num_of_coreK_I_need_to_find; i++) {
		if (edges_from_[first_element_captured].find(vertex_with_needed_coreness[i]) != edges_from_[first_element_captured].end()) {
			clique.emplace_back(vertex_with_needed_coreness[i]);
			count++;
		}
	}

	//if (count != num_of_coreK_I_need_to_find)cout << "Not Found!!!!!" << endl;
	/*else*/ sort(clique.begin(), clique.end());

	return clique;//���|�Ψ�
}