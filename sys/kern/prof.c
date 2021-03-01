#include <sys/param.h>
#include <sys/prof.h>
#include <sys/interrupt.h>
#include <sys/klog.h>
#include <sys/kmem.h>
#include <sys/gmon.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/mimiker.h>
#include <machine/vm_param.h>
#include <sys/types.h>

gmonparam_t _gmonparam = {.state = GMON_PROF_OFF};

/* The macros description are provided in gmon.h */
void init_prof(void) {
  void *profptr;
  gmonparam_t *p = &_gmonparam;

  p->lowpc = rounddown((unsigned long)__kernel_start,
                       HISTFRACTION * sizeof(HISTCOUNTER));
  p->highpc =
    roundup((unsigned long)__etext, HISTFRACTION * sizeof(HISTCOUNTER));
  p->textsize = p->highpc - p->lowpc;
  p->kcountsize = p->textsize / HISTFRACTION;
  p->hashfraction = HASHFRACTION;
  p->fromssize = p->textsize / HASHFRACTION;
  
  p->tolimit = (p->textsize * ARCDENSITY) / 100;
  if (p->tolimit < MINARCS)
    p->tolimit = MINARCS;
  else if (p->tolimit > MAXARCS)
    p->tolimit = MAXARCS;
  p->tossize = p->tolimit * sizeof(tostruct_t);

  int size = p->kcountsize + p->tossize + p->fromssize;
  int aligned_size = align(size, PAGESIZE);
  profptr = kmem_alloc(aligned_size, M_NOWAIT | M_ZERO);
  if (profptr == NULL) {
    klog("Not enough memory for profiling!\n");
    return;
  }
  assert(is_aligned(profptr, sizeof(tostruct_t)));
  p->tos = (tostruct_t *)profptr;
  profptr += p->tossize;
  assert(is_aligned(profptr, sizeof(u_short)));
  p->kcount = (u_short *)profptr;
  profptr += p->kcountsize;
  assert(is_aligned(profptr, sizeof(u_short)));
  p->froms = (u_short *)profptr;
  p->state = GMON_PROF_ON;
}