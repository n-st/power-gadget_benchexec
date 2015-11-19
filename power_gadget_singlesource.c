/*
Copyright (c) 2012, Intel Corporation

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Written by Martin Dimitrov, Carl Strickland */

#define _GNU_SOURCE

#ifndef _h_cpuid
#define _h_cpuid

#include <stdint.h>

typedef struct cpuid_info_t {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
} cpuid_info_t;

void cpuid(uint32_t eax_in, uint32_t ecx_in, cpuid_info_t *info);
uint32_t get_processor_signature();
cpuid_info_t get_processor_topology(uint32_t level);

#endif
/*
Copyright (c) 2012, Intel Corporation

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Written by Martin Dimitrov, Carl Strickland */



void
cpuid(uint32_t eax_in, uint32_t ecx_in,
      cpuid_info_t *ci)
{
    asm (
#if defined(__LP64__)           /* 64-bit architecture */
        "cpuid;"                /* execute the cpuid instruction */
        "movl %%ebx, %[ebx];"   /* save ebx output */
#else                           /* 32-bit architecture */
        "pushl %%ebx;"          /* save ebx */
        "cpuid;"                /* execute the cpuid instruction */
        "movl %%ebx, %[ebx];"   /* save ebx output */
        "popl %%ebx;"           /* restore ebx */
#endif
        : "=a"(ci->eax), [ebx] "=r"(ci->ebx), "=c"(ci->ecx), "=d"(ci->edx)
        : "a"(eax_in), "c"(ecx_in)
    );
}

uint32_t
get_processor_signature()
{
    cpuid_info_t info;
    cpuid(0x1, 0, &info);
    return info.eax;
}

cpuid_info_t
get_processor_topology(uint32_t level)
{
    cpuid_info_t info;
    cpuid(0xb, level, &info);
    return info;
}

#if 0
#include <stdio.h>
void cast_uint_to_str(char* out, uint32_t in)
{
    int i;
    uint32_t mask = 0x000000ff;
    for (i = 0; i < 4; i++) {
        out[i] = (char)((in & (mask<<(i*8))) >> (i*8));
    }
    out[4] = '\0';
}

void get_vendor(char* vendor)
{
    cpuid_info_t c;
    char vendor1[5];
    char vendor2[5];
    char vendor3[5];
    cpuid(0, 0, &c);
    cast_uint_to_str(vendor1, c.ebx);
    cast_uint_to_str(vendor2, c.edx);
    cast_uint_to_str(vendor3, c.ecx);
    sprintf(vendor, "%s%s%s", vendor1, vendor2, vendor3);
}

void cpuid_test()
{
    char vendor[12];
    get_vendor(vendor);
    printf("%s\n", vendor);
}

int main()
{
    cpuid_test();
    uint32_t ps;
    ps = get_processor_signature();
    printf("processor signature: 0x%08x\n", ps);
}
#endif
/*
Copyright (c) 2012, Intel Corporation

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Written by Martin Dimitrov, Carl Strickland */

#ifndef _h_msr_t
#define _h_msr_t

#include <stdint.h>

#define MY_ERROR -1

/* Replacement for pow() where the base is 2 and the power is unsigned and
 * less than 31 (will get invalid numbers if 31 or greater) */
#define B2POW(e) (((e) == 0) ? 1 : (2 << ((e) - 1)))

/* General (Sandy Bridge Client/Server) */
#define MSR_RAPL_POWER_UNIT 0x606 /* Unit Multiplier used in RAPL Interfaces (R/O) */

/* PKG (Sandy Bridge Client/Server) */
#define MSR_RAPL_PKG_POWER_LIMIT   0x610 /* PKG RAPL Power Limit Control (R/W) */
#define MSR_RAPL_PKG_ENERGY_STATUS 0x611 /* PKG Energy Status (R/O) */
#define MSR_RAPL_PKG_PERF_STATUS   0x613 /* PKG Performance Throttling Status (R/O) */
#define MSR_RAPL_PKG_POWER_INFO    0x614 /* PKG RAPL Parameters (R/O) */

/* DRAM (Sandy Bridge Server) */
#define MSR_RAPL_DRAM_POWER_LIMIT   0x618 /* DRAM RAPL Power Limit Control (R/W) */
#define MSR_RAPL_DRAM_ENERGY_STATUS 0x619 /* DRAM Energy Status (R/O) */
#define MSR_RAPL_DRAM_PERF_STATUS   0x61b /* DRAM Performance Throttling Status (R/O) */
#define MSR_RAPL_DRAM_POWER_INFO    0x61c /* DRAM RAPL Parameters (R/O) */

/* PP0 (Sandy Bridge Client/Server) */
#define MSR_RAPL_PP0_POWER_LIMIT   0x638 /* PP0 RAPL Power Limit Control (R/W) */
#define MSR_RAPL_PP0_ENERGY_STATUS 0x639 /* PP0 Energy Status (R/O) */
#define MSR_RAPL_PP0_POLICY        0x63a /* PP0 Performance Throttling Status (R/O) */
#define MSR_RAPL_PP0_PERF_STATUS   0x63b /* PP0 Balance Policy (R/W) */

/* PP1 (Sandy Bridge Gen2 Client) */
#define MSR_RAPL_PP1_POWER_LIMIT   0x640 /* PP1 RAPL Power Limit Control (R/W) */
#define MSR_RAPL_PP1_ENERGY_STATUS 0x641 /* PP1 Energy Status (R/O) */
#define MSR_RAPL_PP1_POLICY        0x642 /* PP1 Balance Policy (R/W) */

/* Common MSR Structures */
typedef struct rapl_power_limit_control_msr_t {
    uint64_t power_limit         : 15;
    uint64_t limit_enabled       : 1;
    uint64_t clamp_enabled       : 1;
    uint64_t limit_time_window_y : 5;
    uint64_t limit_time_window_f : 2;
    uint64_t                     : 7;
    uint64_t lock_enabled        : 1;
    uint64_t                     : 32;
} rapl_power_limit_control_msr_t;

/* Wrap-around time of many hours. */
typedef struct rapl_parameters_msr_t {
    uint64_t thermal_spec_power        : 15;
    uint64_t                           : 1;
    uint64_t minimum_power             : 15;
    uint64_t                           : 1;
    uint64_t maximum_power             : 15;
    uint64_t                           : 1;
    uint64_t maximum_limit_time_window : 6;
    uint64_t                           : 10;
} rapl_parameters_msr_t;

/* Updated every ~1ms. Wraparound time of 60s under load. */
typedef struct energy_status_msr_t {
    uint64_t total_energy_consumed : 32;
    uint64_t                       : 32;
} energy_status_msr_t;

typedef struct performance_throttling_status_msr_t {
    uint64_t accumulated_throttled_time : 32;
    uint64_t                            : 32;
} performance_throttling_status_msr_t;

typedef struct balance_policy_msr_t {
    uint64_t priority_level : 5;
    uint64_t                : 32;
    uint64_t                : 27;
} balance_policy_msr_t;


/* General */
typedef struct rapl_unit_multiplier_msr_t {
    uint64_t power  : 4;
    uint64_t        : 4;
    uint64_t energy : 5;
    uint64_t        : 3;
    uint64_t time   : 4;
    uint64_t        : 32;
    uint64_t        : 12;
} rapl_unit_multiplier_msr_t;

/* PKG */
typedef struct pkg_rapl_power_limit_control_msr_t {
    uint64_t power_limit_1         : 15;
    uint64_t limit_enabled_1       : 1;
    uint64_t clamp_enabled_1       : 1;
    uint64_t limit_time_window_y_1 : 5;
    uint64_t limit_time_window_f_1 : 2;
    uint64_t                       : 8;
    uint64_t power_limit_2         : 15;
    uint64_t limit_enabled_2       : 1;
    uint64_t clamp_enabled_2       : 1;
    uint64_t limit_time_window_y_2 : 5;
    uint64_t limit_time_window_f_2 : 2;
    uint64_t                       : 7;
    uint64_t lock_enabled          : 1;
} pkg_rapl_power_limit_control_msr_t;

typedef energy_status_msr_t pkg_energy_status_msr_t;
typedef rapl_parameters_msr_t pkg_rapl_parameters_msr_t;
typedef performance_throttling_status_msr_t pkg_performance_throttling_status_msr_t;

/* PP0 */
typedef rapl_power_limit_control_msr_t pp0_rapl_power_limit_control_msr_t;
typedef energy_status_msr_t pp0_energy_status_msr_t;
typedef balance_policy_msr_t pp0_balance_policy_msr_t;
typedef performance_throttling_status_msr_t pp0_performance_throttling_status_msr_t;

/* PP1 */
typedef rapl_power_limit_control_msr_t pp1_rapl_power_limit_control_msr_t;
typedef energy_status_msr_t pp1_energy_status_msr_t;
typedef balance_policy_msr_t pp1_balance_policy_msr_t;
typedef performance_throttling_status_msr_t pp1_performance_throttling_status_msr_t;

/* DRAM */
typedef rapl_power_limit_control_msr_t dram_rapl_power_limit_control_msr_t;
typedef energy_status_msr_t dram_energy_status_msr_t;
typedef rapl_parameters_msr_t dram_rapl_parameters_msr_t;
typedef performance_throttling_status_msr_t dram_performance_throttling_status_msr_t;

