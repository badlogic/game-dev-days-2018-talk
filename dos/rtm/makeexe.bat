REM build EXE
del *.exe
del *.obj
bc source\rtm.bas /O/T/A/C:512, , rtm.lst;
link /EX /NOE /NOD:BRUN45.LIB RTM, RTM.EXE, NUL, +source\rtmasm.LIB+c:\qb45\bcom45.lib