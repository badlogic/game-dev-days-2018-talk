' ---------------------------------------------------------------------------
'
'                                  DQBtools
'                                version 1.21
'
'              by Angelo Mottola - Enhanced Creations 1998-99
'
' This program contains a font editor, a mouse cursor editor and, soon, a
' sounds datafile handler, developed using the DirectQB library, for the
' DirectQB library.
'
' ===========================================================================
' IMPORTANT: While in the font editor, use the arrow keys to resize your
' PLEASE     characters!! Old fonts saved with DQBtools 1.0 can still be
' READ!!     loaded without problems, but when you'll save them again with
'            this new release, they'll be converted to the new format.
'            Pay attention because DQBsetFont now accepts only fonts with
'            this new format, so convert your old charsets now!!
' ===========================================================================
'
' Contact informations:
'
' Angelo Mottola                        angelillo@bigfoot.com
' Enhanced Creations Homepage           http://ec.quickbasic.com
'
' Thanks for using DirectQB and DQBtools!
'
' ---------------------------------------------------------------------------
DEFINT A-Z
'$INCLUDE: 'DIRECTQB.BI'
DECLARE SUB Init ()
DECLARE SUB Main ()
DECLARE SUB Message (Text$)
DECLARE SUB Center (y1, Text$, c)
DECLARE SUB DrawButton (x1, y1, Text$)
DECLARE SUB PushButton (x1, y1, Text$)
DECLARE SUB DrawWindow (x1, y1, x2, y2, Title$, c)
DECLARE SUB DrawFontGUI ()
DECLARE SUB DrawCurGUI ()
DECLARE SUB HandleFontGUI ()
DECLARE SUB HandleCurGUI ()
DECLARE SUB DrawChar ()
DECLARE SUB EncodeCur ()
DECLARE SUB DecodeCur ()
DECLARE SUB LoadData ()
DECLARE SUB SaveData ()
DECLARE FUNCTION Query (Title$, Text$, Buttons)

DIM SHARED SBok AS INTEGER, tool AS INTEGER, FileName AS STRING
DIM SHARED FontExt AS STRING * 3, CurExt AS STRING * 3
DIM SHARED Char AS INTEGER, Font AS STRING * 2305
DIM SHARED Col AS INTEGER, Cursor AS STRING * 64
DIM SHARED Buf1(130) AS INTEGER, Buf2(130) AS INTEGER, Modified AS INTEGER

ON ERROR GOTO ErrorHandler

Init
Main

SCREEN 0: WIDTH 80
COLOR 15, 1: PRINT SPACE$(80): LOCATE 1, 1
PRINT "DirectQB Tools v1.21 by Angelo Mottola - Enhanced Creations 1998-99"
COLOR 7, 0: PRINT "Get the latest DirectQB updates at the Enhanced Creations Homepage, at:"
COLOR 14, 0: PRINT SPC(26); "http://ec.quickbasic.com"
COLOR 7, 0: PRINT "For any hint, comment or bug report, email me at "; : COLOR 14, 0
PRINT "angelillo@bigfoot.com"; : COLOR 7, 0: PRINT "."
PRINT "Thanks for using DirectQB and DirectQB Tools!"

DQBclose
END

ErrorHandler:
DQBmouseHide
DQBcopyLayer VIDEO, 2
SELECT CASE ERR
CASE 100: Msg$ = "Unknown font file format!"
CASE 101: Msg$ = "Unknown cursor file format!"
CASE ELSE: Msg$ = "Internal program error!"
END SELECT
WinWidth = (LEN(Msg$) * 8) + 16
DrawWindow ((320 - WinWidth) \ 2), 70, (((320 - WinWidth) \ 2) + WinWidth), 130, "Error", 40
Center 90, Msg$, 31
DrawButton 128, 105, "  OK  "
DQBmouseShow
DO
  x = DQBmouseX: y = DQBmouseY
  IF DQBmouseLB AND x > 128 AND x < 192 AND y > 105 AND y < 120 THEN EXIT DO
LOOP
PushButton 128, 105, "  OK  "
DQBmouseHide
DQBcopyLayer 2, VIDEO
DQBmouseShow
RESUME NEXT

SUB Center (y1, Text$, c)
'
DQBprint VIDEO, Text$, ((320 - (LEN(Text$) * 8)) \ 2), y1, c

END SUB

SUB DecodeCur
'
FOR i = 0 TO 15
  FOR ii = 1 TO 0 STEP -1
    OurSMbyte = ASC(MID$(Cursor, 1 + (i * 2) + (1 - ii)))
    OurCMbyte = ASC(MID$(Cursor, 33 + (i * 2) + (1 - ii)))
    FOR iii = 0 TO 7
      SMbit = DQBreadBit(OurSMbyte, (7 - iii))
      CMbit = DQBreadBit(OurCMbyte, (7 - iii))
      cx = (ii * 8) + (iii): cy = i
      IF NOT SMbit AND NOT CMbit THEN c = 0
      IF NOT SMbit AND CMbit THEN c = 31
      IF SMbit AND NOT CMbit THEN c = 1
      IF SMbit AND CMbit THEN c = 22
      DQBpset VIDEO, (51 + cx), (25 + cy), c
    NEXT iii
  NEXT ii
NEXT i

END SUB

SUB DrawButton (x1, y1, Text$)
'
x2 = x1 + (LEN(Text$) * 8) + 15
DQBboxf VIDEO, x1, y1, x2, (y1 + 15), 28
DQBline VIDEO, x1, y1, x1, (y1 + 14), 29
DQBline VIDEO, x1, y1, (x2 - 1), y1, 29
DQBline VIDEO, (x1 + 1), (y1 + 15), x2, (y1 + 15), 27
DQBline VIDEO, x2, (y1 + 1), x2, (y1 + 15), 27
DQBprint VIDEO, Text$, (x1 + 8), (y1 + 4), 31

END SUB

SUB DrawChar
'
DQBmouseHide
FOR i = 0 TO 7
  FOR ii = 0 TO 7
    IF DQBpoint(VIDEO, 81 + ii, 107 + i) = 31 THEN
      DQBboxf VIDEO, (10 + (ii * 10)), (25 + (i * 10)), (18 + (ii * 10)), (33 + (i * 10)), 31
    ELSE
      DQBboxf VIDEO, (10 + (ii * 10)), (25 + (i * 10)), (18 + (ii * 10)), (33 + (i * 10)), 0
    END IF
  NEXT ii
NEXT i
DQBbox VIDEO, 9, 24, 89, 104, 20
FOR i = 0 TO 6
  DQBline VIDEO, (19 + (i * 10)), 24, (19 + (i * 10)), 104, 20
  DQBline VIDEO, 9, (34 + (i * 10)), 89, (34 + (i * 10)), 20
NEXT i
DQBbox VIDEO, 9, 24, (9 + (ASC(MID$(Font, 2049 + Char, 1)) * 10)), (24 + (ASC(MID$(Font, 2305, 1)) * 10)), 40
DQBmouseShow

END SUB

SUB DrawCurGUI
'
DQBmouseHide
DQBboxf VIDEO, 0, 0, 319, 8, 1
DQBprint VIDEO, "DirectQB Tools - Cursor editor", 1, 1, 31
DQBboxf VIDEO, 0, 9, 319, 17, 27
DQBprint VIDEO, "File  Tool  Options", 9, 10, 0
DQBboxf VIDEO, 0, 18, 319, 190, 21
DQBbox VIDEO, 70, 24, 232, 186, 24
DQBboxf VIDEO, 71, 25, 231, 185, 1
DQBbox VIDEO, 71, 25, 231, 185, 20
FOR i = 0 TO 15
  DQBline VIDEO, 71 + (i * 10), 25, 71 + (i * 10), 184, 20
  DQBline VIDEO, 71, 25 + (i * 10), 231, 25 + (i * 10), 20
NEXT i
DQBboxf VIDEO, 50, 24, 67, 41, 1
DecodeCur
FOR i = 0 TO 15
  FOR ii = 0 TO 15
    DQBboxf VIDEO, (72 + (i * 10)), (26 + (ii * 10)), (80 + (i * 10)), (34 + (ii * 10)), DQBpoint(VIDEO, 51 + i, 25 + ii)
  NEXT ii
