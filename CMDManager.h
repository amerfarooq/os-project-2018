#pragma once

#include "util.h"
#include "Scheduler.h"

using namespace std;

class CMDManager {

	public:
		void insert(string);
		void printCommands();
		void PRT();

	private:
		queue<string> commands;
		void processCommand(string);

		void UNB();
		void INC();
		void CRT(string);

} cmdManager;

void CMDManager::insert(string a){
	commands.push(a);
	processCommand(a);
}

void CMDManager::printCommands() {
	queue<string> temp = commands;

	while(!temp.empty()) {
		cout << temp.front();
		temp.pop();
	}
}

void CMDManager::processCommand(string cmd){
	if (cmd.length() == 3) {
		if (cmd == "UNB") UNB();
		else if (cmd == "INC") INC();
		else if (cmd == "PRT") PRT();
		else goto errMSG;
		return;
	}
	else if (cmd.length() > 3)
	{
		if (cmd.substr(0,4) == "CRT ") {
			CRT(cmd.substr(cmd.find(" ") + 1));
			return;
		}
		else goto errMSG;
	}
	errMSG:
		cout << "Invalid Command!" << endl << endl;
}

void CMDManager::UNB() {
	scheduler.unblock();
}

void CMDManager::INC() {
	++globalTime;
	scheduler.run();
}

void CMDManager::PRT() {
	scheduler.spawnBroadcaster();
}

void CMDManager::CRT(string fileName) {
	ifstream instrFile(fileName);
	if (!instrFile) {
		cout << fileName << " failed to open!" << endl << endl;
		return;
	}

	PCB* newProc = new PCB;

	newProc->timeAdmitted = globalTime;
	newProc->pid = pidAssigner;
	++pidAssigner;

	insertInstructions(newProc, instrFile);
	scheduler.addReadyProc(newProc);
}
