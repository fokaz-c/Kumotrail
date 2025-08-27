/*
 * Copyright 2025 Fokaz
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "timer.h"
#include "include/timer.h"

//base addresses for both TIMER GROUPS 
#define TIMG_0_BASE_ADDR 0x6001F000
#define TIMG_1_BASE_ADDR 0x60020000

// ===== TIMG_0 Register Definitions =====

// Timer Configuration Registers
#define TIMG_0_T0CONFIG_REG         ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0000))
#define TIMG_0_T0LO_REG             ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0004))
#define TIMG_0_T0HI_REG             ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0008))
#define TIMG_0_T0UPDATE_REG         ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x000C))
#define TIMG_0_T0ALARMLO_REG        ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0010))
#define TIMG_0_T0ALARMHI_REG        ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0014))
#define TIMG_0_T0LOADLO_REG         ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0018))
#define TIMG_0_T0LOADHI_REG         ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x001C))
#define TIMG_0_T0LOAD_REG           ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0020))
#define TIMG_0_T1CONFIG_REG         ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0024))
#define TIMG_0_T1LO_REG             ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0028))
#define TIMG_0_T1HI_REG             ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x002C))
#define TIMG_0_T1UPDATE_REG         ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0030))
#define TIMG_0_T1ALARMLO_REG        ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0034))
#define TIMG_0_T1ALARMHI_REG        ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0038))
#define TIMG_0_T1LOADLO_REG         ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x003C))
#define TIMG_0_T1LOADHI_REG         ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0040))
#define TIMG_0_T1LOAD_REG           ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0044))

// Watchdog Timer Registers
#define TIMG_0_WDTCONFIG0_REG       ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0048))
#define TIMG_0_WDTCONFIG1_REG       ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x004C))
#define TIMG_0_WDTCONFIG2_REG       ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0050))
#define TIMG_0_WDTCONFIG3_REG       ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0054))
#define TIMG_0_WDTCONFIG4_REG       ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0058))
#define TIMG_0_WDTCONFIG5_REG       ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x005C))
#define TIMG_0_WDTFEED_REG          ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0060))
#define TIMG_0_WDTWPROTECT_REG      ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0064))

// RTC Calibration Registers
#define TIMG_0_RTCCALICFG_REG       ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0068))
#define TIMG_0_RTCCALICFG1_REG      ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x006C))
#define TIMG_0_RTCCALICFG2_REG      ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0080))

// Interrupt Registers
#define TIMG_0_INT_ENA_TIMERS_REG   ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0070))
#define TIMG_0_INT_RAW_TIMERS_REG   ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0074))
#define TIMG_0_INT_ST_TIMERS_REG    ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0078))
#define TIMG_0_INT_CLR_TIMERS_REG   ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x007C))

// Miscellaneous Registers
#define TIMG_0_NTIMG_DATE_REG       ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x00F8))
#define TIMG_0_REGCLK_REG           ((volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x00FC))

// ===== TIMG_1 Register Definitions =====

// Timer Configuration Registers
#define TIMG_1_T0CONFIG_REG         ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0000))
#define TIMG_1_T0LO_REG             ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0004))
#define TIMG_1_T0HI_REG             ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0008))
#define TIMG_1_T0UPDATE_REG         ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x000C))
#define TIMG_1_T0ALARMLO_REG        ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0010))
#define TIMG_1_T0ALARMHI_REG        ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0014))
#define TIMG_1_T0LOADLO_REG         ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0018))
#define TIMG_1_T0LOADHI_REG         ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x001C))
#define TIMG_1_T0LOAD_REG           ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0020))
#define TIMG_1_T1CONFIG_REG         ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0024))
#define TIMG_1_T1LO_REG             ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0028))
#define TIMG_1_T1HI_REG             ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x002C))
#define TIMG_1_T1UPDATE_REG         ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0030))
#define TIMG_1_T1ALARMLO_REG        ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0034))
#define TIMG_1_T1ALARMHI_REG        ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0038))
#define TIMG_1_T1LOADLO_REG         ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x003C))
#define TIMG_1_T1LOADHI_REG         ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0040))
#define TIMG_1_T1LOAD_REG           ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0044))

// Watchdog Timer Registers
#define TIMG_1_WDTCONFIG0_REG       ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0048))
#define TIMG_1_WDTCONFIG1_REG       ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x004C))
#define TIMG_1_WDTCONFIG2_REG       ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0050))
#define TIMG_1_WDTCONFIG3_REG       ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0054))
#define TIMG_1_WDTCONFIG4_REG       ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0058))
#define TIMG_1_WDTCONFIG5_REG       ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x005C))
#define TIMG_1_WDTFEED_REG          ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0060))
#define TIMG_1_WDTWPROTECT_REG      ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0064))

// RTC Calibration Registers
#define TIMG_1_RTCCALICFG_REG       ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0068))
#define TIMG_1_RTCCALICFG1_REG      ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x006C))
#define TIMG_1_RTCCALICFG2_REG      ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0080))

// Interrupt Registers
#define TIMG_1_INT_ENA_TIMERS_REG   ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0070))
#define TIMG_1_INT_RAW_TIMERS_REG   ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0074))
#define TIMG_1_INT_ST_TIMERS_REG    ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0078))
#define TIMG_1_INT_CLR_TIMERS_REG   ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x007C))

// Miscellaneous Registers
#define TIMG_1_NTIMG_DATE_REG       ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x00F8))
#define TIMG_1_REGCLK_REG           ((volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x00FC))


void timer_init(void){
    
}

