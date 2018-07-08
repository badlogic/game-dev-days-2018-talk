DECLARE SUB drawall ()
DECLARE SUB savecolmap ()
DECLARE SUB savemap ()
DECLARE SUB DrawGrid ()
DECLARE SUB Click ()
'$INCLUDE: 'DIRECTQB.BI'
TYPE poly
   x1 AS INTEGER
   x2 AS INTEGER
   x3 AS INTEGER
   x4 AS INTEGER
   y1 AS INTEGER
   y2 AS INTEGER
   y3 AS INTEGER
   y4 AS INTEGER
   z1 AS INTEGER
   z2 AS INTEGER
   z3 AS INTEGER
   z4 AS INTEGER
   p AS INTEGER
   texnum AS INTEGER
END TYPE

TYPE door
   op AS INTEGER
   in AS INTEGER
END TYPE


DIM SHARED floor(1000) AS poly
DIM SHARED wall(1000) AS poly
DIM SHARED floorindex%
DIM SHARED wallindex%
DIM SHARED doors(20) AS door

DIM SHARED mode%, tnum%, camx%, camy%, wcoff%, doornum%, itemnum%, itemtype%
DIM SHARED enemnum%, enemtype%
DIM SHARED file$
CONST FLOORCEILmode = 0
CONST WALLmodev = 1
CONST WALLmodeh = 2
CONST DOORmodev = 3
CONST DOORmodeh = 4
CONST CAMmode = 5
CONST ITEMmode = 6
CONST ENEMmode = 7

CONST IThealth1% = 2
CONST IThealth2% = 3
CONST ITnut1% = 4

CONST ENtype1% = 5      'blauer matrose
CONST ENtype2% = 6      'roter matrose
CONST ENtype4% = 7      'kapit„n Iglu

dqberr = DQBinit(2, 0, 0)
SCREEN 13


DrawGrid
DQBcopyLayer VIDEO, 2

mode% = FLOORCEILmode
itemtype% = 2
enemtype% = 5

DO
   drawall
   DQBcopyLayer 1, VIDEO
   Click
   i$ = INKEY$
   IF i$ = "m" THEN
      mode% = mode% + 1
      IF mode% > 7 THEN mode% = 0
   END IF
   IF i$ = "," THEN
      mode% = mode% - 1
      IF mode% < 0 THEN mode% = 7
   END IF
   IF i$ = "+" THEN tnum% = tnum% + 1
   IF i$ = "-" THEN tnum% = tnum% - 1
   IF i$ = "o" THEN wcoff% = wcoff% + 64
   IF i$ = "l" THEN wcoff% = wcoff% - 64
   IF i$ = "i" THEN
      itemtype% = itemtype% + 1
      IF itemtype% = 5 THEN itemtype = 2
   END IF
   IF i$ = "e" THEN
      enemtype% = enemtype% + 1
      IF enemtype% = 8 THEN enemtype% = 5
   END IF
LOOP UNTIL i$ = CHR$(27)

INPUT "DATEINAME:", file$
IF file$ <> "" THEN
   savemap
   savecolmap
END IF
DQBinitText
DQBclose

