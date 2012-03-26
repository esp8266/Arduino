# ----------------------------------------------------------------------------
#         ATMEL Microcontroller Software Support 
# ----------------------------------------------------------------------------
# Copyright (c) 2010, Atmel Corporation
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following condition is met:
#
# - Redistributions of source code must retain the above copyright notice,
# this list of conditions and the disclaimer below.
#
# Atmel's name may not be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
# DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# ----------------------------------------------------------------------------

# define SAM series
SAM3N=SAM3N00A SAM3N00B SAM3N0A SAM3N0B SAM3N0C SAM3N1A SAM3N1B SAM3N1C SAM3N2A SAM3N2B SAM3N2C SAM3N4A SAM3N4B SAM3N4C
SAM3S=SAM3S1A SAM3S1B SAM3S1C SAM3S2A SAM3S2B SAM3S2C SAM3S4A SAM3S4B SAM3S4C
SAM3SD8=SAM3S8B SAM3S8C SAM3SD8B SAM3SD8C
SAM3U=SAM3U1C SAM3U1E SAM3U2C SAM3U2E SAM3U4C SAM3U4E
SAM3XA=SAM3A4C SAM3A8C SAM3X4C SAM3X4E SAM3X8C SAM3X8E SAM3X8H
SAM4S=SAM4S8B SAM4S8C SAM4S16B SAM4S16C

SAM_SERIES=$(SAM3N) $(SAM3S) $(SAM3SD8) $(SAM3U) $(SAM3XA) $(SAM4S)
