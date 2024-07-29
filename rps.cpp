#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

const int ROCK = 0, PAPER = 1, SCISSORS = 2, NUM_ACTIONS = 3;
uniform_real_distribution<double> unif(0, 1);
default_random_engine re;

class RPSTrainer {
private:
	double regretSum[NUM_ACTIONS], regretSumOpp[NUM_ACTIONS], strategy[NUM_ACTIONS], strategySum[NUM_ACTIONS], oppStrategy[NUM_ACTIONS], strategySumOpp[NUM_ACTIONS];

	void getStrategy(double *s, double *r, double *sum){
		double normalizingSum = 0;
		for (int i = 0; i < NUM_ACTIONS; i++){
			s[i] = r[i] > 0 ? r[i] : 0;
			normalizingSum += s[i];
		}
		for (int i = 0; i < NUM_ACTIONS; i++){
			if (normalizingSum > 0){
				s[i] /= normalizingSum;
			} else {
				s[i] = 1.0 / NUM_ACTIONS;
			}
			sum[i] += s[i];
		}
	}

	int getAction(double *s){
		double r = unif(re);
		int a = 0;
		double cumulativeProbability = 0;
		while (a < NUM_ACTIONS - 1){
			cumulativeProbability += s[a];
			if (r < cumulativeProbability){
				break;
			}
			a++;
		}
		return a;
	}

public:
	void train(int iterations){
		double actionUtility[NUM_ACTIONS] = {};
		for (int i = 0; i < iterations; i++){
			getStrategy(strategy, regretSum, strategySum);
			getStrategy(oppStrategy, regretSumOpp, strategySumOpp);
			int myAction = getAction(strategy);
			int otherAction = getAction(oppStrategy);
			actionUtility[otherAction] = 0;
			actionUtility[otherAction == NUM_ACTIONS - 1 ? 0 : otherAction + 1] = 1;
			actionUtility[otherAction == 0 ? NUM_ACTIONS - 1 : otherAction - 1] = -1;
			for (int a = 0; a < NUM_ACTIONS; a++){
				regretSum[a] += actionUtility[a] - actionUtility[myAction];
				regretSumOpp[a] += actionUtility[otherAction] - actionUtility[a];
			} 
		}
	}

	double* getAverageStrategy(){
		static double avgStrategy[NUM_ACTIONS] = {};
		double normalizingSum = 0;
		for (int a = 0; a < NUM_ACTIONS; a++){
			normalizingSum += strategySum[a];
		}
		for (int a = 0; a < NUM_ACTIONS; a++){
			if (normalizingSum > 0){
				avgStrategy[a] = strategySum[a] / normalizingSum;
			} else {
				avgStrategy[a] = 1.0 / NUM_ACTIONS;
			}
		}
		return avgStrategy;
	}
};



int main(){
	RPSTrainer trainer;
	trainer.train(1000000);
	double *avg = trainer.getAverageStrategy();
	for (int i = 0; i < NUM_ACTIONS; i++){
		cout << avg[i] << " \n"[i == NUM_ACTIONS - 1];
	}
}