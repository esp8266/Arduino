/*
 *   Copyright 2020 M Hightower
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

#ifndef MMU_IRAM_H
#define MMU_IRAM_H

#include <stdint.h>
#include <c_types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MMU_CRE_STATUS {
  uint32_t v_cfg;
  uint32_t map;
  uint32_t p;
  uint32_t v;
}  mmu_cre_status_t;

extern mmu_cre_status_t mmu_status;
extern int Cache_Read_Enable_status;

#ifdef __cplusplus
}
#endif
#endif
