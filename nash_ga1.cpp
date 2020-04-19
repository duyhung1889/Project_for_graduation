# include <bits/stdc++.h>

using namespace std;

# define pb push_back

# define w1 0.3
# define w2 0.3
# define w3 0.4

# define T 100
# define R 200
# define S 100

# define z1 0.7
# define z2 0.83
# define pop_size 20
# define expand_pz 40

float dur[T];
vector<int> adj_f[T];
vector<int> adj_b[T];
bool treq[T][S];
float lexp[R][S];
int t, r, s;
float maxdur = 0;
float zz;

float random_() {
	return rand() / (RAND_MAX + 1.);
}


class Task {
	public:
		float sched;
		bool assig[R];
		int id;

		void print() {
			cout << "****************************************" << endl;
			cout << "SCHED : " << endl;
			cout << sched << endl;
			cout << endl << "ASSIGN : " << endl;
			for(int j = 0; j < r; j++) {
				cout << assig[j] << " ";
			}
			cout << endl;
			cout << "****************************************" << endl;
		}
};

class Indi {
	public:
		Task task[T];
		//Khởi tạo cá thể quần thể
		Indi() {
			queue<int>Q;
			for(int i = 0; i < t; i++) {
				task[i].id = i;
				//Những task không phụ thuộc vào task nào cả
				//Khởi tạo ngẫu nhiên thời gian bắt đầu của các task đó
				if (adj_b[i].size() == 0) {
					Q.push(i);
					task[i].sched = random_() * (dur[i] + maxdur) / 8;
				}
			}

			while (!Q.empty()) {
				int u = Q.front(); Q.pop();
				for(int v : adj_f[u]) {
					task[v].sched = max(task[v].sched, (float)(task[u].sched + dur[u] + (random_() - 0.5) * (dur[v] + maxdur )/ 4));
					Q.push(v);
				}
			}
			//Chuẩn hóa không âm
			for(int i = 0; i < t; i++) task[i].sched = max(task[i].sched, (float)0);


			for(int i = 0; i < t; i++) {
				for(int j = 0; j < r; j++) {
					task[i].assig[j] = random_() < 0.6 ? 1 : 0;
				}
			}

		}

		void print() {
			cout << "SCHED : " << endl;
			for(int i = 0; i < t; i++) {
				cout << i + 1 << "\t" << task[i].sched << endl;
			}
			cout << endl << "ASSIGN : " << endl;
			for(int i = 0; i < t; i++) {
				cout << i + 1 << " :\t";
				for(int j = 0; j < r; j++) {
					cout << task[i].assig[j] << " ";
				}
				cout << endl;
			}
			cout << "****************************************" << endl;
		}
};

Indi best;
//Hàm mục tiêu vân là cổ điển
// float duration(Indi a, int i) {
// 	float ti_start = 0;
// 	int bb = -1;
// 	for (int b : adj_b[i]) {
// 		if (a.task[b].sched + dur[b] > ti_start) {
// 			bb = b;
// 			ti_start = a.task[b].sched + dur[b];
// 		}
// 	}
// 	float ti_idle = a.task[i].sched - ti_start;
// 	// tại sao ti-idle <0 thì ti_delay không bằng 0 như lý thuyết? 
// 	// nếu ti_idle >= 0 thì ti_delay = 1/(1+ti_idel) như lý thuyết -> không hiểu cách tạo ra các hàm pow ấy như thế nào? 
// 	float ti_delay = ti_idle < 0 ? 1 / (1 - pow(ti_idle / dur[bb], 3)) : 1 / (1 + pow((ti_idle + dur[i]) / (zz - ti_start), 2));
// 	return ti_delay;
// }
//cung(i,j)
float duration(Indi a, int i) {
	float ti_start = 0;
	int bb = -1;

	float t_deadline = zz - dur[i];

	if (adj_b[i].size() == 0){
		float ti_start = 0;
		float ti_duration;
		if (a.task[i].sched < t_deadline) {
			// 1- 0.9
			ti_duration = 0.9 + 0.1 * (t_deadline - a.task[i].sched) / (t_deadline - ti_start);
		}
		else {
			//0.9 
			ti_duration = 0.9 - (a.task[i].sched - t_deadline) / dur[i];
		}
		return ti_duration;
	}

	else {
		float ti_duration = 0;
		int n = 0;
		for (int b: adj_b[i]) {
			float ti_start = a.task[b].sched + dur[b];

			if (a.task[i].sched < ti_start) {
				ti_duration += 2 * (a.task[i].sched - (a.task[b].sched + dur[b] / 2)) / dur[b]; // làm đc 50% thì task i có thể làm, nhân 2 để đưa điểm lên
			}
			else if (a.task[i].sched < t_deadline) {
				ti_duration += 0.9 + 0.1 * (t_deadline - a.task[i].sched) / (t_deadline - ti_start);
			}
			else {
				ti_duration += 0.9 - (a.task[i].sched - t_deadline) / dur[i];
			}
			n ++;
		}
		
		return ti_duration / n;
	}
	
}