NEXT i
DrawButton 240, 25, CHR$(24)
DrawButton 240, 45, CHR$(25)
DrawButton 240, 65, CHR$(26)
DrawButton 240, 85, CHR$(27)
DrawButton 240, 105, CHR$(18)
DrawButton 240, 125, CHR$(29)
DrawButton 240, 145, "R"
DQBbox VIDEO, 50, 24, 67, 41, 24
FOR i = 0 TO 3
  DQBbox VIDEO, 52, 59 + (i * 20), 64, 74 + (i * 20), 24
NEXT i
DQBbox VIDEO, 52, 99, 64, 114, 28
DQBboxf VIDEO, 53, 60, 63, 73, 1
DQBboxf VIDEO, 53, 80, 63, 93, 0
DQBboxf VIDEO, 53, 100, 63, 113, 31
DQBboxf VIDEO, 53, 120, 63, 133, 22
DQBboxf VIDEO, 0, 191, 319, 199, 3
DQBprint VIDEO, FileName, (315 - (LEN(FileName) * 8)), 192, 0
DQBmouseShow
DQBwait 10
Col = 2
Modified = FALSE

END SUB

SUB DrawFontGUI
'
DQBmouseHide
DQBboxf VIDEO, 0, 0, 319, 8, 1
DQBprint VIDEO, "DirectQB Tools - Font editor", 1, 1, 31
DQBprint VIDEO, "File  Tool  Options  Test", 9, 10, 0
DQBboxf VIDEO, 0, 18, 319, 190, 21
DQBboxf VIDEO, 103, 24, 312, 185, 0
DQBbox VIDEO, 102, 23, 313, 186, 24
FOR i = 104 TO 296 STEP 16
  DQBboxf VIDEO, i, 25, (i + 7), 184, 104
NEXT i
FOR i = 0 TO 127
  x = ((i MOD 13) * 16) + 104
  y = ((i \ 13) * 8) + 25
  DQBprint VIDEO, CHR$(i), x, y, 31
NEXT i
FOR i = 0 TO 127
  x = ((i MOD 13) * 16) + 104
  y = ((i \ 13) * 8) + 105
  DQBprint VIDEO, CHR$(128 + i), x, y, 31
NEXT i
DQBsetFont Font
FOR i = 0 TO 127
  x = ((i MOD 13) * 16) + 112
  y = ((i \ 13) * 8) + 25
  DQBprint VIDEO, CHR$(i), x, y, 40
NEXT i
FOR i = 0 TO 127
  x = ((i MOD 13) * 16) + 112
  y = ((i \ 13) * 8) + 105
  DQBprint VIDEO, CHR$(128 + i), x, y, 40
NEXT i
DQBsetBIOSfont
DQBboxf VIDEO, 9, 24, 89, 104, 0
DQBbox VIDEO, 8, 23, 90, 105, 24
DQBboxf VIDEO, 80, 106, 89, 115, 0
DQBbox VIDEO, 79, 105, 90, 116, 24
DQBbox VIDEO, 9, 24, 89, 104, 20
FOR i = 0 TO 6
  DQBline VIDEO, (19 + (i * 10)), 24, (19 + (i * 10)), 104, 20
  DQBline VIDEO, 9, (34 + (i * 10)), 89, (34 + (i * 10)), 20
NEXT i
DrawButton 8, 125, CHR$(24)
DrawButton 38, 125, CHR$(25)
DrawButton 68, 125, CHR$(18)
DrawButton 8, 145, CHR$(26)
DrawButton 38, 145, CHR$(27)
DrawButton 68, 145, CHR$(29)
DrawButton 8, 165, "R"
DrawButton 38, 165, CHR$(10)
DrawButton 68, 165, "U"
Char = 65
DQBsetTextBackCol 2
DQBsetTextStyle SOLID
DQBsetFont Font
DQBprint VIDEO, CHR$(Char), 112, 65, 10
DQBsetTextBackCol 0
DQBprint VIDEO, CHR$(Char), 81, 107, 31
DQBsetTextStyle NONE
DQBsetBIOSfont
DQBprint VIDEO, "ASCII", 8, 107, 31
DQBprint VIDEO, STR$(Char), 45, 107, 3
DQBboxf VIDEO, 0, 191, 319, 199, 3
DQBprint VIDEO, FileName, (315 - (LEN(FileName) * 8)), 192, 0
DQBmouseShow
DrawChar
DQBwait 10
Modified = FALSE

END SUB

SUB DrawWindow (x1, y1, x2, y2, Title$, c)
'
DQBboxf VIDEO, x1 + 1, y1 + 1, x2 + 1, y2 + 1, 0
DQBboxf VIDEO, x1, y1, x2, y2, 26
DQBline VIDEO, x1, y1, x1, (y2 - 1), 28
DQBline VIDEO, x1, y1, (x2 - 1), y1, 28
DQBline VIDEO, (x1 + 1), y2, x2, y2, 24
DQBline VIDEO, x2, (y1 + 1), x2, y2, 24
DQBboxf VIDEO, (x1 + 1), (y1 + 1), (x2 - 1), (y1 + 9), c
tx = (((x2 - x1) - (LEN(Title$) * 8)) \ 2) + x1
DQBprint VIDEO, Title$, tx, (y1 + 2), 31

END SUB

SUB EncodeCur
'
FOR i = 0 TO 15
  FOR ii = 1 TO 0 STEP -1
    OurSMbyte = ASC(MID$(Cursor, 1 + (i * 2) + (1 - ii), 1))
    OurCMbyte = ASC(MID$(Cursor, 33 + (i * 2) + (1 - ii), 1))
    FOR iii = 0 TO 7
      p = DQBpoint(VIDEO, 51 + (ii * 8) + (7 - iii), 25 + i)
      SELECT CASE p
      CASE 31
        OurSMbyte = DQBresetBit(OurSMbyte, iii)
        OurCMbyte = DQBsetBit(OurCMbyte, iii)
      CASE 0
        OurSMbyte = DQBresetBit(OurSMbyte, iii)
        OurCMbyte = DQBresetBit(OurCMbyte, iii)
      CASE 1
        OurSMbyte = DQBsetBit(OurSMbyte, iii)
        OurCMbyte = DQBresetBit(OurCMbyte, iii)
      CASE 22
        OurSMbyte = DQBsetBit(OurSMbyte, iii)
        OurCMbyte = DQBsetBit(OurCMbyte, iii)
      END SELECT
    NEXT iii
    MID$(Cursor, 1 + (i * 2) + (1 - ii), 1) = CHR$(OurSMbyte)
    MID$(Cursor, 33 + (i * 2) + (1 - ii), 1) = CHR$(OurCMbyte)
  NEXT ii
NEXT i

END SUB

