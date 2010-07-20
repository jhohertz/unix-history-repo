/***********************license start***************
 *  Copyright (c) 2003-2008 Cavium Networks (support@cavium.com). All rights
 *  reserved.
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *      * Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *
 *      * Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials provided
 *        with the distribution.
 *
 *      * Neither the name of Cavium Networks nor the names of
 *        its contributors may be used to endorse or promote products
 *        derived from this software without specific prior written
 *        permission.
 *
 *  TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND CAVIUM NETWORKS MAKES NO PROMISES, REPRESENTATIONS
 *  OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *  RESPECT TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
 *  REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
 *  DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR
 *  PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET
 *  POSSESSION OR CORRESPONDENCE TO DESCRIPTION.  THE ENTIRE RISK ARISING OUT
 *  OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
 *
 *
 *  For any questions regarding licensing please contact marketing@caviumnetworks.com
 *
 ***********************license end**************************************/






/**
 * @file
 *
 * Support library for the hardware work queue timers.
 *
 * <hr>$Revision: 42180 $<hr>
 */
#include "executive-config.h"
#include "cvmx-config.h"
#include "cvmx.h"
#include "cvmx-sysinfo.h"
#include "cvmx-tim.h"
#include "cvmx-bootmem.h"

/* CSR typedefs have been moved to cvmx-csr-*.h */

/**
 * Global structure holding the state of all timers.
 */
CVMX_SHARED cvmx_tim_t cvmx_tim;


#ifdef CVMX_ENABLE_TIMER_FUNCTIONS
/**
 * Setup a timer for use. Must be called before the timer
 * can be used.
 *
 * @param tick      Time between each bucket in microseconds. This must not be
 *                  smaller than 1024/(clock frequency in MHz).
 * @param max_ticks The maximum number of ticks the timer must be able
 *                  to schedule in the future. There are guaranteed to be enough
 *                  timer buckets such that:
 *                  number of buckets >= max_ticks.
 * @return Zero on success. Negative on error. Failures are possible
 *         if the number of buckets needed is too large or memory
 *         allocation fails for creating the buckets.
 */
int cvmx_tim_setup(uint64_t tick, uint64_t max_ticks)
{
    cvmx_tim_mem_ring0_t    config_ring0;
    cvmx_tim_mem_ring1_t    config_ring1;
    uint64_t                timer_id;
    int                     error = -1;
#if !(defined(__KERNEL__) && defined(linux))
    cvmx_sysinfo_t         *sys_info_ptr = cvmx_sysinfo_get();
    uint64_t                cpu_clock_hz = sys_info_ptr->cpu_clock_hz;
#else
    uint64_t                cpu_clock_hz = octeon_get_clock_rate();
#endif
    uint64_t                hw_tick_ns;
    uint64_t                hw_tick_ns_allowed;
    uint64_t                tick_ns = 1000 * tick;
    int                     i;
    uint32_t                temp;

    /* for the simulator */
    if (cpu_clock_hz == 0)
      cpu_clock_hz = 333000000;

    hw_tick_ns = 1024 * 1000000000ull / cpu_clock_hz;
    /* 
     * Doulbe the minmal allowed tick to 2* HW tick.  tick between
     * (hw_tick_ns, 2*hw_tick_ns) will set config_ring1.s.interval 
     * to zero, or 1024 cycles. This is not enough time for the timer unit
     * to fetch the bucket data, Resulting in timer ring error interrupt 
     * be always generated. Avoid such setting in software 
     */
    hw_tick_ns_allowed = hw_tick_ns *2;

    /* Make sure the timers are stopped */
    cvmx_tim_stop();

    /* Reinitialize out timer state */
    memset(&cvmx_tim, 0, sizeof(cvmx_tim));
    

    if ((tick_ns < (hw_tick_ns_allowed)) || (tick_ns > 4194304 * hw_tick_ns))
      {
	cvmx_dprintf("init: tick wrong size. Requested tick %lu(ns) is smaller than" 
		     " the minimal ticks allowed by hardware %lu(ns)\n", 
		     tick_ns, hw_tick_ns_allowed);
	return error;
      }

    for (i=2; i<20; i++)
    {
      if (tick_ns < (hw_tick_ns << i))
	break;
    }

    cvmx_tim.max_ticks = (uint32_t)max_ticks;
    cvmx_tim.bucket_shift = (uint32_t)(i - 1 + 10);
    cvmx_tim.tick_cycles = tick * cpu_clock_hz / 1000000;

    temp = (max_ticks * cvmx_tim.tick_cycles) >> cvmx_tim.bucket_shift;

    /* round up to nearest power of 2 */
    temp -= 1;
    temp = temp | (temp >> 1);
    temp = temp | (temp >> 2);
    temp = temp | (temp >> 4);
    temp = temp | (temp >> 8);
    temp = temp | (temp >> 16);
    cvmx_tim.num_buckets = temp + 1;

    /* ensure input params fall into permitted ranges */
    if ((cvmx_tim.num_buckets < 3) || cvmx_tim.num_buckets > 1048576)
      {
	cvmx_dprintf("init: num_buckets out of range\n");
	return error;
      }

    /* Allocate the timer buckets from hardware addressable memory */
    cvmx_tim.bucket = cvmx_bootmem_alloc(CVMX_TIM_NUM_TIMERS * cvmx_tim.num_buckets
					 * sizeof(cvmx_tim_bucket_entry_t), CVMX_CACHE_LINE_SIZE);
    if (cvmx_tim.bucket == NULL)
      {
	cvmx_dprintf("init: allocation problem\n");
	return error;
      }
    memset(cvmx_tim.bucket, 0, CVMX_TIM_NUM_TIMERS * cvmx_tim.num_buckets * sizeof(cvmx_tim_bucket_entry_t));

    cvmx_tim.start_time = 0;

    /* Loop through all timers */
    for (timer_id = 0; timer_id<CVMX_TIM_NUM_TIMERS; timer_id++)
    {
        cvmx_tim_bucket_entry_t *bucket = cvmx_tim.bucket + timer_id * cvmx_tim.num_buckets;
        /* Tell the hardware where about the bucket array */
        config_ring0.u64 = 0;
        config_ring0.s.first_bucket = cvmx_ptr_to_phys(bucket) >> 5;
        config_ring0.s.num_buckets = cvmx_tim.num_buckets - 1;
        config_ring0.s.ring = timer_id;
        cvmx_write_csr(CVMX_TIM_MEM_RING0, config_ring0.u64);

        /* Tell the hardware the size of each chunk block in pointers */
        config_ring1.u64 = 0;
        config_ring1.s.enable = 1;
        config_ring1.s.pool = CVMX_FPA_TIMER_POOL;
        config_ring1.s.words_per_chunk = CVMX_FPA_TIMER_POOL_SIZE / 8;
        config_ring1.s.interval = (1 << (cvmx_tim.bucket_shift - 10)) - 1;
        config_ring1.s.ring = timer_id;
        cvmx_write_csr(CVMX_TIM_MEM_RING1, config_ring1.u64);
    }

    return 0;
}
#endif