/*
 *  For documentaion see: "Intel64 and IA-32 Architectures Software Developer's Manual" Volume 3B, Appendix B  Model-Specific registers.
 * http://www.intel.com/Assets/PDF/manual/253669.pdf  In a nutshell, search the manual and find the MSR number that you wish to read/write.
 * Then use the read_msr_t/write_msr_t functions and extract_bit functions to get the info you need.
 */

/**
 * Read the given MSR on the given CPU.
 *
 * @return            0 on success and MY_ERROR on failure
 */
int read_msr(int cpu, uint64_t address, uint64_t *val);

/**
 * Write the given value to the given MSR on the given CPU.
 *
 * @return            0 on success and MY_ERROR on failure
 */
int write_msr(int cpu, uint64_t address, uint64_t val);
#endif
/*
Copyright (c) 2012, Intel Corporation

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Written by Martin Dimitrov, Carl Strickland */

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>



/*
 * read_msr
 *
 * Will return 0 on success and MY_ERROR on failure.
 */
int
read_msr(int       cpu,
         uint64_t  address,
         uint64_t *value)
{
    int   err = 0;
    char  msr_path[32];
    FILE *fp;

    sprintf(msr_path, "/dev/cpu/%d/msr", cpu);
    err = ((fp = fopen(msr_path, "r")) == NULL);
    if (!err)
        err = (fseek(fp, address, SEEK_CUR) != 0);
    if (!err)
        err = (fread(value, sizeof(uint64_t), 1, fp) != 1);
    if (fp != NULL)
        fclose(fp);
    return err;
}



/*
 * write_msr
 *
 * Will return 0 on success and MY_ERROR on failure.
 */

int
write_msr(int      cpu,
          uint64_t address,
          uint64_t value)
{
    int   err = 0;
    char  msr_path[32];
    FILE *fp;

    sprintf(msr_path, "/dev/cpu/%d/msr", cpu);
    err = ((fp = fopen(msr_path, "w")) == NULL);
    if (!err)
        err = (fseek(fp, address, SEEK_CUR) != 0);
    if (!err)
        err = (fwrite(&value, sizeof(uint64_t), 1, fp) != 1);
    if (fp != NULL)
        fclose(fp);
    return err;
}

/*
Copyright (c) 2012, Intel Corporation

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Written by Martin Dimitrov, Carl Strickland */


/*! \file rapl.h
 *  Library header file.
 */

#ifndef _h_rapl
#define _h_rapl

#include <stdint.h>

#define MY_ERROR -1

/* Power Domains */
#define RAPL_PKG  0      /*!< \brief Package power domain */
#define RAPL_PP0  1      /*!< \brief Core power domain */
#define RAPL_PP1  2      /*!< \brief Uncore power domain */
#define RAPL_DRAM 3      /*!< \brief DRAM power domain */
#define RAPL_NR_DOMAIN 4 /*!< \brief Number of power domains */

enum RAPL_DOMAIN { PKG, PP0, PP1, DRAM };

char* RAPL_DOMAIN_STRINGS[RAPL_NR_DOMAIN];

typedef struct APIC_ID_t {
    uint64_t smt_id;
    uint64_t core_id;
    uint64_t pkg_id;
    uint64_t os_id;
} APIC_ID_t;

int init_rapl();
int terminate_rapl();

/* Wraparound values for total energy consumed and accumulated throttled time.
 * These values are computed within init_rapl(). */
double MAX_ENERGY_STATUS_JOULES;   /* default: 65536 */
double MAX_THROTTLED_TIME_SECONDS; /* default: 4194304 */

uint64_t get_num_rapl_nodes_pkg();
uint64_t get_num_rapl_nodes_pp0();
uint64_t get_num_rapl_nodes_pp1();
uint64_t get_num_rapl_nodes_dram();

uint64_t is_supported_msr(uint64_t msr);
uint64_t is_supported_domain(uint64_t power_domain);

/* General */

/*! \brief RAPL power limit control structure, PKG domain */
typedef struct pkg_rapl_power_limit_control_t {
    double   power_limit_watts_1;
    double   limit_time_window_seconds_1;
    uint64_t limit_enabled_1;
    uint64_t clamp_enabled_1;
    double   power_limit_watts_2;
    double   limit_time_window_seconds_2;
    uint64_t limit_enabled_2;
    uint64_t clamp_enabled_2;
    uint64_t lock_enabled;
} pkg_rapl_power_limit_control_t;

/*! \brief RAPL parameters info structure, PKG domain */
typedef struct pkg_rapl_parameters_t {
    double thermal_spec_power_watts;
    double minimum_power_watts;
    double maximum_power_watts;
    double maximum_limit_time_window_seconds;
} pkg_rapl_parameters_t;
int get_pkg_rapl_power_limit_control_t(uint64_t node, pkg_rapl_power_limit_control_t *rapl_power_limit_control);
int get_pkg_total_energy_consumed(uint64_t node, double *total_energy_consumed);
int get_pkg_rapl_parameters_t(uint64_t node, pkg_rapl_parameters_t *rapl_parameters);
int get_pkg_accumulated_throttled_time(uint64_t node, double *accumulated_throttled_time_seconds);
int set_pkg_rapl_power_limit_control_t(uint64_t node, pkg_rapl_power_limit_control_t *rapl_power_limit_control);

/*! \brief RAPL power limit control structure, DRAM domain */
typedef struct dram_rapl_power_limit_control_t {
    double   power_limit_watts;
    double   limit_time_window_seconds;
    uint64_t limit_enabled;
    uint64_t clamp_enabled;
    uint64_t lock_enabled;
} dram_rapl_power_limit_control_t;

/*! \brief RAPL parameters info structure, DRAM domain */
typedef struct dram_rapl_parameters_t {
    double thermal_spec_power_watts;
    double minimum_power_watts;
    double maximum_power_watts;
    double maximum_limit_time_window_seconds;
} dram_rapl_parameters_t;
int get_dram_rapl_power_limit_control_t(uint64_t node, dram_rapl_power_limit_control_t *rapl_power_limit_control);
int get_dram_total_energy_consumed(uint64_t node, double *total_energy_consumed);
int get_dram_rapl_parameters_t(uint64_t node, dram_rapl_parameters_t *rapl_parameters);
int get_dram_accumulated_throttled_time(uint64_t node, double *accumulated_throttled_time_seconds);
int set_dram_rapl_power_limit_control_t(uint64_t node, dram_rapl_power_limit_control_t *rapl_power_limit_control);


/*! \brief RAPL power limit control structure, PP0 domain */
typedef struct pp0_rapl_power_limit_control_t {
    double   power_limit_watts;
    double   limit_time_window_seconds;
    uint64_t limit_enabled;
    uint64_t clamp_enabled;
    uint64_t lock_enabled;
} pp0_rapl_power_limit_control_t;
int get_pp0_rapl_power_limit_control_t(uint64_t node, pp0_rapl_power_limit_control_t *rapl_power_limit_control);
int get_pp0_total_energy_consumed(uint64_t node, double *total_energy_consumed);
int get_pp0_balance_policy(uint64_t node, uint64_t *priority_level);
int get_pp0_accumulated_throttled_time(uint64_t node, double *accumulated_throttled_time_seconds);
int set_pp0_rapl_power_limit_control_t(uint64_t node, pp0_rapl_power_limit_control_t *rapl_power_limit_control);
int set_pp0_balance_policy(uint64_t node, uint64_t priority_level);


/*! \brief RAPL power limit control structure, PP1 domain */
typedef struct pp1_rapl_power_limit_control_t {
    double   power_limit_watts;
    double   limit_time_window_seconds;
    uint64_t limit_enabled;
    uint64_t clamp_enabled;
    uint64_t lock_enabled;
} pp1_rapl_power_limit_control_t;
int get_pp1_rapl_power_limit_control_t(uint64_t node, pp1_rapl_power_limit_control_t *rapl_power_limit_control);
int get_pp1_total_energy_consumed(uint64_t node, double *total_energy_consumed);
int get_pp1_balance_policy(uint64_t node, uint64_t *priority_level);
int set_pp1_rapl_power_limit_control_t(uint64_t node, pp1_rapl_power_limit_control_t *rapl_power_limit_control);
int set_pp1_balance_policy(uint64_t node, uint64_t priority_level);

/* Utilities */

/*! \brief Use the RDTSC instruction to read the time-stamp counter */
int read_tsc(uint64_t *tsc);

/* Required by Power Gadget */
int get_pp0_freq_mhz(uint64_t node, uint64_t *freq);

#endif
/*
Copyright (c) 2012, Intel Corporation

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Written by Martin Dimitrov, Carl Strickland
 * http://software.intel.com/en-us/articles/power-gov */



/*! \file rapl.c
 * Intel(r) Power Governor library implementation
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <sched.h>
#include <unistd.h>


char* RAPL_DOMAIN_STRINGS[RAPL_NR_DOMAIN] = {
    "package",
    "core",
    "uncore",
    "dram"
};


/* rapl msr availablility */
#define MSR_SUPPORT_MASK 0xff
unsigned char *msr_support_table;

/* Global Variables */
double RAPL_TIME_UNIT;
double RAPL_ENERGY_UNIT;
double RAPL_POWER_UNIT;

