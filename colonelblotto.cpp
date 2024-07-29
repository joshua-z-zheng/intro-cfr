#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

const int NUM_ACTIONS = 21;

uniform_real_distribution<double> unif(0, 1);
default_random_engine re;

vector<vector<int>> ps; //sz = 21
vector<int> v;

void generate_strategy(int cur){
	if (cur == 3){
		if (accumulate(v.begin(), v.end(), 0) == 5){
			ps.push_back(v);
		}
		return;
	}
	for (int i = 0; i <= 5; i++){
		v.push_back(i);
		generate_strategy(cur + 1);
		v.pop_back();
	}
}

int get_utility(vector<int> a, vector<int> b){
	int cnt_a = 0, cnt_b = 0;
	for (int i = 0; i < 3; i++){
		if (a[i] > b[i]){
			cnt_a++;
		} else if (b[i] > a[i]){
			cnt_b++;
		}
	}
	return cnt_a > cnt_b ? 1 : (cnt_a < cnt_b ? -1 : 0);
}


class BlottoTrainer {
	//initialize
	//get strategies
	//calculate utilities
	//add regrets
private:
	vector<double> regret_sum, opp_regret_sum, strategy_sum, opp_strategy_sum;

	vector<double> get_strategy(vector<double> r, vector<double> &sum){
		double cnt = 0;
		vector<double> strategy(NUM_ACTIONS);
		for (int i = 0; i < NUM_ACTIONS; i++){
			strategy[i] = r[i] > 0 ? r[i] : 0;
			cnt += strategy[i];
		}
		for (int i = 0; i < NUM_ACTIONS; i++){
			if (cnt > 0){
				strategy[i] /= cnt;
			} else {
				strategy[i] = 1.0 / NUM_ACTIONS;
			}
			sum[i] += strategy[i];
		}
		return strategy;
	} 

	int get_action(vector<double> strategy){
		double cnt = 0, p = unif(re);
		for (int i = 0; i < NUM_ACTIONS; i++){
			cnt += strategy[i];
			if (cnt > p){
				return i;
			}
		}
		return NUM_ACTIONS - 1;
	}

public:
	BlottoTrainer() : regret_sum(NUM_ACTIONS), opp_regret_sum(NUM_ACTIONS), strategy_sum(NUM_ACTIONS), opp_strategy_sum(NUM_ACTIONS) {}

	void train(int iterations){
		for (int j = 0; j < iterations; j++){
			vector<double> strategy = get_strategy(regret_sum, strategy_sum);
			vector<double> opp_strategy = get_strategy(opp_regret_sum, opp_strategy_sum);
			int action = get_action(strategy);
			int opp_action = get_action(opp_strategy);
			for (int i = 0; i < NUM_ACTIONS; i++){
				regret_sum[i] += get_utility(ps[i], ps[opp_action]) - get_utility(ps[action], ps[opp_action]);
				opp_regret_sum[i] += get_utility(ps[i], ps[action]) - get_utility(ps[opp_action], ps[action]);
			}
		}
	}

	vector<double> get_average_strategy(){
		double cnt = 0;
		vector<double> avg_strategy(NUM_ACTIONS);
		for (int i = 0; i < NUM_ACTIONS; i++){
			cnt += strategy_sum[i];
		}
		for (int i = 0; i < NUM_ACTIONS; i++){
			if (cnt > 0){
				avg_strategy[i] = strategy_sum[i] / cnt;
			} else {
				avg_strategy[i] = 1.0 / NUM_ACTIONS;
			}
		}
		return avg_strategy;
	}
};


int main(){
	generate_strategy(0);
	BlottoTrainer trainer;

	trainer.train(1000000);
	vector<double> avg = trainer.get_average_strategy();
	for (int i = 0; i < NUM_ACTIONS; i++){
		cout << avg[i] << " \n"[i == NUM_ACTIONS - 1];
	}
}