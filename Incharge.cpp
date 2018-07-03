#include "CMDManager.h"

#define BUFF_SIZE 1024

using namespace std;

int fd[2];
void readPipe(int);
void terminateIncharge(int);
bool killFlag;

//////////////////////////////MAIN//////////////////////////////////
int main() {

	pipe(fd);
	pid_t pid = fork();

	if (pid < 0) {
		cout << "Error creating pipe" << endl;
		return 0;
	}

	else if (pid == 0) {	// Controller
		close(fd[1]);
		signal(SIGALRM, readPipe);

		while(true && !killFlag) {}
	}

	else {	// Incharge
		close(fd[0]);
		signal(SIGINT, terminateIncharge);

		while(true && !killFlag) {
			char msg[BUFF_SIZE];
			cin.getline(msg, BUFF_SIZE);
			cout.flush();
			write(fd[1], msg, sizeof(msg));
			kill(pid, SIGALRM);
			cout << endl;
		}
	}
}

void readPipe(int sig) {
	char msg[BUFF_SIZE];
	read(fd[0], msg, sizeof(msg));

	if (string(msg) == "END") {
		cmdManager.PRT();
		kill(getppid(), SIGINT);
		killFlag = true;
	}
	cmdManager.insert(string(msg));
}
void terminateIncharge(int sig) {
	killFlag = true;
}