uint64_t  num_nodes = 0;
uint64_t num_core_threads = 0; // number of physical threads per core
uint64_t num_pkg_threads = 0;  // number of physical threads per package
uint64_t num_pkg_cores = 0;    // number of cores per package
uint64_t os_cpu_count = 0;     // numbeer of OS cpus

APIC_ID_t *os_map;
APIC_ID_t **pkg_map;

/* Pre-computed variables used for time-window calculation */
const double LN2 = 0.69314718055994530941723212145817656807550013436025;
const double A_F[4] = { 1.0, 1.1, 1.2, 1.3 };
const double A_LNF[4] = {
    0.0000000000000000000000000000000000000000000000000000000,
    0.0953101798043249348602046211453853175044059753417968750,
    0.1823215567939545922460098381634452380239963531494140625,
    0.2623642644674910595625760834082029759883880615234375000
};

typedef struct rapl_unit_multiplier_t {
    double power;
    double energy;
    double time;
} rapl_unit_multiplier_t;

typedef struct rapl_power_limit_control_t {
    double       power_limit_watts;
    double       limit_time_window_seconds;
    uint64_t limit_enabled;
    uint64_t clamp_enabled;
    uint64_t lock_enabled;
} rapl_power_limit_control_t;

typedef struct rapl_parameters_t {
    double thermal_spec_power_watts;
    double minimum_power_watts;
    double maximum_power_watts;
    double maximum_limit_time_window_seconds;
} rapl_parameters_t;

// Use numactl in order to find out how many nodes are in the system
// and assign a cpu per node. The reason is that we
//
// Ideally we could include <numa.h> and use the library calls.
// However, I found that numactl-devel is not included by default
// in SLES11.1, which would make it harder to setup the tool.
// This is uglier, but hopefully everyone has numacta
uint64_t  get_num_rapl_nodes_pkg();
uint64_t  get_num_rapl_nodes_pkg();
uint64_t  get_num_rapl_nodes_pkg();

// OS specific
int
bind_context(cpu_set_t *new_context, cpu_set_t *old_context) {

    int err = 0;
    int ret = 0;

    if (old_context != NULL) {
      err = sched_getaffinity(0, sizeof(cpu_set_t), old_context);
      if(0 != err)
        ret = MY_ERROR;
    }

    err += sched_setaffinity(0, sizeof(cpu_set_t), new_context);
    if(0 != err)
        ret = MY_ERROR;

    return ret;
}

int
bind_cpu(uint64_t cpu, cpu_set_t *old_context) {

    int err = 0;
    cpu_set_t cpu_context;

    CPU_ZERO(&cpu_context);
    CPU_SET(cpu, &cpu_context);
    err += bind_context(&cpu_context, old_context);

    return err;
}

// Parse the x2APIC_ID_t into SMT, core and package ID.
// http://software.intel.com/en-us/articles/intel-64-architecture-processor-topology-enumeration
void
parse_apic_id(cpuid_info_t info_l0, cpuid_info_t info_l1, APIC_ID_t *my_id){

    // Get the SMT ID
    uint64_t smt_mask_width = info_l0.eax & 0x1f;
    uint64_t smt_mask = ~((-1) << smt_mask_width);
    my_id->smt_id = info_l0.edx & smt_mask;

    // Get the core ID
    uint64_t core_mask_width = info_l1.eax & 0x1f;
    uint64_t core_mask = (~((-1) << core_mask_width ) ) ^ smt_mask;
    my_id->core_id = (info_l1.edx & core_mask) >> smt_mask_width;

    // Get the package ID
    uint64_t pkg_mask = (-1) << core_mask_width;
    my_id->pkg_id = (info_l1.edx & pkg_mask) >> core_mask_width;
}



// For documentation, see:
// http://software.intel.com/en-us/articles/intel-64-architecture-processor-topology-enumeration
int
build_topology() {

    int err;
    uint64_t i,j;
    uint64_t max_pkg = 0;
    os_cpu_count = sysconf(_SC_NPROCESSORS_CONF);
    cpu_set_t curr_context;
    cpu_set_t prev_context;

    // Construct an os map: os_map[APIC_ID ... APIC_ID]
    os_map = (APIC_ID_t *) malloc(os_cpu_count * sizeof(APIC_ID_t));

    for(i=0; i < os_cpu_count; i++){

        err = bind_cpu(i, &prev_context);

        cpuid_info_t info_l0 = get_processor_topology(0);
        cpuid_info_t info_l1 = get_processor_topology(1);

        os_map[i].os_id = i;
        parse_apic_id(info_l0, info_l1, &os_map[i]);

        num_core_threads = info_l0.ebx & 0xffff;
        num_pkg_threads = info_l1.ebx & 0xffff;

        if(os_map[i].pkg_id > max_pkg)
            max_pkg = os_map[i].pkg_id;

        err = bind_context(&prev_context, NULL);

        //printf("smt_id: %u core_id: %u pkg_id: %u os_id: %u\n",
        //   os_map[i].smt_id, os_map[i].core_id, os_map[i].pkg_id, os_map[i].os_id);

    }

    num_pkg_cores = num_pkg_threads / num_core_threads;
    num_nodes = max_pkg + 1;

    // Construct a pkg map: pkg_map[pkg id][APIC_ID ... APIC_ID]
    pkg_map = (APIC_ID_t **) malloc(num_nodes * sizeof(APIC_ID_t*));
    for(i = 0; i < num_nodes; i++)
        pkg_map[i] = (APIC_ID_t *) malloc(num_pkg_threads * sizeof(APIC_ID_t));

    uint64_t p, t;
    for(i = 0; i < os_cpu_count; i++){
        p = os_map[i].pkg_id;
        t = os_map[i].smt_id * num_pkg_cores + os_map[i].core_id;
        pkg_map[p][t] = os_map[i];
    }

    //for(i=0; i< num_nodes; i++)
    //    for(j=0; j<num_pkg_threads; j++)
    //        printf("smt_id: %u core_id: %u pkg_id: %u os_id: %u\n",
    //            pkg_map[i][j].smt_id, pkg_map[i][j].core_id,
    //            pkg_map[i][j].pkg_id, pkg_map[i][j].os_id);

    return err;
}

/*!
 * \brief Intialize the power_gov library for use.
 *
 * This function must be called before calling any other function from the power_gov library.
 * \return 0 on success, -1 otherwise
 */
