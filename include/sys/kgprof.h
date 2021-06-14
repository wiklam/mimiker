#ifndef _SYS_KGPROF_H_
#define _SYS_KGPROF_H_

typedef struct timer timer_t;

#if KGPROF
timer_t *stat_timer_get(void);
void init_kgprof(void);
void kgprof_tick(void);
void kgprof_set_profrate(int profrate);
#else
#define stat_timer_get() NULL
#define init_kgprof() __nothing
#define kgprof_tick() __nothing
#define kgprof_set_profrate(x) __nothing
#endif

#endif /* !_SYS_KGPROF_H_ */
