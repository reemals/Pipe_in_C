/* Processes, Signals and Interprocess Communication */
/* By: Reem Alsaeed */
/* Date: May, 24, 2019 */
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "fork.h"

/* This is the handler of the alarm signal. It just updates was_alarm */
void alrm_handler(int i)
{
	if(was_alarm == 0){
		was_alarm = 1;
		printf("Alarm signal updated\n");
	}
	else{
		printf("3 seconds has passed\n");
		printf("Process %d did not finish in time\n", getpid());
		write(2,"marker: At least one process did not finish\n", 44);
	  kill(getpid(), SIGKILL);
	}
}

/* Prints string s using perror and exits. Also checks errno to make */
/* sure that it is not zero (if it is just prints s followed by newline */
/* using fprintf to the standard error */
void f_error(char *s)
{
	perror(s);
	if (errno == 0){
      fprintf(stderr, "%s\n", s);
  }
	exit(1);
}

/* Creates a child process using fork and a function from the exec family */
/* The standard input output and error are replaced by the last three */
/* arguments to implement redirection and piping */
pid_t start_child(const char *path, char *const argv[], int fdin, int fdout, int fderr)
{
	if(was_alarm == 0){
		printf("Signal was not updated\n");
	}
	int pid = fork();
	signal(SIGALRM, alrm_handler);
	alarm(3);

	if (pid<0)
		f_error("fork failed");

	if (pid==0){
		if (fdout && dup2(4,STDOUT_FILENO)<0)
			f_error("dup2 failed");
		if (!fdout && dup2(3,STDIN_FILENO)<0)
			f_error("dup2 failed");

		if(dup2(fderr, STDERR_FILENO) < 0)
			f_error("dup2 failes");

		close(fderr);
		if(dup2(fdin, fdout) < 0)
			f_error("dup2 failes");

		close(3);
    close(4);
		if(!fdout){
			if(dup2(5, STDOUT_FILENO) < 0)
				f_error("dup2 failes");

			close(5);
		}
		execlp(path,argv[1],argv[2], (char *)NULL);
		f_error("execlp failed");
		exit(1);
	}
	return pid;
}