int
init_rapl()
{
    int      err = 0;
    uint32_t processor_signature;

    processor_signature = get_processor_signature();
    msr_support_table = (unsigned char*) calloc(MSR_SUPPORT_MASK, sizeof(unsigned char));

    /* RAPL MSRs by Table
     *   35-11: SandyBridge
     *     MSR_RAPL_POWER_UNIT  MSR_PKG_POWER_LIMIT   MSR_PKG_ENERGY_STATUS
     *     MSR_PKG_POWER_INFO   MSR_PP0_POWER_LIMIT   MSR_PP0_ENERGY_STATUS
     *     MSR_PP0_POLICY       MSR_PP0_PERF_STATUS
     *   35-12: SandyBridge Client
     *     MSR_PP1_POWER_LIMIT  MSR_PP1_ENERGY_STATUS MSR_PP1_POLICY
     *   35-13: SandyBridge Server
     *     MSR_PKG_PERF_STATUS  MSR_DRAM_POWER_LIMIT  MSR_DRAM_ENERGY_STATUS
     *     MSR_DRAM_PERF_STATUS MSR_DRAM_POWER_INFO
     *   35-14: IvyBridge
     *     N/A
     *   35-15: IvyBridge Server
     *     MSR_PKG_PERF_STATUS  MSR_DRAM_POWER_LIMIT  MSR_DRAM_ENERGY_STATUS
     *     MSR_DRAM_PERF_STATUS MSR_DRAM_POWER_INFO
     *   35-16: IvyBridge Server
     *     N/A
     *   35-17: Haswell
     *     N/A
     *   35-18: Haswell
     *     N/A
     *   35-19: Haswell
     *     MSR_PP1_POWER_LIMIT  MSR_PP1_ENERGY_STATUS MSR_PP1_POLICY
     */

    switch (processor_signature & 0xfffffff0) {
    case 0x306e0: /* IvyBridge server: 0x306eX (Tables 35:11,12,14,15,16) */
        msr_support_table[MSR_RAPL_POWER_UNIT & MSR_SUPPORT_MASK]          = 1;
        msr_support_table[MSR_RAPL_PKG_POWER_LIMIT & MSR_SUPPORT_MASK]     = 1;
        msr_support_table[MSR_RAPL_PKG_ENERGY_STATUS & MSR_SUPPORT_MASK]   = 1;
        msr_support_table[MSR_RAPL_PKG_PERF_STATUS & MSR_SUPPORT_MASK]     = 1;
        msr_support_table[MSR_RAPL_PKG_POWER_INFO & MSR_SUPPORT_MASK]      = 1;
        msr_support_table[MSR_RAPL_DRAM_POWER_LIMIT & MSR_SUPPORT_MASK]    = 1;
        msr_support_table[MSR_RAPL_DRAM_ENERGY_STATUS & MSR_SUPPORT_MASK]  = 1;
        msr_support_table[MSR_RAPL_DRAM_PERF_STATUS & MSR_SUPPORT_MASK]    = 1;
        msr_support_table[MSR_RAPL_DRAM_POWER_INFO & MSR_SUPPORT_MASK]     = 1;
        msr_support_table[MSR_RAPL_PP0_POWER_LIMIT & MSR_SUPPORT_MASK]     = 1;
        msr_support_table[MSR_RAPL_PP0_ENERGY_STATUS & MSR_SUPPORT_MASK]   = 1;
        msr_support_table[MSR_RAPL_PP0_POLICY & MSR_SUPPORT_MASK]          = 1;
        msr_support_table[MSR_RAPL_PP0_PERF_STATUS & MSR_SUPPORT_MASK]     = 1;
        msr_support_table[MSR_RAPL_PP1_POWER_LIMIT & MSR_SUPPORT_MASK]     = 1;
        msr_support_table[MSR_RAPL_PP1_ENERGY_STATUS & MSR_SUPPORT_MASK]   = 1;
        msr_support_table[MSR_RAPL_PP1_POLICY & MSR_SUPPORT_MASK]          = 1;
        break;
    case 0x40660: /* Haswell:            0x4066X (Tables 35:11,12,14,17,19) */
    case 0x40650: /* Haswell:            0x4065X (Tables 35:11,12,14,17,18,19) */
    case 0x306c0: /* Haswell:            0x306cX (Tables 35:11,12,14,17,19) */
    case 0x306a0: /* IvyBridge client:   0x306aX (Tables 35:11,12,14) */
    case 0x206a0: /* SandyBridge client: 0x206aX (Tables 35:11,12) */
        msr_support_table[MSR_RAPL_POWER_UNIT & MSR_SUPPORT_MASK]          = 1;
        msr_support_table[MSR_RAPL_PKG_POWER_LIMIT & MSR_SUPPORT_MASK]     = 1;
        msr_support_table[MSR_RAPL_PKG_ENERGY_STATUS & MSR_SUPPORT_MASK]   = 1;
        msr_support_table[MSR_RAPL_PKG_PERF_STATUS & MSR_SUPPORT_MASK]     = 0; //
        msr_support_table[MSR_RAPL_PKG_POWER_INFO & MSR_SUPPORT_MASK]      = 1;
        msr_support_table[MSR_RAPL_DRAM_POWER_LIMIT & MSR_SUPPORT_MASK]    = 0; //
        msr_support_table[MSR_RAPL_DRAM_ENERGY_STATUS & MSR_SUPPORT_MASK]  = 0; //
        msr_support_table[MSR_RAPL_DRAM_PERF_STATUS & MSR_SUPPORT_MASK]    = 0; //
        msr_support_table[MSR_RAPL_DRAM_POWER_INFO & MSR_SUPPORT_MASK]     = 0; //
        msr_support_table[MSR_RAPL_PP0_POWER_LIMIT & MSR_SUPPORT_MASK]     = 1;
        msr_support_table[MSR_RAPL_PP0_ENERGY_STATUS & MSR_SUPPORT_MASK]   = 1;
        msr_support_table[MSR_RAPL_PP0_POLICY & MSR_SUPPORT_MASK]          = 1;
        msr_support_table[MSR_RAPL_PP0_PERF_STATUS & MSR_SUPPORT_MASK]     = 1;
        msr_support_table[MSR_RAPL_PP1_POWER_LIMIT & MSR_SUPPORT_MASK]     = 1; //
        msr_support_table[MSR_RAPL_PP1_ENERGY_STATUS & MSR_SUPPORT_MASK]   = 1; //
        msr_support_table[MSR_RAPL_PP1_POLICY & MSR_SUPPORT_MASK]          = 1; //
        break;
    //case 0x20650: /* Valgrind */
    case 0x206d0: /* SandyBridge server: 0x206dX (Tables 35:11,13) */
        msr_support_table[MSR_RAPL_POWER_UNIT & MSR_SUPPORT_MASK]          = 1;
        msr_support_table[MSR_RAPL_PKG_POWER_LIMIT & MSR_SUPPORT_MASK]     = 1;
        msr_support_table[MSR_RAPL_PKG_ENERGY_STATUS & MSR_SUPPORT_MASK]   = 1;
        msr_support_table[MSR_RAPL_PKG_PERF_STATUS & MSR_SUPPORT_MASK]     = 1; //
        msr_support_table[MSR_RAPL_PKG_POWER_INFO & MSR_SUPPORT_MASK]      = 1;
        msr_support_table[MSR_RAPL_DRAM_POWER_LIMIT & MSR_SUPPORT_MASK]    = 1; //
        msr_support_table[MSR_RAPL_DRAM_ENERGY_STATUS & MSR_SUPPORT_MASK]  = 1; //
        msr_support_table[MSR_RAPL_DRAM_PERF_STATUS & MSR_SUPPORT_MASK]    = 1; //
        msr_support_table[MSR_RAPL_DRAM_POWER_INFO & MSR_SUPPORT_MASK]     = 1; //
        msr_support_table[MSR_RAPL_PP0_POWER_LIMIT & MSR_SUPPORT_MASK]     = 1;
        msr_support_table[MSR_RAPL_PP0_ENERGY_STATUS & MSR_SUPPORT_MASK]   = 1;
        msr_support_table[MSR_RAPL_PP0_POLICY & MSR_SUPPORT_MASK]          = 1;
        msr_support_table[MSR_RAPL_PP0_PERF_STATUS & MSR_SUPPORT_MASK]     = 1;
        msr_support_table[MSR_RAPL_PP1_POWER_LIMIT & MSR_SUPPORT_MASK]     = 0; //
        msr_support_table[MSR_RAPL_PP1_ENERGY_STATUS & MSR_SUPPORT_MASK]   = 0; //
        msr_support_table[MSR_RAPL_PP1_POLICY & MSR_SUPPORT_MASK]          = 0; //
        break;
    default:
        fprintf(stderr, "RAPL not supported, or machine model %x not recognized.\n", processor_signature);
        return MY_ERROR;
    }

    err = read_rapl_units();
    err += build_topology();

    /* 32 is the width of these fields when they are stored */
    MAX_ENERGY_STATUS_JOULES = (double)(RAPL_ENERGY_UNIT * (pow(2, 32) - 1));
    MAX_THROTTLED_TIME_SECONDS = (double)(RAPL_TIME_UNIT * (pow(2, 32) - 1));

    return err;
}

/*!
 * \brief Terminate the power_gov library.
 *
 * Call this function function to cleanup resources and terminate the
 * power_gov library.
 * \return 0 on success
 */
int
terminate_rapl()
{
    uint64_t i;

    if(NULL != os_map)
        free(os_map);

    if(NULL != pkg_map){
        for(i = 0; i < num_nodes; i++)
            free(pkg_map[i]);
        free(pkg_map);
    }

    if(NULL != msr_support_table)
        free(msr_support_table);

    return 0;
}

/*!
 * \brief Check if MSR is supported on this machine.
 * \return 1 if supported, 0 otherwise
 */
uint64_t
is_supported_msr(uint64_t msr)
{
    return (uint64_t)msr_support_table[msr & MSR_SUPPORT_MASK];
}

/*!
 * \brief Check if power domain (PKG, PP0, PP1, DRAM) is supported on this machine.
 *
 * Currently server parts support: PKG, PP0 and DRAM and
 * client parts support PKG, PP0 and PP1.
 *
 * \return 1 if supported, 0 otherwise
 */
uint64_t
is_supported_domain(uint64_t power_domain)
{
    uint64_t supported = 0;

    switch (power_domain) {
    case RAPL_PKG:
        supported = is_supported_msr(MSR_RAPL_PKG_POWER_LIMIT);
        break;
    case RAPL_PP0:
        supported = is_supported_msr(MSR_RAPL_PP0_POWER_LIMIT);
        break;
    case RAPL_PP1:
        supported = is_supported_msr(MSR_RAPL_PP1_POWER_LIMIT);
        break;
    case RAPL_DRAM:
        supported = is_supported_msr(MSR_RAPL_DRAM_POWER_LIMIT);
        break;
    }

    return supported;
}

/*!
 * \brief Get the number of RAPL nodes (package domain) on this machine.
 *
 * Get the number of package power domains, that you can control using RAPL.
 * This is equal to the number of CPU packages in the system.
 *
 * \return number of RAPL nodes.
 */
uint64_t
get_num_rapl_nodes_pkg()
{
    return num_nodes;
}

/*!
 * \brief Get the number of RAPL nodes (pp0 domain) on this machine.
 *
 * Get the number of pp0 (core) power domains, that you can control
 * using RAPL. Currently all the cores on a package belong to the same
 * power domain, so currently this is equal to the number of CPU packages in
 * the system.
 *
 * \return number of RAPL nodes.
 */
uint64_t
get_num_rapl_nodes_pp0()
{
    return num_nodes;
}

/*!
 * \brief Get the number of RAPL nodes (pp1 domain) on this machine.
 *
 * Get the number of pp1(uncore) power domains, that you can control using RAPL.
 * This is equal to the number of CPU packages in the system.
 *
 * \return number of RAPL nodes.
 */
uint64_t
get_num_rapl_nodes_pp1()
{
    return num_nodes;
}

