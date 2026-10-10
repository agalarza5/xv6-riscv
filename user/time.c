#include "kernel/types.h"
#include "kernel/pstat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(2, "Usage: time command [args...]\n");
    exit(1);
  }

  int start = uptime();

  int pid = fork();

  if (pid < 0) {
    fprintf(2, "fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    // Child runs the requested command.
    exec(argv[1], &argv[1]);

    // Only reached if exec fails.
    fprintf(2, "exec failed\n");
    exit(1);
  }

  // Parent waits for the child.
  int status;
  struct rusage usage;

  if (wait2(&status, &usage) < 0) {
    fprintf(2, "wait2 failed\n");
    exit(1);
  }

  int end = uptime();
  int elapsed = end - start;

  int percent = 0;

  if (elapsed > 0) {
    percent = (usage.cputime * 100) / elapsed;
  }

  printf("elapsed time: %d ticks\n", elapsed);
  printf("CPU time: %d ticks\n", usage.cputime);
  printf("CPU usage: %d%%\n", percent);

  exit(0);
}
