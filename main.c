/* Assignment 1: Processes, Signals and Interprocess Communication */
/* By: Reem Alsaeed */
/* Date: May, 24, 2019 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
int was_alarm=0;

/* The main program does much of the work. parses the command line arguments */
/* sets up the alarm and the alarm signal handler opens the files and pipes */
/* for redirection etc., invoke start_child, closes files that should be  */
/* closed, waits for the children to finish and reports their status */
/* or exits printing a message and kill its children if they do not die */
/* in time (just the parent exiting may not be enough to kill the children) */
void alrm_handler(int i);
void f_error(char *s);
pid_t start_child(const char *path, char *const argv[], int fdin, int fdout, int fderr);

int main(int argc, char *argv[])
{
  int i;
  int post = 0;
  for(i = 0 ; i < argc ; i++){
    if(strcmp(argv[i], "-p-") == 0){
      post = i;
      argv[i] = NULL;
    }
  }
  if(post == 0)
    f_error("Missing -p-");

  signal(SIGUSR1, alrm_handler);
  raise(SIGUSR1);

  pid_t pid1, pid2, wid;
  int fd[2];

  if (pipe(fd)<0)
      f_error("Error creating pipe");

  int output = open("test.out",O_TRUNC| O_WRONLY|O_CREAT, 0666);    //5
  if(output < 0)
    f_error("ERROR: opening test.out");
  int er1 = open("test.err1",O_TRUNC| O_WRONLY|O_CREAT, 0666);      //6
  if(er1 < 0)
    f_error("ERROR: opening test.err1");
  int er2 = open("test.err2",O_TRUNC| O_WRONLY|O_CREAT, 0666);      //7
  if(er2 < 0)
    f_error("ERROR: opening test.err2");

  FILE *input = freopen("test.in", "r", stdin);
  if(input == NULL)
    f_error("ERROR: opening test.in");

  pid1 = start_child(argv[1], argv, fd[1], STDOUT_FILENO, er1);
  fclose(input);
  pid2 = start_child(argv[post+1], argv+post, fd[0], STDIN_FILENO, er2);

  close(fd[0]);
  close(fd[1]);

  int status;
  wid=wait(&status);
  if (wid==pid1)
    printf("Process %s finished with status %d\n",argv[1],WEXITSTATUS(status));
  if (wid==pid2)
    printf("Process %s finished with status %d\n",argv[post+1],WEXITSTATUS(status));
  wid=wait(&status);
  if (wid==pid1)
    printf("Process %s finished with status %d\n",argv[1],WEXITSTATUS(status));
  if (wid==pid2)
    printf("Process %s finished with status %d\n",argv[post+1],WEXITSTATUS(status));

  return 0;
}
