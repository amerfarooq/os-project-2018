#pragma once

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <string>
#include <iostream>
#include <vector>
#include <assert.h>
#include <queue>
#include <fstream>

using namespace std;

int globalTime = 0;
int pidAssigner = 0;
bool blockFlag;
bool endFlag;

enum ProcessState { READY, RUNNING, BLOCKED };
enum InstrType { ARITHMETIC, REPLACE, CMD };
enum ArithmeticOp { ADD, SUBTRACT, SET };

class Instruction{

	protected:
		InstrType type;

	public:
		InstrType getInstrType() {
			return type;
		}
};

class ArithmeticInstr : public Instruction {

	private:
		int num;
		ArithmeticOp op;

	public:
		int getNum() {
			return num;
		}

		ArithmeticInstr(int n, ArithmeticOp opr) {
			num = n;
			type = ARITHMETIC;
			op = opr;
		}

		ArithmeticOp getOp() {
			return op;
		}
};

class ReplaceInstr : public Instruction {

	private:
		string fileName;

	public:
		string getFileName() {
			return fileName;
		}
		ReplaceInstr(string file) {
			fileName = file;
			type = REPLACE;
		}
};

class CMDInstr : public Instruction {

	private:
		string cmd;

	public:
		string getCMD() {
			return cmd;
		}
		CMDInstr(string c) {
			cmd = c;
			type = CMD;
		}
};

struct PCB {
	vector<Instruction*> instr;
	int pid;
	int pc;
	int timeAdmitted;
	int serviceTime;
	int	val;

	PCB() {
		pid = -1;
		pc = 0;
		timeAdmitted = -1;
		serviceTime = 0;
		val = 0;
	}

	void printInstr() {
		for (int i = 0; i < instr.size(); ++i) {
			InstrType type = (*instr[i]).getInstrType();

			if (type == CMD) {
				cout << "CMD: ";
				cout << static_cast<CMDInstr*>(instr[i])->getCMD() << endl;
			}
			else if (type == ARITHMETIC) {
				auto inst = static_cast<ArithmeticInstr*>(instr[i]);
				auto op = inst->getOp();
				int num = inst->getNum();

				string temp;
				switch(op) {
					case ADD:
						temp = "Add ";
						break;
					case SUBTRACT:
						temp = "Subtract ";
						break;
					case SET:
						temp = "Set ";
						break;
				}
				temp += to_string(num);
				cout << "ARITHMETIC: " << temp << endl;
			}
			else {
				cout << "RENAME: " << static_cast<ReplaceInstr*>(instr[i])->getFileName() << endl;
			}
		}
	}
	void printProcInfo() {
		cout << "PID: " << pid << ", Start Time: " << timeAdmitted
			 << ", CPU Time: " << serviceTime
			 << ", Val: " << val << endl;
	}
	void deleteInstrs() {
		for (auto it = instr.begin(); it != instr.end(); ++it)
			delete (*it);

		instr.clear();
	}

	~PCB() {
		deleteInstrs();
	}
};

void insertInstructions(PCB* proc, ifstream& instrFile) {
	auto instrArr = &proc->instr;

	string instr;
	while(getline(instrFile, instr))
	{
		if (instr.length() == 1)
		{
			if (instr == "B")
				(*instrArr).push_back(new CMDInstr(instr));
			else if (instr == "E")
				(*instrArr).push_back(new CMDInstr(instr));
		}
		else {
			if (instr[0] == 'S') {
				int num = stoi(instr.substr(instr.find(" ") + 1));
				(*instrArr).push_back(new ArithmeticInstr(num, SET));
			}
			else if (instr[0] == 'A') {
				int num = stoi(instr.substr(instr.find(" ") + 1));
				(*instrArr).push_back(new ArithmeticInstr(num, ADD));
			}
			else if (instr[0] == 'D') {
				int num = stoi(instr.substr(instr.find(" ") + 1));
				(*instrArr).push_back(new ArithmeticInstr(num, SUBTRACT));
			}
			else if (instr[0] == 'R') {
				string fileN = instr.substr(instr.find(" ") + 1);
				(*instrArr).push_back(new ReplaceInstr(fileN));
			}
			else {
				cout << "Invalid Instruction encountered whilst"
					 << " reading from file " << endl;
			}
		}
	}
}
