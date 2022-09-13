#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include <cctype>
#include <cstring>
#include <vector>
#include <algorithm>
//#define MAX_NUM_OF_VERTICES 15
#define MAX_NUM_OF_VERTICES 82500
using namespace std;

enum vertex_valid_status {
	v_INVALID_ALWAYS,//自始至終就沒有這個點
	v_INVALID_MADE,//這個點一直都存在，但在逐步找尋coreness的過程中這個點被INVALID了
	v_VALID//這個點一直都存在，而且一直都VALID
};

enum edge_valid_status {
	e_INVALID_ALWAYS,
	e_INVALID_MADE,
	e_VALID
};


//bool edged[MAX_NUM_OF_VERTICES + 1][MAX_NUM_OF_VERTICES + 1]; //需要的記憶體過大
short **edged;
short vertex_valid[MAX_NUM_OF_VERTICES + 1];
int degree[MAX_NUM_OF_VERTICES + 1];
int coreness[MAX_NUM_OF_VERTICES + 1];
int num_of_vertices_still_unused = 0;
vector<int> Find_coreness(const int k);

vector<int>vertex_with_needed_coreness;
int biggest_coreness = 0;
int num_of_coreK_I_need_to_find = 0;
vector<int>clique;
vector<int> Find_clique();

int main() {
	edged = new short*[MAX_NUM_OF_VERTICES + 1];
	for (int i = 0; i <= MAX_NUM_OF_VERTICES; i++) {
		edged[i] = new short[MAX_NUM_OF_VERTICES + 1];

		for (int j = 0; j <= MAX_NUM_OF_VERTICES; j++) edged[i][j] = e_INVALID_ALWAYS;
	}




	//memset(edged, false, sizeof(bool));//好像不需要，因為本來預設就是false
	memset(vertex_valid, v_INVALID_ALWAYS, sizeof(short));
	memset(degree, 0, sizeof(int));
	memset(coreness, 0, sizeof(int));
	//memset(chosen_for_clique, false, sizeof(bool));//好像不需要，因為本來預設就是false

	string input_txt_name = "open_testcase_ds.txt"; //open_testcase_ds //sample_input
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

		if (a > b) {
			int temp = a;
			a = b;
			b = temp;
		}

		edged[a][b] = e_VALID;
		if (vertex_valid[a] != v_VALID)num_of_vertices_still_unused++;
		if (vertex_valid[b] != v_VALID)num_of_vertices_still_unused++;
		vertex_valid[a] = vertex_valid[b] = v_VALID;
		degree[a]++; degree[b]++;
	}
	input_fh.close();
	//以上在處理題目給定的資訊

	//以下求K-core並且輸出
	int k; cin >> k;
	vertex_with_needed_coreness = Find_coreness(k);

	ofstream output_coreness_txt_fh;
	output_coreness_txt_fh.open(output_Kcore_txt_name);
	for (int i = 0; i <= MAX_NUM_OF_VERTICES; i++) {
		if (coreness[i] >= k) output_coreness_txt_fh << i << " " << coreness[i] << endl;
	}
	output_coreness_txt_fh.close();
	//以上求K-core並且輸出


	//以下求出Clique
	num_of_coreK_I_need_to_find = biggest_coreness + 1;//如果biggest_coreness是3，我就需要找剛剛好4個vertex
	clique = Find_clique();
	ofstream output_clique_txt_fh;
	output_clique_txt_fh.open(output_Clique_txt_name);
	for(const auto &v:clique) {
		output_clique_txt_fh << v << endl;
	}
	output_clique_txt_fh.close();


	system("pause");
}



vector<int> Find_coreness(const int k) {
	bool flag_end = false;

	vector<int>vertex_deleted_during_this_degree;//這個本來應該在for迴圈裡面宣告比較好，但是因為return 需要，所以只好放在外面，然後每個for的iteration都要記得把這個清空
	for (int cur_degree = k; !flag_end; cur_degree++) {
		flag_end = true;

		bool flag_prev_iteration_did_nothing = false;
		//vector<int>vertex_deleted_during_this_degree;
		vertex_deleted_during_this_degree.clear();

		while (!flag_prev_iteration_did_nothing) {
			flag_prev_iteration_did_nothing = true;

			vector <int> vertex_need_to_be_deleted_this_iteration;
			for (int i = 0; i <= MAX_NUM_OF_VERTICES; i++) {
				if (vertex_valid[i] == v_VALID && degree[i] < cur_degree) {
					vertex_need_to_be_deleted_this_iteration.emplace_back(i);
					vertex_deleted_during_this_degree.emplace_back(i);

					vertex_valid[i] = v_INVALID_MADE;
					num_of_vertices_still_unused--;
				}
			}

			for (const auto &v : vertex_need_to_be_deleted_this_iteration) {
				for (int i = 0; i <= v; i++) {
					if (edged[i][v] == e_VALID) {
						flag_prev_iteration_did_nothing = false;
						edged[i][v] = e_INVALID_MADE;
						degree[i]--; degree[v]--;
					}
				}
				for (int i = v; i <= MAX_NUM_OF_VERTICES; i++) {
					if (edged[v][i] == e_VALID) {
						flag_prev_iteration_did_nothing = false;
						edged[v][i] = e_INVALID_MADE;
						degree[v]--; degree[i]--;
					}
				}
			}
		}

		for (const auto &v : vertex_deleted_during_this_degree) {
			coreness[v] = cur_degree - 1;
			biggest_coreness = cur_degree - 1;
		}

		/*for (int i = 0; i <= MAX_NUM_OF_VERTICES; i++)for (int j = 0; j <= MAX_NUM_OF_VERTICES; j++) {
			if (edged[i][j]) {
				flag_end = false;
				break;
			}
		}*/
		if (num_of_vertices_still_unused)flag_end = false;
	}



	return vertex_deleted_during_this_degree;
}





vector<int> Find_clique() {
	vector <int>clique;

	int first_element_captured = vertex_with_needed_coreness.front();
	clique.emplace_back(first_element_captured);

	int count = 1;
	for (int i = 1/*第一個元素我已經用了*/; i < vertex_with_needed_coreness.size() && count < num_of_coreK_I_need_to_find; i++) {
		if (edged[first_element_captured][vertex_with_needed_coreness[i]]  == e_INVALID_MADE) {
			clique.emplace_back(vertex_with_needed_coreness[i]);
			count++;
		}
	}

	if (count != num_of_coreK_I_need_to_find)cout << "Not Found!!!!!" << endl;
	else {
		sort(clique.begin(), clique.end());

		return clique;
	}

}