#include <bits/stdc++.h>
using namespace std;
typedef long long ll;


const int PASS = 0, BET = 1, NUM_ACTIONS = 2;
uniform_real_distribution<double> unif(0, 1);
default_random_engine re;
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());


class KuhnTrainer {
public:
	class Node {
	public:
		string info_set;
		vector<double> regret_sum, strategy, strategy_sum;

		Node() : regret_sum(NUM_ACTIONS), strategy(NUM_ACTIONS), strategy_sum(NUM_ACTIONS) {}

		vector<double> get_strategy(double realization_weight){
			double sum = 0;
			for (int i = 0; i < NUM_ACTIONS; i++){
				strategy[i] = regret_sum[i] > 0 ? regret_sum[i] : 0;
				sum += strategy[i];
			}
			for (int i = 0; i < NUM_ACTIONS; i++){
				if (sum > 0){
					strategy[i] /= sum;
				} else {
					strategy[i] = 1.0 / NUM_ACTIONS;
				}
				strategy_sum[i] += realization_weight * strategy[i];
			}
			return strategy;
		}

		vector<double> get_avg_strategy(){
			vector<double> avg_strategy(NUM_ACTIONS);
			double sum = 0;
			for (int i = 0; i < NUM_ACTIONS; i++){
				sum += strategy_sum[i];
			}
			for (int i = 0; i < NUM_ACTIONS; i++){
				if (sum > 0){
					avg_strategy[i] = strategy_sum[i] / sum;
				} else {
					avg_strategy[i] = 1.0 / NUM_ACTIONS;
				}
			}
			return avg_strategy;
		}

		string to_str(){
			string ret = info_set + ": [";
			vector<double> avg_strategy = get_avg_strategy();
			for (int i = 0; i < NUM_ACTIONS; i++){
				ret += to_string(avg_strategy[i]) + ", ";
			}
			ret += "]";
			return ret;
		}
	};

map<string, Node> node_map;

private:
	double cfr(vector<int> cards, string history, double p0, double p1){
		int plays = history.size();
		int player = plays % 2;
		int opponent = 1 - player;
		if (plays > 1){
			bool terminal_pass = history[plays - 1] == 'p';
			bool double_bet = history.substr(plays - 2, plays) == "bb";
			bool higher = cards[player] > cards[opponent];
			if (terminal_pass){
				if (history == "pp"){
					return higher ? 1 : -1;
				} else {
					return 1;
				}
			} else if (double_bet) {
				return higher ? 2 : -2;
			}
		}


		string info_set = to_string(cards[player]) + history;
		if (!node_map.count(info_set)){
			node_map[info_set] = Node();
		} 

		Node node = node_map[info_set];
		node.info_set = info_set;

		vector<double> strategy = node.get_strategy(player == 0 ? p0 : p1);
		vector<double> util(NUM_ACTIONS);
		double node_util = 0;

		for (int i = 0; i < NUM_ACTIONS; i++){
			string next_history = history + (i == 0 ? "p" : "b");
			util[i] = player == 0 ? -cfr(cards, next_history, p0 * strategy[i], p1) : -cfr(cards, next_history, p0, p1 * strategy[i]);
			node_util += strategy[i] * util[i];
		}

		for (int i = 0; i < NUM_ACTIONS; i++){
			double regret = util[i] - node_util;
			node.regret_sum[i] += (player == 0 ? p1 : p0) * regret;
		}

		node_map[info_set] = node;

		return node_util;
	}

public:
	void train(int iterations){
		vector<int> cards = {1, 2, 3};
		double util = 0;
		for (int i = 0; i < iterations; i++){
			for (int c1 = cards.size() - 1; c1 > 0; c1--){
				int c2 = rng() % (c1 + 1);
				swap(cards[c1], cards[c2]);
			}
			util += cfr(cards, "", 1, 1);
		}
		cout << "Average game value: " << util / iterations << '\n';
		for (auto &[k, n] : node_map){
			cout << n.to_str() << '\n';
		}
	}
};


int main(){
	int iterations = 1000000;
	KuhnTrainer trainer;
	trainer.train(iterations);
}




