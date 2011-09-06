/*
 %atmel_license%
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

#elif (defined __SAM3A8C__) || (defined __SAM3A4C__) || (defined __SAM3A2C__) /* SAM3A */
  #include "cmsis/sam3xa/include/sam3xa.h"
  #include "cmsis/sam3xa/source/templates/system_sam3xa.h"
  #include "cmsis/sam3xa/source/templates/exceptions.h"

#elif (defined __SAM3X8C__) || (defined __SAM3X8E__) || (defined __SAM3X8G__) || (defined __SAM3X8H__) /* SAM3X8 */
  #include "cmsis/sam3xa/include/sam3xa.h"
  #include "cmsis/sam3xa/source/templates/system_sam3xa.h"
  #include "cmsis/sam3xa/source/templates/exceptions.h"
#elif (defined __SAM3X4C__) || (defined __SAM3X4E__) || (defined __SAM3X4G__) || (defined __SAM3X4H__) /* SAM3X4 */
  #include "cmsis/sam3xa/include/sam3xa.h"
  #include "cmsis/sam3xa/source/templates/system_sam3xa.h"
  #include "cmsis/sam3xa/source/templates/exceptions.h"
#elif (defined __SAM3X2C__) || (defined __SAM3X2E__) || (defined __SAM3X2G__) || (defined __SAM3X2H__) /* SAM3X2 */
  #include "cmsis/sam3xa/include/sam3xa.h"
  #include "cmsis/sam3xa/source/templates/system_sam3xa.h"
  #include "cmsis/sam3xa/source/templates/exceptions.h"

#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM3_ */
