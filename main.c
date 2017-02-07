#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <wiringPi.h>
#include "fsm.h"
#include "task.h"
#include "interp.h"

void cofm_setup (void);
fsm_t* cofm_fsm_new (void);
extern int change;

void purse_setup (void);
fsm_t* purse_fsm_new (void);
extern int button;
extern int timer;


// Utility functions, should be elsewhere
struct timespec now, next_activation, timeout;

// Wait until next_activation (clock_gettime and nanosleep)
void delay_until (struct timespec* next_activation)
{
  // struct timeval now, timeout;
  // gettimeofday (&now, NULL);
  clock_gettime (CLOCK_MONOTONIC, &now);
  timespec_sub (&timeout, next_activation, &now);
  // select (0, NULL, NULL, NULL, &timeout);
  nanosleep (&now, NULL);
}


/* Coffee machine with credit check and give change activation */
void* main_ec (void* arg)
{
  struct timespec clk_period = { 0, 250 * 1000 };
  struct timespec next_activation;

  fsm_t* cofm_fsm = cofm_fsm_new ();
  fsm_t* purse_fsm = purse_fsm_new ();

  // gettimeofday (&next_activation, NULL);
  clock_gettime (CLOCK_MONOTONIC, &next_activation);
  while (1) {
    fsm_fire (cofm_fsm);
    fsm_fire (purse_fsm);
    timespec_add (&next_activation, &next_activation, &clk_period);
    delay_until (&next_activation);
  }
  return NULL;
}

int main ()
{
  wiringPiSetup();

  purse_setup ();
  cofm_setup ();
  task_new ("ec", main_ec, 0, 0, 1, 1024);
  interp_run ();
  /*	while (scanf("%d %d %d %d", &button, &coin, &change, &timer) == 4)
	timeval_add (&next_activation, &next_activation, &clk_period);
	delay_until (&next_activation);
  */
  return 0;
}

