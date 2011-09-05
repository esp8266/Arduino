/* %ATMEL_LICENCE% */

#ifndef _SAM3U_GPBR_COMPONENT_
#define _SAM3U_GPBR_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR General Purpose Backup Register */
/* ============================================================================= */
/** \addtogroup SAM3U_GPBR General Purpose Backup Register */
/*@{*/

#ifndef __ASSEMBLY__
/** \brief Gpbr hardware registers */
typedef struct {
  RwReg SYS_GPBR[4]; /**< \brief (Gpbr Offset: 0x0) General Purpose Backup Register */
} Gpbr;
#endif /* __ASSEMBLY__ */
/* -------- SYS_GPBR[4] : (GPBR Offset: 0x0) General Purpose Backup Register -------- */
#define SYS_GPBR_GPBR_VALUE_Pos 0
#define SYS_GPBR_GPBR_VALUE_Msk (0xffffffffu << SYS_GPBR_GPBR_VALUE_Pos) /**< \brief (SYS_GPBR[4]) Value of GPBR x */
#define SYS_GPBR_GPBR_VALUE(value) ((SYS_GPBR_GPBR_VALUE_Msk & ((value) << SYS_GPBR_GPBR_VALUE_Pos)))

/*@}*/


#endif /* _SAM3U_GPBR_COMPONENT_ */
