
SET TMP=C:\Temp
SET TEMP=C:\Temp

SET UVEXE=C:\Keil\UV4\UV4.EXE

%UVEXE% -rb arm_cortexM0x_math.uvproj -t"DSP_Lib CM0 LE" -o"DSP_Lib CM0 LE.txt"
%UVEXE% -rb arm_cortexM3x_math.uvproj -t"DSP_Lib CM3 LE" -o"DSP_Lib CM3 LE.txt"
%UVEXE% -rb arm_cortexM4x_math.uvproj -t"DSP_Lib CM4 LE" -o"DSP_Lib CM4 LE.txt"
%UVEXE% -rb arm_cortexM4x_math.uvproj -t"DSP_Lib CM4 LE FPU" -o"DSP_Lib CM4 LE FPU.txt"
