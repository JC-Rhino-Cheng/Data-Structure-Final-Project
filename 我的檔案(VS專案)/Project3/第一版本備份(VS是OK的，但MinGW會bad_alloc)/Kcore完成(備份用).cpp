#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include <cctype>
#include <cstring>
#include <vector>
//#define MAX_NUM_OF_VERTICES 15
#define MAX_NUM_OF_VERTICES 82500
using namespace std;

enum valid_status {
	INVALID_ALWAYS,//自始至終就沒有這個點
	INVALID_MADE,//這個點一直都存在，但在逐步找尋coreness的過程中這個點被INVALID了
	VALID//這個點一直都存在，而且一直都VALID
};


//bool edged[MAX_NUM_OF_VERTICES + 1][MAX_NUM_OF_VERTICES + 1]; //需要的記憶體過大
bool **edged;
int vertex_valid[MAX_NUM_OF_VERTICES + 1];
int degree[MAX_NUM_OF_VERTICES + 1];
int coreness[MAX_NUM_OF_VERTICES + 1];
int num_of_vertices = 0;

void Find_coreness(const int k);

int main() {
	edged = new bool*[MAX_NUM_OF_VERTICES + 1];
	for (int i = 0; i <= MAX_NUM_OF_VERTICES; i++) {
		edged[i] = new bool[MAX_NUM_OF_VERTICES + 1];

		for (int j = 0; j <= MAX_NUM_OF_VERTICES; j++) edged[i][j] = false;
	}



	
	//memset(edged, false, sizeof(bool));//好像不需要，因為本來預設就是false
	memset(vertex_valid, INVALID_ALWAYS, sizeof(int));
	memset(degree, 0, sizeof(int));
	memset(coreness, 0, sizeof(int));

	string input_txt_name = "sample_input.txt"; //open_testcase_ds //sample_input
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

		edged[a][b] = true;
		if (vertex_valid[a] != VALID)num_of_vertices++;
		if (vertex_valid[b] != VALID)num_of_vertices++;
		vertex_valid[a] = vertex_valid[b] = VALID;
		degree[a]++; degree[b]++;
	}
	input_fh.close();

	int k; cin >> k;
	Find_coreness(k);

	ofstream output_coreness_txt_fh;
	output_coreness_txt_fh.open(output_Kcore_txt_name);
	for (int i = 0; i <= MAX_NUM_OF_VERTICES; i++) {
		if (coreness[i] >= k) output_coreness_txt_fh << i << " " << coreness[i] << endl;
	}
	output_coreness_txt_fh.close();




	system("pause");
}



void Find_coreness(const int k) {
	bool flag_end = false;

	for (int cur_degree = k; !flag_end; cur_degree++) {
		flag_end = true;

		bool flag_prev_iteration_did_nothing = false;
		vector<int>vertex_deleted_during_this_degree;

		while (!flag_prev_iteration_did_nothing) {
			flag_prev_iteration_did_nothing = true;

			vector <int> vertex_need_to_be_deleted_this_iteration;
			for (int i = 0; i <= MAX_NUM_OF_VERTICES; i++) {
				if (vertex_valid[i] == VALID && degree[i] < cur_degree) {
					vertex_need_to_be_deleted_this_iteration.emplace_back(i);
					vertex_deleted_during_this_degree.emplace_back(i);

					vertex_valid[i] = INVALID_MADE;
					num_of_vertices--;
				}
			}

			for (const auto &v : vertex_need_to_be_deleted_this_iteration) {
				for (int i = 0; i <= v; i++) {
					if (edged[i][v]) {
						flag_prev_iteration_did_nothing = false;
						edged[i][v] = false;
						degree[i]--; degree[v]--;
					}
				}
				for (int i = v; i <= MAX_NUM_OF_VERTICES; i++) {
					if (edged[v][i]) {
						flag_prev_iteration_did_nothing = false;
						edged[v][i] = false;
						degree[v]--; degree[i]--;
						
					}
				}
			}
		}

		for (const auto &v : vertex_deleted_during_this_degree) {
			coreness[v] = cur_degree - 1;
		}

		/*for (int i = 0; i <= MAX_NUM_OF_VERTICES; i++)for (int j = 0; j <= MAX_NUM_OF_VERTICES; j++) {
			if (edged[i][j]) {
				flag_end = false;
				break;
			}
		}*/
		if (num_of_vertices)flag_end = false;
	}
}
