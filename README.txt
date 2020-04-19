Thuật toán tabu search cho bài toán tối ưu hóa phân bổ nguồn lực

1. Mô hình bài toán
Một dự án phần mềm cơ bản gồm các thông số sau 
    • Có m Tasks, biểu diễn bới tập T = {t1­,t2,..,tm}. Task thứ i kéo dài trong ti đơn vị thời gian. được lưu vào mảng fdur.
    • Có n Resources (tài nguyên của dự án là các lập trình viên), biểu diễn bới tập R = {r1­,r2,..,rn}
    • Có p loại skills (kĩ năng) cần xem xét để thực hiện các task của dự án, biểu diễn bới tập S = {s1­,s2,..,sp}

Các tasks có mối quan hệ phụ thuộc là một đồ thị có hướng không chu trình, biểu diễn bởi tập cạnh E = { (a, bj) | (1 ≤ a < b ≤ m) }, mỗi cặp (a, b) cho biết task a cần hoàn thành trước khi task b bắt đầu thực hiện để đảm bảo chất lượng của task b.
Mỗi task cần có những skill để thực hiện, được biểu diễn bởi ma trận TREQ kích thước mxq chỉ gồm các giá trị 0, 1, giá trị TREQ[i][k] = 1 thể hiện T[i] cần skill S[k] để thực hiện và ngược lại.
Mỗi resource có một mức độ kinh nghiệm (experience) với từng skill, biểu diễn bới ma LEXP kích thước mxq, giá trị LEXP[j][k] của phần tử ở hàng j cột k là số thực từ 0 đến 1, thể hiện mức độ kinh nghiệm của lập trình viên thứ j (R[j]) đối với skill S[k], , trong đó 0  nghĩa là lập trình viên thứ R[j] không có kinh nghiệm đối với skill S[k], còn 1 nghĩa là lập trình viên thứ R[j] có kinh nghiệm rất tốt với skill S[k].

Mục tiêu của bài toán là tìm ra một dãy thời điểm thực hiện các task (lập lịch), và một cách phân bổ các resource cho các task (phân bổ nguồn lực).

Mỗi phương án được coi là nghiệm x cần tìm bao gồm 2 thành phần
	• Thông tin về tiến độ thực hiện của mỗi task, biểu diễn bởi 1 vector sched m chiều, trong đó sched[i] là giá trị thời gian cho biết thời điểm lập lịch bắt đầu task thứ i.
	• Thông tin về việc phân bổ các resource cho các task, được biểu diễn bởi ma trận assign có kích thước mxn gồm các giá trị 0 và 1, trong đó assign[i][j] bằng 1 thể hiện sự lựa chọn của task thứ i đối với resource j, và bằng 0 thể hiện task i không lựa chọn resource.

3.1 Khởi tạo cá thể quần thể. gồm 2 thành phần
- sched
	- duyệt đồ thị 
	• nếu không có cung (s, i) trong đồ thị, nghĩa là task i không cần đợi task nào hoàn thành trước ta gán sched[i] = random() * (dur[i] + maxdur) / 8
	• ngược lại nếu có cung (u, v) ta gán sched[v] = max(sched[v], (sched[u] + dur[u] + (random() - 0.5) * (dur[v] + maxdur ) / 4);
- assign
	- khởi tạo ngẫu nhiên với xác suất 0.5

3.2 Đột biến
- sched : 
for i = 1 to m:
    if random < 0.1 :
	float dt = random_() - 0.5;
        cập nhật neighbor.sched[j] = sched[j] + dt * (dur[j] + maxdur) / 4 với j là tất cả các task cần thực hiện sau task i.
    endif
endfor
- assign :
	• đột biến đảo giá trị của assign[i][j] với xác suất 0.05
	• lai hóa, di chuyển việc làm của resource j từ task i1 sang task i2. nghĩa là hoán vị hai giá trị assign[i1][j] và assign[i2][j] với xác suất 0.15
3.3 Lai hóa
- sched :
    chọn một điểm bất kỳ, lai hóa trên toàn bộ cây con với gốc đó
- assign :	

3.4 Xác định 2 cá thể giống nhau (để lọai trừ khỏi tabu list)
2 cá thể gọi là giống nhau nếu 2 điều khiện sau xảy ra :
	- giá trị sched hơn kém nhau không quá 20%
	- các giá trị assign giống nhau hơn 83%

# Note : các giá trị ngưỡng trong bài toán được tìm ra trong quá trình test

4. Chương trình
Ngôn ngữ C++14
run command : g++ -std=c++14 tabu_search-o run.cpp  && ./run data.txt
run command : g++ -std=c++14 nash_ga1.cpp -o run && ./run data.txt
Thư mục bao gồm:
	data.txt
	tabu_search.cpp : 
	nash_ga1.cpp : implementation thuật toán Nash GA, mỗi player là một task
	nash_ga2.cpp : //
	indi.cpp : class cá thể, sử dụng cho tabu_seach và nash_ga1
input file data.txt bao gồm
- dòng thứ nhất gồm 4 số nguyên m, n, p. z là số lượng tasks, resources, skills, thời gian dự kiến phải hoàn thành của dự án
- dòng thứ 2 gồm m số thực, giá trị dur[i] thể hiện thời gian để hoàn thành từng task
- dòng thứ 3 gồm 1 số nguyên ne, là số lượng các cạnh trong đồ thị
- ne dòng tiếp theo, mỗi dòng gồm 2 số nguyên a, b. Cung (a, b) thể hiện task b cần thực hiện sau task a để đạt được hiệu quả tốt.
- m dòng tiếp theo, mỗi dòng gồm p số nguyên (0 hoặc 1) biểu diễn ma trận TREQ
- n dòng tiếp theo, mỗi dòng gồm p số thực, biểu diễn ma trận LEXP



	