/*!
 * \brief Get the number of RAPL nodes (DRAM domain) on this machine.
 *
 * Get the number of DRAM power domains, that you can control using RAPL.
 * This is equal to the number of CPU packages in the system.
 *
 * \return number of RAPL nodes.
 */
uint64_t
get_num_rapl_nodes_dram()
{
    return num_nodes;
}

uint64_t
pkg_node_to_cpu(uint64_t node)
{
    return pkg_map[node][0].os_id;
}

uint64_t
pp0_node_to_cpu(uint64_t node)
{
    return pkg_map[node][0].os_id;
}

uint64_t
pp1_node_to_cpu(uint64_t node)
{
    return pkg_map[node][0].os_id;
}

uint64_t
dram_node_to_cpu(uint64_t node)
{
    return pkg_map[node][0].os_id;
}

double
convert_to_watts(uint64_t raw)
{
    return RAPL_POWER_UNIT * raw;
}

double
convert_to_joules(uint64_t raw)
{
    return RAPL_ENERGY_UNIT * raw;
}

double
convert_to_seconds(uint64_t raw)
{
    return RAPL_TIME_UNIT * raw;
}

double
convert_from_limit_time_window(uint64_t Y,
                               uint64_t F)
{
    return B2POW(Y) * A_F[F] * RAPL_TIME_UNIT;
}

uint64_t
convert_from_watts(double converted)
{
    return converted / RAPL_POWER_UNIT;
}

uint64_t
compute_Y(uint64_t F,
          double   time)
{
    return (log((double)(time / RAPL_TIME_UNIT)) - A_LNF[F]) / LN2;
}

void
convert_to_limit_time_window(double    time,
                             uint64_t *Y,
                             uint64_t *F)
{
    uint64_t current_Y = 0;
    uint64_t current_F = 0;
    double       current_time = 0.0;
    double       current_delta = 0.0;
    double       delta = 2147483648.0;
    for (current_F = 0; current_F < 4; ++current_F) {
        current_Y = compute_Y(current_F, time);
        current_time = convert_from_limit_time_window(current_Y, current_F);
        current_delta = time - current_time;
        if (current_delta >= 0 && current_delta < delta) {
            delta = current_delta;
            *F = current_F;
            *Y = current_Y;
        }
    }
}

int
get_rapl_unit_multiplier(uint64_t                cpu,
                         rapl_unit_multiplier_t *unit_obj)
{
    int                        err = 0;
    uint64_t                   msr;
    rapl_unit_multiplier_msr_t unit_msr;

    err = !is_supported_msr(MSR_RAPL_POWER_UNIT);
    if (!err) {
        err = read_msr(cpu, MSR_RAPL_POWER_UNIT, &msr);
    }
    if (!err) {
        unit_msr = *(rapl_unit_multiplier_msr_t *)&msr;

        unit_obj->time = 1.0 / (double)(B2POW(unit_msr.time));
        unit_obj->energy = 1.0 / (double)(B2POW(unit_msr.energy));
        unit_obj->power = 1.0 / (double)(B2POW(unit_msr.power));
    }

    return err;
}

/* Common methods (should not be interfaced directly) */

int
get_rapl_power_limit_control(uint64_t                    cpu,
                             uint64_t                    msr_address,
                             rapl_power_limit_control_t *domain_obj)
{
    int                            err = 0;
    uint64_t                       msr;
    rapl_power_limit_control_msr_t domain_msr;
    cpu_set_t old_context;

    err = !is_supported_msr(msr_address);
    if (!err) {
        bind_cpu(cpu, &old_context); // improve performance on Linux
        err = read_msr(cpu, msr_address, &msr);
        bind_context(&old_context, NULL);
    }

    if (!err) {
        domain_msr = *(rapl_power_limit_control_msr_t *)&msr;

        domain_obj->power_limit_watts = convert_to_watts(domain_msr.power_limit);
        domain_obj->limit_time_window_seconds = convert_from_limit_time_window(domain_msr.limit_time_window_y,
                                                domain_msr.limit_time_window_f);
        domain_obj->limit_enabled = domain_msr.limit_enabled;
        domain_obj->clamp_enabled = domain_msr.clamp_enabled;
        domain_obj->lock_enabled = domain_msr.lock_enabled;
    }

    return err;
}

int
get_total_energy_consumed(uint64_t  cpu,
                          uint64_t  msr_address,
                          double   *total_energy_consumed_joules)
{
    int                 err = 0;
    uint64_t            msr;
    energy_status_msr_t domain_msr;
    cpu_set_t old_context;

    err = !is_supported_msr(msr_address);
    if (!err) {
        bind_cpu(cpu, &old_context); // improve performance on Linux
        err = read_msr(cpu, msr_address, &msr);
        bind_context(&old_context, NULL);
    }

    if(!err) {
        domain_msr = *(energy_status_msr_t *)&msr;

        *total_energy_consumed_joules = convert_to_joules(domain_msr.total_energy_consumed);
    }

    return err;
}

int
get_rapl_parameters(uint64_t           cpu,
                    uint64_t           msr_address,
                    rapl_parameters_t *domain_obj)
{
    int                   err = 0;
    uint64_t              msr;
    rapl_parameters_msr_t domain_msr;
    cpu_set_t old_context;

    err = !is_supported_msr(msr_address);
    if (!err) {
        bind_cpu(cpu, &old_context); // improve performance on Linux
        err = read_msr(cpu, msr_address, &msr);
        bind_context(&old_context, NULL);
    }

    if (!err) {
        domain_msr = *(rapl_parameters_msr_t *)&msr;

        domain_obj->thermal_spec_power_watts = convert_to_watts(domain_msr.thermal_spec_power);
        domain_obj->minimum_power_watts = convert_to_watts(domain_msr.minimum_power);
        domain_obj->maximum_power_watts = convert_to_watts(domain_msr.maximum_power);
        domain_obj->maximum_limit_time_window_seconds = convert_to_seconds(domain_msr.maximum_limit_time_window);
    }

    return err;
}

int
get_accumulated_throttled_time(uint64_t  cpu,
                               uint64_t  msr_address,
                               double   *accumulated_throttled_time_seconds)
{
    int                                 err = 0;
    uint64_t                            msr;
    performance_throttling_status_msr_t domain_msr;
    cpu_set_t old_context;

    err = !is_supported_msr(msr_address);
    if (!err) {
        bind_cpu(cpu, &old_context); // improve performance on Linux
        err = read_msr(cpu, msr_address, &msr);
        bind_context(&old_context, NULL);
    }

    if (!err) {
        domain_msr = *(performance_throttling_status_msr_t *)&msr;

        *accumulated_throttled_time_seconds = convert_to_seconds(domain_msr.accumulated_throttled_time);
    }

    return err;
}

int
get_balance_policy(uint64_t  cpu,
                   uint64_t  msr_address,
                   uint64_t *priority_level)
{
    int                  err = 0;
    uint64_t             msr;
    balance_policy_msr_t domain_msr;
    cpu_set_t old_context;

    err = !is_supported_msr(msr_address);
    if (!err) {
        bind_cpu(cpu, &old_context); // improve performance on Linux
        err = read_msr(cpu, msr_address, &msr);
        bind_context(&old_context, NULL);
    }

    if(!err) {
        domain_msr = *(balance_policy_msr_t *)&msr;

        *priority_level = domain_msr.priority_level;
    }

    return err;
}

int
set_rapl_power_limit_control(uint64_t                    cpu,
                             uint64_t                    msr_address,
                             rapl_power_limit_control_t *domain_obj)
{
    int                            err = 0;
    uint64_t                       msr;
    rapl_power_limit_control_msr_t domain_msr;
    cpu_set_t old_context;

    uint64_t y;
    uint64_t f;

    err = !is_supported_msr(msr_address);
    if (!err) {
        bind_cpu(cpu, &old_context); // improve performance on Linux
        err = read_msr(cpu, msr_address, &msr);
        bind_context(&old_context, NULL);
    }

    if (!err) {
        domain_msr = *(rapl_power_limit_control_msr_t *)&msr;

        domain_msr.power_limit = convert_from_watts(domain_obj->power_limit_watts);
        domain_msr.limit_enabled = domain_obj->limit_enabled;
        domain_msr.clamp_enabled = domain_obj->clamp_enabled;
        convert_to_limit_time_window(domain_obj->limit_time_window_seconds, &y, &f);
        domain_msr.limit_time_window_y = y;
        domain_msr.limit_time_window_f = f;
        domain_msr.lock_enabled = domain_obj->lock_enabled;

        msr = *(uint64_t *)&domain_msr;
        err = write_msr(cpu, msr_address, msr);
    }

    return err;
}

int
set_balance_policy(uint64_t cpu,
                   uint64_t msr_address,
                   uint64_t priority_level)
{
    int                  err = 0;
    uint64_t             msr;
    balance_policy_msr_t domain_msr;
    cpu_set_t old_context;

    err = !is_supported_msr(msr_address);
    if (!err) {
        bind_cpu(cpu, &old_context); // improve performance on Linux
        err = read_msr(cpu, msr_address, &msr);
        bind_context(&old_context, NULL);
    }

    if(!err) {
        domain_msr = *(balance_policy_msr_t *)&msr;

        domain_msr.priority_level = priority_level;

        msr = *(uint64_t *)&domain_msr;
        err = write_msr(cpu, msr_address, msr);
    }

    return err;
}


