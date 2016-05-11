/*****************************************************************************
* SST platform-independent public interface
*
* Copyright (C) 2006 Miro Samek and Robert Ward. All rights reserved.
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL) as published by the Free Software
* Foundation and appearing in the file GPL.TXT included in the packaging of
* this file. Please note that GPL Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL ("Copyleft").
*
* Contact information:
* Email:    miro@quantum-leaps.com
* Internet: www.quantum-leaps.com
*****************************************************************************/
#ifndef __SST_H
#define __SST_H

#define SST_INT_LOCK()                          {}/*(_DINT())*/
#define SST_INT_UNLOCK()                        {}/*(_EINT())*/

#define SST_MAX_PRIO                            8 
#define TICK_ISR_PRIO                           0xFF
#define INTER_PRIO                              0x07
#define IO2_PRIO                                0x06
#define IO1_PRIO                                0x05

#define CODE                                    

#define SST_ISR_ENTRY()                         (++sst_int_nest)

#define SST_ISR_EXIT() do                       \
{                                               \
    --sst_int_nest;                             \
    if (sst_int_nest == (uint8_t)0)             \
    {                                           \
        sst_schedule();                         \
    }                                           \
} while (0)

typedef unsigned char                           sst_bool_t;
typedef unsigned char                           sst_uint8_t;
typedef signed   char                           sst_int8_t;
typedef unsigned int                            sst_uint16_t;
typedef signed   int                            sst_int16_t;
typedef unsigned long                           sst_uint32_t;


typedef sst_uint32_t sst_signal;
typedef void *sst_param;

typedef struct sst_event_tag
{
    sst_signal sig;		/* type of the event */
    sst_param  par;		/* the value of the event post */
} sst_event;

typedef void (*sst_task)(sst_event e);
typedef void (*sst_idle)();

void sst_task_create(
                     sst_task task, 
                     sst_uint8_t prio, 
                     sst_event *queue, 
                     sst_uint8_t qlen,
                     sst_signal sig, 
                     sst_param  par);


sst_bool_t sst_post(sst_uint8_t prio, sst_signal sig, sst_param  par);

void sst_run(void);

sst_uint8_t sst_mutex_lock(sst_uint8_t prio_ceiling);

void sst_mutex_unlock(sst_uint8_t org_prio);

void sst_schedule(void);
void task_init();
sst_bool_t sst_idle_register(sst_idle onidle);

/* public-scope objects */
extern sst_uint8_t sst_curr_prio;     /* current priority of the executing task */

#endif
