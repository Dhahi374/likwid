/*
 * =======================================================================================
 *
 *      Filename:  perfmon_nehalemEX.h
 *
 *      Description:  Header File of perfmon module for Nehalem EX.
 *
 *      Version:   <VERSION>
 *      Released:  <DATE>
 *
 *      Author:  Jan Treibig (jt), jan.treibig@gmail.com
 *      Project:  likwid
 *
 *      Copyright (C) 2013 Jan Treibig 
 *
 *      This program is free software: you can redistribute it and/or modify it under
 *      the terms of the GNU General Public License as published by the Free Software
 *      Foundation, either version 3 of the License, or (at your option) any later
 *      version.
 *
 *      This program is distributed in the hope that it will be useful, but WITHOUT ANY
 *      WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 *      PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License along with
 *      this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * =======================================================================================
 */

#include <perfmon_nehalemEX_events.h>
#include <perfmon_nehalemEX_groups.h>
#include <error.h>
#include <affinity.h>

#define NUM_COUNTERS_NEHALEMEX 7

//static int perfmon_numCountersNehalemEX = NUM_COUNTERS_NEHALEMEX;
static int perfmon_numGroupsNehalemEX = NUM_GROUPS_NEHALEMEX;
static int perfmon_numArchEventsNehalemEX = NUM_ARCH_EVENTS_NEHALEMEX;

/* This SUCKS: There are only subtle difference between NehalemEX
 * and Westmere EX Uncore. Still one of them is that one field is 
 * 1 bit shifted. Thank you Intel for this mess!!! Do you want 
 * to change the register definitions for every architecture?*/

/* MBOX macros */

