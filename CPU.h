#pragma once

#include "util.h"

class CPU {

    public:
        //void setQuantum(int quant) { quantum = quant; }

        void admitProcess(PCB* proc) {
            if (this->proc) {
                cerr << "Processs already admitted" << endl;
                return;
            }
            this->proc = proc;
        }

        void run() {
            // Extract instruction pointed to by the PC of the process in the CPU
            Instruction* instr = proc->instr[proc->pc];
            InstrType instrType = instr->getInstrType();

            ++proc->pc;
            ++proc->serviceTime;

            if (instrType == ARITHMETIC) {
                auto cInstr = static_cast<ArithmeticInstr*>(instr);

                //cout << "Arithmetic at " << (proc->pc - 1) << " of " << proc->pid << endl;
                switch(cInstr->getOp()) {
                    case SET:
                        proc->val = cInstr->getNum();
                        break;

                    case ADD:
                        proc->val += cInstr->getNum();
                        break;

                    case SUBTRACT:
                        proc->val -= cInstr->getNum();
                        break;
                }
            }
            else if (instrType == CMD) {
                auto cInstr = static_cast<CMDInstr*>(instr);

                if (cInstr->getCMD() == "E") {
                    endFlag = true;
                    delete proc;
                    proc = nullptr;
                }
                else if (cInstr->getCMD() == "B") {
                    blockFlag = true;
                }
            }
            else {
                auto cInstr = static_cast<ReplaceInstr*>(instr);
                auto fName = cInstr->getFileName();

                ifstream instrFile(fName);
            	if (!instrFile) {
            		cout << fName << " failed to open!" << endl << endl;
            		return;
            	}
                proc->val = 0;
                proc->deleteInstrs();
                proc->pc = 0;
                insertInstructions(proc, instrFile);
            }
        }

        bool empty() { returzn !proc; }

        PCB* extractProc() {
            assert(proc != nullptr);
            auto temp = proc;
            proc = nullptr;
            return temp;
        }

        PCB* getProc() {
            return proc;
        }

    private:
        PCB* proc = nullptr;

} cpu;
