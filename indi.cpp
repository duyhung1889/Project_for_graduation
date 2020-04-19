# include<bits/stdc++.h>
using namespace std;

# define pb push_back

# define w1 0.4
# define w2 0.3
# define w3 0.3

# define T 100
# define R 200
# define S 100

# define z1 0.7
# define z2 0.83


float random_() {
	return rand() / (RAND_MAX + 1.);
}

float dur[T];
//hai mảng này là gì??
vector<int> adj_f[T];
vector<int> adj_b[T];
bool treq[T][S];
float lexp[R][S];
int t, r, s;
float maxdur = 0;
float zz;

class Indi {
	public:
		float sched[T];
		bool assig[T][R];
		//Fitn là biến gì ?
		float fitn;
		//Cá thể?? 
		Indi() {
			//Hàng đợi này là gì? Duyệt BFS
			queue<int>Q;
	
			for(int i = 0; i < t; i++) {
				if (adj_b[i].size() == 0) {
					Q.push(i);
					sched[i] = random_() * (dur[i] + maxdur) / 8;	
				}
			}

			while (!Q.empty()) {
				
				int u = Q.front();
				
				Q.pop();
				for(int v : adj_f[u]) {
				sched[v] = max(sched[v], (float)(sched[u] + dur[u] + (random_() - 0.5) * (dur[v] + maxdur )/ 4));
					Q.push(v);
				}
			}
			for(int i = 0; i < t; i++) sched[i] = max(sched[i], (float)0);

			fitn = 0;
			for(int i = 0; i < t; i++) {
				for(int j = 0; j < r; j++) {
					assig[i][j] = random_() > 0.6 ? 1 : 0;
				}
			}
		}
		// biến này dùng để làm gì??// kiểm tra thuộc Tabulist 
		bool operator == (Indi &b) {
			float d = 0;
			float z = 0;
			for(int i = 0; i < t; i++) {
				d += abs(sched[i] - b.sched[i]);
				z += max(sched[i], b.sched[i]);
			}
			if ((d/z) > 1 - z1) {
				return 0;
			}

			float s = 0;
			for(int i = 0; i < t; i++) {
				for(int j = 0; j < r; j++) {
					s += assig[i][j] == b.assig[i][j];
				}
			}
			if (s / (t * r) < z2) return 0;
			return 1;
		}

		Indi(const Indi &b) {
			fitn = b.fitn;
			for(int i = 0; i < t; i++) {
				sched[i] = b.sched[i];
			}
			for(int i = 0; i < t; i++) {
				for(int j = 0; j < r; j++) {
					assig[i][j] = b.assig[i][j];
				}
			}
		}

		void print() {
			cout << "****************************************" << endl;
			cout << "SCHED : " << endl;
			for(int i = 0; i < t; i++) {
				cout << i << "\t" << sched[i] << endl;
			}
			cout << endl << "ASSIGN : " << endl;
			for(int i = 0; i < t; i++) {
				cout << i << " :\t";
				for(int j = 0; j < r; j++) {
					cout << assig[i][j] << " ";
				}
				cout << endl;
			}
			cout << "****************************************" << endl;
		}
};