/**
 * Start the hardware timer processing
 */
void cvmx_tim_start(void)
{
    cvmx_tim_control_t control;

    control.u64 = 0;
    control.s.enable_dwb = 1;
    control.s.enable_timers = 1;

    /* Remember when we started the timers */
    cvmx_tim.start_time = cvmx_get_cycle();
    cvmx_write_csr(CVMX_TIM_REG_FLAGS, control.u64);
}


/**
 * Stop the hardware timer processing. Timers stay configured.
 */
void cvmx_tim_stop(void)
{
    cvmx_tim_control_t control;
    control.u64 = 0;
    control.s.enable_dwb = 0;
    control.s.enable_timers = 0;
    cvmx_write_csr(CVMX_TIM_REG_FLAGS, control.u64);
}


/**
 * Stop the timer. After this the timer must be setup again
 * before use.
 */
#ifdef CVMX_ENABLE_TIMER_FUNCTIONS
void cvmx_tim_shutdown(void)
{
    uint32_t                bucket;
    uint64_t                timer_id;
    uint64_t                entries_per_chunk;

    /* Make sure the timers are stopped */
    cvmx_tim_stop();

    entries_per_chunk = CVMX_FPA_TIMER_POOL_SIZE/8 - 1;

    /* Now walk all buckets freeing the chunks */
    for (timer_id = 0; timer_id<CVMX_TIM_NUM_TIMERS; timer_id++)
    {
        for (bucket=0; bucket<cvmx_tim.num_buckets; bucket++)
        {
            uint64_t chunk_addr;
            uint64_t next_chunk_addr;
            cvmx_tim_bucket_entry_t *bucket_ptr = cvmx_tim.bucket + timer_id * cvmx_tim.num_buckets + bucket;
            CVMX_PREFETCH128(CAST64(bucket_ptr));  /* prefetch the next cacheline for future buckets */

            /* Each bucket contains a list of chunks */
            chunk_addr = bucket_ptr->first_chunk_addr;
            while (bucket_ptr->num_entries)
            {
#ifdef DEBUG
                cvmx_dprintf("Freeing Timer Chunk 0x%llx\n", CAST64(chunk_addr));
#endif
                /* Read next chunk pointer from end of the current chunk */
                next_chunk_addr = cvmx_read_csr(CVMX_ADD_SEG(CVMX_MIPS_SPACE_XKPHYS, chunk_addr + CVMX_FPA_TIMER_POOL_SIZE - 8));

                cvmx_fpa_free(cvmx_phys_to_ptr(chunk_addr), CVMX_FPA_TIMER_POOL, 0);
                chunk_addr = next_chunk_addr;
                if (bucket_ptr->num_entries > entries_per_chunk)
                    bucket_ptr->num_entries -= entries_per_chunk;
                else
                    bucket_ptr->num_entries = 0;
            }
        }
    }
}

#endif