SUB HandleCurGUI
'
x = DQBmouseX: y = DQBmouseY
IF DQBmouseLB THEN
  DQBwait 1
  IF y > 25 AND y < 185 THEN
    IF x > 71 AND x < 231 THEN
      px = ((x - 71) \ 10): py = ((y - 25) \ 10)
      SELECT CASE Col
      CASE 0: c = 1
      CASE 1: c = 0
      CASE 2: c = 31
      CASE 3: c = 22
      END SELECT
      DQBmouseHide
      DQBboxf VIDEO, (72 + (px * 10)), (26 + (py * 10)), (80 + (px * 10)), (34 + (py * 10)), c
      DQBpset VIDEO, 51 + px, 25 + py, c
      DQBmouseShow
      EncodeCur
      Modified = TRUE
    END IF
    IF x > 53 AND x < 63 AND y > 60 AND y < 140 THEN
      IF (y - 60) MOD 20 < 13 THEN
        DQBmouseHide
        DQBbox VIDEO, 52, 59 + (Col * 20), 64, 74 + (Col * 20), 24
        Col = (y - 60) \ 20
        DQBbox VIDEO, 52, 59 + (Col * 20), 64, 74 + (Col * 20), 28
        DQBmouseShow
      END IF
    END IF
    IF x > 240 AND x < 264 THEN
      IF (y - 25) MOD 20 < 16 THEN
        Button = (y - 25) \ 20
        SELECT CASE Button
        CASE 0
          PushButton 240, 25, CHR$(24)
          DQBmouseHide
          GET (51, 25)-(66, 25), Buf1
          GET (51, 26)-(66, 40), Buf2
          PUT (51, 25), Buf2, PSET
          PUT (51, 40), Buf1, PSET
          GOSUB DrawCur
          DQBmouseShow
        CASE 1
          PushButton 240, 45, CHR$(25)
          DQBmouseHide
          GET (51, 25)-(66, 39), Buf1
          GET (51, 40)-(66, 40), Buf2
          PUT (51, 25), Buf2, PSET
          PUT (51, 26), Buf1, PSET
          GOSUB DrawCur
          DQBmouseShow
        CASE 2
          PushButton 240, 65, CHR$(26)
          DQBmouseHide
          GET (51, 25)-(65, 40), Buf1
          GET (66, 25)-(66, 40), Buf2
          PUT (52, 25), Buf1, PSET
          PUT (51, 25), Buf2, PSET
          GOSUB DrawCur
          DQBmouseShow
        CASE 3
          PushButton 240, 85, CHR$(27)
          DQBmouseHide
          GET (52, 25)-(66, 40), Buf1
          GET (51, 25)-(51, 40), Buf2
          PUT (51, 25), Buf1, PSET
          PUT (66, 25), Buf2, PSET
          GOSUB DrawCur
          DQBmouseShow
        CASE 4
          PushButton 240, 105, CHR$(18)
          DQBmouseHide
          FOR i = 0 TO 7
            GET (51, 25 + i)-(66, 25 + i), Buf1
            GET (51, 40 - i)-(66, 40 - i), Buf2
            PUT (51, 25 + i), Buf2, PSET
            PUT (51, 40 - i), Buf1, PSET
          NEXT i
          GOSUB DrawCur
          DQBmouseShow
        CASE 5
          PushButton 240, 125, CHR$(29)
          DQBmouseHide
          FOR i = 0 TO 7
            GET (51 + i, 25)-(51 + i, 40), Buf1
            GET (66 - i, 25)-(66 - i, 40), Buf2
            PUT (51 + i, 25), Buf2, PSET
            PUT (66 - i, 25), Buf1, PSET
          NEXT i
          GOSUB DrawCur
          DQBmouseShow
        CASE 6
          PushButton 240, 145, "R"
          DQBmouseHide
          FOR rx = 0 TO 7
            FOR ry = 0 TO 7
              p1 = DQBpoint(VIDEO, 51 + rx, 25 + ry)
              p2 = DQBpoint(VIDEO, 66 - ry, 25 + rx)
              p3 = DQBpoint(VIDEO, 66 - rx, 40 - ry)
              p4 = DQBpoint(VIDEO, 51 + ry, 40 - rx)
              DQBpset VIDEO, 51 + rx, 25 + ry, p4
              DQBpset VIDEO, 66 - ry, 25 + rx, p1
              DQBpset VIDEO, 66 - rx, 40 - ry, p2
              DQBpset VIDEO, 51 + ry, 40 - rx, p3
            NEXT ry
          NEXT rx
          GOSUB DrawCur
          DQBmouseShow
        END SELECT
      END IF
    END IF
  END IF
END IF
EXIT SUB

DrawCur:
FOR i = 0 TO 15
  FOR ii = 0 TO 15
    DQBboxf VIDEO, (72 + (i * 10)), (26 + (ii * 10)), (80 + (i * 10)), (34 + (ii * 10)), DQBpoint(VIDEO, 51 + i, 25 + ii)
  NEXT ii
NEXT i
EncodeCur
RETURN

END SUB

SUB HandleFontGUI
'
x = DQBmouseX: y = DQBmouseY
a = DQBmouseLB: b = DQBmouseRB
k$ = INKEY$
IF k$ = CHR$(0) + "M" THEN
  s = ASC(MID$(Font, Char + 2049, 1))
  IF s < 8 THEN s = s + 1: MID$(Font, Char + 2049, 1) = CHR$(s): DrawChar
END IF
IF k$ = CHR$(0) + "K" THEN
  s = ASC(MID$(Font, Char + 2049, 1))
  IF s > 1 THEN s = s - 1: MID$(Font, Char + 2049, 1) = CHR$(s): DrawChar
END IF
IF k$ = CHR$(0) + "P" THEN
  s = ASC(MID$(Font, 2305, 1))
  IF s < 8 THEN s = s + 1: MID$(Font, 2305, 1) = CHR$(s): DrawChar
END IF
IF k$ = CHR$(0) + "H" THEN
  s = ASC(MID$(Font, 2305, 1))
  IF s > 1 THEN s = s - 1: MID$(Font, 2305, 1) = CHR$(s): DrawChar
END IF
IF a OR b THEN
  IF x > 9 AND x < 89 AND y > 24 AND y < 104 THEN
    px = (x - 10) \ 10
    py = (y - 25) \ 10
    DQBwait 1
    DQBmouseHide
    IF a THEN
      DQBpset VIDEO, (81 + px), (107 + py), 31
    ELSE
      DQBpset VIDEO, (81 + px), (107 + py), 0
    END IF
    DrawChar
  END IF
