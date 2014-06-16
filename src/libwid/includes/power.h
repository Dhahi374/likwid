/*
 * =======================================================================================
 *
 *      Filename:  power.h
 *
 *      Description:  Header File Power Module
 *                    Implements Intel RAPL Interface.
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

#ifndef POWER_H
#define POWER_H

#include <types.h>
#include <registers.h>
#include <bitUtil.h>
#include <msr.h>
#include <error.h>

extern PowerInfo power_info;
extern  const uint32_t power_regs[4];

extern void power_init(int cpuId);
static inline void power_start(PowerData* data, int cpuId, PowerType type);
static inline void power_stop(PowerData* data, int cpuId, PowerType type);
static inline int power_read(int cpuId, uint64_t reg, uint32_t *data);
static inline int power_tread(int socket_fd, int cpuId, uint64_t reg, uint32_t *data);
static inline double power_printEnergy(PowerData* data);

static double
power_printEnergy(PowerData* data)
{
    return  (double) ((data->after - data->before) * power_info.energyUnit);
}

static void
power_start(PowerData* data, int cpuId, PowerType type)
{
	uint64_t result = 0;
	if (msr_read(cpuId, power_regs[type], &result))
	{
		data->before = 0;
		return;
	}
    data->before = extractBitField(result,32,0);
}

static void
power_stop(PowerData* data, int cpuId, PowerType type)
{
	uint64_t result = 0;
	if (msr_read(cpuId, power_regs[type], &result))
	{
		data->after = 0;
		return;
	}
    data->after = extractBitField(result,32,0);
}

static int
power_read(int cpuId, uint64_t reg, uint32_t *data)
{
	uint64_t result = 0;
	if (msr_read(cpuId, reg, &result))
	{
		*data = 0;
		return -EIO;
	}
    *data = extractBitField(result,32,0);
    return 0;
}

static int
power_tread(int socket_fd, int cpuId, uint64_t reg, uint32_t *data)
{
	uint64_t result = 0;
	if (msr_tread(socket_fd, cpuId, reg, &result))
	{
		*data = 0;
		return -EIO;
	}
    *data = extractBitField(result,32,0);
    return 0;
}

#endif /*POWER_H*/