float experience(Indi a, int i) {
	int ns = 0;
	float ti_exp = 0;
	for(int j = 0; j < s; j++) {
		float sume = 0;
		for(int k = 0; k < r; k++) {
			float lexpkj = a.task[i].assig[k] * lexp[k][j];
			sume += lexpkj;
		}
		ti_exp += (treq[i][j] * sume / (sume + 1));
		ns += treq[i][j];
	}
	return ti_exp / (ns + 1e-9);
}

float assignment(Indi a, int g) {
	float f_assg = 0;
	int q = 0;
	for(int k = 0; k < r; k++) {
		if (!a.task[g].assig[k]) continue;
		float tk_conf = 0;
		//tk_ass này là gì nhỉ? 
		float tk_assg = 0;
		for(int i = 0; i < t - 1; i++) {
			for(int j = i + 1; j < t; j++) {
				tk_assg += min(dur[i], dur[j]);	
			}
		}
		//"Để giải quyết nhược điểm đưa ra, khi tính rj
		//conflict ta lấy thời gian làm việc nhiều task, nhân với số task đang làm đồng thời"
		// Nhưng cách code này là chỉ cộng với xung đột giữa các task thôi? 
		for(int i = 0; i < t - 1; i++) {
			for(int j = i + 1; j < t; j++) {
				if (a.task[i].assig[k] and a.task[j].assig[k]) {
					float ti_start = a.task[i].sched;
					float ti_end = ti_start + dur[i];
					float tj_start = a.task[j].sched;
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

float fitness(Indi a, int k) { // FITNESS CUA TASK k
	return w1 * duration(a, k) + w2 * experience(a, k) + w3 * assignment(a, k);
}
//Điểm tối ưu của cả phương án 
float fitness(Indi a) {
	float res = 0;
	for(int i = 0; i < t; i++) res += fitness(a, i);
	return res / t;
}
Task copy(Task a) {
	Task b = a;
	return b;
}

Indi copy(Indi a){
	Indi b = a;
	return b;
}
//Đột biến trên một cá thể. 
Task mutate(Task a) {
	int k = a.id;
	Task d = copy(a);
	//Đột biến với xâc suất 0.5-1
	float dt = random_() - 0.5;
	d.sched = d.sched + dt * (dur[k] + maxdur) / 4;
	d.sched = max(d.sched, (float)0);
	//Đột biến phân bổ r với xác xuất 0.05 
	for(int j = 0; j < r; j++) {
		if (random_() < 0.05) d.assig[j] = !d.assig[j];
	}
	return d;
}
// lai task a vs b
Task cross(Task a, Task b) {
	Task d, e;
	if (random_() > 0.5) {
		d = copy(a);
		e = copy(b);
	}
	else {
		d = copy(b);
		e = copy(a);
	}
	//e.ched - d.ched: chênh lệch shed giưax 2 task 
	d.sched = d.sched + random_() * (e.sched - d.sched);

	int x = rand() % r;
	int y = x + rand() % (r - x);
	//Đây là đổi hết r từ task e sang task d phải không? 
	for(int i = 0; i < r; i++) {
		if (i >= x and i <= y) d.assig[i] = e.assig[i];
	}
	return d;
}
//So sánh 2 task, task nào có điểm cao hơn 
bool cmp(Task a, Task b) {
	Indi besta = copy(best);
	Indi bestb = copy(best);
	besta.task[a.id] = a;
	bestb.task[b.id] = b;
	return fitness(besta, a.id) > fitness(bestb, b.id);
}

void read_data(string file) {
	ifstream data(file);
	data >> t >> r >> s >> zz;
	for(int i = 0; i < t; i++) {
		data >> dur[i];
		maxdur = max(maxdur, dur[i]);
	}
	int n_edge, a, b;
	data >> n_edge;
	for(int i = 0; i < n_edge; i++) {
		data >> a >> b;
		a--; b--;
		adj_f[a].pb(b);
		adj_b[b].pb(a);
	}

	for(int i = 0; i < t; i++) {
		for(int j = 0; j < s; j++) {
			data >> treq[i][j];
		}
	}
	float max_lexp[S];
	for(int j = 0; j < s; j++) max_lexp[j] = 0;
	for(int i = 0; i < r; i++) {
		for(int j = 0; j < s; j++) {
			data >> lexp[i][j];
			max_lexp[j] = max(lexp[i][j], max_lexp[j]);
		}
	}

	for(int i = 0; i < r; i++) {
		for(int j = 0; j < s; j++) {
			lexp[i][j] /= max_lexp[j];
		}
	}

}

int main(int argc, char ** argv) {
	read_data(argv[1]);
	vector<Indi> pop_(pop_size);
	vector<vector<Task> > pop(t);
	//khởi tạo lần lượt các quần thể P tương ứng vs các task (mỗi quần thể gồm 20 cá thể)
	for(int i = 0; i < t; i++) {
		for(int j = 0; j < pop_size; j++) {
			pop[i].pb(pop_[j].task[i]);
		}
	}
	// Khởi tạo trạng thái S cần tìm, 
	for(int i = 0; i < t; i++) {
		best.task[i] = pop[i][rand()%pop_size];
	}
	
	for(int e = 0; e < 1000; e++) {

		// TIM TRANG THAI NASH, GAN NO CHO CA THE BEST

			float fp = 0;
			float fc = -1;
			cout << "FITNESS KET QUA BAI TOAN: \n";
			for(int _ = 0; _ < 10; _++) {
				fp = fitness(best);
				for(int i = 0; i < t; i++) {
					sort(pop[i].begin(), pop[i].end(), cmp);
				}

				for(int i = 0; i < t; i++) {
					best.task[i] = pop[i][0];
				}
				fc = fitness(best);
				if (fc == fp) break;
				
				cout << _ << "\t" << fc << endl;
			}

		// TIEN HOA QUAN THE, MO RONG QUAN THE LEN expand_pz CA THE, mở rộng thêm 40 
		for(int i = 0; i < t; i++) {
			while(pop[i].size() < expand_pz) {
				if (random_() < 0.5) { // DOT BIEN
					int p = rand() % pop_size;
					pop[i].pb(mutate(pop[i][p]));
				}
				else { //LAI GHEP
					int p1 = rand() % pop_size;
					int p2 = rand() % pop_size;
					pop[i].pb(cross(pop[i][p1], pop[i][p2]));
				}
			}
		}

		// CHON LOC, CHON pop_size CA THE TOT NHAT TU expand_pz CA THE
		for(int i = 0; i < t; i++) {
			sort(pop[i].begin(), pop[i].end(), cmp);
			while(pop[i].size() > pop_size) pop[i].pop_back();
		}

		// CAP NHAT LAI CA THE BEST
		for(int i = 0; i < t; i++) {
			best.task[i] = pop[i][0];
		}
		// IN KET QUA
		cout << e << "\t" << fitness(best) << endl;
		if (e%10 == 0) {
			cout << "****************************************" << endl;
			cout << "FITNESS : \n";
			for(int i = 0; i < t; i++) {
				printf("%d\t%6.5f\t%6.5f\t%6.5f\n", i + 1, duration(best, i), experience(best, i), assignment(best, i) );
			}
			best.print();
		}
		cout << endl;
	}
	return 0;
}