SUB Click
IF DQBmouseLB% THEN
   IF mode% = CAMmode THEN
      camx = DQBmouseX * 16
      camy = DQBmouseY * 16
   END IF
   IF mode% = FLOORCEILmode THEN
      x1 = (DQBmouseX% \ 4) * 4
      y1 = (DQBmouseY% \ 4) * 4
      DO
         DQBcopyLayer 2, 1
         x2 = (DQBmouseX% \ 4) * 4
         y2 = (DQBmouseY% \ 4) * 4
         
         drawall
         DQBbox 1, x1, y1, x2, y2, 4
         DQBpset 1, x2, y2, 15
         DQBcopyLayer 1, VIDEO
      LOOP UNTIL NOT DQBmouseLB%
  
      FOR x = x1 * 16 TO x2 * 16 - 64 STEP 64
         FOR z = y1 * 16 TO y2 * 16 - 64 STEP 64
         IF wallindex% < 1000 THEN
            wall(wallindex%).x1 = x
            wall(wallindex%).y1 = 32 + wcoff%
            wall(wallindex%).z1 = z

            wall(wallindex%).x2 = x
            wall(wallindex%).y2 = 32 + wcoff%
            wall(wallindex%).z2 = z + 64

            wall(wallindex%).x3 = x + 64
            wall(wallindex%).y3 = 32 + wcoff%
            wall(wallindex%).z3 = z + 64

            wall(wallindex%).x4 = x + 64
            wall(wallindex%).y4 = 32 + wcoff%
            wall(wallindex%).z4 = z
            wall(wallindex%).texnum = tnum%
            wallindex% = wallindex% + 1
           END IF
           IF floorindex% < 1000 THEN
            floor(floorindex%).x1 = x
            floor(floorindex%).y1 = -32
            floor(floorindex%).z1 = z

            floor(floorindex%).x2 = x
            floor(floorindex%).y2 = -32
            floor(floorindex%).z2 = z + 64

            floor(floorindex%).x3 = x + 64
            floor(floorindex%).y3 = -32
            floor(floorindex%).z3 = z + 64

            floor(floorindex%).x4 = x + 64
            floor(floorindex%).y4 = -32
            floor(floorindex%).z4 = z
            floor(floorindex%).texnum = tnum%
            floorindex% = floorindex% + 1
           END IF
         NEXT z
      NEXT x
   END IF
   IF mode% = WALLmodev THEN
      x1 = (DQBmouseX \ 4) * 4
      y1 = (DQBmouseY \ 4) * 4
      DO
         x2 = x1
         y2 = (DQBmouseY \ 4) * 4
         drawall
         DQBline 1, x1, y1, x2, y2, 6
         DQBpset 1, x2, y2, 15
         DQBcopyLayer 1, VIDEO
      LOOP UNTIL NOT DQBmouseLB%
      
     
      IF y1 > y2 THEN SWAP y1, y2
      FOR z = y1 * 16 TO y2 * 16 - 64 STEP 64
      IF wallindex% < 1000 THEN
         wall(wallindex%).x1 = x1 * 16
         wall(wallindex%).y1 = 32 + wcoff%
         wall(wallindex%).z1 = z
           
         wall(wallindex%).x2 = x1 * 16
         wall(wallindex%).y2 = -32 + wcoff%
         wall(wallindex%).z2 = z

         wall(wallindex%).x3 = x1 * 16
         wall(wallindex%).y3 = -32 + wcoff%
         wall(wallindex%).z3 = z + 64

         wall(wallindex%).x4 = x1 * 16
         wall(wallindex%).y4 = 32 + wcoff%
         wall(wallindex%).z4 = z + 64
         wall(wallindex%).texnum = tnum%
         wallindex% = wallindex% + 1
         END IF
      NEXT z
   END IF
   IF mode% = WALLmodeh THEN
      x1 = (DQBmouseX \ 4) * 4
      y1 = (DQBmouseY \ 4) * 4
      DO
         x2 = (DQBmouseX \ 4) * 4
         y2 = y1
         drawall
         DQBline 1, x1, y1, x2, y2, 6
         DQBpset 1, x2, y2, 15
         DQBcopyLayer 1, VIDEO
      LOOP UNTIL NOT DQBmouseLB%
    
      IF x1 > x2 THEN SWAP x1, x2
      FOR x = x1 * 16 TO x2 * 16 - 64 STEP 64
      IF wallindex% < 1000 THEN
         wall(wallindex%).x1 = x
         wall(wallindex%).y1 = 32 + wcoff%
         wall(wallindex%).z1 = y1 * 16
          
         wall(wallindex%).x2 = x
         wall(wallindex%).y2 = -32 + wcoff%
         wall(wallindex%).z2 = y1 * 16

         wall(wallindex%).x3 = x + 64
         wall(wallindex%).y3 = -32 + wcoff%
         wall(wallindex%).z3 = y1 * 16

         wall(wallindex%).x4 = x + 64
         wall(wallindex%).y4 = 32 + wcoff%
         wall(wallindex%).z4 = y1 * 16
         wall(wallindex%).texnum = tnum%
         wallindex% = wallindex% + 1
      END IF
      NEXT x
   END IF
   IF mode% = DOORmodev THEN
      x1 = (DQBmouseX \ 4) * 4
      y1 = (DQBmouseY \ 4) * 4
      DO
         x2 = x1
         y2 = (DQBmouseY \ 4) * 4
         drawall
         DQBline 1, x1, y1, x2, y2, doornum% + 100
         DQBpset 1, x2, y2, 15
         DQBcopyLayer 1, VIDEO
      LOOP UNTIL NOT DQBmouseLB%
      IF y1 > y2 THEN SWAP y1, y2
      FOR z = y1 * 16 TO y2 * 16 - 64 STEP 64
      IF wallindex% < 1000 THEN
         wall(wallindex%).x1 = x1 * 16
         wall(wallindex%).y1 = 32
         wall(wallindex%).z1 = z
          
         wall(wallindex%).x2 = x1 * 16
         wall(wallindex%).y2 = -32
         wall(wallindex%).z2 = z

         wall(wallindex%).x3 = x1 * 16
         wall(wallindex%).y3 = -32
         wall(wallindex%).z3 = z + 64

         wall(wallindex%).x4 = x1 * 16
         wall(wallindex%).y4 = 32
         wall(wallindex%).z4 = z + 64
         wall(wallindex%).texnum = tnum%
         wall(wallindex%).p = 1
         doors(doornum%).op = 0
         doors(doornum%).in = wallindex%
         wallindex% = wallindex% + 1
         doornum% = doornum% + 1
         END IF
      NEXT z
   END IF
   IF mode% = DOORmodeh THEN
      x1 = (DQBmouseX \ 4) * 4
      y1 = (DQBmouseY \ 4) * 4
      DO
         x2 = (DQBmouseX \ 4) * 4
         y2 = y1
         drawall
         DQBline 1, x1, y1, x2, y2, doornum% + 100
         DQBpset 1, x2, y2, 15
         DQBcopyLayer 1, VIDEO
      LOOP UNTIL NOT DQBmouseLB%
      IF x1 > x2 THEN SWAP x1, x2
      FOR x = x1 * 16 TO x2 * 16 - 64 STEP 64
      IF wallindex% < 1000 THEN
         wall(wallindex%).x1 = x
         wall(wallindex%).y1 = 32
         wall(wallindex%).z1 = y1 * 16
         
         wall(wallindex%).x2 = x
         wall(wallindex%).y2 = -32
         wall(wallindex%).z2 = y1 * 16

         wall(wallindex%).x3 = x + 64
         wall(wallindex%).y3 = -32
         wall(wallindex%).z3 = y1 * 16

         wall(wallindex%).x4 = x + 64
         wall(wallindex%).y4 = 32
         wall(wallindex%).z4 = y1 * 16
         wall(wallindex%).texnum = tnum%
         wall(wallindex%).p = 1
         doors(doornum%).op = 0
         doors(doornum%).in = wallindex%
         wallindex% = wallindex% + 1
         doornum% = doornum% + 1
         END IF
      NEXT x
   END IF
   IF mode% = ITEMmode THEN
      DO: LOOP UNTIL NOT DQBmouseLB%
      x = DQBmouseX * 16
      z = DQBmouseY * 16
      IF wallindex% < 1000 THEN
         wall(wallindex%).x1 = x
         wall(wallindex%).y1 = 0
         wall(wallindex%).z1 = z
        
         wall(wallindex%).x2 = x
         wall(wallindex%).y2 = 0
         wall(wallindex%).z2 = z

         wall(wallindex%).x3 = x
         wall(wallindex%).y3 = 0
         wall(wallindex%).z3 = z

         wall(wallindex%).x4 = x
         wall(wallindex%).y4 = 0
         wall(wallindex%).z4 = z
         wall(wallindex%).texnum = tnum%
         wall(wallindex%).p = itemtype%
         wallindex% = wallindex% + 1
         END IF
   END IF
   IF mode% = ENEMmode THEN
      DO: LOOP UNTIL NOT DQBmouseLB%
      x = DQBmouseX * 16
      z = DQBmouseY * 16
      IF wallindex% < 1000 THEN
         wall(wallindex%).x1 = x
         wall(wallindex%).y1 = 0
         wall(wallindex%).z1 = z
       
         wall(wallindex%).x2 = x
         wall(wallindex%).y2 = 0
         wall(wallindex%).z2 = z

         wall(wallindex%).x3 = x
         wall(wallindex%).y3 = 0
         wall(wallindex%).z3 = z

         wall(wallindex%).x4 = x
         wall(wallindex%).y4 = 0
         wall(wallindex%).z4 = z
         wall(wallindex%).texnum = tnum%
         wall(wallindex%).p = enemtype%
         wallindex% = wallindex% + 1
         END IF
   END IF
