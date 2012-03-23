/* ----------------------------------------------------------------------------
 *         SAM Software Package License 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2012, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#ifndef _SAM3_
#define _SAM3_

#if (defined __SAM3S8A__) || (defined __SAM3S8B__) || (defined __SAM3S8C__) /* SAM3S8 */
  #include "cmsis/sam3s8/include/sam3s8.h"
  #include "cmsis/sam3s8/source/templates/system_sam3s8.h"
  #include "cmsis/sam3s8/source/templates/exceptions.h"
#elif (defined __SAM3SD8A__) || (defined __SAM3SD8B__) || (defined __SAM3SD8C__) /* SAM3SD8 */
  #include "cmsis/sam3s8/include/sam3s8.h"
  #include "cmsis/sam3s8/source/templates/system_sam3s8.h"
  #include "cmsis/sam3s8/source/templates/exceptions.h"

#elif (defined __SAM3S4C__) || (defined __SAM3S4B__) || (defined __SAM3S4A__) /* SAM3S4 */
  #include "cmsis/sam3s/include/sam3s.h"
  #include "cmsis/sam3s/source/templates/system_sam3s.h"
  #include "cmsis/sam3s/source/templates/exceptions.h"
#elif (defined __SAM3S2C__) || (defined __SAM3S2B__) || (defined __SAM3S2A__) /* SAM3S2 */
  #include "cmsis/sam3s/include/sam3s.h"
  #include "cmsis/sam3s/source/templates/system_sam3s.h"
  #include "cmsis/sam3s/source/templates/exceptions.h"
#elif (defined __SAM3S1C__) || (defined __SAM3S1B__) || (defined __SAM3S1A__) /* SAM3S1 */
  #include "cmsis/sam3s/include/sam3s.h"
  #include "cmsis/sam3s/source/templates/system_sam3s.h"
  #include "cmsis/sam3s/source/templates/exceptions.h"

#elif (defined __SAM3U4C__) || (defined __SAM3U4E__) /* SAM3U4 */
  #include "cmsis/sam3u/include/sam3u.h"
  #include "cmsis/sam3u/source/templates/system_sam3u.h"
  #include "cmsis/sam3u/source/templates/exceptions.h"
#elif (defined __SAM3U2C__) || (defined __SAM3U2E__) /* SAM3U2 */
  #include "cmsis/sam3u/include/sam3u.h"
  #include "cmsis/sam3u/source/templates/system_sam3u.h"
  #include "cmsis/sam3u/source/templates/exceptions.h"
#elif (defined __SAM3U1C__) || (defined __SAM3U1E__) /* SAM3U1 */
  #include "cmsis/sam3u/include/sam3u.h"
  #include "cmsis/sam3u/source/templates/system_sam3u.h"
  #include "cmsis/sam3u/source/templates/exceptions.h"

#elif (defined __SAM3N4C__) || (defined __SAM3N4B__) || (defined __SAM3N4A__) /* SAM3N4 */
  #include "cmsis/sam3n/include/sam3n.h"
  #include "cmsis/sam3n/source/templates/system_sam3n.h"
  #include "cmsis/sam3n/source/templates/exceptions.h"
#elif (defined __SAM3N2C__) || (defined __SAM3N2B__) || (defined __SAM3N2A__) /* SAM3N2 */
  #include "cmsis/sam3n/include/sam3n.h"
  #include "cmsis/sam3n/source/templates/system_sam3n.h"
  #include "cmsis/sam3n/source/templates/exceptions.h"
#elif (defined __SAM3N1C__) || (defined __SAM3N1B__) || (defined __SAM3N1A__) /* SAM3N1 */
  #include "cmsis/sam3n/include/sam3n.h"
  #include "cmsis/sam3n/source/templates/system_sam3n.h"
  #include "cmsis/sam3n/source/templates/exceptions.h"
#elif (defined __SAM3N0C__) || (defined __SAM3N0B__) || (defined __SAM3N0A__) /* SAM3N0 */
  #include "cmsis/sam3n/include/sam3n.h"
  #include "cmsis/sam3n/source/templates/system_sam3n.h"
  #include "cmsis/sam3n/source/templates/exceptions.h"
#elif (defined __SAM3N00B__) || (defined __SAM3N00A__) /* SAM3N00 */
  #include "cmsis/sam3n/include/sam3n.h"
  #include "cmsis/sam3n/source/templates/system_sam3n.h"
  #include "cmsis/sam3n/source/templates/exceptions.h"

#elif (defined __SAM3A8C__) || (defined __SAM3A4C__) || (defined __SAM3A2C__) /* SAM3A */
  #include "cmsis/sam3xa/include/sam3xa.h"
  #include "cmsis/sam3xa/source/templates/system_sam3xa.h"
  #include "cmsis/sam3xa/source/templates/exceptions.h"

#elif (defined __SAM3X8C__) || (defined __SAM3X8E__) || (defined __SAM3X8H__) /* SAM3X8 */
  #include "cmsis/sam3xa/include/sam3xa.h"
  #include "cmsis/sam3xa/source/templates/system_sam3xa.h"
  #include "cmsis/sam3xa/source/templates/exceptions.h"
#elif (defined __SAM3X4C__) || (defined __SAM3X4E__) /* SAM3X4 */
  #include "cmsis/sam3xa/include/sam3xa.h"
  #include "cmsis/sam3xa/source/templates/system_sam3xa.h"
  #include "cmsis/sam3xa/source/templates/exceptions.h"
#elif (defined __SAM3A4C__) || (defined __SAM3A8C__) /* SAM3A */
  #include "cmsis/sam3xa/include/sam3xa.h"
  #include "cmsis/sam3xa/source/templates/system_sam3xa.h"
  #include "cmsis/sam3xa/source/templates/exceptions.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM3_ */
