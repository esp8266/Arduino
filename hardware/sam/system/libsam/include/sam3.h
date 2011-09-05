/*
 %atmel_license%
*/

#ifndef _SAM3_
#define _SAM3_

#if (defined __sam3s8a__) || (defined __sam3s8b__) || (defined __sam3s8c__) /* SAM3S8 */
  #include "cmsis/sam3s8/include/sam3s8.h"
  #include "cmsis/sam3s/include/system_sam3s8.h"
#elif (defined __sam3sd8a__) || (defined __sam3sd8b__) || (defined __sam3sd8c__) /* SAM3SD8 */
  #include "cmsis/sam3s8/include/sam3s8.h"
  #include "cmsis/sam3s/include/system_sam3s8.h"
#elif (defined __sam3s4c__) || (defined __sam3s4b__) || (defined __sam3s4a__) /* SAM3S4 */
  #include "cmsis/sam3s/include/sam3s.h"
  #include "cmsis/sam3s/include/system_sam3s.h"
#elif (defined __sam3s2c__) || (defined __sam3s2b__) || (defined __sam3s2a__) /* SAM3S2 */
  #include "cmsis/sam3s/include/sam3s.h"
  #include "cmsis/sam3s/include/system_sam3s.h"
#elif (defined __sam3s1c__) || (defined __sam3s1b__) || (defined __sam3s1a__) /* SAM3S1 */
  #include "cmsis/sam3s/include/sam3s.h"
  #include "cmsis/sam3s/include/system_sam3s.h"
#elif (defined __sam3u4c__) || (defined __sam3u4e__) /* SAM3U4 */
  #include "cmsis/sam3u/include/sam3u.h"
  #include "cmsis/sam3s/include/system_sam3u.h"
#elif (defined __sam3u2c__) || (defined __sam3u2e__) /* SAM3U2 */
  #include "cmsis/sam3u/include/sam3u.h"
  #include "cmsis/sam3s/include/system_sam3u.h"
#elif (defined __sam3u1c__) || (defined __sam3u1e__) /* SAM3U1 */
  #include "cmsis/sam3u/include/sam3u.h"
  #include "cmsis/sam3s/include/system_sam3u.h"
#elif (defined __sam3n4c__) || (defined __sam3n4b__) || (defined __sam3n4a__) /* SAM3N4 */
  #include "cmsis/sam3n/include/sam3n.h"
  #include "cmsis/sam3s/include/system_sam3n.h"
#elif (defined __sam3n2c__) || (defined __sam3n2b__) || (defined __sam3n2a__) /* SAM3N2 */
  #include "cmsis/sam3n/include/sam3n.h"
  #include "cmsis/sam3s/include/system_sam3n.h"
#elif (defined __sam3n1c__) || (defined __sam3n1b__) || (defined __sam3n1a__) /* SAM3N1 */
  #include "cmsis/sam3n/include/sam3n.h"
  #include "cmsis/sam3s/include/system_sam3n.h"
#elif (defined __sam3a8c__) || (defined __sam3a4c__) || (defined __sam3a2c__) /* SAM3A */
  #include "cmsis/sam3xa/include/sam3xa.h"
  #include "cmsis/sam3s/include/system_sam3xa.h"
#elif (defined __sam3x8c__) || (defined __sam3x8e__) || (defined __sam3x8g__) || (defined __sam3x8h__) /* SAM3X8 */
  #include "cmsis/sam3xa/include/sam3xa.h"
  #include "cmsis/sam3s/include/system_sam3xa.h"
#elif (defined __sam3x4c__) || (defined __sam3x4e__) || (defined __sam3x4g__) || (defined __sam3x4h__) /* SAM3X4 */
  #include "cmsis/sam3xa/include/sam3xa.h"
  #include "cmsis/sam3s/include/system_sam3xa.h"
#elif (defined __sam3x2c__) || (defined __sam3x2e__) || (defined __sam3x2g__) || (defined __sam3x2h__) /* SAM3X2 */
  #include "cmsis/sam3xa/include/sam3xa.h"
  #include "cmsis/sam3s/include/system_sam3xa.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM3_ */