END IF

END SUB

SUB drawall
DQBcopyLayer 2, 1

FOR index = 0 TO floorindex% - 1
   DQBbox 1, floor(index).x1 \ 16, floor(index).z1 \ 16, floor(index).x3 \ 16, floor(index).z3 \ 16, 9
NEXT index

FOR index = 0 TO wallindex% - 1
   SELECT CASE wall(index).p
   CASE 2:
      DQBpset 1, (wall(index).x1) \ 16, wall(index).z1 \ 16, 2
      GOTO skip
   CASE 3:
      DQBpset 1, (wall(index).x1) \ 16, wall(index).z1 \ 16, 3
      GOTO skip
   CASE 4:
      DQBpset 1, (wall(index).x1) \ 16, wall(index).z1 \ 16, 4
      GOTO skip
   CASE 5:
      DQBpset 1, (wall(index).x1) \ 16, wall(index).z1 \ 16, 5
      GOTO skip
   CASE 6:
      DQBpset 1, (wall(index).x1) \ 16, wall(index).z1 \ 16, 6
      GOTO skip
   CASE 7:
      DQBpset 1, (wall(index).x1) \ 16, wall(index).z1 \ 16, 7
      GOTO skip
   CASE 8:
      DQBpset 1, (wall(index).x1) \ 16, wall(index).z1 \ 16, 8
      GOTO skip

   END SELECT
   IF wall(index).y1 = 32 AND wall(index).y2 <> 32 THEN
      IF wall(index).p = 0 THEN DQBline 1, wall(index).x1 \ 16, wall(index).z1 \ 16, wall(index).x3 \ 16, wall(index).z3 \ 16, 3
   END IF
   IF wall(index).y1 = 96 AND wall(index).y2 <> 96 THEN
      IF wall(index).z3 = wall(index).z1 THEN
         FOR x = wall(index).x1 \ 16 TO wall(index).x3 \ 16 STEP 2
            DQBpset 1, x, wall(index).z2 \ 16, 14
         NEXT x
      ELSE
         FOR z = wall(index).z1 \ 16 TO wall(index).z3 \ 16 STEP 2
            DQBpset 1, wall(index).x2 \ 16, z, 14
         NEXT z
      END IF
   END IF
