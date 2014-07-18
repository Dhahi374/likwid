/*
 * =======================================================================================
 *
 *      Filename:  affinity_types.h
 *
 *      Description:  Type Definitions for affinity Module
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

#ifndef AFFINITY_TYPES_H
#define AFFINITY_TYPES_H

typedef struct {
    bstring tag;
    uint32_t numberOfProcessors;
    uint32_t numberOfCores;
    int*  processorList;
} AffinityDomain;

typedef struct {
    uint32_t numberOfAffinityDomains;
    uint32_t numberOfSocketDomains;
    uint32_t numberOfNumaDomains;
    uint32_t numberOfProcessorsPerSocket;
    uint32_t numberOfCacheDomains;
    uint32_t numberOfCoresPerCache;
    uint32_t numberOfProcessorsPerCache;
    AffinityDomain* domains;
} AffinityDomains;

typedef AffinityDomains* AffinityDomains_t;

#endif /*AFFINITY_TYPES_H*/