END IF
IF a THEN
  IF x > 8 AND x < 98 AND y > 125 AND y < 185 THEN
    IF (x - 8) MOD 30 < 24 THEN
      IF (y - 125) MOD 20 < 16 THEN
        Button = (((y - 125) \ 20) * 3) + ((x - 8) \ 30)
        SELECT CASE Button
        CASE 0
          PushButton 8, 125, CHR$(24)
          DQBmouseHide
          GET (81, 108)-(88, 114), Buf1
          GET (81, 107)-(88, 107), Buf2
          PUT (81, 107), Buf1, PSET
          PUT (81, 114), Buf2, PSET
          DQBmouseShow
          DrawChar
        CASE 1
          PushButton 38, 125, CHR$(25)
          DQBmouseHide
          GET (81, 107)-(88, 113), Buf1
          GET (81, 114)-(88, 114), Buf2
          PUT (81, 107), Buf2, PSET
          PUT (81, 108), Buf1, PSET
          DQBmouseShow
          DrawChar
        CASE 2
          PushButton 68, 125, CHR$(18)
          DQBmouseHide
          FOR i = 0 TO 3
            GET (81, 107 + i)-(88, 107 + i), Buf1
            GET (81, 114 - i)-(88, 114 - i), Buf2
            PUT (81, 107 + i), Buf2, PSET
            PUT (81, 114 - i), Buf1, PSET
          NEXT i
          DQBmouseShow
          DrawChar
        CASE 3
          PushButton 8, 145, CHR$(26)
          DQBmouseHide
          GET (81, 107)-(87, 114), Buf1
          GET (88, 107)-(88, 114), Buf2
          PUT (82, 107), Buf1, PSET
          PUT (81, 107), Buf2, PSET
          DQBmouseShow
          DrawChar
        CASE 4
          PushButton 38, 145, CHR$(27)
          DQBmouseHide
          GET (81, 107)-(81, 114), Buf1
          GET (82, 107)-(88, 114), Buf2
          PUT (88, 107), Buf1, PSET
          PUT (81, 107), Buf2, PSET
          DQBmouseShow
          DrawChar
        CASE 5
          PushButton 68, 145, CHR$(29)
          DQBmouseHide
          FOR i = 0 TO 3
            GET (81 + i, 107)-(81 + i, 114), Buf1
            GET (88 - i, 107)-(88 - i, 114), Buf2
            PUT (81 + i, 107), Buf2, PSET
            PUT (88 - i, 107), Buf1, PSET
          NEXT i
          DQBmouseShow
          DrawChar
        CASE 6
          PushButton 8, 165, "R"
          DQBmouseHide
          FOR rx = 0 TO 3
            FOR ry = 0 TO 3
              p1 = DQBpoint(VIDEO, 81 + rx, 107 + ry)
              p2 = DQBpoint(VIDEO, 88 - ry, 107 + rx)
              p3 = DQBpoint(VIDEO, 88 - rx, 114 - ry)
              p4 = DQBpoint(VIDEO, 81 + ry, 114 - rx)
              DQBpset VIDEO, 81 + rx, 107 + ry, p4
              DQBpset VIDEO, 88 - ry, 107 + rx, p1
              DQBpset VIDEO, 88 - rx, 114 - ry, p2
              DQBpset VIDEO, 81 + ry, 114 - rx, p3
            NEXT ry
          NEXT rx
          DQBmouseShow
          DrawChar
        CASE 7
          PushButton 38, 165, CHR$(10)
          DQBmouseHide
          Buf1(0) = 64: Buf1(1) = 8
          FOR i = 2 TO 130: Buf1(i) = &H1F1F: NEXT i
          PUT (81, 107), Buf1
          DQBmouseShow
          DrawChar
        CASE 8
          PushButton 68, 165, "U"
          DQBmouseHide
          DQBsetFont Font
          DQBsetTextBackCol 0
          DQBsetTextStyle SOLID
          DQBprint VIDEO, CHR$(Char), 81, 107, 31
          DQBsetTextStyle NONE
          DQBsetBIOSfont
          DQBmouseShow
          DrawChar
        END SELECT
      END IF
    END IF
  END IF
  IF x > 103 AND x < 312 AND y > 24 AND y < 185 THEN
    IF y > 104 THEN
      nChar = 128 + (((y - 105) \ 8) * 13) + ((x - 104) \ 16)
    ELSE
      nChar = (((y - 25) \ 8) * 13) + ((x - 104) \ 16)
    END IF
    IF x > 279 THEN
      IF y > 96 AND y < 105 THEN nChar = -1
      IF y > 176 THEN nChar = -1
    END IF
    IF nChar <> Char AND nChar > 0 THEN
      DQBmouseHide
      FOR i = 0 TO 7
        n = 0
        FOR ii = 0 TO 7
          IF DQBpoint(VIDEO, 81 + ii, 107 + i) = 31 THEN n = DQBsetBit(n, 7 - ii)
        NEXT ii
        MID$(Font, (Char * 8) + i + 1, 1) = CHR$(n)
      NEXT i
      DQBsetFont Font
      IF Char > 127 THEN
        xc = (((Char - 128) MOD 13) * 16) + 112
        yc = (((Char - 128) \ 13) * 8) + 105
      ELSE
        xc = ((Char MOD 13) * 16) + 112
        yc = ((Char \ 13) * 8) + 25
      END IF
      DQBsetTextBackCol 0
      DQBsetTextStyle SOLID
      DQBprint VIDEO, CHR$(Char), xc, yc, 40
      Char = nChar
      IF y > 104 THEN
        xc = (((Char - 128) MOD 13) * 16) + 112
        yc = (((Char - 128) \ 13) * 8) + 105
      ELSE
        xc = ((Char MOD 13) * 16) + 112
        yc = ((Char \ 13) * 8) + 25
      END IF
      DQBsetTextBackCol 2
      DQBprint VIDEO, CHR$(Char), xc, yc, 10
      DQBsetTextBackCol 0
      DQBprint VIDEO, CHR$(Char), 81, 107, 31
      DQBsetBIOSfont
      DQBsetTextStyle SOLID
      DQBsetTextBackCol 21
      ascii$ = LTRIM$(STR$(Char)): IF LEN(ascii$) < 3 THEN ascii$ = ascii$ + SPACE$(3 - LEN(ascii$))
      DQBprint VIDEO, ascii$, 53, 107, 3
      DQBsetTextBackCol 0
      DQBsetTextStyle NONE
      DQBmouseShow
      DrawChar
      Modified = TRUE
    END IF
  END IF
END IF

END SUB

SUB Init
'
COLOR 14: PRINT "DirectQB Tools version 1.1 - by Angelo Mottola, Enhanced Creations 1998"
COLOR 7: PRINT "Initializing DirectQB...";
r = DQBinit(2, 0, 0)
IF r <> 0 THEN
  PRINT : PRINT
  PRINT "Error: ";
  SELECT CASE r
  CASE 1: PRINT "this program requires a 386 or better CPU to run!"
  CASE 2: PRINT "no expanded memory managers were found on your system!"
  CASE 3: PRINT "this program requires 128 KB of free EMS memory to run!"
  END SELECT
  PRINT "Program aborted"
  DQBclose
  END
END IF
IF NOT DQBmouseDetected THEN
  PRINT : PRINT
  PRINT "Error: mouse not detected!"
  PRINT "Program aborted"
  DQBclose
  END
END IF
PRINT "done"
PRINT "Loading configuration...";
OPEN "DQBTOOLS.CFG" FOR BINARY AS #1
IF LOF(1) = 0 THEN
  tool = 1: PUT #1, , tool
  FontExt = "FNT": PUT #1, , FontExt
  CurExt = "CUR": PUT #1, , CurExt
  CLOSE #1
ELSE
  GET #1, , tool: GET #1, , FontExt: GET #1, , CurExt
  CLOSE #1
END IF
PRINT "done"
DEF SEG = &HFFA6
FOR i = 0 TO 1023
  MID$(Font, i + 1, 1) = CHR$(PEEK(&HE + i))
NEXT i
FOR i = 1025 TO 2048: MID$(Font, i, 1) = CHR$(0): NEXT i
FOR i = 0 TO 256: MID$(Font, 2049 + i, 1) = CHR$(8): NEXT i
MID$(Cursor, 1, 32) = STRING$(32, 255)
MID$(Cursor, 33, 32) = STRING$(32, 0)

SCREEN 13
DQBresetMouse
DQBboxf 1, 0, 0, 319, 8, 1
DQBprint 1, "DirectQB Tools", 1, 1, 31
DQBboxf 1, 0, 9, 319, 17, 27
DQBboxf 1, 0, 18, 319, 190, 21
DQBboxf 1, 0, 191, 319, 199, 3
DQBprint 1, "File  Tool  Options  Test", 9, 10, 0
DQBcopyLayer 1, VIDEO
DrawWindow 50, 50, 269, 149, "Welcome", 32
Center 70, "DirectQB Tools", 31
Center 79, "version 1.21", 31
Center 96, "by Angelo Mottola,", 31
Center 105, "Enhanced Creations 1998-99", 31
DrawButton 136, 130, " OK "
DQBmouseShow
DO
  x = DQBmouseX: y = DQBmouseY
  IF DQBmouseLB THEN
    IF x > 136 AND x < 184 AND y > 130 AND y < 146 THEN EXIT DO
  END IF
LOOP
PushButton 136, 130, " OK "
DQBmouseHide
DQBcopyLayer 1, VIDEO
DQBmouseShow
SELECT CASE tool
CASE 1: FileName = "NONAME." + FontExt: DrawFontGUI
CASE 2: FileName = "NONAME." + CurExt: DrawCurGUI
END SELECT

END SUB

SUB LoadData
'
DIM File(1000) AS STRING, Dir(1000) AS STRING
DIM NumFiles AS INTEGER, NumDirs AS INTEGER
DIM FilePos AS INTEGER, DirPos AS INTEGER
DIM FileCur AS SINGLE, DirCur AS SINGLE
DIM FileStep AS SINGLE, DirStep AS SINGLE
DIM FileSel AS STRING