/* Interface */

/* PKG */

/*!
 * \brief Get a pointer to the RAPL PKG power-limit control register (pkg_rapl_power_limit_control_t).
 *
 * Use the RAPL PKG power-limit control register in order to define power limiting
 * policies on the package power domain.
 * Modify the components of pkg_rapl_power_limit_control_t in order to describe your
 * power limiting policy. Then enforce your new policy using set_pkg_rapl_power_limit_control.
 * At minimum, you should set:
 * - power_limit_watts_1, the power limit to enforce.
 * - limit_enabled_1, enable/disable the power limit.
 * - clamp_enabled_1, when set RAPL is able to overwrite OS requested frequency (full RAPL control).
 *
 * Optionally, you can tune:
 * - limit_time_window_seconds_1, the time slice granularity over which RAPL enforces the power limit
 *
 * \return 0 on success, -1 otherwise
 */
int
get_pkg_rapl_power_limit_control(uint64_t                        node,
                                 pkg_rapl_power_limit_control_t *pkg_obj)
{
    int                                err = 0;
    uint64_t                           msr;
    uint64_t cpu = pkg_node_to_cpu(node);
    pkg_rapl_power_limit_control_msr_t pkg_msr;
    cpu_set_t old_context;

    err = !is_supported_msr(MSR_RAPL_PKG_POWER_LIMIT);
    if (!err) {
        bind_cpu(cpu, &old_context); // improve performance on Linux
        err = read_msr(cpu, MSR_RAPL_PKG_POWER_LIMIT, &msr);
        bind_context(&old_context, NULL);
    }

    if (!err) {
        pkg_msr = *(pkg_rapl_power_limit_control_msr_t *)&msr;

        pkg_obj->power_limit_watts_1 = convert_to_watts(pkg_msr.power_limit_1);
        pkg_obj->limit_time_window_seconds_1 = convert_from_limit_time_window(pkg_msr.limit_time_window_y_1, pkg_msr.limit_time_window_f_1);
        pkg_obj->limit_enabled_1 = pkg_msr.limit_enabled_1;
        pkg_obj->clamp_enabled_1 = pkg_msr.clamp_enabled_1;
        pkg_obj->power_limit_watts_2 = convert_to_watts(pkg_msr.power_limit_2);
        pkg_obj->limit_time_window_seconds_2 = convert_from_limit_time_window(pkg_msr.limit_time_window_y_2, pkg_msr.limit_time_window_f_2);
        pkg_obj->limit_enabled_2 = pkg_msr.limit_enabled_2;
        pkg_obj->clamp_enabled_2 = pkg_msr.clamp_enabled_2;
        pkg_obj->lock_enabled = pkg_msr.lock_enabled;
    }

    return err;
}

/*!
 * \brief Get a pointer to the RAPL PKG energy consumed register.
 *
 * This read-only register provides energy consumed in joules
 * for the package power domain since the last machine reboot (or energy register wraparound)
 *
 * \return 0 on success, -1 otherwise
 */
int
get_pkg_total_energy_consumed(uint64_t  node,
                              double   *total_energy_consumed_joules)
{
    uint64_t cpu = pkg_node_to_cpu(node);
    return get_total_energy_consumed(cpu, MSR_RAPL_PKG_ENERGY_STATUS, total_energy_consumed_joules);
}

/*!
 * \brief Get a pointer to the RAPL PKG power info register
 *
 * This read-only register provides information about
 * the max/min power limiting settings available on the machine.
 * This register is defined in the pkg_rapl_parameters_t data structure.
 *
 * \return 0 on success, -1 otherwise
 */
int
get_pkg_rapl_parameters(uint64_t               node,
                        pkg_rapl_parameters_t *pkg_obj)
{
    uint64_t cpu = pkg_node_to_cpu(node);
    return get_rapl_parameters(cpu, MSR_RAPL_PKG_POWER_INFO, (rapl_parameters_t*)pkg_obj);
}

/*!
 * \brief Get a pointer to the RAPL PKG throttled time register
 *
 * This read-only register provides information about the amount of time,
 * that a RAPL power limiting policy throttled processor speed in order
 * to prevent a package power limit from being violated.
 *
 * \return 0 on success, -1 otherwise
 */
int
get_pkg_accumulated_throttled_time(uint64_t  node,
                                   double   *accumulated_throttled_time_seconds)
{
    uint64_t cpu = pkg_node_to_cpu(node);
    return get_accumulated_throttled_time(cpu, MSR_RAPL_PKG_PERF_STATUS, accumulated_throttled_time_seconds);
}

/*!
 * \brief Write the RAPL PKG power-limit control register (pkg_rapl_power_limit_control_t).
 *
 * Write the RAPL PKG power-limit control register in order to define power limiting
 * policies on the package power domain.
 *
 * \return 0 on success, -1 otherwise
 */
int
set_pkg_rapl_power_limit_control(uint64_t                        node,
                                 pkg_rapl_power_limit_control_t *pkg_obj)
{
    int      err = 0;
    uint64_t msr;
    uint64_t cpu = pkg_node_to_cpu(node);
    pkg_rapl_power_limit_control_msr_t pkg_msr;
    cpu_set_t old_context;

    uint64_t y;
    uint64_t f;

    err = !is_supported_msr(MSR_RAPL_PKG_POWER_LIMIT);
    if (!err) {
        bind_cpu(cpu, &old_context); // improve performance on Linux
        err = read_msr(cpu, MSR_RAPL_PKG_POWER_LIMIT, &msr);
        bind_context(&old_context, NULL);
    }

    if(!err) {
        pkg_msr = *(pkg_rapl_power_limit_control_msr_t *)&msr;

        pkg_msr.power_limit_1 = convert_from_watts(pkg_obj->power_limit_watts_1);
        pkg_msr.limit_enabled_1 = pkg_obj->limit_enabled_1;
        pkg_msr.clamp_enabled_1 = pkg_obj->clamp_enabled_1;
        convert_to_limit_time_window(pkg_obj->limit_time_window_seconds_1, &y, &f);
        pkg_msr.limit_time_window_y_1 = y;
        pkg_msr.limit_time_window_f_1 = f;
        pkg_msr.power_limit_2 = convert_from_watts(pkg_obj->power_limit_watts_2);
        pkg_msr.limit_enabled_2 = pkg_obj->limit_enabled_2;
        pkg_msr.clamp_enabled_2 = pkg_obj->clamp_enabled_2;
        convert_to_limit_time_window(pkg_obj->limit_time_window_seconds_2, &y, &f);
        pkg_msr.limit_time_window_y_2 = y;
        pkg_msr.limit_time_window_f_2 = f;
        pkg_msr.lock_enabled = pkg_obj->lock_enabled;

        msr = *(uint64_t *)&pkg_msr;
        err = write_msr(cpu, MSR_RAPL_PKG_POWER_LIMIT, msr);
    }

    return err;
}


/* DRAM */

/*!
 * \brief Get a pointer to the RAPL DRAM power-limit control register (dram_rapl_power_limit_control_t).
 *
 * (Server parts only)
 *
 * Use the RAPL DRAM power-limit control register in order to define power limiting
 * policies on the DRAM power domain.
 * Modify the components of dram_rapl_power_limit_control_t in order to describe your
 * power limiting policy. Then enforce your new policy using set_dram_rapl_power_limit_control
 *  At minimum, you should set:
 * - power_limit_watts, the power limit to enforce.
 * - limit_enabled, enable/disable the power limit.
 *
 * Optionally, you can tune:
 * - limit_time_window_seconds, the time slice granularity over which RAPL enforces the power limit
 *
 * \return 0 on success, -1 otherwise
 */
int
get_dram_rapl_power_limit_control(uint64_t                         node,
                                  dram_rapl_power_limit_control_t *dram_obj)
{
    uint64_t cpu = dram_node_to_cpu(node);
    return get_rapl_power_limit_control(cpu, MSR_RAPL_DRAM_POWER_LIMIT, (rapl_power_limit_control_t*)dram_obj);
}

/*!
 * \brief Get a pointer to the RAPL DRAM energy consumed register.
 *
 * (Server parts only)
 *
 * This read-only register provides energy consumed in joules
 * for the DRAM power domain since the last machine reboot (or energy register wraparound)
 *
 * \return 0 on success, -1 otherwise
 */
int
get_dram_total_energy_consumed(uint64_t  node,
                               double   *total_energy_consumed_joules)
{
    uint64_t cpu = dram_node_to_cpu(node);
    return get_total_energy_consumed(cpu, MSR_RAPL_DRAM_ENERGY_STATUS, total_energy_consumed_joules);
}

/*!
 * \brief Get a pointer to the RAPL DRAM power info register
 *
 * (Server parts only)
 *
 * This read-only register provides information about
 * the max/min power limiting settings available on the machine.
 * This register is defined in the dram_rapl_parameters_t data structure.
 *
 * \return 0 on success, -1 otherwise
 */
int
get_dram_rapl_parameters(uint64_t                node,
                         dram_rapl_parameters_t *dram_obj)
{
    uint64_t cpu = dram_node_to_cpu(node);
    return get_rapl_parameters(cpu, MSR_RAPL_DRAM_POWER_INFO, (rapl_parameters_t*)dram_obj);
}