//Hàm mục tiêu F_duration(x)
float duration(Indi a) {
	float t_delay = 0;
	for(int i = 0; i < t; i++) {
		float ti_start = 0;
		int bb = -1;
		for (int b : adj_b[i]) {
			if (a.sched[b] + dur[b] > ti_start) {
				bb = b;
				ti_start = a.sched[b] + dur[b];
			}
		}
		float ti_idle = a.sched[i] - ti_start;
		//Hình như đây là hàm mục tiêu cổ điển? 
		float ti_delay = ti_idle < 0 ? 1 / (1 - pow(ti_idle / dur[bb],3)) : 1 / (1 + pow((ti_idle + dur[i]) / (zz - ti_start), 2));
	
		t_delay += ti_delay;
	}

	return t_delay / t;
}
//Hàm mục tiêu về kinh nghiệm LTV? 
float experience(Indi a) {
	float t_exp = 0;
	for(int i = 0; i < t; i++) {
		int ns = 0;
		float ti_exp = 0;
		for(int j = 0; j < s; j++) {
			float sume = 0;
			for(int k = 0; k < r; k++) {
				float lexpkj = a.assig[i][k] * lexp[k][j];
				sume += lexpkj;
			}
			ti_exp += (treq[i][j] * sume / (sume + 1));
			ns += treq[i][j];
		}
		t_exp += ti_exp / (ns + 1e-9);

	}
	return t_exp / t;
}
//Hàm mục tiêu ĐỀ XUẤT Xung đột thời gian của phân bổ nguồn lực 
float assignment(Indi a) {
	float f_assg = 0;
	for(int k = 0; k < r; k++) {
		float tk_conf = 0;
		float tk_assg = 0;
		for(int i = 0; i < t - 1; i++) {
			for(int j = i + 1; j < t; j++) {
				tk_assg += min(dur[i], dur[j]);	
			}
		}
		for(int i = 0; i < t - 1; i++) {
			for(int j = i + 1; j < t; j++) {
				if (a.assig[i][k] and a.assig[j][k]) {
					float ti_start = a.sched[i];
					float ti_end = ti_start + dur[i];
					float tj_start = a.sched[j];
					float tj_end = tj_start + dur[j];
					float confl = min(ti_end, tj_end) - max(ti_start, tj_start);
					tk_conf += confl > 0 ? confl : 0;
				}
			}
		}
		f_assg += tk_conf / (tk_assg + 1e-9);
	}
	return 1 / (1 + f_assg);
}

float fitness(Indi a) { // FITNESS CUA TOAN BO CA THE
	return w1 * duration(a) + w2 * experience(a) + w3 * assignment(a);
}

//Vì sao lại cần hàm Copy??
Indi copy(Indi a) {
	Indi x;
	for(int i = 0; i < t; i++) {
		x.sched[i] = a.sched[i];
	}
	for(int i = 0; i < t; i++) {
		for(int j = 0; j < r; j++) {
			x.assig[i][j] = a.assig[i][j];
		}
	}
	return x;
}
//Phương pháp này để tạo ra các neighborhood
Indi mutate(Indi c) { // DOT BIEN TREN TOAN BO CA THE
	Indi d = copy(c);
	//For này xử lí đột biến ở ti_sched 
	for(int i = 0; i < t; i++) {
		if (random_() < 0.1) {
			float dt = random_() - 0.5;
			queue<int> Q; Q.push(i);
			d.sched[i] = d.sched[i] + dt * (dur[i] + maxdur) / 4;
			while (!Q.empty()) {
				int u = Q.front(); Q.pop();
				for(int v : adj_f[u]) {
					d.sched[v] = d.sched[v] + dt * (dur[v] + maxdur) / 4;
					Q.push(v);
				}
			}
		}	
	}

	for(int i = 0; i < t; i++) {
		d.sched[i] = max(d.sched[i], (float)0);
	}
	//Fàm này xử lý đột biến ở ti_assign phải ko 
	for(int i = 0; i < t; i++) {
		for(int j = 0; j < r; j++) {
			if (random_() < 0.05) d.assig[i][j] = !d.assig[i][j];
			if (random_() < 0.15) {
				int tm = rand() % t;
				bool z = d.assig[i][j];
				d.assig[i][j] = d.assig[tm][j];
				d.assig[tm][j] = z;
			}
		}
	}

	return d;
}
//kiểm tra cá thể có nằm trong tabu list hay không? 
bool is_contain(vector<Indi> p, Indi c) {
	for (int i = p.size() - 1; i >= 0; i--) {
		if (p[i] == c) return 1;
	}
	return 0;
}
//hàm này dùng làm gì??
bool cmp(Indi a, Indi b) {
	return a.fitn > b.fitn;
}
