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

pthread_mutex_t mutex, mutex_timer_set;

typedef struct timer_for_param {
  void * param;
  unsigned long endtime;

} timer_for_param;

typedef struct timer_list {
  struct timer_list * next;
  timer_for_param * value;

} timer_list;

static void add_to_list(timer_list *list, timer_for_param *tfp)
{
  //pthread_mutex_lock(&mutex);
  if(list->value == NULL)
  {
    list->value = malloc(sizeof(timer_for_param));
    list->value = tfp;
  }
  else
  {
    if(list->value->endtime > tfp->endtime)
    {
	    timer_list *newlist = malloc(sizeof(newlist));
	    newlist->value = tfp;
	    newlist->next = list;
	    list = newlist;
	    return;
    }
    timer_list *tmp = list;
    while(list->value->endtime < tfp->endtime && list->next != NULL)
      list = list->next;

    timer_list *tmp2 = malloc(sizeof(tmp2));
    tmp2->value = malloc(sizeof(timer_for_param));

    tmp2->value = tfp;
    tmp2->next = list->next;
    list->next = tmp2;

    list = tmp;
  }
  //pthread_mutex_unlock(&mutex);
}

static void remove_from_list(timer_list *list)
{
  pthread_mutex_lock(&mutex);
  if(list->next != NULL)
  {
    timer_list *tmp = list;
    list = list->next;
    free(tmp->value);
    free(tmp);
  }
  else
  {
    free(list->value);
    list->value = NULL;
  }
  pthread_mutex_unlock(&mutex);
}


static timer_list *TIMER;


void timer_launch(Uint32 delay);

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
  pthread_mutex_lock(&mutex);

  sdl_push_event(TIMER->value->param);
  printf ("sdl_push_event(%p) appelée au temps %ld\n", TIMER->value->param, get_time ());

  timer_list *l = malloc(sizeof(l));
  l  = TIMER->next;
  remove_from_list(TIMER);


  if(l != NULL && l->value != NULL)
  {
    unsigned long et = TIMER->value->endtime;
    unsigned long ct = get_time();

    timer_launch((Uint32)(et-ct)/1000);
   }
  pthread_mutex_unlock(&mutex);


}

void* demon(void *arg)
{
    struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;

    sigaction(SIGALRM, &sa, NULL);
    for(int i = 0 ; i < NSIG; ++i)
    {
	    if(i != SIGALRM) {
		sigaddset(&sa.sa_mask, i);
	     }
    }

    while(1)
    {
      sigsuspend(&sa.sa_mask); //: le thread va blocker le programe jusqu'a un signal qui n'est pas dans set_of_mask arrive.
    }


    return NULL;
}

// timer_init returns 1 if timers are fully implemented, 0 otherwise
int timer_init (void)
{
  pthread_t tid;
  pthread_mutex_init(&mutex, NULL);
  pthread_mutex_init(&mutex_timer_set, NULL);
  TIMER = malloc(sizeof(TIMER));
  TIMER->value = NULL;

  //créer le thread
  pthread_create(&tid, NULL, demon, NULL);

  //blocker le SIGALRM dans tous les autres threads
  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGALRM);
  sigprocmask(SIG_BLOCK, &mask, NULL);

  return 1; // Implementation quite ready
}

void timer_launch(Uint32 delay)
{
  struct itimerval timer;

  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = 0;
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 0;

  while(delay >= 1000)
  {
    delay -= 1000;
    ++timer.it_value.tv_sec;
  }
  timer.it_value.tv_usec = delay * 1000;
  setitimer(ITIMER_REAL, &timer, NULL);
}

void timer_set (Uint32 delay, void *param)
{
  pthread_mutex_lock(&mutex_timer_set);

  timer_for_param *t = malloc(sizeof(timer_for_param));
  t->param = param;
  t->endtime = get_time() + delay*1000;

  add_to_list(TIMER, t);
  unsigned long et = TIMER->value->endtime;
  unsigned long ct = get_time();
  unsigned long d = (et-ct)/1000;

  timer_launch(d);
  pthread_mutex_unlock(&mutex_timer_set);
 }


#endif