DQBmouseHide
SELECT CASE tool
CASE 1: Ext$ = "." + FontExt: t$ = "Load font"
CASE 2: Ext$ = "." + CurExt: t$ = "Load mouse cursor"
END SELECT
DrawWindow 20, 28, 300, 170, t$, 32
DQBprint VIDEO, "Files", 30, 52, 31
DQBboxf VIDEO, 31, 61, 124, 139, 21
DQBline VIDEO, 30, 60, 124, 60, 24
DQBline VIDEO, 30, 60, 30, 139, 24
DQBline VIDEO, 125, 61, 125, 140, 28
DQBline VIDEO, 31, 140, 125, 140, 28
DrawButton 127, 60, CHR$(30)
DrawButton 127, 125, CHR$(31)
DQBboxf VIDEO, 127, 76, 150, 124, 22
DQBline VIDEO, 127, 76, 127, 124, 24
DQBline VIDEO, 150, 76, 150, 124, 27
DQBprint VIDEO, "Dirs/Units", 175, 52, 31
DQBboxf VIDEO, 176, 61, 256, 139, 21
DQBline VIDEO, 176, 60, 256, 60, 24
DQBline VIDEO, 176, 60, 176, 139, 24
DQBline VIDEO, 257, 61, 257, 140, 28
DQBline VIDEO, 177, 140, 257, 140, 28
DrawButton 259, 60, CHR$(30)
DrawButton 259, 125, CHR$(31)
DQBboxf VIDEO, 259, 76, 282, 124, 22
DQBline VIDEO, 259, 76, 259, 124, 24
DQBline VIDEO, 282, 76, 282, 124, 27
DrawButton 90, 148, "  OK  "
DrawButton 166, 148, "CANCEL"
GOSUB ShowListLoad
DO
  x = DQBmouseX: y = DQBmouseY
  IF DQBmouseLB THEN
    IF y > 148 AND y < 164 THEN
      IF x > 90 AND x < 154 THEN
        PushButton 90, 148, "  OK  "
        EXIT DO
      ELSEIF x > 166 AND x < 230 THEN
        PushButton 166, 148, "CANCEL"
        FileSel = "": EXIT DO
      END IF
    END IF
    IF y > 60 AND y < 140 THEN
      IF x > 127 AND x < 151 AND y < 76 AND FilePos > 0 THEN
        PushButton 127, 60, CHR$(30)
        DQBmouseHide
        FilePos = FilePos - 1: FileCur = FileCur - FileStep
        DQBboxf VIDEO, 31, 61, 124, 139, 21
        DQBboxf VIDEO, 128, 76, 149, 124, 22
        FOR i = 0 TO 9
          IF INSTR(File(FilePos + i), ".") > 0 THEN
            a$ = LEFT$(File(FilePos + i), INSTR(File(FilePos + i), ".") - 1)
          ELSE
            a$ = File(FilePos + i)
          END IF
          IF LEN(a$) > 12 THEN a$ = LEFT$(a$, 10) + ".."
          DQBprint VIDEO, a$, 32, 61 + (i * 8), 30
        NEXT i
        DQBboxf VIDEO, 128, 76 + INT(FileCur), 149, 84 + INT(FileCur), 4
        DQBmouseShow
      END IF
      IF x > 127 AND x < 151 AND y > 125 AND FilePos < NumFiles - 9 THEN
        PushButton 127, 125, CHR$(31)
        DQBmouseHide
        FilePos = FilePos + 1: FileCur = FileCur + FileStep
        DQBboxf VIDEO, 31, 61, 124, 139, 21
        DQBboxf VIDEO, 128, 76, 149, 124, 22
        FOR i = 0 TO 9
          IF INSTR(File(FilePos + i), ".") > 0 THEN
            a$ = LEFT$(File(FilePos + i), INSTR(File(FilePos + i), ".") - 1)
          ELSE
            a$ = File(FilePos + i)
          END IF
          IF LEN(a$) > 12 THEN a$ = LEFT$(a$, 10) + ".."
          DQBprint VIDEO, a$, 32, 61 + (i * 8), 30
        NEXT i
        DQBboxf VIDEO, 128, 76 + INT(FileCur), 149, 84 + INT(FileCur), 4
        DQBmouseShow
      END IF
      IF x > 259 AND x < 283 AND y < 76 AND DirPos > 0 THEN
        PushButton 259, 60, CHR$(30)
        DQBmouseHide
        DirPos = DirPos - 1: DirCur = DirCur - DirStep
        DQBboxf VIDEO, 176, 61, 256, 139, 21
        DQBboxf VIDEO, 260, 76, 281, 124, 22
        FOR i = 0 TO 9
          a$ = Dir(DirPos + i)
          IF LEN(a$) > 10 THEN a$ = LEFT$(a$, 8) + ".."
          DQBprint VIDEO, a$, 177, 61 + (i * 8), 30
        NEXT i
        DQBboxf VIDEO, 260, 76 + INT(DirCur), 281, 84 + INT(DirCur), 4
        DQBmouseShow
      END IF
      IF x > 259 AND x < 283 AND y > 125 AND DirPos < NumDirs - 9 THEN
        PushButton 259, 125, CHR$(31)
        DQBmouseHide
        DirPos = DirPos + 1: DirCur = DirCur + DirStep
        DQBboxf VIDEO, 176, 61, 256, 139, 21
        DQBboxf VIDEO, 260, 76, 281, 124, 22
        FOR i = 0 TO 9
          a$ = Dir(DirPos + i)
          IF LEN(a$) > 10 THEN a$ = LEFT$(a$, 8) + ".."
          DQBprint VIDEO, a$, 177, 61 + (i * 8), 30
        NEXT i
        DQBboxf VIDEO, 260, 76 + INT(DirCur), 281, 84 + INT(DirCur), 4
        DQBmouseShow
      END IF
      IF x > 30 AND x < 125 THEN
        IF (FilePos + ((y - 61) \ 8)) < NumFiles THEN
          FileSel = File(FilePos + ((y - 61) \ 8))
          DQBsetTextStyle SOLID
          DQBsetTextBackCol 26
          path$ = DQBdrive$ + ":\" + DQBpath$
          IF LEN(path$) > 20 THEN path$ = LEFT$(path$, 18) + ".."
          IF LEN(path$) <> 3 THEN path$ = path$ + "\"
          path$ = path$ + FileSel
          IF LEN(path$) < 33 THEN path$ = path$ + SPACE$(33 - LEN(path$))
          DQBmouseHide
          DQBprint VIDEO, path$, 30, 42, 0
          DQBsetTextStyle NONE
          DQBmouseShow
        END IF
      END IF
      IF x > 175 AND x < 257 THEN
        DirSel = DirPos + ((y - 61) \ 8)
        IF DirSel > (NumDirs - DQBnumDrives) THEN
          IF DirSel <= NumDirs THEN
            DQBsetDrive CHR$(DirSel - NumDirs + DQBnumDrives + 64)
          END IF
        ELSE
          IF DQBchDir(Dir(DirPos + ((y - 61) \ 8))) THEN ERROR 77
        END IF
        GOSUB ShowListLoad
        DQBwait 20
      END IF
    END IF
  END IF
LOOP

IF FileSel <> "" THEN
  FileName = FileSel
  SELECT CASE tool
  CASE 1
    OPEN FileSel FOR BINARY AS #1
    IF LOF(1) <> 2048 AND LOF(1) <> 2305 THEN CLOSE #1: ERROR 100: EXIT SUB
    IF LOF(1) = 2048 THEN
      FOR i = 1 TO 2048: a$ = " ": GET #1, , a$: MID$(Font, i, 1) = a$: NEXT i
      FOR i = 0 TO 256: MID$(Font, 2049 + i, 1) = CHR$(8): NEXT i
    ELSE
      GET #1, , Font
    END IF
    CLOSE #1: EXIT SUB
  CASE 2
    OPEN FileSel FOR BINARY AS #1
    IF LOF(1) <> 64 THEN CLOSE #1: ERROR 101: EXIT SUB
    GET #1, , Cursor: CLOSE #1: EXIT SUB
  END SELECT
END IF
EXIT SUB


ShowListLoad:
DQBmouseHide
FOR i = 0 TO 1000: File(i) = "": Dir(i) = "": NEXT i
NumFiles = 0
File(0) = DQBdir$("*" + Ext$, ATTRIB.A)
IF File(0) <> "" THEN
  DO
    NumFiles = NumFiles + 1
    File(NumFiles) = DQBdir$("", ATTRIB.A)
  LOOP WHILE File(NumFiles) <> ""
END IF
NumDirs = 0
Dir(0) = DQBdir$("*.", ATTRIB.D)
IF Dir(0) <> "" THEN
  DO
    NumDirs = NumDirs + 1
    Dir(NumDirs) = DQBdir$("", ATTRIB.D)
  LOOP WHILE Dir(NumDirs) <> ""
END IF
NumDirs = NumDirs - 1
FOR i = 1 TO DQBnumDrives
  NumDirs = NumDirs + 1
  Dir(NumDirs) = "[-" + CHR$(64 + i) + "-]"