skip:
NEXT index

FOR index = 0 TO doornum% - 1
   DQBline 1, wall(doors(index).in).x1 \ 16, wall(doors(index).in).z1 \ 16, wall(doors(index).in).x3 \ 16, wall(doors(index).in).z3 \ 16, 50 + index
NEXT index


DQBpset 1, camx \ 16, camy \ 16, 14

DQBpset 1, DQBmouseX, DQBmouseY, 15
SELECT CASE mode%
   CASE FLOORCEILmode:
      DQBboxf 1, 0, 191, 320, 200, 0
      DQBprint 1, "FLOORCEILmode TEXNUM:" + STR$(tnum%) + " WCOFF:" + STR$(wcoff%), 0, 191, 4
   CASE WALLmodev:
      DQBboxf 1, 0, 191, 320, 200, 0
      DQBprint 1, "WALLmodev TEXNUM:" + STR$(tnum%) + " WCOFF:" + STR$(wcoff%), 0, 191, 4
   CASE WALLmodeh:
      DQBboxf 1, 0, 191, 320, 200, 0
      DQBprint 1, "WALLmodeh TEXNUM:" + STR$(tnum%) + " WCOFF:" + STR$(wcoff%), 0, 191, 4
   CASE DOORmodev:
      DQBboxf 1, 0, 191, 320, 200, 0
      DQBprint 1, "DOORmodev DOORNUM:" + STR$(doornum%) + " TEXNUM:" + STR$(tnum%), 0, 192, 4
   CASE DOORmodeh:
      DQBboxf 1, 0, 191, 320, 200, 0
      DQBprint 1, "DOORmodeh DOORNUM:" + STR$(doornum%) + " TEXNUM:" + STR$(tnum%), 0, 192, 4
   CASE CAMmode:
      DQBboxf 1, 0, 191, 320, 200, 0
      DQBprint 1, "CAMmode x:" + STR$(camx%) + " z:" + STR$(camy%) + " WCOFF:" + STR$(wcoff%), 0, 191, 4
   CASE ITEMmode:
      DQBboxf 1, 0, 191, 320, 200, 0
      DQBprint 1, "ITEMmode ITEMTYPE:" + STR$(itemtype%), 0, 191, 4
   CASE ENEMmode:
      DQBboxf 1, 0, 191, 320, 200, 0
      DQBprint 1, "ENEMmode ENEMTYPE:" + STR$(enemtype%), 0, 191, 4