/*!
 * \brief Get a pointer to the RAPL DRAM throttled time register
 *
 * (Server parts only)
 *
 * This read-only register provides information about the amount of time,
 * that a RAPL power limiting policy throttled DRAM bandwidth in order
 * to prevent a DRAM power limit from being violated.
 *
 * \return 0 on success, -1 otherwise
 */
int
get_dram_accumulated_throttled_time(uint64_t  node,
                                    double   *accumulated_throttled_time_seconds)
{
    uint64_t cpu = dram_node_to_cpu(node);
    return get_accumulated_throttled_time(cpu, MSR_RAPL_DRAM_PERF_STATUS, accumulated_throttled_time_seconds);
}

/*!
 * \brief Write the RAPL DRAM power-limit control register (dram_rapl_power_limit_control_t).
 *
 * (Server parts only)
 *
 * Write the RAPL DRAM power-limit control register in order to define power limiting
 * policies on the DRAM power domain.
 *
 * \return 0 on success, -1 otherwise
 */
int
set_dram_rapl_power_limit_control(uint64_t                         node,
                                  dram_rapl_power_limit_control_t *dram_obj)
{
    uint64_t cpu = dram_node_to_cpu(node);
    return set_rapl_power_limit_control(cpu, MSR_RAPL_DRAM_POWER_LIMIT, (rapl_power_limit_control_t*)dram_obj);
}


/* PP0 */

/*!
 * \brief Get a pointer to the RAPL PP0 power-limit control register (pp0_rapl_power_limit_control_t).
 *
 * Use the RAPL PP0 power-limit control register in order to define power limiting
 * policies on the PP0 (core) power domain.
 * Modify the components of pp0_rapl_power_limit_control_t in order to describe your
 * power limiting policy. Then enforce your new policy using set_pp0_rapl_power_limit_control
 *  At minimum, you should set:
 * - power_limit_watts, the power limit to enforce.
 * - limit_enabled, enable/disable the power limit.
 *
 * Optionally, you can tune:
 * - limit_time_window_seconds, the time slice granularity over which RAPL enforces the power limit
 *
 * \return 0 on success, -1 otherwise
 */
int
get_pp0_rapl_power_limit_control(uint64_t                        node,
                                 pp0_rapl_power_limit_control_t *pp0_obj)
{
    uint64_t cpu = pp0_node_to_cpu(node);
    return get_rapl_power_limit_control(cpu, MSR_RAPL_PP0_POWER_LIMIT, (rapl_power_limit_control_t*)pp0_obj);
}

/*!
 * \brief Get a pointer to the RAPL PP0 energy consumed register.
 *
 * This read-only register provides energy consumed in joules
 * for the PP0 (core) power domain since the last machine reboot (or energy register wraparound)
 *
 * \return 0 on success, -1 otherwise
 */
int
get_pp0_total_energy_consumed(uint64_t  node,
                              double   *total_energy_consumed_joules)
{
    uint64_t cpu = pp0_node_to_cpu(node);
    return get_total_energy_consumed(cpu, MSR_RAPL_PP0_ENERGY_STATUS, total_energy_consumed_joules);
}

/*!
 * \brief Get a pointer to the RAPL PP0 priority level register
 *
 * (Client parts only)
 *
 * Use the RAPL PP0 priority level register in order to provide an input
 * to the power budgeting algorithm on how to distribute power between the
 * PP0 (core) place and PP1 (uncore - graphics). The default value gives
 * priority to the uncore power plane. After modifying the register
 * enforce your setting using set_pp0_balance_policy.
 * The value 31 is considered highest priority and 0 lowest.
 *
 * \return 0 on success, -1 otherwise
 */
int
get_pp0_balance_policy(uint64_t  node,
                       uint64_t *priority_level)
{
    uint64_t cpu = pp0_node_to_cpu(node);
    return get_balance_policy(cpu, MSR_RAPL_PP0_POLICY, priority_level);
}

/*!
 * \brief Get a pointer to the RAPL PP0 throttled time register
 *
 * This read-only register provides information about the amount of time,
 * that a RAPL power limiting policy throttled processor speed in order
 * to prevent a PP0 (core) power limit from being violated.
 *
 * \return 0 on success, -1 otherwise
 */
int
get_pp0_accumulated_throttled_time(uint64_t  node,
                                   double   *accumulated_throttled_time_seconds)
{
    uint64_t cpu = pp0_node_to_cpu(node);
    return get_accumulated_throttled_time(cpu, MSR_RAPL_PP0_PERF_STATUS, accumulated_throttled_time_seconds);
}

/*!
 * \brief Write the RAPL PP0 power-limit control register (pp0_rapl_power_limit_control_t).
 *
 * Write the RAPL PP0 power-limit control register in order to define power limiting
 * policies on the PP0 power domain.
 *
 * \return 0 on success, -1 otherwise
 */
int
set_pp0_rapl_power_limit_control(uint64_t                        node,
                                 pp0_rapl_power_limit_control_t *pp0_obj)
{
    uint64_t cpu = pp0_node_to_cpu(node);
    return set_rapl_power_limit_control(cpu, MSR_RAPL_PP0_POWER_LIMIT, (rapl_power_limit_control_t*)pp0_obj);
}

/*!
 * \brief Write to the RAPL PP0 priority level register
 *
 * (Client parts only)
 *
 * Use the RAPL PP0 priority level register in order to provide an input
 * to the power budgeting algorithm on how to distribute power between the
 * PP0 (core) place and PP1 (uncore - graphics). The default value gives
 * priority to the uncore power plane.
 *
 * \return 0 on success, -1 otherwise
 */
int
set_pp0_balance_policy(uint64_t node,
                       uint64_t priority_level)
{
    uint64_t cpu = pp0_node_to_cpu(node);
    return set_balance_policy(cpu, MSR_RAPL_PP0_POLICY, priority_level);
}


/* PP1 */

/*!
 * \brief Get a pointer to the RAPL PP1 power-limit control register (pp1_rapl_power_limit_control_t).
 *
 * (Client parts only)
 *
 * Use the RAPL PP1 power-limit control register in order to define power limiting
 * policies on the PP1 (uncore) power domain.
 * Modify the components of pp1_rapl_power_limit_control_t in order to describe your
 * power limiting policy. Then enforce your new policy using set_pp1_rapl_power_limit_control
 *  At minimum, you should set:
 * - power_limit_watts, the power limit to enforce.
 * - limit_enabled, enable/disable the power limit.
 *
 * Optionally, you can tune:
 * - limit_time_window_seconds, the time slice granularity over which RAPL enforces the power limit
 *
 * \return 0 on success, -1 otherwise
 */
int
get_pp1_rapl_power_limit_control(uint64_t                        node,
                                 pp1_rapl_power_limit_control_t *pp1_obj)
{
    uint64_t cpu = pp1_node_to_cpu(node);
    return get_rapl_power_limit_control(cpu, MSR_RAPL_PP1_POWER_LIMIT, (rapl_power_limit_control_t*)pp1_obj);
}

/*!
 * \brief Get a pointer to the RAPL PP1 energy consumed register.
 *
 * (Client parts only)
 *
 * This read-only register provides energy consumed in joules
 * for the PP1 (uncore) power domain since the last machine reboot (or energy register wraparound)
 *
 * \return 0 on success, -1 otherwise
 */
int
get_pp1_total_energy_consumed(uint64_t  node,
                              double   *total_energy_consumed_joules)
{
    uint64_t cpu = pp1_node_to_cpu(node);
    return get_total_energy_consumed(cpu, MSR_RAPL_PP1_ENERGY_STATUS, total_energy_consumed_joules);
}

/*!
 * \brief Get a pointer to the RAPL PP1 priority level register
 *
 * (Client parts only)
 *
 * Use the RAPL PP1 priority level register in order to provide an input
 * to the power budgeting algorithm on how to distribute power between the
 * PP0 (core) place and PP1 (uncore - graphics). The default value gives
 * priority to the uncore power plane. After modifying the register
 * enforce your setting using set_pp1_balance_policy.
 * The value 31 is considered highest priority and 0 lowest.
 *
 * \return 0 on success, -1 otherwise
 */
int
get_pp1_balance_policy(uint64_t  node,
                       uint64_t *priority_level)
{
    uint64_t cpu = pp1_node_to_cpu(node);
    return get_balance_policy(cpu, MSR_RAPL_PP1_POLICY, priority_level);
}

/*!
 * \brief Write the RAPL PP1 power-limit control register (pp1_rapl_power_limit_control_t).
 *
 * (Client parts only)
 *
 * Write the RAPL PP1 power-limit control register in order to define power limiting
 * policies on the PP1 (uncore) power domain.
 *
 * \return 0 on success, -1 otherwise
 */
int
set_pp1_rapl_power_limit_control(uint64_t                        node,
                                 pp1_rapl_power_limit_control_t *pp1_obj)
{
    uint64_t cpu = pp1_node_to_cpu(node);
    return set_rapl_power_limit_control(cpu, MSR_RAPL_PP1_POWER_LIMIT, (rapl_power_limit_control_t*)pp1_obj);
}