NEXT i
path$ = DQBdrive$ + ":\" + DQBpath$
IF LEN(path$) > 20 THEN path$ = LEFT$(path$, 18) + ".."
IF LEN(path$) <> 3 THEN path$ = path$ + "\"
path$ = path$ + "*" + Ext$
IF LEN(path$) < 33 THEN path$ = path$ + SPACE$(33 - LEN(path$))
DQBsetTextStyle SOLID
DQBsetTextBackCol 26
DQBprint VIDEO, path$, 30, 42, 0
DQBboxf VIDEO, 31, 61, 124, 139, 21
DQBboxf VIDEO, 176, 61, 256, 139, 21
DQBboxf VIDEO, 128, 76, 149, 124, 22
DQBboxf VIDEO, 260, 76, 281, 124, 22
FilePos = 0: DirPos = 0: FileCur = 0: DirCur = 0
IF NumFiles <= 10 THEN
  FileStep = 0
  DQBboxf VIDEO, 128, 76, 149, 124, 4
ELSE
  FileStep = 40 / (NumFiles - 9)
  DQBboxf VIDEO, 128, 76, 149, 84, 4
END IF
IF NumDirs <= 10 THEN
  DirStep = 0
  DQBboxf VIDEO, 260, 76, 281, 124, 4
ELSE
  DirStep = 40 / (NumDirs - 9)
  DQBboxf VIDEO, 260, 76, 281, 84, 4
END IF
DQBsetTextStyle NONE
FOR i = 0 TO 9
  IF INSTR(File(i), ".") > 0 THEN
    a$ = LEFT$(File(i), INSTR(File(i), ".") - 1)
  ELSE
    a$ = File(FilePos + i)
  END IF
  IF LEN(a$) > 12 THEN a$ = LEFT$(a$, 10) + ".."
  IF a$ <> "" THEN
    DQBprint VIDEO, a$, 32, 61 + (i * 8), 30
  END IF
  a$ = Dir(i)
  IF LEN(a$) > 10 THEN a$ = LEFT$(a$, 8) + ".."
  DQBprint VIDEO, a$, 177, 61 + (i * 8), 30
NEXT i
DQBmouseShow
FileSel = ""
RETURN

END SUB

SUB Main
'
DO
  SELECT CASE tool
  CASE 1: HandleFontGUI
  CASE 2: HandleCurGUI
  END SELECT
  IF DQBmouseLB AND DQBmouseY < 18 AND DQBmouseY > 8 THEN
    SELECT CASE DQBmouseX
    CASE 0 TO 47
      DQBmouseHide
      DQBcopyLayer VIDEO, 1
      DQBboxf VIDEO, 1, 20, 56, 68, 0
      DQBboxf VIDEO, 0, 19, 55, 67, 26
      DQBline VIDEO, 0, 19, 54, 19, 28
      DQBline VIDEO, 0, 19, 0, 66, 28
      DQBline VIDEO, 55, 20, 55, 67, 24
      DQBline VIDEO, 1, 67, 55, 67, 24
      DQBprint VIDEO, "New", 8, 24, 0
      DQBprint VIDEO, "Open", 8, 32, 0
      DQBprint VIDEO, "Save", 8, 40, 0
      DQBprint VIDEO, "About", 8, 48, 0
      DQBprint VIDEO, "Quit", 8, 56, 0
      DQBprint VIDEO, "File", 9, 10, 14
      DQBmouseShow
      DQBwait 10
      DO
        x = DQBmouseX: y = DQBmouseY
        IF DQBmouseLB THEN
          IF x < 56 AND y > 24 AND y < 64 THEN
            Sel = (y - 24) \ 8
            SELECT CASE Sel
            CASE 0
              DQBmouseHide
              DQBprint VIDEO, "New", 8, 24, 14
              Cancel = FALSE
              IF Modified THEN
                SELECT CASE tool
                CASE 1: Title$ = "New font": Text$ = "Font has been modified. Save?"
                CASE 2: Title$ = "New mouse cursor": Text$ = "Cursor has been modified. Save?"
                END SELECT
                SELECT CASE Query(Title$, Text$, 1)
                CASE 1: SaveData
                CASE 3: Cancel = TRUE
                END SELECT
              END IF
              IF NOT Cancel THEN
                SELECT CASE tool
                CASE 1
                  DEF SEG = &HFFA6
                  FOR i = 0 TO 1023
                    MID$(Font, i + 1, 1) = CHR$(PEEK(&HE + i))
                  NEXT i
                  FOR i = 1025 TO 2048: MID$(Font, i, 1) = CHR$(0): NEXT i
                  FOR i = 0 TO 256: MID$(Font, 2049 + i, 1) = CHR$(8): NEXT i
                  DrawFontGUI
                  DQBmouseHide
                  DQBcopyLayer VIDEO, 1
                  FileName = "NONAME." + FontExt
                CASE 2
                  MID$(Cursor, 1, 32) = STRING$(32, 255)
                  MID$(Cursor, 33, 32) = STRING$(32, 0)
                  DrawCurGUI
                  DQBmouseHide
                  DQBcopyLayer VIDEO, 1
                  FileName = "NONAME." + CurExt
                END SELECT
              END IF
              EXIT DO
            CASE 1
              DQBmouseHide
              DQBprint VIDEO, "Open", 8, 32, 14
              LoadData
              SELECT CASE tool
              CASE 1: DrawFontGUI
              CASE 2: DrawCurGUI
              END SELECT
              DQBmouseHide
              DQBcopyLayer VIDEO, 1
              EXIT DO
            CASE 2
              DQBmouseHide
              DQBprint VIDEO, "Save", 8, 40, 14
              SaveData
              DQBmouseHide
              DQBboxf 1, 0, 191, 319, 199, 3
              DQBprint VIDEO, FileName, (315 - (LEN(FileName) * 8)), 192, 0
              EXIT DO
            CASE 3
              DQBmouseHide
              DQBprint VIDEO, "About", 8, 48, 14
              DrawWindow 20, 30, 300, 170, "About", 32
              DrawButton 128, 150, "  OK  "
              Center 50, "DirectQB Tools", 31
              Center 62, "version 1.21", 30
              Center 80, "by Angelo Mottola,", 31
              Center 89, "Enhanced Creations 1998-99", 31
              Center 100, "Visit Enhanced Creations at", 30
              Center 109, "http://ec.quickbasic.com", 31
              Center 122, "Part of the DirectQB Library,", 31
              Center 131, "by Enhanced Creations 1998-99", 31
              DQBmouseShow
              DO
                x = DQBmouseX: y = DQBmouseY
                IF DQBmouseLB AND x > 128 AND x < 192 AND y > 150 AND y < 165 THEN EXIT DO
              LOOP
              PushButton 128, 150, "  OK  "
              EXIT DO
            CASE 4
              DQBmouseHide
              DQBprint VIDEO, "Quit", 8, 56, 14
              IF Query("Quit DQBtools", "Really quit?", 0) = 1 THEN
                IF Modified THEN
                  SELECT CASE tool
                  CASE 1: Text$ = "Font has been modified. Save?"
                  CASE 2: Text$ = "Cursor has been modified. Save?"
                  END SELECT
                  IF Query("Shutting down", Text$, 0) = 1 THEN SaveData
                END IF
                EXIT SUB
              END IF
              EXIT DO
            END SELECT
          ELSE
            EXIT DO
          END IF
        END IF
      LOOP
      DQBmouseHide
      DQBcopyLayer 1, VIDEO
      DQBprint VIDEO, "File  Tool  Options", 9, 10, 0
      DQBmouseShow
    CASE 48 TO 95
      DQBmouseHide
      DQBcopyLayer VIDEO, 1
      DQBboxf VIDEO, 49, 20, 168, 44, 0
      DQBboxf VIDEO, 48, 19, 167, 43, 26
      DQBline VIDEO, 48, 19, 166, 19, 28
      DQBline VIDEO, 48, 19, 48, 42, 28
      DQBline VIDEO, 167, 20, 167, 43, 24
      DQBline VIDEO, 49, 43, 167, 43, 24
      DQBprint VIDEO, "Font editor", 56, 24, 0
      DQBprint VIDEO, "Cursor editor", 56, 32, 0
      DQBprint VIDEO, "Tool", 57, 10, 14
      DQBmouseShow
      DQBwait 10
      DO
        x = DQBmouseX: y = DQBmouseY
        IF DQBmouseLB THEN
          IF x > 48 AND x < 167 AND y > 24 AND y < 40 THEN
            Sel = (y - 24) \ 8
            SELECT CASE Sel
            CASE 0
              DQBmouseHide
              DQBprint VIDEO, "Font editor", 56, 24, 14
              IF tool <> 1 THEN
                IF Modified THEN
                  IF Query("Changing tool", "Cursor has been modified. Save?", 0) = 1 THEN SaveData
                END IF
                MID$(Cursor, 1, 32) = STRING$(32, 255)
                MID$(Cursor, 33, 32) = STRING$(32, 0)
                FileName = "NONAME." + FontExt
                tool = 1: DrawFontGUI
                DQBmouseHide
                DQBcopyLayer VIDEO, 1
              END IF
              DQBwait 10
              EXIT DO
            CASE 1
              DQBmouseHide
              DQBprint VIDEO, "Cursor editor", 56, 32, 14
              IF tool <> 2 THEN
                IF Modified THEN
                  IF Query("Changing tool", "Font has been modified. Save?", 0) = 1 THEN SaveData
                END IF
                DEF SEG = &HFFA6
                FOR i = 0 TO 1023
                  MID$(Font, i + 1, 1) = CHR$(PEEK(&HE + i))
                NEXT i
                FOR i = 1025 TO 2048: MID$(Font, i, 1) = CHR$(0): NEXT i
                FOR i = 0 TO 255: MID$(Font, 2049 + i, 1) = CHR$(8): NEXT i
                FileName = "NONAME." + CurExt
                tool = 2: DrawCurGUI
                DQBmouseHide
                DQBcopyLayer VIDEO, 1
              END IF
              DQBwait 10
              EXIT DO
            END SELECT
          ELSE
            EXIT DO
          END IF
        END IF
      LOOP
      DQBmouseHide
      DQBcopyLayer 1, VIDEO
      DQBprint VIDEO, "File  Tool  Options", 9, 10, 0
      DQBmouseShow
    CASE 96 TO 160
      DQBmouseHide
      DQBcopyLayer VIDEO, 1
      DQBprint VIDEO, "Options", 105, 10, 14
      DrawWindow 60, 40, 260, 165, "Options", 42
      Center 55, "Start DQB Tools with", 31
      DrawButton 100, 68, " Font editor "
      DrawButton 100, 88, "Cursor editor"
      DQBbox VIDEO, 98, 46 + (tool * 20), 221, 65 + (tool * 20), 28
      Center 110, "Extensions", 31
      Center 118, "Extensions still not", 43
      Center 126, "configurable in this", 43
      Center 134, "DQB Tools release", 43
      DrawButton 128, 145, "  OK  "
      DQBmouseShow
      DQBwait 10
      nTool = tool
      DO
        x = DQBmouseX: y = DQBmouseY
        IF DQBmouseLB THEN
          IF x > 128 AND x < 192 AND y > 145 AND y < 160 THEN EXIT DO
          IF x > 100 AND x < 220 AND y > 68 AND y < 108 THEN
            IF (y - 68) MOD 20 < 16 THEN
              DQBmouseHide
              DQBbox VIDEO, 98, 46 + (nTool * 20), 221, 65 + (nTool * 20), 26
              nTool = 1 + ((y - 68) \ 20)
              DQBbox VIDEO, 98, 46 + (nTool * 20), 221, 65 + (nTool * 20), 28
              SELECT CASE nTool
              CASE 1: PushButton 100, 68, " Font editor "
              CASE 2: PushButton 100, 88, "Cursor editor"
              END SELECT
            END IF
          END IF
        END IF
      LOOP
      PushButton 128, 145, "  OK  "
      OPEN "DQBTOOLS.CFG" FOR BINARY AS #1
      PUT #1, , nTool: PUT #1, , FontExt: PUT #1, , CurExt: CLOSE #1
      DQBmouseHide
      DQBcopyLayer 1, VIDEO
      DQBprint VIDEO, "File  Tool  Options", 9, 10, 0
      DQBmouseShow
    CASE 176 TO 210
      IF tool = 1 THEN
        DQBmouseHide
        DQBcopyLayer VIDEO, 1
        DQBprint VIDEO, "Test", 177, 10, 14
        DrawWindow 40, 35, 280, 160, "Font test", 2
        DQBsetFont Font
        DQBprint VIDEO, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 56, 55, 31
        DQBprint VIDEO, "abcdefghijklmnopqrstuvwxyz", 56, 65, 31
        DQBprint VIDEO, "01234567890", 56, 75, 31
        DQBprint VIDEO, ".,:;+-*/[]{}=?!œ$%&", 56, 85, 31
        DQBprint VIDEO, "This is just a test!!", 56, 95, 31
        DQBprints VIDEO, "This is bold text", 56, 105, 31, BOLD
        DQBprints VIDEO, "This is italic text", 56, 115, 31, ITALIC
        DQBprints VIDEO, "This is underlined text", 56, 125, 31, UNDERLINED
        DQBsetBIOSfont
        DrawButton 128, 140, "  OK  "
        DQBmouseShow
        DQBwait 10
        DO
          IF DQBmouseLB THEN
            IF DQBmouseY > 140 AND DQBmouseY < 156 THEN
              IF DQBmouseX > 128 AND DQBmouseX < 192 THEN EXIT DO
            END IF
          END IF
        LOOP
        PushButton 128, 140, "  OK  "
        DQBmouseHide
        DQBcopyLayer 1, VIDEO
        DQBmouseShow
      END IF
    END SELECT
  END IF
