#pragma once

#include "util.h"
#include "CPU.h"
#include <deque>

class Scheduler {

    public:
        void addReadyProc(PCB* proc) {
            // Asssuming multiple instances of same process can exist
            ready.push_back(proc);
        }

        int getQuantum() const { return this->quantum; }

        void dispatch() {
            cpu.admitProcess(ready.front());
            ready.pop_front();
        }

        void run() {
        	if (cpu.empty()) {
                if (!ready.empty()) {
                    dispatch();
                }
                else return;
            }

        	cpu.run();

            if (blockFlag) {
                contextSwitch(BLOCKED);
                blockFlag = false;
                tracker = 0;
                return;
            }

            if (endFlag) {
                if (!ready.empty()) dispatch();
                tracker = 0;
                endFlag = false;
                return;
            }

            ++tracker;
            if (tracker == quantum) {
                contextSwitch(READY);
                tracker = 0;
            }
        }

        void unblock() {
            if (blocked.empty()) return;

            auto temp = blocked.front();
            ready.push_back(temp);
            blocked.pop_front();
        }

        void spawnBroadcaster() {
            if (!fork()) {	// Broadcast Process

                cout << "-----------------------------------------------------" << endl << endl;

        		cout  << "Current system state:" << endl
        			  << "***********************" << endl
        			  << "Current Time: " << globalTime << endl << endl;

        		cout << "|---------------------|" << endl;
        		cout << "|   Running Process   |" << endl;
        		cout << "|---------------------|" << endl;
        		auto currentProc = cpu.getProc();
        		if (currentProc) currentProc->printProcInfo();
        		cout << endl;

        		cout << "|---------------------|" << endl;
        		cout << "|  Blocked Processes  |" << endl;
        		cout << "|---------------------|" << endl;
                printProcesses(BLOCKED);

        		cout << "|---------------------|" << endl;
        		cout << "|   Ready Processes   |" << endl;
        		cout << "|---------------------|" << endl;
        		printProcesses(READY);

                cout << "-----------------------------------------------------" << endl << endl;
        }
    }

    private:
        deque<PCB*> ready;
        deque<PCB*> blocked;
        int quantum = 3;
        int tracker = 0;

        void contextSwitch(ProcessState qType) {
            auto prevProc = cpu.extractProc();

            if (qType == READY)
                ready.push_back(prevProc);
            else if (qType == BLOCKED)
                blocked.push_back(prevProc);

            if (!ready.empty()) dispatch();
        }

        void printProcesses(ProcessState queue) {
            deque<PCB*>* qPtr;

            if (queue == READY) qPtr = &ready;
            else if (queue == BLOCKED) qPtr = &blocked;
            else return;

            if ((*qPtr).empty()) return;

            for (auto it = (*qPtr).cbegin(); it != (*qPtr).cend(); ++it) {
                (*it)->printProcInfo();
            }
            cout << endl;
        }

} scheduler;
