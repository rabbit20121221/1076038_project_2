#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#pragma warning (disable:4996)
using namespace std;

// 功能實作
void algorithm();
char show_op(int inst_type);
char readin_file[20];
ofstream outfile;
char outfile_name[40] = "output";

// 控制字元
char add_op[] = "ADD";
char sub_op[] = "SUB";
char mul_op[] = "MUL";
char div_op[] = "DIV";
char addi_op[] = "ADDI";

// 數量宣告
const int readline = 10;
const int total_rf = 9;
const int total_rat = 9;
const int rsADD = 3;
const int rsMUL = 2;

// 其他
int line_id = 1;
int input_cycle = 1;
int inst_now = 0;
int register_used = 0;
int rs_space_add = rsADD;
int rs_space_mul = rsMUL;

struct input_file_line {
	int inst_type = 0;    // add, sub, mul, div, addi
	char out_reg[5];
	char in_reg_1[5];
	char in_reg_2[5];
	int out_reg_id = 0;
	int in_reg_1_id = 0;
	int in_reg_2_id = 0;
	int issue_cycle = 0;
	int dispatch = 0;
}input_file_line[readline];

struct rs_vec {
	int state = 0;
	int inst_id = 0;
	int inst_type = 0;
	int source_a = 0;
	int source_b = 0;
	int write_cycle = 0;
	int in_rat_id = 0;
	int ready = 0;
}RS[rsADD + rsMUL + 1];    // 1~3 add,sub .. 4~5 mul,div

struct buffer_1_vector {
	int state = 0;
	int rs_id = 0;
	int inst_type = 0;
	int source_a = 0;
	int source_b = 0;
}buffer_1;

struct buffer_2_vector {
	int state = 0;
	int rs_id = 0;    // for rs
	int inst_type = 0;
	int source_a = 0;
	int source_b = 0;
}buffer_2;

// table array
int RAT_F[total_rat + 1] = { 0 };
int RF_F[total_rf + 1] = { 0 };


