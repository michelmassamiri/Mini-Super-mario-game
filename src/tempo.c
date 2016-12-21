#define _GNU_SOURCE
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

void handler(int sig)
{
  printf("le thread qui a appelé a le numéro %lu\n", pthread_self());
}

void* demon(void *arg)
{
    struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;

    sigaction(SIGALRM, &sa, NULL);

    while(1)
      sigsuspend(&sa.sa_mask); //: le thread va blocker le programe jusqu'a un signal qui n'est pas dans set_of_mask arrive.

    return NULL;
}

// timer_init returns 1 if timers are fully implemented, 0 otherwise
int timer_init (void)
{
  // TODO
  pthread_t tid;
  struct itimerval timer;

  //configuer le timer
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = 150000;
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 150000;

  //créer le thread
  pthread_create(&tid, NULL, demon, NULL);

  //blocker le SIGALRM dans tous les autres threads
  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGALRM);
  sigprocmask(SIG_BLOCK, &mask, NULL);

  setitimer(ITIMER_REAL, &timer, NULL);
  setitimer(ITIMER_REAL, &timer, NULL);
  setitimer(ITIMER_REAL, &timer, NULL);
  setitimer(ITIMER_REAL, &timer, NULL);
  setitimer(ITIMER_REAL, &timer, NULL);

  return 0; // Implementation not ready
}

void timer_set (Uint32 delay, void *param)
{
  // TODO
}


#endif