#define MBOX_GATE_NEHEX(NUM)  \
    flags = 0x41ULL; \
    switch (event->cfgBits)  \
    {  \
        case 0x00:   /* primary Event */  \
            flags |= (event->eventId<<9);  \
            break;  \
        case 0x01: /* secondary Events */  \
            /* TODO fvid index is missing defaults to 0 */   \
            flags |= (1<<7); /* toggle flag mode */   \
            flags |= (event->eventId<<19);   \
            switch (event->eventId)   \
            {   \
                case 0x00: /* CYCLES_DSP_FILL: DSP */   \
                    {   \
                        uint64_t dsp_flags = 0x0ULL;   \
                        dsp_flags |= (event->umask<<7);  \
                        CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_M##NUM##_PMON_DSP, dsp_flags));   \
                    }   \
                    break;   \
                case 0x01: /* CYCLES_SCHED_MODE: ISS */   \
                    {   \
                        uint32_t iss_flags = 0x0UL;   \
                        iss_flags |= (event->umask<<4);   \
                        CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_M##NUM##_PMON_ISS, iss_flags));   \
                    }    \
                    break;   \
                case 0x05: /* CYCLES_PGT_STATE: PGT */   \
                    {   \
                        uint32_t pgt_flags = 0x0UL;   \
                        pgt_flags |= (event->umask<<6);   \
                        CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_M##NUM##_PMON_PGT, pgt_flags));   \
                    }    \
                    break;   \
                case 0x06: /* BCMD_SCHEDQ_OCCUPANCY: MAP */   \
                    {   \
                        uint32_t map_flags = 0x0UL;   \
                        map_flags |= (event->umask<<6);   \
                        CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_M##NUM##_PMON_MAP, map_flags));   \
                    }   \
                    break;   \
            }    \
            break;   \
        case 0x02: /* DRAM_CMD: PLD/ISS */   \
            flags |= (event->eventId<<9);   \
            {   \
                uint32_t pld_flags = 0x0UL;   \
                uint32_t iss_flags = 0x0UL;   \
                pld_flags |= (event->umask<<8);   \
                if (event->cmask != 0)   \
                {   \
                    iss_flags |= (event->cmask<<7);   \
                    pld_flags |= 1; /* toggle cmd flag */   \
                }   \
                CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_M##NUM##_PMON_PLD, pld_flags));   \
                CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_M##NUM##_PMON_ISS, iss_flags));   \
            }   \
            break;   \
        case 0x03: /* DSP_FILL: DSP */   \
            flags |= (event->eventId<<9);   \
            {   \
                uint64_t dsp_flags = 0x0ULL;   \
                dsp_flags |= (event->umask<<7);   \
                CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_M##NUM##_PMON_DSP, dsp_flags));   \
            }   \
            break;   \
        case 0x04: /* DRAM_MISC: PLD */   \
            flags |= (event->eventId<<9);   \
            {   \
                uint64_t pld_flags = 0x0ULL;   \
                switch (event->cmask)   \
                {   \
                    case 0x0:   \
                        pld_flags |= (1<<16);   \
                        pld_flags |= (event->umask<<19);   \
                        break;   \
                    case 0x1:   \
                        pld_flags |= (event->umask<<18);   \
                        break;   \
                    case 0x2:   \
                        pld_flags |= (event->umask<<17);   \
                        break;   \
                    case 0x3:   \
                        pld_flags |= (event->umask<<7);   \
                        break;   \
                }   \
                CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_M##NUM##_PMON_PLD, pld_flags));   \
            }   \
            break;   \
        case 0x05: /* FRM_TYPE: ISS */   \
            flags |= (event->eventId<<9);   \
            {   \
                uint32_t iss_flags = 0x0UL;   \
                iss_flags |= event->umask;   \
                CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_M##NUM##_PMON_ISS, iss_flags));   \
            }   \
        break;   \
        case 0x06: /* FVC_EV0: FVC */   \
            flags |= (event->eventId<<9);   \
            {   \
                uint32_t fvc_flags = 0x0UL;   \
                fvc_flags |= (event->umask<<11);   \
                if (event->umask == 0x5)   \
                {   \
                    fvc_flags |= (event->cmask<<5);   \
                }   \
                else   \
                {   \
                    fvc_flags |= (event->cmask<<8);   \
                }   \
                CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_M##NUM##_PMON_ZDP, fvc_flags));   \
                VERBOSEPRINTREG(cpu_id, MSR_M##NUM##_PMON_ZDP, fvc_flags, FVC_EV0) \
            }   \
            break;   \
        case 0x07: /* FVC_EV1: FVC */   \
            flags |= (event->eventId<<9);   \
            {   \
                uint32_t fvc_flags = 0x0UL;   \
                fvc_flags |= (event->umask<<14);   \
                if (event->umask == 0x5)   \
                {   \
                    fvc_flags |= (event->cmask<<5);   \
                }   \
                else   \
                {   \
                    fvc_flags |= (event->cmask<<8);   \
                }   \
                CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_M##NUM##_PMON_ZDP, fvc_flags));   \
                VERBOSEPRINTREG(cpu_id, MSR_M##NUM##_PMON_ZDP, fvc_flags, FVC_EV1) \
            }   \
            break;   \
        case 0x08: /* FVC_EV2: FVC */   \
            flags |= (event->eventId<<9);   \
            {   \
                uint32_t fvc_flags = 0x0UL;   \
                fvc_flags |= (event->umask<<17);   \
                if (event->umask == 0x5)   \
                {   \
                    fvc_flags |= (event->cmask<<5);   \
                }   \
                else   \
                {   \
                    fvc_flags |= (event->cmask<<8);   \
                }   \
                CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_M##NUM##_PMON_ZDP, fvc_flags));   \
                VERBOSEPRINTREG(cpu_id, MSR_M##NUM##_PMON_ZDP, fvc_flags, FVC_EV2) \
            }   \
            break;   \
        case 0x09: /* FVC_EV3: FVC(ZDP) */   \
        flags |= (event->eventId<<9);   \
        {   \
            uint32_t fvc_flags = 0x0UL;   \
            fvc_flags |= (event->umask<<20);   \
            if (event->umask == 0x5)   \
            {   \
                fvc_flags |= (event->cmask<<5);   \
            }   \
            else   \
            {   \
                fvc_flags |= (event->cmask<<8);   \
            }   \
            CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_M##NUM##_PMON_ZDP, fvc_flags));   \
        }   \
        break;   \
        case 0x0A: /* ISS_SCHED: ISS */   \
        flags |= (event->eventId<<9);   \
        {   \
            uint32_t iss_flags = 0x0UL;   \
            iss_flags |= (event->umask<<10);   \
            CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_M##NUM##_PMON_ISS, iss_flags));   \
        }   \
        break;   \
        case 0x0B: /* PGT_PAGE_EV: PGT */   \
        flags |= (event->eventId<<9);   \
        {   \
            uint32_t pgt_flags = 0x0UL;   \
            pgt_flags |= event->umask;   \
            CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_M##NUM##_PMON_PGT, pgt_flags));   \
        }   \
        break;   \
        case 0x0C: /* PGT_PAGE_EV2: PGT */   \
        flags |= (event->eventId<<9);   \
        {   \
            uint32_t pgt_flags = 0x0UL;   \
            pgt_flags |= (event->umask<<11);   \
            CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_M##NUM##_PMON_PGT, pgt_flags));   \
        }   \
        break;   \
        case 0x0D: /* THERM_TRP_DN: THR */   \
        flags |= (event->eventId<<9);   \
        {   \
            uint32_t thr_flags = 0x0UL;   \
            thr_flags |= (1<<3);   \
            thr_flags |= (event->umask<<9);   \
            CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_M##NUM##_PMON_PGT, thr_flags));   \
        }   \
        break;   \
    }

#define RBOX_GATE(NUM)  \
    flags = 0x01ULL; /* set local enable flag */ \
    switch (event->eventId) {  \
        case 0x00:  \
            flags |= (event->umask<<1); /* configure sub register */   \
            {  \
                uint32_t iperf_flags = 0x0UL;   \
                iperf_flags |= (event->cfgBits<<event->cmask); /* configure event */  \
                switch (event->umask) { /* pick correct iperf register */  \
                    case 0x00: \
                               CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_R##NUM##_PMON_IPERF0_P0, iperf_flags));   \
                    break; \
                    case 0x01: \
                               CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_R##NUM##_PMON_IPERF1_P0, iperf_flags));   \
                    break; \
                    case 0x06: \
                               CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_R##NUM##_PMON_IPERF0_P1, iperf_flags));   \
                    break; \
                    case 0x07: \
                               CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_R##NUM##_PMON_IPERF1_P1, iperf_flags));   \
                    break; \
                    case 0x0C: \
                               CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_R##NUM##_PMON_IPERF0_P2, iperf_flags));   \
                    break; \
                    case 0x0D: \
                               CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_R##NUM##_PMON_IPERF1_P2, iperf_flags));   \
                    break; \
                    case 0x12: \
                               CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_R##NUM##_PMON_IPERF0_P3, iperf_flags));   \
                    break; \
                    case 0x13: \
                               CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_R##NUM##_PMON_IPERF1_P3, iperf_flags));   \
                    break; \
                } \
            } \
            break; \
        case 0x01: \
            flags |= (event->umask<<1); /* configure sub register */   \
            { \
                uint32_t qlx_flags = 0x0UL;   \
                qlx_flags |= (event->cfgBits); /* configure event */  \
                if (event->cmask) qlx_flags |= (event->cmask<<4);  \
                switch (event->umask) { /* pick correct qlx register */  \
                    case 0x02: \
                               CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_R##NUM##_PMON_QLX_P0, qlx_flags));   \
                    break; \
                    case 0x03: \
                               CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_R##NUM##_PMON_QLX_P0, (qlx_flags<<8)));   \
                    break; \
                    case 0x08: \
                               CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_R##NUM##_PMON_QLX_P0, qlx_flags));   \
                    break; \
                    case 0x09: \
                               CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_R##NUM##_PMON_QLX_P1, (qlx_flags<<8)));   \
                    break; \
                    case 0x0E: \
                               CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_R##NUM##_PMON_QLX_P0, qlx_flags));   \
                    break; \
                    case 0x0F: \
                               CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_R##NUM##_PMON_QLX_P2, (qlx_flags<<8)));   \
                    break; \
                    case 0x14: \
                               CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_R##NUM##_PMON_QLX_P0, qlx_flags));   \
                    break; \
                    case 0x15: \
                               CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, MSR_R##NUM##_PMON_QLX_P3, (qlx_flags<<8)));   \
                    break; \
                } \
            } \
            break; \
    }


int perfmon_setupCounterThread_nehalemEX(int thread_id, PerfmonEventSet* eventSet)
{
    uint64_t flags = 0x0ULL;
    int cpu_id = groupSet->threads[thread_id].processorId;

    for (int i=0;i < eventSet->numberOfEvents;i++)
    {
        PerfmonCounterIndex index = eventSet->events[i].index;
        PerfmonEvent *event = &(eventSet->events[i].event);
        uint64_t reg = westmereEX_counter_map[index].configRegister;
        eventSet->events[i].threadCounter[thread_id].init = TRUE;
        switch (westmereEX_counter_map[index].type)
        {
            case PMC:
                CHECK_MSR_READ_ERROR(msr_read(cpu_id, reg, &flags));
                flags &= ~(0xFFFFU);   /* clear lower 16bits */

                /* Intel with standard 8 bit event mask: [7:0] */
                flags |= (event->umask<<8) + event->eventId;

                if (event->cfgBits != 0) /* set custom cfg and cmask */
                {
                    flags &= ~(0xFFFFU<<16);  /* clear upper 16bits */
                    flags |= ((event->cmask<<8) + event->cfgBits)<<16;
                }

                CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, reg, flags));
                VERBOSEPRINTREG(cpu_id, reg, flags, PMC_EV_SEL)
                break;

            case FIXED:
                break;

            case MBOX0:
                MBOX_GATE_NEHEX(0);
                CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, reg, flags));
                VERBOSEPRINTREG(cpu_id, reg, flags, MBOX0_CTRL)
                break;

            case MBOX1:
                MBOX_GATE_NEHEX(1);
                CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, reg, flags));
                VERBOSEPRINTREG(cpu_id, reg, flags, MBOX1_CTRL)
                break;

            case BBOX0:
            case BBOX1:
                flags = 0x1ULL; /* set enable bit */
                flags |=  (event->eventId<<1);
                CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, reg , flags));
                VERBOSEPRINTREG(cpu_id, reg, flags, BBOX_CTRL)
                break;

            case RBOX0:
                RBOX_GATE(0);
                CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, reg , flags));
                VERBOSEPRINTREG(cpu_id, reg, flags, RBOX0_CTRL)
                break;

            case RBOX1:
                RBOX_GATE(1);
                CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, reg , flags));
                VERBOSEPRINTREG(cpu_id, reg, flags, RBOX1_CTRL)
                break;

            case WBOX:
                if (event->eventId == 0xFF)  /* Fixed Counter */
                {
                    flags = 0x1ULL; /* set enable bit */
                }
                else
                {
                    flags |= (1<<22); /* set enable bit */
                    flags |= (event->umask<<8) + event->eventId;
                }
                CHECK_MSR_WRITE_ERROR(msr_write(cpu_id, reg , flags));
                VERBOSEPRINTREG(cpu_id, reg, flags, WBOX_CTRL)
                break;

            default:
                /* should never be reached */
                break;
        }
    }
    return 0;
}