/*!
 * \brief Write to the RAPL PP1 priority level register
 *
 * (Client parts only)
 *
 * Use the RAPL PP1 priority level register in order to provide an input
 * to the power budgeting algorithm on how to distribute power between the
 * PP0 (core) place and PP1 (uncore - graphics). The default value gives
 * priority to the uncore power plane.
 *
 * \return 0 on success, -1 otherwise
 */
int
set_pp1_balance_policy(uint64_t node,
                       uint64_t priority_level)
{
    uint64_t cpu = pp1_node_to_cpu(node);
    return set_balance_policy(cpu, MSR_RAPL_PP1_POLICY, priority_level);
}

/* Utilities */

int
read_rapl_units()
{
    int                    err = 0;
    rapl_unit_multiplier_t unit_multiplier;

    err = get_rapl_unit_multiplier(0, &unit_multiplier);
    if (!err) {
        RAPL_TIME_UNIT = unit_multiplier.time;
        RAPL_ENERGY_UNIT = unit_multiplier.energy;
        RAPL_POWER_UNIT = unit_multiplier.power;
    }

    return err;
}

// TODO: Improve the error handling of this function
// According to documentation the RDTSC can fail
// under protected or virtual mode, when certain flags are set
int
read_tsc(uint64_t *tsc)
{
    uint32_t d;
    uint32_t a;

    asm("rdtsc;"
        "mov %%edx, %0;"      // move edx into d
        "mov %%eax, %1;"      // move eax into a
        : "=r" (d), "=r" (a)  // output
        :                     // input
        : "%edx", "eax"       // clobbered regiters
        );

    *tsc = (uint64_t)d << 32 | a;

    return 0;
}

/* Required by Power Gadget */

int
get_os_freq(uint64_t cpu, uint64_t *freq)
{
    char path[60];
    int ret = 0;
    int out = 0;
    FILE *fp;

    out = sprintf(path, "%s%lu%s", "/sys/devices/system/cpu/cpu",cpu,"/cpufreq/cpuinfo_cur_freq");

    if(out > 0)
        fp = fopen(path, "r");

    if(NULL != fp){
        fscanf(fp, "%lu", freq);
        fclose(fp);
    }
    else{
        ret = MY_ERROR;
    }

    return ret;
}


// Uses the OS (and not the PMU counters) to retrieve the frequency
int
get_pp0_freq_mhz(uint64_t node, uint64_t *freq)
{
    int ret = 0;
    int i;

    // If all the cores are on the same power domain, report the average freq
    if( get_num_rapl_nodes_pp0() == get_num_rapl_nodes_pkg())
    {
        uint64_t sum_freq = 0;
        uint64_t cpu_freq = 0;

        for(i=0; i<num_pkg_threads; i++)
        {
            uint64_t os_cpu = pkg_map[node][i].os_id;
            ret = get_os_freq(os_cpu, &cpu_freq);
            sum_freq += cpu_freq;
        }

        if(0 == ret)
            *freq =  (sum_freq / num_pkg_threads) / 1000.0;
    }
    else
    {
        uint64_t cpu_freq = 0;
        uint64_t os_cpu = pp0_node_to_cpu(node);
        ret = get_os_freq(os_cpu, &cpu_freq);
        if(0 == ret)
            *freq = cpu_freq / 1000.0;
    }

    return ret;
}

/*
Copyright (c) 2012, Intel Corporation

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Written by Martin Dimitrov, Carl Strickland */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>


char         *progname;
const char   *version = "2.2";
uint64_t      num_node = 0;
uint64_t      delay_us = 1000000;
double        delay_unit = 1000000.0;

double **cum_energy_J = NULL;
double measurement_start_time, measurement_end_time;

double
get_rapl_energy_info(uint64_t power_domain, uint64_t node)
{
    int          err;
    double       total_energy_consumed;

    switch (power_domain) {
    case PKG:
        err = get_pkg_total_energy_consumed(node, &total_energy_consumed);
        break;
    case PP0:
        err = get_pp0_total_energy_consumed(node, &total_energy_consumed);
        break;
    case PP1:
        err = get_pp1_total_energy_consumed(node, &total_energy_consumed);
        break;
    case DRAM:
        err = get_dram_total_energy_consumed(node, &total_energy_consumed);
        break;
    default:
        err = MY_ERROR;
        break;
    }

    return total_energy_consumed;
}

void
convert_time_to_string(struct timeval tv, char* time_buf)
{
    time_t sec;
    int msec;
    struct tm *timeinfo;
    char tmp_buf[9];

    sec = tv.tv_sec;
    timeinfo = localtime(&sec);
    msec = tv.tv_usec/1000;

    strftime(tmp_buf, 9, "%H:%M:%S", timeinfo);
    sprintf(time_buf, "%s:%d",tmp_buf,msec);
}

double
convert_time_to_sec(struct timeval tv)
{
    double elapsed_time = (double)(tv.tv_sec) + ((double)(tv.tv_usec)/1000000);
    return elapsed_time;
}


void
do_print_energy_info()
{
    int i = 0;
    int domain = 0;
    uint64_t node = 0;
    double new_sample;
    double delta;
    double power;

    double prev_sample[num_node][RAPL_NR_DOMAIN];
    double power_watt[num_node][RAPL_NR_DOMAIN];
    cum_energy_J = malloc(num_node * sizeof(double*));
    for (i = 0; i < num_node; i++) {
        cum_energy_J[i] = malloc(RAPL_NR_DOMAIN * sizeof(double));
    }

    char time_buffer[32];
    struct timeval tv;
    int msec;
    uint64_t tsc;
    uint64_t freq;

    /* don't buffer if piped */
    setbuf(stdout, NULL);

    fprintf(stdout, "cpu_count=%lu\n", num_node);

    /* Read initial values */
    for (i = node; i < num_node; i++) {
        for (domain = 0; domain < RAPL_NR_DOMAIN; ++domain) {
            if(is_supported_domain(domain)) {
                prev_sample[i][domain] = get_rapl_energy_info(domain, i);
            }
        }
    }

    gettimeofday(&tv, NULL);
    measurement_start_time = convert_time_to_sec(tv);
    measurement_end_time = measurement_start_time;

    fprintf(stdout, "start_time=%f\n", measurement_start_time);

    /* Begin sampling */
    while (1) {

        usleep(delay_us);

        for (i = node; i < num_node; i++) {
            for (domain = 0; domain < RAPL_NR_DOMAIN; ++domain) {
                if(is_supported_domain(domain)) {
                    new_sample = get_rapl_energy_info(domain, i);
                    delta = new_sample - prev_sample[i][domain];

                    /* Handle wraparound */
                    if (delta < 0) {
                        delta += MAX_ENERGY_STATUS_JOULES;
                    }

                    prev_sample[i][domain] = new_sample;

                    cum_energy_J[i][domain] += delta;
                }
            }
        }

        gettimeofday(&tv, NULL);
        measurement_end_time = convert_time_to_sec(tv);
    }
}

void
usage()
{
    fprintf(stdout, "\nIntel(r) Power Gadget %s\n", version);
    fprintf(stdout, "\nUsage: \n");
    fprintf(stdout, "%s [-e [sampling delay (ms) ] optional]\n", progname);
    fprintf(stdout, "\nExample: %s -e 1000 -d 10\n", progname);
    fprintf(stdout, "\n");
}


int
cmdline(int argc, char **argv)
{
    int             opt;
    uint64_t    delay_ms_temp = 1000;

    progname = argv[0];

    while ((opt = getopt(argc, argv, "e:")) != -1) {
        switch (opt) {
        case 'e':
            delay_ms_temp = atoi(optarg);
            if(delay_ms_temp > 50) {
                delay_us = delay_ms_temp * 1000;
            } else {
                fprintf(stdout, "Sampling delay must be greater than 50 ms.\n");
                return -1;
            }
            break;
        case 'h':
            usage();
            exit(0);
            break;
        default:
            usage();
            return -1;
        }
    }
    return 0;
}

void sigint_handler(int signum)
{
    int i, domain;
    uint64_t freq;

    fprintf(stdout, "end_time=%f\n", measurement_end_time);
    fprintf(stdout, "duration=%f\n", measurement_end_time - measurement_start_time);

    if (cum_energy_J != NULL) {
        for (i = 0; i < num_node; i++) {
            if (cum_energy_J[i] == NULL) {
                continue;
            }

            for (domain = 0; domain < RAPL_NR_DOMAIN; ++domain) {
                if (is_supported_domain(domain)) {
                    char *domain_string = RAPL_DOMAIN_STRINGS[domain];
                    fprintf(stdout, "cpu%d_%s_Joules=%f\n", i, domain_string, cum_energy_J[i][domain]);
                }
            }
        }
    }

    terminate_rapl();
    exit(0);
}

int
main(int argc, char **argv)
{
    int i = 0;
    int ret = 0;

    /* Clean up if we're told to exit */
    signal(SIGINT, sigint_handler);

    // First init the RAPL library
    if (0 != init_rapl()) {
        fprintf(stdout, "Init failed!\n");
        terminate_rapl();
        return MY_ERROR;
    }
    num_node = get_num_rapl_nodes_pkg();

    ret = cmdline(argc, argv);
    if (ret) {
        terminate_rapl();
        return ret;
    }

    do_print_energy_info();

    terminate_rapl();
}