LOOP

END SUB

SUB Message (Text$)
'
DQBmouseHide
DQBboxf VIDEO, 0, 191, 319, 199, 3
DQBprint VIDEO, Text$, 1, 192, 0
DQBmouseShow

END SUB

SUB PushButton (x1, y1, Text$)
'
x2 = x1 + (LEN(Text$) * 8) + 15
DQBmouseHide
DQBboxf VIDEO, x1, y1, x2, (y1 + 15), 26
DQBline VIDEO, x1, y1, x1, (y1 + 14), 25
DQBline VIDEO, x1, y1, (x2 - 1), y1, 25
DQBline VIDEO, (x1 + 1), (y1 + 15), x2, (y1 + 15), 27
DQBline VIDEO, x2, (y1 + 1), x2, (y1 + 15), 27
DQBprint VIDEO, Text$, (x1 + 9), (y1 + 5), 29
DQBmouseShow

DQBwait 10

DQBmouseHide
DQBboxf VIDEO, x1, y1, x2, (y1 + 15), 28
DQBline VIDEO, x1, y1, x1, (y1 + 14), 29
DQBline VIDEO, x1, y1, (x2 - 1), y1, 29
DQBline VIDEO, (x1 + 1), (y1 + 15), x2, (y1 + 15), 27
DQBline VIDEO, x2, (y1 + 1), x2, (y1 + 15), 27
DQBprint VIDEO, Text$, (x1 + 8), (y1 + 4), 31
DQBmouseShow

END SUB

FUNCTION Query (Title$, Text$, Buttons)
'
DQBmouseHide
DQBcopyLayer VIDEO, 2
DrawWindow 30, 70, 290, 130, Title$, 32
Center 90, Text$, 31
SELECT CASE Buttons
CASE 0
  DrawButton 120, 105, "YES"
  DrawButton 168, 105, "NO"
CASE 1
  DrawButton 84, 105, "YES"
  DrawButton 132, 105, "NO"
  DrawButton 172, 105, "CANCEL"
END SELECT
DQBmouseShow
DO
  x = DQBmouseX: y = DQBmouseY
  IF DQBmouseLB AND y > 105 AND y < 121 THEN
    SELECT CASE Buttons
    CASE 0
      IF x > 120 AND x < 160 THEN
        PushButton 120, 105, "YES"
        Query = 1: EXIT DO
      ELSEIF x > 168 AND x < 200 THEN
        PushButton 168, 105, "NO"
        Query = 2: EXIT DO
      END IF
    CASE 1
      IF x > 84 AND x < 124 THEN
        PushButton 84, 105, "YES"
        Query = 1: EXIT DO
      ELSEIF x > 132 AND x < 156 THEN
        PushButton 132, 105, "NO"
        Query = 2: EXIT DO
      ELSEIF x > 172 AND x < 236 THEN
        PushButton 172, 105, "CANCEL"
        Query = 3: EXIT DO
      END IF
    END SELECT
  END IF