int main(int argc, char *argv[]) {
	cout << "輸入檔名: ";
	cin >> readin_file;

	ifstream infile(readin_file);
	// 初始化
	for (int i = 1, j = 0; i < total_rf + 1; i++) {
		RF_F[i] = j;
		j += 2;
	}

	char temp01[10], temp02[5], temp03[5], temp04[5];
	string get_file_line;
	while (getline(infile, get_file_line)) {
		istringstream iss(get_file_line);
		iss >> temp01 >> temp02 >> temp03 >> temp04;
		cout << "讀檔完成~" << endl;
		if (!strcmp(temp01, add_op))
			input_file_line[line_id].inst_type = 1;
		else if (!strcmp(temp01, sub_op))
			input_file_line[line_id].inst_type = 2;
		else if (!strcmp(temp01, mul_op))
			input_file_line[line_id].inst_type = 3;
		else if (!strcmp(temp01, div_op))
			input_file_line[line_id].inst_type = 4;
		else if (!strcmp(temp01, addi_op))
			input_file_line[line_id].inst_type = 5;
		else {
			cout << "檔案內容有錯" << endl;
			system("pause");
			exit(0);
		}
		strcpy(input_file_line[line_id].out_reg, strtok(temp02, ","));
		strcpy(input_file_line[line_id].in_reg_1, strtok(temp03, ","));
		strcpy(input_file_line[line_id].in_reg_2, temp04);
		char temp_variable[5];
		strncpy(temp_variable, input_file_line[line_id].out_reg + 1, sizeof(input_file_line[line_id].out_reg));
		input_file_line[line_id].out_reg_id = atoi(temp_variable);
		memset(temp_variable, 0, sizeof(temp_variable));
		if (input_file_line[line_id].out_reg_id > register_used)
			register_used = input_file_line[line_id].out_reg_id;
		strncpy(temp_variable, input_file_line[line_id].in_reg_1 + 1, sizeof(input_file_line[line_id].in_reg_1));
		input_file_line[line_id].in_reg_1_id = atoi(temp_variable);
		memset(temp_variable, 0, sizeof(temp_variable));
		if (input_file_line[line_id].in_reg_1_id > register_used)
			register_used = input_file_line[line_id].in_reg_1_id;
		if (input_file_line[line_id].inst_type == 5) {
			strncpy(temp_variable, input_file_line[line_id].in_reg_2, sizeof(input_file_line[line_id].in_reg_2));
			input_file_line[line_id].in_reg_2_id = atoi(temp_variable);
		}
		else {
			strncpy(temp_variable, input_file_line[line_id].in_reg_2 + 1, sizeof(input_file_line[line_id].in_reg_2));
			input_file_line[line_id].in_reg_2_id = atoi(temp_variable);
			if (input_file_line[line_id].in_reg_2_id > register_used)
				register_used = input_file_line[line_id].in_reg_2_id;
		}
		memset(temp_variable, 0, sizeof(temp_variable));
		line_id++;
	}
	infile.close();
	cout << endl;
	line_id -= 1;
	cout << "\nCycle? ";
	cin >> input_cycle;
	cout << endl;
	if (input_cycle == 0) {
		
		char *str_temp = (char*)malloc(5 * sizeof(char));
		sprintf(str_temp, "%d", input_cycle);
		strcat(outfile_name, "_");
		strcat(outfile_name, str_temp);
		strcat(outfile_name, ".txt");
		outfile.open(outfile_name);
		outfile << "Cycle " << input_cycle << endl << endl;
		outfile << "RF: " << endl;
		for (int i = 1; i < register_used + 1; i++) {
			outfile << "F" << i << ": " << RF_F[i] << " ";
		}
		outfile << endl;
		outfile << "RAT: " << endl;
		for (int i = 1; i < register_used + 1; i++) {
			if (RAT_F[i] != 0) {
				outfile << "F" << i << ": " << "RS" << RAT_F[i] << " ";
			}
			else {
				outfile << "F" << i << ": " << " ";
			}
		}
		outfile << endl;
		outfile << "RS: ";
		for (int i = 1; i < rsADD + 1; i++) {
			if (RS[i].inst_type != 0) {
				outfile << " RS" << i << ": " << show_op(RS[i].inst_type) << " ";
				if (RS[i].source_a % 2999 == 0 && RS[i].source_a != 0) {
					outfile << " RS" << RS[i].source_a / 2999;
				}
				else {
					outfile << RS[i].source_a;
				}
				if (RS[i].source_b % 2999 == 0 && RS[i].source_b != 0) {
					outfile << " RS" << RS[i].source_b / 2999;
				}
				else {
					outfile << " " << RS[i].source_b;
				}
				outfile << " " << " ";
			}
			else {
				outfile << " RS" << i << " ";
				outfile << " ";
			}
		}
		if (buffer_1.state == 1) {
			outfile << "\nBuffer: (RS" << buffer_1.rs_id << ") " << buffer_1.source_a << " " << show_op(RS[buffer_1.rs_id].inst_type) << " " << buffer_1.source_b << endl << endl;
		}
		else {
			outfile << "\nBuffer: X" << endl;
		}
		for (int i = rsADD + 1; i < rsADD + rsMUL + 1; i++) {
			if (RS[i].inst_type != 0) {
				outfile << " RS" << i << ": " << show_op(RS[i].inst_type) << " ";
				if (RS[i].source_a % 2999 == 0 && RS[i].source_a != 0) {
					outfile << " RS" << RS[i].source_a / 2999;
				}
				else {
					outfile << RS[i].source_a;
				}
				if (RS[i].source_b % 2999 == 0 && RS[i].source_b != 0) {
					outfile << " RS" << RS[i].source_b / 2999;
				}
				else {
					outfile << " " << RS[i].source_b;
				}
				outfile << " ";
			}
			else {
				outfile << "RS" << i << ": ";
				outfile << " ";
			}
		}
		if (buffer_2.state == 1) {
			outfile << "\nBuffer: (RS" << buffer_2.rs_id << ") " << buffer_2.source_a << " " << show_op(RS[buffer_2.rs_id].inst_type) << " " << buffer_2.source_b << endl << endl;
		}
		else {
			outfile << "\nBuffer: X" << endl;
		}
		system("pause");
		exit(0);
	}
	else {
		algorithm();
	}
	char filename_output[40] = "output";
	char *str_temp = (char*)malloc(5 * sizeof(char));
	sprintf(str_temp, "%d", input_cycle);
	strcat(filename_output, "_");
	strcat(filename_output, str_temp);
	strcat(filename_output, ".txt");
	outfile.open(filename_output);
	outfile << "Cycle: " << input_cycle << endl << endl;
	outfile << "RF: " << endl;
	for (int i = 1; i < register_used + 1; i++) {
		outfile << "F" << i << ": " << RF_F[i] << " ";
	}
	outfile << endl;
	outfile << "RAT: " << endl;
	for (int i = 1; i < register_used + 1; i++) {
		if (RAT_F[i] != 0) {
			outfile << "F" << i << ": " << "RS" << RAT_F[i] << " ";
		}
		else {
			outfile << "F" << i << ": " << " ";
		}
	}
	outfile << endl;
	outfile << "RS: ";
	for (int i = 1; i < rsADD + 1; i++) {
		if (RS[i].inst_type != 0) {
			outfile << " RS" << i << ": " << show_op(RS[i].inst_type) << " ";
			if (RS[i].source_a % 2999 == 0 && RS[i].source_a != 0) {
				outfile << " RS" << RS[i].source_a / 2999;
			}
			else {
				outfile << RS[i].source_a;
			}
			if (RS[i].source_b % 2999 == 0 && RS[i].source_b != 0) {
				outfile << " RS" << RS[i].source_b / 2999;
			}
			else {
				outfile << " " << RS[i].source_b;
			}
			outfile << " " << " ";
		}
		else {
			outfile << " RS" << i << " ";
			outfile << " ";
		}
	}
	if (buffer_1.state == 1) {
		outfile << "\nBuffer: (RS" << buffer_1.rs_id << ") " << buffer_1.source_a << " " << show_op(RS[buffer_1.rs_id].inst_type) << " " << buffer_1.source_b << endl << endl;
	}
	else {
		outfile << "\nBuffer: X" << endl;
	}
	for (int i = rsADD + 1; i < rsADD + rsMUL + 1; i++) {
		if (RS[i].inst_type != 0) {
			outfile << " RS" << i << ": " << show_op(RS[i].inst_type) << " ";
			if (RS[i].source_a % 2999 == 0 && RS[i].source_a != 0) {
				outfile << " RS" << RS[i].source_a / 2999;
			}
			else {
				outfile << RS[i].source_a;
			}
			if (RS[i].source_b % 2999 == 0 && RS[i].source_b != 0) {
				outfile << " RS" << RS[i].source_b / 2999;
			}
			else {
				outfile << " " << RS[i].source_b;
			}
			outfile << " ";
		}
		else {
			outfile << "RS" << i << ": ";
			outfile << " ";
		}
	}
	if (buffer_2.state == 1) {
		outfile << "\nBuffer: (RS" << buffer_2.rs_id << ") " << buffer_2.source_a << " " << show_op(RS[buffer_2.rs_id].inst_type) << " " << buffer_2.source_b << endl << endl;
	}
	else {
		outfile << "\nBuffer: X" << endl;
	}
	cout << "執行完成，請參閱 " << outfile_name << endl;
	system("pause");
	return 0;
}


