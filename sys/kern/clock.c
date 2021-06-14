#define KL_LOG KL_TIME
#include <sys/callout.h>
#include <sys/sched.h>
#include <sys/mimiker.h>
#include <sys/klog.h>
#include <sys/timer.h>
#include <sys/kgprof.h>

static systime_t now = 0;
static timer_t *sys_tm = NULL;
static timer_t *stat_tm = NULL;

systime_t getsystime(void) {
  return now;
}

static void stat_clock(timer_t *tm, void *arg) {
  kgprof_tick();
}

static void clock_cb(timer_t *tm, void *arg) {
  bintime_t bin = binuptime();
  now = bt2st(&bin);
  if(stat_tm == NULL)
    stat_clock(tm, arg);
  callout_process(now);
  sched_clock();
}

void init_clock(void) {
  sys_tm = tm_reserve(NULL, TMF_PERIODIC);
  stat_tm = stat_timer_get();

  if (sys_tm == NULL)
    panic("Missing suitable timer for maintenance of system clock!");
  tm_init(sys_tm, clock_cb, NULL);
  if (tm_start(sys_tm, TMF_PERIODIC | TMF_TIMESOURCE, (bintime_t){},
               HZ2BT(CLK_TCK)))
    panic("Failed to start system clock!");
  klog("System clock uses \'%s\' hardware timer.", sys_tm->tm_name);

  if (stat_tm != NULL) {
    tm_init(stat_tm, stat_clock, NULL);
    if (tm_start(stat_tm, TMF_PERIODIC, (bintime_t){}, HZ2BT(STAT_TCK))) {
      klog("Failed to start statclock, using system clock instead!");
      tm_release(stat_tm);
      stat_tm = NULL;
    } else {
      kgprof_set_profrate(STAT_TCK);
      klog("Statclock uses \'%s\' hardware timer.", stat_tm->tm_name);
    }
  }
}
