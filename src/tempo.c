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

pthread_mutex_t mutex;

typedef struct timer_for_param {
  void * param;
  unsigned long endtime;

} timer_for_param;

typedef struct timer_list {
  struct timer_list * next;
  timer_for_param * value;

} timer_list;

static void add_to_list(timer_list *list, void* param, unsigned long endtime)
{
  pthread_mutex_lock(&mutex);
  //check if head hasn't been created
  if (list == NULL) {
      list = malloc(sizeof(list));
      if(list == NULL) {
          printf("Failed to create head node");
      }
      list->value = malloc(sizeof(timer_for_param));
      list->value->param = param;
      list->value->endtime = endtime;
      list->next = NULL;
  }
  else
  {
    //create a new node
    timer_list *newNode;
    newNode = malloc(sizeof(timer_list));
    if(newNode == NULL) {
        printf("Failed to create node");
    }
    newNode->value = malloc(sizeof(timer_for_param));
    newNode->value->param = param;
    newNode->value->endtime = endtime;
    newNode->next = NULL;

    //see if new node should be placed before head
    if (endtime < list->value->endtime) {
        newNode->next = list;
    }

    //search through to find correct spot and insert the node
    else
    {
      timer_list *temp ,*prev, *first;
      temp = list;
      first = list;
      while(temp != NULL && temp->value->endtime <= endtime) {
        prev = temp;
        temp = temp->next;
      }
      newNode->next = temp;
      prev->next = newNode;

      list = first;
    }
  }

  pthread_mutex_unlock(&mutex);
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


timer_list *TIMER = NULL;


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

  remove_from_list(TIMER);

  if(TIMER != NULL && TIMER->value != NULL)
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

    for(int i = 0 ; i < NSIG; ++i){
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
  // TODO
  pthread_t tid;
  pthread_mutex_init(&mutex, NULL);

  //struct itimerval timer;

  //configuer le timer
  /*timer.it_value.tv_sec = 2;
  timer.it_value.tv_usec = 0;
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 0;*/

  //créer le thread
  pthread_create(&tid, NULL, demon, NULL);

  //blocker le SIGALRM dans tous les autres threads
  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGALRM);
  sigprocmask(SIG_BLOCK, &mask, NULL);

  /*setitimer(ITIMER_REAL, &timer, NULL);
  setitimer(ITIMER_REAL, &timer, NULL);
  setitimer(ITIMER_REAL, &timer, NULL);
  setitimer(ITIMER_REAL, &timer, NULL);
  setitimer(ITIMER_REAL, &timer, NULL);*/

  //timer_set(1100, NULL);

  return 1; // Implementation not ready
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
  // TODO

  //TIMER->value = malloc(sizeof(timer_for_param));
  //TIMER->value->param = param;
  //*

  unsigned long endtime = get_time() + delay*1000;

  pthread_mutex_lock(&mutex);
  add_to_list(TIMER, param, endtime);
  pthread_mutex_unlock(&mutex);

  unsigned long et = TIMER->value->endtime;
  unsigned long ct = get_time();
  unsigned long d = (et-ct)/1000;

  timer_launch(d);
  //*/
  //timer_launch(delay);
}


#endif
