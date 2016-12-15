#include <SDL.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>

#include "timer.h"

// Return number of elapsed µsec since... a long time ago
static unsigned long get_time (void)
{
  struct timeval tv;

  gettimeofday (&tv ,NULL);

  // Only count seconds since beginning of 2016 (not jan 1st, 1970)
  tv.tv_sec -= 3600UL * 24 * 365 * 46;

  return tv.tv_sec * 1000000UL + tv.tv_usec;
}

#ifdef PADAWAN

// timer_init returns 1 if timers are fully implemented, 0 otherwise
int timer_init (void)
{
  // TODO

  struct sigaction sa;
  sigset_t mask, oldmask;
  pthread_t* tid;

  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = handler;

  sigprocmask(SIG_BLOCK, &mask, &oldmask);
  sigaction()

  return 0; // Implementation not ready
}

void timer_set (Uint32 delay, void *param)
{
  // TODO
}

void* thread(void *arg)
{
    /*
    while(true)
      sigsuspend(set_of_mask) : le thread va blocker le programe jusqu'a un signal qui n'est pas dans set_of_mask arrive.
    */
}

#endif
