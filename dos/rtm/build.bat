cd source

REM delete previous output files
del *.lib
del *.qlb
del *.obj

REM assemble utils
masm utils.asm, utils.obj, NUL, NUL

REM link directqb and utils into single .qlb
del rtmasm.lib
lib rtmasm.lib +directqb\DQB.LIB, NUL
lib rtmasm.lib +UTILS.OBJ, NUL, rtmasm.lib
link /QU rtmasm.lib, rtmasm.qlb, NUL, c:\qb45\bqlb45.lib

REM clean up
del *.bak

cd ..