LOOP
DQBmouseHide
DQBcopyLayer 2, VIDEO

END FUNCTION

SUB SaveData
'
DIM Dir(1000) AS STRING, NumDirs AS INTEGER
DIM DirPos AS INTEGER, DirCur AS SINGLE
DIM DirStep AS SINGLE, FileSel AS STRING

SELECT CASE tool
CASE 1: t$ = "Save font"
CASE 2: t$ = "Save mouse cursor"
END SELECT
DrawWindow 20, 28, 300, 170, t$, 32
DQBprint VIDEO, "File name", 30, 81, 31
DQBboxf VIDEO, 31, 90, 138, 101, 22
DQBline VIDEO, 30, 89, 138, 89, 24
DQBline VIDEO, 30, 89, 30, 101, 24
DQBline VIDEO, 139, 90, 139, 101, 28
DQBline VIDEO, 31, 102, 139, 102, 28
DQBprint VIDEO, "Dirs/Units", 175, 52, 31
DQBboxf VIDEO, 176, 61, 256, 139, 21
DQBline VIDEO, 176, 60, 256, 60, 24
DQBline VIDEO, 176, 60, 176, 139, 24
DQBline VIDEO, 257, 61, 257, 140, 28
DQBline VIDEO, 177, 140, 257, 140, 28
DrawButton 259, 60, CHR$(30)
DrawButton 259, 125, CHR$(31)
DQBboxf VIDEO, 259, 76, 282, 124, 22
DQBline VIDEO, 259, 76, 259, 124, 24
DQBline VIDEO, 282, 76, 282, 124, 27
DrawButton 90, 148, "  OK  "
DrawButton 166, 148, "CANCEL"
GOSUB ShowListSave
DQBprint VIDEO, FileName + "_", 33, 92, 30
DQBmouseShow
DO
  x = DQBmouseX: y = DQBmouseY
  IF DQBmouseLB THEN
    IF y > 148 AND y < 164 THEN
      IF x > 90 AND x < 154 THEN
        PushButton 90, 148, "  OK  "
        EXIT DO
      ELSEIF x > 166 AND x < 230 THEN
        PushButton 166, 148, "CANCEL"
        EXIT SUB
      END IF
    END IF
    IF y > 60 AND y < 140 THEN
      IF x > 259 AND x < 283 AND y < 76 AND DirPos > 0 THEN
        PushButton 259, 60, CHR$(30)
        DQBmouseHide
        DirPos = DirPos - 1: DirCur = DirCur - DirStep
        DQBboxf VIDEO, 176, 61, 256, 139, 21
        DQBboxf VIDEO, 260, 76, 281, 124, 22
        FOR i = 0 TO 9
          DQBprint VIDEO, Dir(DirPos + i), 177, 61 + (i * 8), 30
        NEXT i
        DQBboxf VIDEO, 260, 76 + INT(DirCur), 281, 84 + INT(DirCur), 4
        DQBmouseShow
      END IF
      IF x > 259 AND x < 283 AND y > 125 AND DirPos < NumDirs - 9 THEN
        PushButton 259, 125, CHR$(31)
        DQBmouseHide
        DirPos = DirPos + 1: DirCur = DirCur + DirStep
        DQBboxf VIDEO, 176, 61, 256, 139, 21
        DQBboxf VIDEO, 260, 76, 281, 124, 22
        FOR i = 0 TO 9
          DQBprint VIDEO, Dir(DirPos + i), 177, 61 + (i * 8), 30
        NEXT i
        DQBboxf VIDEO, 260, 76 + INT(DirCur), 281, 84 + INT(DirCur), 4
        DQBmouseShow
      END IF
      IF x > 175 AND x < 257 THEN
        DirSel = DirPos + ((y - 61) \ 8)
        IF DirSel > (NumDirs - DQBnumDrives) THEN
          IF DirSel <= NumDirs THEN
            DQBsetDrive CHR$(DirSel - NumDirs + DQBnumDrives + 64)
          END IF
        ELSE
          IF DQBchDir(Dir(DirPos + ((y - 61) \ 8))) THEN ERROR 77
        END IF
        GOSUB ShowListSave
        DQBwait 20
      END IF
    END IF
  END IF
  k$ = INKEY$
  IF k$ <> "" THEN
    DQBmouseHide
    k$ = UCASE$(k$)
    SELECT CASE k$
    CASE "A" TO "Z", "0" TO "9", ".", "-", "+", "~", "!"
      IF k$ = "." THEN
        IF INSTR(FileName, ".") = 0 THEN FileName = FileName + "."
      ELSE
        IF INSTR(FileName, ".") > 0 THEN
          IF LEN(FileName) < INSTR(FileName, ".") + 3 THEN
            FileName = FileName + k$
          END IF
        ELSE
          IF LEN(FileName) < 8 THEN FileName = FileName + k$
        END IF
      END IF
    CASE CHR$(8)
      IF LEN(FileName) > 0 THEN FileName = LEFT$(FileName, LEN(FileName) - 1)
    CASE CHR$(13)
      IF LEN(FileName) > 0 THEN
        PushButton 90, 148, "  OK  "
        EXIT DO
      ELSE
        PushButton 166, 148, "CANCEL"
        EXIT SUB
      END IF
    END SELECT
    DQBboxf VIDEO, 31, 90, 138, 101, 22
    DQBprint VIDEO, FileName + "_", 33, 92, 30
    DQBmouseShow
  END IF
LOOP

IF INSTR(FileName, ".") = 0 THEN
  SELECT CASE tool
  CASE 1: FileName = FileName + "." + FontExt
  CASE 2: FileName = FileName + "." + CurExt
  END SELECT
END IF
OPEN FileName FOR BINARY AS #1
IF LOF(1) <> 0 THEN
  IF Query("Warning", "File already exists. Overwrite?", 0) = 2 THEN CLOSE #1: EXIT SUB
END IF
SELECT CASE tool
CASE 1: PUT #1, , Font
CASE 2: PUT #1, , Cursor
END SELECT
CLOSE #1
Modified = FALSE
EXIT SUB


ShowListSave:
DQBmouseHide
FOR i = 0 TO 1000: Dir(i) = "": NEXT i
NumDirs = 0
Dir(0) = DQBdir$("*.", ATTRIB.D)
IF Dir(0) <> "" THEN
  DO
    NumDirs = NumDirs + 1
    Dir(NumDirs) = DQBdir$("", ATTRIB.D)
  LOOP WHILE Dir(NumDirs) <> ""
END IF
NumDirs = NumDirs - 1
FOR i = 1 TO DQBnumDrives
  NumDirs = NumDirs + 1
  Dir(NumDirs) = "[-" + CHR$(64 + i) + "-]"
NEXT i
path$ = DQBdrive$ + ":\" + DQBpath$
IF LEN(path$) > 33 THEN path$ = LEFT$(path$, 31) + ".."
IF LEN(path$) < 33 THEN path$ = path$ + SPACE$(33 - LEN(path$))
DQBsetTextStyle SOLID
DQBsetTextBackCol 26
DQBprint VIDEO, path$, 30, 42, 0
DQBboxf VIDEO, 176, 61, 256, 139, 21
DQBboxf VIDEO, 260, 76, 281, 124, 22
DirPos = 0: DirCur = 0
IF NumDirs <= 10 THEN
  DirStep = 0
  DQBboxf VIDEO, 260, 76, 281, 124, 4
ELSE
  DirStep = 40 / (NumDirs - 9)
  DQBboxf VIDEO, 260, 76, 281, 84, 4
END IF
DQBsetTextStyle NONE
FOR i = 0 TO 9
  a$ = Dir(i)
  IF LEN(a$) > 10 THEN a$ = LEFT$(a$, 8) + ".."
  DQBprint VIDEO, a$, 177, 61 + (i * 8), 30
NEXT i
DQBmouseShow
RETURN

END SUB

