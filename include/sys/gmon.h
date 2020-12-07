/*	$NetBSD: gmon.h,v 1.10 2012/03/09 15:38:03 christos Exp $	*/

/*-
 * Copyright (c) 1982, 1986, 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)gmon.h	8.2 (Berkeley) 1/4/94
 */

#ifndef _SYS_GMON_H_
#define _SYS_GMON_H_

#include <machine/profile.h>

/*
 * fraction of text space to allocate for histogram counters here, 1/2
 */
#ifndef HISTFRACTION
#define HISTFRACTION 2
#endif /* HISTFRACTION */

/*
 * Fraction of text space to allocate for from hash buckets.
 * The value of HASHFRACTION is based on the minimum number of bytes
 * of separation between two subroutine call points in the object code.
 * Given MIN_SUBR_SEPARATION bytes of separation the value of
 * HASHFRACTION is calculated as:
 *
 *	HASHFRACTION = MIN_SUBR_SEPARATION / (2 * sizeof(short) - 1);
 *
 * For example, on the VAX, the shortest two call sequence is:
 *
 *	calls	$0,(r0)
 *	calls	$0,(r0)
 *
 * which is separated by only three bytes, thus HASHFRACTION is
 * calculated as:
 *
 *	HASHFRACTION = 3 / (2 * 2 - 1) = 1
 *
 * Note that the division above rounds down, thus if MIN_SUBR_FRACTION
 * is less than three, this algorithm will not work!
 *
 * In practice, however, call instructions are rarely at a minimal
 * distance.  Hence, we will define HASHFRACTION to be 2 across all
 * architectures.  This saves a reasonable amount of space for
 * profiling data structures without (in practice) sacrificing
 * any granularity.
 */
#define HASHFRACTION 2

/*
 * percent of text space to allocate for tostructs with a minimum.
 */
#define ARCDENSITY 2
#define MINARCS 50
#define MAXARCS ((1 << (unsigned int)(8 * sizeof(HISTCOUNTER))) - 2)

typedef struct tostruct tostruct_t;
struct tostruct {
  u_long selfpc;
  long count;
  u_short link;
  u_short pad;
};

/*
 * The profiling data structures are housed in this structure.
 */

typedef struct gmonparam gmonparam_t;
struct gmonparam {
  int state;
  u_short *kcount;
  u_long kcountsize;
  u_short *froms;
  u_long fromssize;
  tostruct_t *tos;
  u_long tossize;
  long tolimit;
  u_long lowpc;
  u_long highpc;
  u_long textsize;
  u_long hashfraction;
};
extern gmonparam_t _gmonparam;

/*
 * Possible states of profiling.
 */
#define GMON_PROF_ON 0
#define GMON_PROF_BUSY 1
#define GMON_PROF_ERROR 2
#define GMON_PROF_OFF 3

#endif /* !_SYS_GMON_H_ */