END SELECT


END SUB

SUB DrawGrid
FOR x = 0 TO 319 STEP 4
   FOR y = 0 TO 199 STEP 4
      LINE (x, y)-(x + 4, y + 4), 1, B
   NEXT y
NEXT x
END SUB

SUB savecolmap
   CLS
   FOR index = 0 TO wallindex% - 1
      IF wall(index).p > 1 THEN
         DQBpset VIDEO, (wall(index).x1) \ 16, wall(index).z1 \ 16, wall(index).p
         GOTO skip2
      END IF
      IF wall(index).y1 = 32 AND wall(index).y2 <> 32 THEN
         DQBline VIDEO, wall(index).x1 \ 16, wall(index).z1 \ 16, wall(index).x3 \ 16, wall(index).z3 \ 16, 1
      END IF
skip2:
   NEXT index
   FOR index = 0 TO doornum% - 1
      DQBline VIDEO, wall(doors(index).in).x1 \ 16, wall(doors(index).in).z1 \ 16, wall(doors(index).in).x3 \ 16, wall(doors(index).in).z3 \ 16, 50 + index
   NEXT index
   DEF SEG = &HA000
   BSAVE file$ + ".cmp", 0, 64000
   DEF SEG
   SLEEP
END SUB

SUB savemap
OPEN file$ + ".map" FOR OUTPUT AS #1
WRITE #1, camx, camy
WRITE #1, floorindex%
WRITE #1, wallindex%
FOR index = 0 TO floorindex% - 1
   WRITE #1, floor(index).x1, floor(index).y1, floor(index).z1
   WRITE #1, floor(index).x2, floor(index).y2, floor(index).z2
   WRITE #1, floor(index).x3, floor(index).y3, floor(index).z3
   WRITE #1, floor(index).x4, floor(index).y4, floor(index).z4
   WRITE #1, floor(index).p
   WRITE #1, floor(index).texnum
NEXT index
FOR index = 0 TO wallindex% - 1
   WRITE #1, wall(index).x1, wall(index).y1, wall(index).z1
   WRITE #1, wall(index).x2, wall(index).y2, wall(index).z2
   WRITE #1, wall(index).x3, wall(index).y3, wall(index).z3
   WRITE #1, wall(index).x4, wall(index).y4, wall(index).z4
   WRITE #1, wall(index).p
   WRITE #1, wall(index).texnum
NEXT index

CLOSE #1
END SUB