int calculate(int source_a, int source_b, int inst_type) {
	int result = 0;
	if (inst_type == 1 || inst_type == 5)
		result = source_a + source_b;
	else if (inst_type == 2)
		result = source_a - source_b;
	else if (inst_type == 3)
		result = source_a * source_b;
	else if (inst_type == 4)
		result = source_a / source_b;
	else
		result = 0;
	return result;
}

void algorithm() {
	for (int i = 0; i < input_cycle; i++) {
		// 確認 buffer
		if (buffer_1.state == 1) {
			// 確認 WR 與 cycle
			if (i == RS[buffer_1.rs_id].write_cycle) {
				if (input_file_line[RS[buffer_1.rs_id].inst_id + 1].out_reg_id == RS[buffer_1.rs_id].in_rat_id && (input_file_line[RS[buffer_1.rs_id].inst_id + 1].in_reg_1_id != RS[buffer_1.rs_id].in_rat_id && input_file_line[RS[buffer_1.rs_id].inst_id + 1].in_reg_2_id != RS[buffer_1.rs_id].in_rat_id)) {
					// 丟棄
				}
				else {
					if (buffer_1.rs_id == RAT_F[RS[buffer_1.rs_id].in_rat_id]) {
						// 更新
						RAT_F[RS[buffer_1.rs_id].in_rat_id] = 0;
					}
					// 更新 RF	
					RF_F[RS[buffer_1.rs_id].in_rat_id] = calculate(buffer_1.source_a, buffer_1.source_b, buffer_1.inst_type);
				}
				// 更新 AT in RS table
				for (int j = 1; j < rsADD + rsMUL + 1; j++) {
					if (RS[j].source_a % 2999 == 0 && RS[j].source_a / 2999 == buffer_1.rs_id)
						RS[j].source_a = RF_F[RS[buffer_1.rs_id].in_rat_id];
					if (RS[j].source_b % 2999 == 0 && RS[j].source_b / 2999 == buffer_1.rs_id)
						RS[j].source_b = RF_F[RS[buffer_1.rs_id].in_rat_id];
				}
				// 清空 RS table
				RS[buffer_1.rs_id].in_rat_id = 0;
				RS[buffer_1.rs_id].inst_type = 0;
				RS[buffer_1.rs_id].inst_id = 0;
				RS[buffer_1.rs_id].source_a = 0;
				RS[buffer_1.rs_id].source_b = 0;
				RS[buffer_1.rs_id].state = 0;
				RS[buffer_1.rs_id].write_cycle = 0;
				RS[buffer_1.rs_id].ready = 0;
				rs_space_add += 1;
				// 清空暫存器
				buffer_1.inst_type = 0;
				buffer_1.rs_id = 0;
				buffer_1.source_a = 0;
				buffer_1.source_b = 0;
				buffer_1.state = 0;
			}
		}
		if (buffer_1.state == 0) {
			for (int j = 1; j < rsADD + 1; j++) {
				if (RS[j].state == 1 && RS[j].ready == 1) {
					buffer_1.state = 1;
					buffer_1.rs_id = j;
					buffer_1.inst_type = RS[j].inst_type;
					buffer_1.source_a = RS[j].source_a;
					buffer_1.source_b = RS[j].source_b;
					input_file_line[RS[j].inst_id].dispatch = i;
					if (RS[j].inst_type == 1)
						RS[j].write_cycle = i + 2;
					else if (RS[j].inst_type == 2)
						RS[j].write_cycle = i + 2;
					else if (RS[j].inst_type == 5)
						RS[j].write_cycle = i + 2;
					break;
				}
			}
		}
		if (buffer_2.state == 1) {
			if (i == RS[buffer_2.rs_id].write_cycle) {
				if (input_file_line[RS[buffer_2.rs_id].inst_id + 1].out_reg_id == RS[buffer_2.rs_id].in_rat_id
					&& (input_file_line[RS[buffer_2.rs_id].inst_id + 1].in_reg_1_id != RS[buffer_2.rs_id].in_rat_id
						&& input_file_line[RS[buffer_2.rs_id].inst_id + 1].in_reg_2_id != RS[buffer_2.rs_id].in_rat_id)) {
				}
				else {
					if (buffer_2.rs_id == RAT_F[RS[buffer_2.rs_id].in_rat_id]) {
						// 更新 RAT
						RAT_F[RS[buffer_2.rs_id].in_rat_id] = 0;
					}
					// 更新 RF
					RF_F[RS[buffer_2.rs_id].in_rat_id] = calculate(buffer_2.source_a, buffer_2.source_b, buffer_2.inst_type);
				}
				// 更新 RS table
				for (int j = 1; j < rsADD + rsMUL + 1; j++) {
					if (RS[j].source_a % 2999 == 0 && RS[j].source_a / 2999 == buffer_2.rs_id)
						RS[j].source_a = RF_F[RS[buffer_2.rs_id].in_rat_id];
					if (RS[j].source_b % 2999 == 0 && RS[j].source_b / 2999 == buffer_2.rs_id)
						RS[j].source_b = RF_F[RS[buffer_2.rs_id].in_rat_id];
				}
				RS[buffer_2.rs_id].in_rat_id = 0;
				RS[buffer_2.rs_id].inst_type = 0;
				RS[buffer_2.rs_id].inst_id = 0;
				RS[buffer_2.rs_id].source_a = 0;
				RS[buffer_2.rs_id].source_b = 0;
				RS[buffer_2.rs_id].state = 0;
				RS[buffer_2.rs_id].write_cycle = 0;
				RS[buffer_2.rs_id].ready = 0;
				rs_space_mul += 1;
				buffer_2.inst_type = 0;
				buffer_2.rs_id = 0;
				buffer_2.source_a = 0;
				buffer_2.source_b = 0;
				buffer_2.state = 0;
			}
		}
		// 修改 buffer
		if (buffer_2.state == 0) {
			for (int j = rsADD + 1; j < rsADD + rsMUL + 1; j++) {
				if (RS[j].state == 1 && RS[j].ready == 1) {
					buffer_2.state = 1;
					buffer_2.rs_id = j;
					buffer_2.inst_type = RS[j].inst_type;
					buffer_2.source_a = RS[j].source_a;
					buffer_2.source_b = RS[j].source_b;
					input_file_line[RS[j].inst_id].dispatch = i;
					if (RS[j].inst_type == 3) {
						RS[j].write_cycle = i + 10;
					}
					else if (RS[j].inst_type == 4) {
						RS[j].write_cycle = i + 40;
					}
					break;
				}
			}
		}
		// issue
		if ((input_file_line[inst_now + 1].inst_type == 1 || input_file_line[inst_now + 1].inst_type == 2
			|| input_file_line[inst_now + 1].inst_type == 5) && rs_space_add != 0) {
			for (int j = 1; j < rsADD + 1; j++) {
				if (RS[j].state == 0) {
					inst_now += 1;
					RS[j].state = 1;
					RS[j].inst_id = inst_now;
					RS[j].inst_type = input_file_line[inst_now].inst_type;
					RS[j].in_rat_id = input_file_line[inst_now].out_reg_id;
					if (RAT_F[input_file_line[inst_now].in_reg_1_id] == 0) {
						RS[j].source_a = RF_F[input_file_line[inst_now].in_reg_1_id];
					}
					else {
						RS[j].source_a = RAT_F[input_file_line[inst_now].in_reg_1_id] * 2999;
					}
					if (RS[j].inst_type == 5) {
						RS[j].source_b = input_file_line[inst_now].in_reg_2_id;
					}
					else if (RAT_F[input_file_line[inst_now].in_reg_2_id] == 0) {
						RS[j].source_b = RF_F[input_file_line[inst_now].in_reg_2_id];
					}
					else {
						RS[j].source_b = RAT_F[input_file_line[inst_now].in_reg_2_id] * 2999;
					}
					RAT_F[input_file_line[inst_now].out_reg_id] = j;
					rs_space_add -= 1;
					break;
				}
			}
		}
		else if ((input_file_line[inst_now + 1].inst_type == 3 || input_file_line[inst_now + 1].inst_type == 4) && rs_space_mul != 0) {
			for (int j = rsADD + 1; j < rsADD + rsMUL + 1; j++) {
				if (RS[j].state == 0) {
					inst_now += 1;
					RS[j].state = 1;
					RS[j].inst_id = inst_now;
					RS[j].inst_type = input_file_line[inst_now].inst_type;
					RS[j].in_rat_id = input_file_line[inst_now].out_reg_id;
					if (RAT_F[input_file_line[inst_now].in_reg_1_id] == 0) {
						RS[j].source_a = RF_F[input_file_line[inst_now].in_reg_1_id];
					}
					else {
						RS[j].source_a = RAT_F[input_file_line[inst_now].in_reg_1_id] * 2999;
					}
					if (RAT_F[input_file_line[inst_now].in_reg_2_id] == 0) {
						RS[j].source_b = RF_F[input_file_line[inst_now].in_reg_2_id];
					}
					else {
						RS[j].source_b = RAT_F[input_file_line[inst_now].in_reg_2_id] * 2999;
					}
					RAT_F[input_file_line[inst_now].out_reg_id] = j;
					rs_space_mul -= 1;
					break;
				}
			}
		}
		// 更新 ready state
		for (int j = 1; j < rsADD + rsMUL + 1; j++) {
			if ((RS[j].source_a % 2999 != 0 && RS[j].source_a % 2999 != 2999) || RS[j].source_a == 0) {
				if ((RS[j].source_b % 2999 != 0 && RS[j].source_b % 2999 != 2999) || RS[j].source_b == 0) {
					RS[j].ready = 1;
				}
				else {
					RS[j].ready = 0;
				}
			}
		}
		// 檢查 cycle 是否完成
		bool complete = 0;
		if (inst_now == line_id) {
			for (int j = 1; j < total_rat + 1; j++) {
				if (RAT_F[j] != 0) {
					complete = 1;
				}
			}
			if (complete == 0) {
				cout << "\nFinal cycle: " << i << endl << endl;
				input_cycle = i;

				for (int k = 1; k < rsADD + rsMUL + 1; k++) {
					RS[k].inst_type = 0;
					RS[k].in_rat_id = 0;
					RS[k].inst_id = 0;
					RS[k].source_a = 0;
					RS[k].source_b = 0;
					RS[k].state = 0;
					RS[k].write_cycle = 0;
					RS[k].ready = 0;
				}
				buffer_1.inst_type = 0;
				buffer_1.rs_id = 0;
				buffer_1.source_a = 0;
				buffer_1.source_b = 0;
				buffer_1.state = 0;
				buffer_2.inst_type = 0;
				buffer_2.rs_id = 0;
				buffer_2.source_a = 0;
				buffer_2.source_b = 0;
				buffer_2.state = 0;
				break;
			}
		}
	}
}

char show_op(int inst_type) {
	if (inst_type == 0)
		return ' ';
	else if (inst_type == 1 || inst_type == 5)
		return '+';
	else if (inst_type == 2)
		return '-';
	else if (inst_type == 3)
		return '*';
	else if (inst_type == 4)
		return '/';
	else
		return 0;
}
