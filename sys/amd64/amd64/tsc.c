/*-
 * Copyright (c) 1998-2003 Poul-Henning Kamp
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include "opt_clock.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/timetc.h>
#include <sys/kernel.h>
#include <sys/power.h>
#include <sys/smp.h>
#include <machine/clock.h>
#include <machine/md_var.h>
#include <machine/specialreg.h>

uint64_t	tsc_freq;
int		tsc_is_broken;

#ifdef SMP
static int	smp_tsc;
SYSCTL_INT(_kern_timecounter, OID_AUTO, smp_tsc, CTLFLAG_RD, &smp_tsc, 0,
    "Indicates whether the TSC is safe to use in SMP mode");
TUNABLE_INT("kern.timecounter.smp_tsc", &smp_tsc);
#endif

static	unsigned tsc_get_timecount(struct timecounter *tc);

static struct timecounter tsc_timecounter = {
	tsc_get_timecount,	/* get_timecount */
	0,			/* no poll_pps */
 	~0u,			/* counter_mask */
	0,			/* frequency */
	 "TSC",			/* name */
	800,			/* quality (adjusted in code) */
};

void
init_TSC(void)
{
	u_int64_t tscval[2];

	if (bootverbose)
	        printf("Calibrating TSC clock ... ");

	tscval[0] = rdtsc();
	DELAY(1000000);
	tscval[1] = rdtsc();

	tsc_freq = tscval[1] - tscval[0];
	if (bootverbose)
		printf("TSC clock: %lu Hz\n", tsc_freq);

	return;
}

void
init_TSC_tc(void)
{

	if (tsc_freq != 0 && !tsc_is_broken) {
		tsc_timecounter.tc_frequency = tsc_freq;
		tc_init(&tsc_timecounter);
	}
}

static int
sysctl_machdep_tsc_freq(SYSCTL_HANDLER_ARGS)
{
	int error;
	uint64_t freq;

	if (tsc_timecounter.tc_frequency == 0)
		return (EOPNOTSUPP);
	freq = tsc_freq;
	error = sysctl_handle_int(oidp, &freq, sizeof(freq), req);
	if (error == 0 && req->newptr != NULL) {
		tsc_freq = freq;
		tsc_timecounter.tc_frequency = tsc_freq;
	}
	return (error);
}

SYSCTL_PROC(_machdep, OID_AUTO, tsc_freq, CTLTYPE_LONG | CTLFLAG_RW,
    0, sizeof(u_int), sysctl_machdep_tsc_freq, "IU", "");

static unsigned
tsc_get_timecount(struct timecounter *tc)
{
	return (rdtsc());
}
