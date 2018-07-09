DECLARE SUB menu ()
DECLARE SUB loadstat ()
DECLARE SUB SaveState ()
DECLARE SUB dead ()
DECLARE SUB RenderSTAT ()
DECLARE SUB loadlevel ()
DECLARE SUB nextlevel ()
DECLARE SUB LoadWEAP ()
DECLARE SUB LoadSPR ()
DECLARE SUB RenderWEAP ()
DECLARE SUB LoadTEX (file$)
DECLARE FUNCTION CheckSIGHTme% ()
DECLARE SUB fireME ()
DECLARE FUNCTION CheckSIGHT% (x1, y1, x2, y2)
DECLARE SUB Drawmap ()
DECLARE SUB loadthing ()
DECLARE SUB RenderENEM (i AS INTEGER)
DECLARE SUB DoENEMS ()
DECLARE SUB EraseITEM (x AS INTEGER, y AS INTEGER, p AS INTEGER)
DECLARE SUB RenderITEM (i AS INTEGER, w AS INTEGER, h AS INTEGER)
DECLARE SUB DrawSTATUS ()
DECLARE SUB DoDOORS ()
DECLARE SUB AddCAMWALL (index AS INTEGER)
DECLARE SUB TranslateWALL ()
DECLARE SUB RenderWALL ()
DECLARE SUB TranslateFLOOR ()
DECLARE SUB AddCAMFLOOR (index AS INTEGER)
DECLARE SUB RenderFLOOR ()
DECLARE FUNCTION Collision% ()
DECLARE SUB LoadCMP (file$)
DECLARE FUNCTION IsOnScreen% (x1&, y1&, x2&, y2&, x3&, y3&)
DECLARE SUB DoINPUT ()
DECLARE FUNCTION ClipPOLY% (index AS INTEGER)
DECLARE SUB LoadTRIG ()
DECLARE SUB LoadPOLY (file AS STRING)
DECLARE SUB RotateY (index AS INTEGER)
'$INCLUDE: 'source\directqb.bi'

TYPE POL
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

TYPE SORTLIST
   z AS INTEGER
   i AS INTEGER
END TYPE

TYPE KAMERA
   x AS INTEGER
   y AS INTEGER
   z AS INTEGER
END TYPE

TYPE DOOR
   in AS INTEGER
   opening AS INTEGER
END TYPE

TYPE ITEM
   in AS INTEGER
END TYPE

TYPE ENEMY
   in AS INTEGER
   hlth AS INTEGER
   ai AS INTEGER
   move AS INTEGER
   stim AS INTEGER
END TYPE

'$DYNAMIC
DIM SHARED floor(0) AS POL
DIM SHARED wall(0) AS POL
DIM SHARED rpoly(0) AS POL
DIM SHARED slist(0) AS SORTLIST
DIM SHARED doors(30) AS DOOR
DIM SHARED items(30) AS ITEM
DIM SHARED enems(20) AS ENEMY
DIM SHARED eslist(20) AS SORTLIST
DIM SHARED nFLOOR AS INTEGER
DIM SHARED nWALL AS INTEGER
DIM SHARED nDOOR AS INTEGER
DIM SHARED nITEM AS INTEGER
DIM SHARED nENEM AS INTEGER

'$STATIC
DIM SHARED Cosine%(360), Sine%(360)
CONST DEGtoRAD! = 3.141593 / 180
CONST DQBtoRAD! = 360 / 255

CONST DISTtoPLANE1& = 320
CONST DISTtoPLANE2& = 320

CONST COLLISIONLAYER = 2
CONST MAT1LAYER = 3
CONST MAT2LAYER = 4
CONST WEAPLAYER = 5
CONST CAPTWLAYER = 6
CONST CAPTFLAYER = 7

CONST WALLtexBYTE = (64 * 64 + 2) \ 2 + 1
CONST WEAPtexBYTE = (128 * 68 + 2) \ 2 + 1
CONST WEAPtexBYTE2 = (136 * 82 * 2 + 2) \ 2 + 1
CONST STATtexBYTE = (42 * 40 + 2) \ 2 + 1
CONST CAPTtexBYTE = (128 * 64 + 2) \ 2 + 1
DIM SHARED tex(WALLtexBYTE * 14) AS INTEGER
DIM SHARED itm(WALLtexBYTE * 3) AS INTEGER
DIM SHARED enm(CAPTtexBYTE) AS INTEGER
DIM SHARED wep(WEAPtexBYTE2) AS INTEGER
DIM SHARED stt(STATtexBYTE * 3) AS INTEGER
DIM SHARED Pal AS STRING * 780

CONST crad = 1
CONST Speed% = 8
CONST rotvel% = 2
DIM SHARED cam AS KAMERA
DIM SHARED alpha%, slindex%, Updown%, vy%
DIM SHARED oldx%, oldz%
DIM SHARED health%, armor%
DIM SHARED animtim%
DIM SHARED firetim%, weapanim%, watim%
DIM SHARED hitx%, hity%
DIM SHARED exitdoor%, nreach%
DIM SHARED dtim AS INTEGER
DIM SHARED map%
DIM SHARED save$, savetim
DIM SHARED level$, textur$
DIM SHARED modus%
DIM SHARED le%

vy% = 1
weapanim% = 0
firetim% = 10
watim% = 0



SCREEN 13
dqberr = DQBinit(7, 0, 0)
dqberr = DQBloadImage(1, 0, 0, "loads\pal.bmp", Pal, 320, 200)
DQBsetPal Pal
DQBsetTextureSize 64
DQBinstallKeyboard




LoadTRIG
LoadSPR
LoadWEAP

start:
menu


SELECT CASE COMMAND$
   CASE "EASY"
      modus% = 0
   CASE "MEDIUM"
      modus% = 1
   CASE "HARD"
      modus% = 2
   CASE ELSE
      modus% = 1
END SELECT

frame = 1
REM DQBsetFrameRate 40

DO
   DoENEMS
   DoDOORS
   TranslateFLOOR
   RenderFLOOR
   TranslateWALL
   RenderWALL
   RenderWEAP
   IF armor% < 0 THEN armor% = 0
   IF health% < 0 THEN health% = 0
   IF health% = 0 THEN dead
   RenderSTAT
   DoINPUT
   DQBline 1, 155, 100, 159, 100, 14
   DQBline 1, 161, 100, 165, 100, 14
   DQBline 1, 160, 95, 160, 99, 14
   DQBline 1, 160, 101, 160, 105, 14
   REM IF frame THEN DO: LOOP UNTIL DQBframeReady
   IF map% THEN Drawmap
   DQBcopyLayer 1, VIDEO
   DQBclearLayer 1
LOOP UNTIL DQBkey%(KEYESC)

CLOSE #1
GOTO start

DEFSNG A-Z
SUB AddCAMFLOOR (index AS INTEGER)
rpoly(index).x1 = floor(index).x1 - cam.x
rpoly(index).y1 = floor(index).y1 - cam.y
rpoly(index).z1 = floor(index).z1 - cam.z
rpoly(index).x2 = floor(index).x2 - cam.x
rpoly(index).y2 = floor(index).y2 - cam.y
rpoly(index).z2 = floor(index).z2 - cam.z
rpoly(index).x3 = floor(index).x3 - cam.x
rpoly(index).y3 = floor(index).y3 - cam.y
rpoly(index).z3 = floor(index).z3 - cam.z
rpoly(index).x4 = floor(index).x4 - cam.x
rpoly(index).y4 = floor(index).y4 - cam.y
rpoly(index).z4 = floor(index).z4 - cam.z
END SUB

SUB AddCAMWALL (index AS INTEGER)
rpoly(index).x1 = wall(index).x1 - cam.x
rpoly(index).y1 = wall(index).y1 - cam.y
rpoly(index).z1 = wall(index).z1 - cam.z
rpoly(index).x2 = wall(index).x2 - cam.x
rpoly(index).y2 = wall(index).y2 - cam.y
rpoly(index).z2 = wall(index).z2 - cam.z
rpoly(index).x3 = wall(index).x3 - cam.x
rpoly(index).y3 = wall(index).y3 - cam.y
rpoly(index).z3 = wall(index).z3 - cam.z
rpoly(index).x4 = wall(index).x4 - cam.x
rpoly(index).y4 = wall(index).y4 - cam.y
rpoly(index).z4 = wall(index).z4 - cam.z
END SUB

FUNCTION CheckSIGHT% (startx, starty, endx, endy)
   IF ABS(endx - startx) > ABS(endy - starty) THEN
      IF startx > endx THEN SWAP startx, endx: SWAP starty, endy
      IF (endx - startx) = 0 THEN
         k! = (endy - starty)
      ELSE
         k! = (endy - starty) / (endx - startx)
      END IF
      x! = startx
      y! = starty
      WHILE x! <= endx
         x! = x! + 1
         y! = y! + k!
         col = DQBpoint(COLLISIONLAYER, x!, y!)
         IF col = 1 OR col > 49 THEN CheckSIGHT% = 1: EXIT FUNCTION
'         IF col > 4 AND col < 8 AND startx = cam.x \ 16 THEN CheckSIGHT% = 2: endx = x!: endy = y!: EXIT FUNCTION
      WEND
      CheckSIGHT% = 0
   ELSE
      IF starty > endy THEN SWAP startx, endx: SWAP starty, endy
      IF (endy - starty) = 0 THEN
         k! = (endx - startx)
      ELSE
         k! = (endx - startx) / (endy - starty)
      END IF
      x! = startx
      y! = starty
      WHILE y! <= endy
         x! = x! + k!
         y! = y! + 1
         col = DQBpoint(COLLISIONLAYER, x!, y!)
         IF col = 1 OR col > 49 THEN CheckSIGHT% = 1: EXIT FUNCTION
      '   IF col > 4 AND col < 8 AND startx = cam.x \ 16 THEN CheckSIGHT% = 2: endx = x!: endy = y!: EXIT FUNCTION
      WEND
      CheckSIGHT% = 0
   END IF
END FUNCTION

FUNCTION CheckSIGHTme%
   kx! = COS(alpha% * DEGtoRAD)
   ky! = SIN(alpha% * DEGtoRAD)
   x! = cam.x \ 16 + kx!
   y! = cam.z \ 16 + ky!
   DO
   IF DQBpoint(COLLISIONLAYER, x!, y!) > 4 THEN
      CheckSIGHTme% = 1
      hitx% = x!: hity% = y!
      EXIT FUNCTION
   END IF
      x! = x! + kx!
      y! = y! + ky!
   LOOP UNTIL DQBpoint(COLLISIONLAYER, x!, y!) = 1

   kx! = COS((alpha% + 2) * DEGtoRAD)
   ky! = SIN((alpha% + 2) * DEGtoRAD)
   x! = cam.x \ 16 + kx!
   y! = cam.z \ 16 + ky!
   DO
   IF DQBpoint(COLLISIONLAYER, x!, y!) > 4 THEN
      CheckSIGHTme% = 1
      hitx% = x!: hity% = y!
      EXIT FUNCTION
   END IF
      x! = x! + kx!
      y! = y! + ky!
   LOOP UNTIL DQBpoint(COLLISIONLAYER, x!, y!) = 1

   kx! = COS((alpha% - 2) * DEGtoRAD)
   ky! = SIN((alpha% - 2) * DEGtoRAD)
   x! = cam.x \ 16 + kx!
   y! = cam.z \ 16 + ky!
   DO
   IF DQBpoint(COLLISIONLAYER, x!, y!) > 4 THEN
      CheckSIGHTme% = 1
      hitx% = x!: hity% = y!
      EXIT FUNCTION
   END IF
      x! = x! + kx!
      y! = y! + ky!
   LOOP UNTIL DQBpoint(COLLISIONLAYER, x!, y!) = 1


END FUNCTION

FUNCTION ClipPOLY% (index AS INTEGER)
IF rpoly(index).z1 < -rpoly(index).y1 AND rpoly(index).z2 < -rpoly(index).y2 AND rpoly(index).z3 < -rpoly(index).y3 AND rpoly(index).z4 < -rpoly(index).y4 THEN ClipPOLY% = 1: EXIT FUNCTION
IF rpoly(index).z1 < rpoly(index).y1 AND rpoly(index).z2 < rpoly(index).y2 AND rpoly(index).z3 < rpoly(index).y3 AND rpoly(index).z4 < rpoly(index).y4 THEN ClipPOLY% = 1: EXIT FUNCTION
IF rpoly(index).x1 < -rpoly(index).z1 AND rpoly(index).x2 < -rpoly(index).z2 AND rpoly(index).x3 < -rpoly(index).z3 AND rpoly(index).x4 < -rpoly(index).z4 THEN ClipPOLY% = 1: EXIT FUNCTION
IF rpoly(index).z1 < rpoly(index).x1 AND rpoly(index).z2 < rpoly(index).x2 AND rpoly(index).z3 < rpoly(index).x3 AND rpoly(index).z4 < rpoly(index).x4 THEN ClipPOLY% = 1: EXIT FUNCTION
IF rpoly(index).z1 <= 0 AND rpoly(index).z2 <= 0 AND rpoly(index).z3 <= 0 AND rpoly(index).z4 <= 0 THEN ClipPOLY = 1: EXIT FUNCTION
END FUNCTION

FUNCTION Collision%
DIM c1 AS INTEGER
DIM c2 AS INTEGER
DIM c3 AS INTEGER
DIM c4 AS INTEGER
DIM c5 AS INTEGER

   x = cam.x \ 16
   y = cam.z \ 16
   FOR t = -crad TO crad
       c1 = DQBpoint(COLLISIONLAYER, x + t, y - crad)
       SELECT CASE c1
         CASE 1:
            Collision% = 1
            EXIT FUNCTION
         CASE 2:
            EraseITEM x + t, y - crad, c1
         CASE 3:
            EraseITEM x + t, y - crad, c1
         CASE 4:
            EraseITEM x + t, y - crad, c1
         CASE 50 TO 70:
            Collision% = 1
            EXIT FUNCTION
       END SELECT

       c2 = DQBpoint(COLLISIONLAYER, x + t, y + crad)
       SELECT CASE c2
         CASE 1:
            Collision% = 1
            EXIT FUNCTION
         CASE 2:
            EraseITEM x + t, y + crad, c2
         CASE 3:
            EraseITEM x + t, y + crad, c2
         CASE 4:
            EraseITEM x + t, y + crad, c2
         CASE 50 TO 70:
            Collision% = 1
            EXIT FUNCTION
       END SELECT

       c3 = DQBpoint(COLLISIONLAYER, x - crad, y + t)
       SELECT CASE c3
         CASE 1:
            Collision% = 1
            EXIT FUNCTION
         CASE 2:
            EraseITEM x - crad, y + t, c3
         CASE 3:
            EraseITEM x - crad, y + t, c3
         CASE 4:
            EraseITEM x - crad, y + t, c3
         CASE 50 TO 70:
            Collision% = 1
            EXIT FUNCTION
       END SELECT

       c4 = DQBpoint(COLLISIONLAYER, x + crad, y + t)
       SELECT CASE c4
         CASE 1:
            Collision% = 1
            EXIT FUNCTION
         CASE 2:
            EraseITEM x + crad, y + t, c4
         CASE 3:
            EraseITEM x + crad, y + t, c4
         CASE 4:
            EraseITEM x + crad, y + t, c4
         CASE 50 TO 70:
            Collision% = 1
            EXIT FUNCTION
       END SELECT
   NEXT t
   c5 = DQBpoint(COLLISIONLAYER, cam.x \ 16, cam.z \ 16)
       SELECT CASE c5
         CASE 1:
            Collision% = 1
            EXIT FUNCTION
         CASE 2:
            EraseITEM cam.x \ 16, cam.z \ 16, c5
         CASE 3:
            EraseITEM cam.x \ 16, cam.z \ 16, c5
         CASE 4:
            EraseITEM cam.x \ 16, cam.z \ 16, c5
         CASE 50 TO 70:
            Collision% = 1
            EXIT FUNCTION
       END SELECT
END FUNCTION

SUB dead
DIM x AS INTEGER
DIM y AS INTEGER

DQBboxf 1, 0, 0, 319, 199, 15
dqberr = DQBloadImage(1, 0, 0, "scenes\sqrldead.bmp", Pal, 320, 200)
DQBcopyLayer 1, VIDEO

y = 16
x = 24

DO
   IF DQBkey%(KEYDOWN) THEN
      DO: LOOP UNTIL NOT DQBkey%(KEYDOWN)
      DQBprint VIDEO, ">", x, y, 15
      y = y + 20
      IF y > 56 THEN y = 16
      DQBprint VIDEO, ">", x, y, 107
   END IF
   IF DQBkey%(KEYUP) THEN
      DO: LOOP UNTIL NOT DQBkey%(KEYUP)
      DQBprint VIDEO, ">", x, y, 15
      y = y - 20
      IF y < 16 THEN y = 56
      DQBprint VIDEO, ">", x, y, 107
   END IF
   IF DQBkey%(KEYENTER) THEN
      DO: LOOP UNTIL NOT DQBkey%(KEYENTER)
      IF y = 16 THEN
         CLOSE #1
         loadlevel
         EXIT SUB
      END IF
      IF y = 36 THEN
         DQBboxf 1, 0, 0, 319, 199, 15
         dqberr = DQBloadImage(1, 76, 22, "loads\load.bmp", Pal, 168, 155)
         DQBcopyLayer 1, VIDEO
         loadstat
         EXIT SUB
      END IF
      IF y = 56 THEN
         DQBremoveKeyboard
         DQBinitText
         DQBclose
         SYSTEM
      END IF
   END IF
   DQBprint VIDEO, ">", x, y, 107
LOOP


END SUB

SUB DoDOORS

FOR index = 0 TO nDOOR
   IF doors(index).opening = 1 THEN
      dtim = dtim + 1
      IF dtim = 1 THEN
         wall(doors(index).in).y1 = wall(doors(index).in).y1 + 1
         wall(doors(index).in).y2 = wall(doors(index).in).y2 + 1
         wall(doors(index).in).y3 = wall(doors(index).in).y3 + 1
         wall(doors(index).in).y4 = wall(doors(index).in).y4 + 1
         IF wall(doors(index).in).y2 = 32 THEN
            wall(doors(index).in).y1 = -34
            wall(doors(index).in).y2 = -34
            wall(doors(index).in).y3 = -34
            wall(doors(index).in).y4 = -34
            doors(index).opening = 0
            DQBline COLLISIONLAYER, wall(doors(index).in).x1 \ 16, wall(doors(index).in).z1 \ 16, wall(doors(index).in).x3 \ 16, wall(doors(index).in).z3 \ 16, 0
         END IF
         dtim = 0
      END IF
   END IF
NEXT index
END SUB

SUB DoENEMS
   FOR index = 0 TO nENEM - 1
   IF enems(index).hlth > 0 AND enems(index).move = 0 THEN
   IF ABS(wall(enems(index).in).x1 - cam.x) + ABS(wall(enems(index).in).z1 - cam.z) < 900 OR wall(enems(index).in).p = 7 THEN
      x = wall(enems(index).in).x1
      y = wall(enems(index).in).z1
      DQBpset COLLISIONLAYER, x \ 16, y \ 16, 0
      vxe = SGN(cam.x - x)
      vye = SGN(cam.z - y)
      xt = x + vxe
      yt = y + vye
      IF DQBpoint(COLLISIONLAYER, xt \ 16 + vxe * 2, y \ 16) <> 0 THEN vxe = 0
      IF DQBpoint(COLLISIONLAYER, x \ 16, yt \ 16 + vye * 2) <> 0 THEN vye = 0
      x = x + vxe
      y = y + vye
      DQBpset COLLISIONLAYER, x \ 16, y \ 16, wall(enems(index).in).p
      wall(enems(index).in).x1 = x
      wall(enems(index).in).z1 = y
      wall(enems(index).in).x2 = x
      wall(enems(index).in).z2 = wall(enems(index).in).z1
      wall(enems(index).in).x3 = x
      wall(enems(index).in).z3 = wall(enems(index).in).z1
      wall(enems(index).in).x4 = x
      wall(enems(index).in).z4 = wall(enems(index).in).z1
     
      IF ABS(cam.x - x) + ABS(cam.z - y) < 450 OR (wall(enems(index).in).p = 7 AND ABS(cam.x - x) + ABS(cam.z - y) < 700) THEN
         dummy% = RND * 255
         IF dummy% > 240 THEN
         IF CheckSIGHT%(cam.x \ 16, cam.z \ 16, wall(enems(index).in).x1 \ 16, wall(enems(index).in).z1 \ 16) = 0 THEN
            enems(index).move = 1
            enems(index).ai = 4
            accur% = RND * 255
            IF accur% > 10 THEN
               SELECT CASE modus%
               CASE 0:
               IF wall(enems(index).in).p = 7 THEN
                  health% = health% - 10
               ELSE
                  health% = health% - 2
               END IF
               CASE 1:
               IF wall(enems(index).in).p = 7 THEN
                  health% = health% - 15
               ELSE
                  health% = health% - 3
               END IF
               CASE 2:
               IF wall(enems(index).in).p = 7 THEN
                  health% = health% - 20
               ELSE
                  health% = health% - 5
               END IF
               END SELECT
            END IF
         END IF
         ELSE
            enems(index).move = 0
         END IF
      END IF
   END IF
   END IF
   IF enems(index).hlth <= 0 THEN
      enems(index).move = 3
   END IF

      IF animtim% = 3 THEN
         SELECT CASE enems(index).move
         CASE 0:
            enems(index).ai = enems(index).ai + 1
            IF enems(index).ai = 4 THEN enems(index).ai = 0
         CASE 1:
            enems(index).ai = enems(index).ai + 1
            IF enems(index).ai = 6 THEN enems(index).ai = 4
            enems(index).stim = enems(index).stim + 1
            IF enems(index).stim = 10 THEN
               enems(index).stim = 0
               enems(index).ai = 0
               enems(index).move = 0
            END IF
         CASE 2:
            enems(index).stim = enems(index).stim + 1
            IF enems(index).stim = 2 THEN
               enems(index).stim = 0
               enems(index).ai = 0
               enems(index).move = 0
            END IF
         CASE 3:
            enems(index).ai = enems(index).ai + 1
            IF enems(index).ai > 8 THEN
               enems(index).ai = 8
               IF wall(enems(index).in).p = 7 THEN
                  le% = le% + 1
                  IF le% = 20 THEN
                     nextlevel
                     EXIT SUB
                  END IF
               END IF
            END IF
         END SELECT
         END IF
   NEXT index
   animtim% = animtim% + 1
   IF animtim% = 4 THEN animtim% = 0
   
END SUB

SUB DoINPUT

STATIC lm%
'/* Bewegung */

   IF DQBkey%(KEYUP) THEN
         oldx% = cam.x
         oldz% = cam.z
         cam.y = cam.y + Updown%
         cam.z = cam.z - COS(alpha * DEGtoRAD) * Speed%
         cam.x = cam.x - SIN(alpha * DEGtoRAD) * Speed%
         IF Collision% = 1 THEN
            cam.x = oldx
            cam.z = oldz
         END IF
         lm% = 1
   END IF
   IF DQBkey%(KEYDOWN) THEN
         oldx% = cam.x
         oldz% = cam.z
         cam.y = cam.y + Updown%
         cam.x = cam.x + COS(alpha * DEGtoRAD) * Speed%
         cam.z = cam.z - SIN(alpha * DEGtoRAD) * Speed%
         IF Collision% = 1 THEN
            cam.x = oldx
            cam.z = oldz
         END IF
         lm% = 1
   END IF

   IF NOT DQBkey%(KEYDOWN) AND NOT DQBkey%(KEYUP) AND NOT DQBkey%(45) AND NOT DQBkey%(44) THEN
      lm% = 0
   END IF

   IF lm% = 1 THEN
      Updown% = Updown% + vy%
      IF Updown% > 2 THEN vy% = -1
      IF Updown% < -2 THEN vy% = 1
   END IF
   IF lm% = 0 AND cam.y <> 0 THEN
      IF cam.y < 0 THEN cam.y = 0
      IF cam.y > 0 THEN cam.y = cam.y - 3
   END IF
  
   '/* Rotation */
   IF DQBkey%(KEYLEFT) THEN
      alpha = alpha - rotvel
      lm% = 1
   END IF
   IF DQBkey%(KEYRIGHT) THEN
      alpha = alpha + rotvel
      lm% = 1
   END IF
   IF alpha > 360 THEN alpha = 0
   IF alpha < 0 THEN alpha = 360

   '/* Strafen */
   IF DQBkey%(45) THEN
      oldx% = cam.x
      oldz% = cam.z
      cam.y = cam.y + Updown%
      cam.x = cam.x + COS((alpha + 90) * DEGtoRAD) * Speed%
      cam.z = cam.z + SIN((alpha + 90) * DEGtoRAD) * Speed%
      IF Collision% = 1 THEN
         cam.x = oldx
         cam.z = oldz
      END IF
      lm% = 1
   END IF
   IF DQBkey%(44) THEN
      oldx% = cam.x
      oldz% = cam.z
      cam.y = cam.y + Updown%
      cam.x = cam.x + COS((alpha - 90) * DEGtoRAD) * Speed%
      cam.z = cam.z + SIN((alpha - 90) * DEGtoRAD) * Speed%
      IF Collision% = 1 THEN
         cam.x = oldx
         cam.z = oldz
      END IF
      lm% = 1
   END IF

   IF DQBkey(KEYSPACE) THEN
      d = DQBpoint(COLLISIONLAYER, (cam.x + COS(alpha * DEGtoRAD) * 32) \ 16, (cam.z + SIN(alpha * DEGtoRAD) * 32) \ 16)
      IF d >= 50 THEN
         IF d - 50 = exitdoor% THEN CALL nextlevel: GOTO skip1:
         doors(d - 50).opening = 1
      END IF
   END IF
skip1:

   '/* up & down */
   IF DQBkey(83) THEN
      cam.y = cam.y + 3
   END IF
   IF DQBkey(82) THEN
      cam.y = cam.y - 3
   END IF

   '/* weapon */
   IF DQBkey%(29) THEN
   IF armor% > 0 THEN
      IF watim% = 0 THEN
        fireME
        armor% = armor% - 1
        weapanim% = 1
        watim% = 5
      ELSE
        watim% = watim% + 1
        IF watim% > 10 AND watim% < 19 THEN weapanim% = 0
        IF watim% = 19 THEN watim% = 0
      END IF
   END IF
   END IF

   IF NOT DQBkey%(29) THEN
         watim% = 0
         weapanim% = 0
   END IF

   '/* Karte zeichnen? */
   IF DQBkey%(15) THEN
      DO: LOOP UNTIL NOT DQBkey%(15)
      map% = map% + 1
      IF map% = 2 THEN map% = 0
   END IF

   IF DQBkey%(25) THEN
      DO: LOOP UNTIL NOT DQBkey%(25)
      DQBprint VIDEO, "GAME PAUSE - PRESS FIRE TO CONTINUE", 20, 96, 1
      DQBprint VIDEO, "GAME PAUSE - PRESS FIRE TO CONTINUE", 21, 97, 4
      DO: LOOP UNTIL DQBkey%(29)
      DO: LOOP UNTIL NOT DQBkey%(29)
   END IF

   IF DQBkey%(59) THEN SaveState
   IF DQBkey%(63) THEN loadstat

   c5% = DQBpoint(COLLISIONLAYER, cam.x \ 16, cam.z \ 16)
   SELECT CASE c5%
       CASE 2:
         EraseITEM cam.x \ 16, cam.z \ 16, c5%
       CASE 3:
         EraseITEM cam.x \ 16, cam.z \ 16, c5%
       CASE 4:
         EraseITEM cam.x \ 16, cam.z \ 16, c5%
   END SELECT
END SUB

SUB Drawmap
FOR index = 0 TO nWALL - 1
   IF wall(index).y1 = wall(index).y3 THEN
      DQBboxf 1, wall(index).x1 \ 16, wall(index).z1 \ 16, wall(index).x3 \ 16, wall(index).z3 \ 16, 0
      DQBbox 1, wall(index).x1 \ 16, wall(index).z1 \ 16, wall(index).x3 \ 16, wall(index).z3 \ 16, 1
   ELSE
      DQBline 1, wall(index).x1 \ 16, wall(index).z1 \ 16, wall(index).x3 \ 16, wall(index).z3 \ 16, 3
   END IF
   IF wall(index).p > 0 THEN DQBpset 1, wall(index).x1 \ 16, wall(index).z1 \ 16, wall(index).p
NEXT index
DQBpset 1, cam.x \ 16, cam.z \ 16, 4
END SUB

SUB DrawSTATUS
DQBprint 1, "HEALTH:" + STR$(health%), 1, 1, 1
DQBprint 1, "HEALTH:" + STR$(health%), 0, 0, 50

DQBprint 1, "ARMOR:" + STR$(armor%), 101, 1, 1
DQBprint 1, "ARMOR:" + STR$(armor%), 100, 0, 50

END SUB

SUB EraseITEM (x AS INTEGER, y AS INTEGER, p AS INTEGER)
   FOR index = 0 TO nITEM - 1
      IF wall(items(index).in).x1 = x * 16 AND wall(items(index).in).z1 = y * 16 THEN
                           
         IF p = 2 THEN
            IF health% = 100 THEN EXIT SUB
            health% = health% + 20
            IF health% > 100 THEN health% = 100
         END IF
         IF p = 3 THEN
            IF health% = 100 THEN EXIT SUB
            health% = health% + 50
            IF health% > 100 THEN health% = 100
         END IF
         IF p = 4 THEN
            IF armor% = 100 THEN EXIT SUB
            armor% = armor% + 40
            IF armor% > 100 THEN armor% = 100
         END IF
         DQBpset COLLISIONLAYER, x, y, 0
         wall(items(index).in).y1 = -64
      END IF
   NEXT index
END SUB

SUB fireME
x = cam.x \ 16 + COS(alpha% * DEGtoRAD) * 700 \ 16
y = cam.z \ 16 + SIN(alpha% * DEGtoRAD) * 700 \ 16

IF CheckSIGHTme% = 1 THEN
   FOR index = 0 TO nENEM
      IF wall(enems(index).in).x1 \ 16 = hitx% AND wall(enems(index).in).z1 \ 16 = hity% THEN
         enems(index).hlth = enems(index).hlth - 10
         IF enems(index).hlth > 0 THEN
            enems(index).ai = 6
            IF enems(index).move = 1 THEN enems(index).stim = 0
            enems(index).move = 2
         ELSE
            enems(index).ai = 5
            enems(index).move = 3
         END IF
         DQBpset COLLISIONLAYER, hitx%, hity%, 0
      END IF
   NEXT index
END IF

END SUB

FUNCTION IsOnScreen% (x1&, y1&, x2&, y2&, x3&, y3&)
IF x1& > 20000 OR x1& < -20000 THEN EXIT FUNCTION
IF x2& > 20000 OR x2& < -20000 THEN EXIT FUNCTION
IF x3& > 20000 OR x3& < -20000 THEN EXIT FUNCTION
IF y1& > 20000 OR y1& < -20000 THEN EXIT FUNCTION
IF y2& > 20000 OR y2& < -20000 THEN EXIT FUNCTION
IF y3& > 20000 OR y3& < -20000 THEN EXIT FUNCTION
IF x1& < 0 AND x2& < 0 AND x3& < 0 THEN EXIT FUNCTION
IF x1& > 319 AND x2& > 319 AND x3& > 319 THEN EXIT FUNCTION
IF y1& < 0 AND y2& < 0 AND y3& < 0 THEN EXIT FUNCTION
IF y1& > 199 AND y2& > 199 AND y3& > 199 THEN EXIT FUNCTION
IsOnScreen% = 1

END FUNCTION

SUB LoadCMP (file$)
DEF SEG = DQBmapLayer(COLLISIONLAYER)
BLOAD file$, 0
DEF SEG
END SUB

SUB loadlevel
OPEN "level.dat" FOR INPUT AS #1
health% = 100
armor% = 70
CALL nextlevel
END SUB

SUB LoadPOLY (file AS STRING)
OPEN file FOR INPUT AS #2
INPUT #2, cam.x, cam.z
INPUT #2, nFLOOR
INPUT #2, nWALL

REDIM floor(nFLOOR - 1) AS POL
REDIM rpoly(nWALL - 1) AS POL
REDIM slist(nWALL - 1) AS SORTLIST
REDIM wall(nWALL - 1) AS POL


FOR index = 0 TO nFLOOR - 1
   INPUT #2, floor(index).x1, floor(index).y1, floor(index).z1
   INPUT #2, floor(index).x2, floor(index).y2, floor(index).z2
   INPUT #2, floor(index).x3, floor(index).y3, floor(index).z3
   INPUT #2, floor(index).x4, floor(index).y4, floor(index).z4
   INPUT #2, floor(index).p
   INPUT #2, floor(index).texnum
NEXT index

FOR index = 0 TO nWALL - 1
   INPUT #2, wall(index).x1, wall(index).y1, wall(index).z1
   INPUT #2, wall(index).x2, wall(index).y2, wall(index).z2
   INPUT #2, wall(index).x3, wall(index).y3, wall(index).z3
   INPUT #2, wall(index).x4, wall(index).y4, wall(index).z4
   INPUT #2, wall(index).p
   INPUT #2, wall(index).texnum
   IF wall(index).p = 1 THEN
      doors(nDOOR).in = index
      doors(nDOOR).opening = 0
      nDOOR = nDOOR + 1
   END IF
   IF wall(index).p = 2 THEN
      items(nITEM).in = index
      nITEM = nITEM + 1
   END IF
   IF wall(index).p = 3 THEN
      items(nITEM).in = index
      nITEM = nITEM + 1
   END IF
   IF wall(index).p = 4 THEN
      items(nITEM).in = index
      nITEM = nITEM + 1
   END IF
   IF wall(index).p > 4 THEN
      enems(nENEM).in = index
      enems(nENEM).move = 0
      enems(nENEM).ai = 0
      enems(nENEM).stim = 0
      SELECT CASE wall(index).p
         CASE 5:
            enems(nENEM).hlth = 30
         CASE 6:
            enems(nENEM).hlth = 60
         CASE 7:
            enems(nENEM).hlth = 360
      END SELECT
      nENEM = nENEM + 1
   END IF
NEXT index

'PRINT STR$(nFLOOR) + " Boden-Polygone geladen"
'PRINT STR$(nWALL) + " Wand- & Sprite Polygone geladen"
'SLEEP

CLOSE #2
END SUB

SUB LoadSPR
DEF SEG = DQBmapLayer(MAT1LAYER)
   BLOAD "loads\mat1.spr", 0
DEF SEG
DEF SEG = DQBmapLayer(MAT2LAYER)
   BLOAD "loads\mat2.spr", 0
DEF SEG
DEF SEG = DQBmapLayer(CAPTWLAYER)
   BLOAD "loads\captw.spr", 0
DEF SEG
DEF SEG = DQBmapLayer(CAPTFLAYER)
   BLOAD "loads\captf.spr", 0
DEF SEG
dqberr = DQBloadImage(1, 0, 0, "loads\litleh.bmp", Pal, 320, 200)
DQBget 1, 0, 0, 63, 63, VARSEG(itm(0)), VARPTR(itm(0))
DQBget 1, 64, 0, 127, 63, VARSEG(itm(0)), VARPTR(itm(WALLtexBYTE))
DQBget 1, 128, 0, 191, 63, VARSEG(itm(0)), VARPTR(itm(WALLtexBYTE * 2))

DEF SEG = VARSEG(stt(0))
BLOAD "loads\status.tex", 0
DEF SEG


END SUB

SUB loadstat

OPEN "game.cfg" FOR INPUT AS #2
   INPUT #2, s$
   IF s$ = "no" THEN
      save$ = "NO GAME SAVED YET"
      savetim = 0
      CLOSE #2
      EXIT SUB
   END IF
CLOSE #2

OPEN "saves\save.dat" FOR INPUT AS #2
   INPUT #2, level$
   INPUT #2, textur$
   INPUT #2, cam.x, cam.z, alpha%, health%, armor%
   INPUT #2, nENEM
   INPUT #2, nITEM
   INPUT #2, nFLOOR
   INPUT #2, nWALL
   nDOOR = 0
   nITEM = 0
   INPUT #2, exitdoor%

   REDIM floor(nFLOOR - 1) AS POL
   REDIM rpoly(nWALL - 1) AS POL
   REDIM slist(nWALL - 1) AS SORTLIST
   REDIM wall(nWALL - 1) AS POL

   DEF SEG = VARSEG(tex(0))
      BLOAD textur$ + ".tex", 0
   DEF SEG

   FOR index = 0 TO nENEM - 1
      INPUT #2, enems(index).in
      INPUT #2, enems(index).hlth
      INPUT #2, enems(index).ai
      INPUT #2, enems(index).move
      INPUT #2, enems(index).stim
   NEXT index

   FOR index = 0 TO nFLOOR - 1
      INPUT #2, floor(index).x1
      INPUT #2, floor(index).y1
      INPUT #2, floor(index).z1
      INPUT #2, floor(index).x2
      INPUT #2, floor(index).y2
      INPUT #2, floor(index).z2
      INPUT #2, floor(index).x3
      INPUT #2, floor(index).y3
      INPUT #2, floor(index).z3
      INPUT #2, floor(index).x4
      INPUT #2, floor(index).y4
      INPUT #2, floor(index).z4
      INPUT #2, floor(index).p
      INPUT #2, floor(index).texnum
   NEXT index

   FOR index = 0 TO nWALL - 1
      INPUT #2, wall(index).x1
      INPUT #2, wall(index).y1
      INPUT #2, wall(index).z1
      INPUT #2, wall(index).x2
      INPUT #2, wall(index).y2
      INPUT #2, wall(index).z2
      INPUT #2, wall(index).x3
      INPUT #2, wall(index).y3
      INPUT #2, wall(index).z3
      INPUT #2, wall(index).x4
      INPUT #2, wall(index).y4
      INPUT #2, wall(index).z4
      INPUT #2, wall(index).p
      INPUT #2, wall(index).texnum
      IF wall(index).p = 1 THEN
         doors(nDOOR).in = index
         doors(nDOOR).opening = 0
         nDOOR = nDOOR + 1
      END IF
      IF wall(index).p = 2 THEN
         items(nITEM).in = index
         nITEM = nITEM + 1
      END IF
      IF wall(index).p = 3 THEN
         items(nITEM).in = index
         nITEM = nITEM + 1
      END IF
      IF wall(index).p = 4 THEN
         items(nITEM).in = index
         nITEM = nITEM + 1
      END IF
   NEXT index
CLOSE #2

DEF SEG = DQBmapLayer(COLLISIONLAYER)
   BLOAD "saves\Save.cmp", 0
DEF SEG

CLOSE #1
OPEN "level.dat" FOR INPUT AS #1


DO
INPUT #1, type$
SELECT CASE type$
   CASE "level":
      INPUT #1, l$, dummy$, dummy2, dummy3
      IF l$ = level$ THEN EXIT DO
   CASE "scene":
      INPUT #1, numpics%
      FOR t = 1 TO numpics%
         INPUT #1, dummy
      NEXT t
END SELECT
LOOP

save$ = "SAVED GAME LOADED"
savetim = 151
DQBprint VIDEO, save$, 0, 0, 1
DQBprint VIDEO, save$, 1, 1, 4
DQBprint VIDEO, "FIRE WHEN READY", 0, 8, 1
DQBprint VIDEO, "FIRE WHEN READY", 1, 9, 4
DO: LOOP UNTIL DQBkey%(29)
DO: LOOP UNTIL NOT DQBkey%(29)

END SUB

SUB LoadTEX (file$)

DEF SEG = VARSEG(tex(0))
BLOAD file$, 0
DEF SEG


'DEF SEG = VARSEG(itm(0))
'   BLOAD "loads\items.tex", 0
'DEF SEG

END SUB

SUB LoadTRIG
DEF SEG = VARSEG(Cosine%(0))
BLOAD "TABLE\Cosine.dat", VARPTR(Cosine%(0))
DEF SEG = VARSEG(Sine%(0))
BLOAD "TABLE\Sine.dat", VARPTR(Sine%(0))
DEF SEG
END SUB

SUB LoadWEAP
DEF SEG = DQBmapLayer(WEAPLAYER)
   BLOAD "loads\weapon.tex", 0
DEF SEG
END SUB

SUB menu
DIM x AS INTEGER
DIM y AS INTEGER

dqberr = DQBloadImage(1, 0, 0, "scenes\menu.bmp", Pal, 320, 200)
DQBsetPal Pal
DQBcopyLayer 1, VIDEO

y = 98
x = 18

DO
   IF DQBkey%(KEYDOWN) THEN
      DO: LOOP UNTIL NOT DQBkey%(KEYDOWN)
      DQBprint VIDEO, ">", x, y, 15
      y = y + 20
      IF y > 138 THEN y = 98
      DQBprint VIDEO, ">", x, y, 107
   END IF
   IF DQBkey%(KEYUP) THEN
      DO: LOOP UNTIL NOT DQBkey%(KEYUP)
      DQBprint VIDEO, ">", x, y, 15
      y = y - 20
      IF y < 98 THEN y = 138
      DQBprint VIDEO, ">", x, y, 107
   END IF
   IF DQBkey%(KEYENTER) THEN
      DO: LOOP UNTIL NOT DQBkey%(KEYENTER)
      IF y = 98 THEN
         loadlevel
         EXIT SUB
      END IF
      IF y = 118 THEN
         DQBboxf 1, 0, 0, 319, 199, 15
         dqberr = DQBloadImage(1, 76, 22, "loads\load.bmp", Pal, 168, 155)
         DQBcopyLayer 1, VIDEO
         loadstat
         EXIT SUB
      END IF
      IF y = 138 THEN
         DQBremoveKeyboard
         DQBinitText
         DQBclose
         SYSTEM
      END IF
   END IF
   DQBprint VIDEO, ">", x, y, 107
LOOP

END SUB

SUB nextlevel

begin:

nDOOR = 0
nENEM = 0
nITEM = 0

vy% = 1
weapanim% = 0
firetim% = 10
dtim% = 0
watim% = 0

INPUT #1, type$

SELECT CASE type$
   CASE "level":
      DQBclearLayer VIDEO
      DQBclearLayer 1
      DQBboxf 1, 0, 0, 319, 199, 15
      dqberr = DQBloadImage(1, 76, 22, "loads\load.bmp", Pal, 168, 155)
      DQBcopyLayer 1, VIDEO
      DQBprint VIDEO, "Loading Level...", 1, 1, 1
      DQBprint VIDEO, "Loading Level...", 0, 0, 4
      INPUT #1, level$, textur$, exitdoor%, alpha%
      LoadPOLY level$ + ".map"
      LoadCMP level$ + ".cmp"
      LoadTEX textur$ + ".tex"
      DQBprint VIDEO, "Fire when ready", 1, 12, 1
      DQBprint VIDEO, "Fire when ready", 0, 11, 4
      DO: LOOP UNTIL DQBkey%(29)
      DO: LOOP UNTIL NOT DQBkey%(29)
      CASE "end":
      DQBremoveKeyboard
      DQBinitText
      DQBclose
      SYSTEM
      CASE "scene":
         INPUT #1, numpics%
         FOR ps = 1 TO numpics%
            INPUT #1, pic$
            DQBclearLayer VIDEO
            dqberr = DQBloadImage(1, 0, 0, pic$, Pal, 320, 200)
            DQBsetPal Pal
            DQBcopyLayer 1, VIDEO
            DO: LOOP UNTIL DQBkey%(KEYESC)
            DO: LOOP UNTIL NOT DQBkey%(KEYESC)
         NEXT ps
         GOTO begin
END SELECT
END SUB

SUB RenderENEM (i AS INTEGER)
IF rpoly(i).z1 <= 0 THEN rpoly(i).z1 = 1

FOR index = 0 TO nENEM
   IF enems(index).in = i THEN
      aphase% = enems(index).ai
      EXIT FOR
   END IF
NEXT index


IF wall(i).p = 7 THEN
   IF aphase% > 3 AND aphase% < 6 THEN
      w = 50
   ELSE
      w = 64
   END IF
   x1& = 160 + (rpoly(i).x1 + w \ 2) * DISTtoPLANE1& \ rpoly(i).z1
   y1& = 100 - (rpoly(i).y1 - 32) * DISTtoPLANE2& \ rpoly(i).z1

   x2& = 160 + (rpoly(i).x1 - w \ 2) * DISTtoPLANE1& \ rpoly(i).z1
   y2& = 100 - (rpoly(i).y1 + 32) * DISTtoPLANE2& \ rpoly(i).z1
ELSE
   w = 48
   h = 48
   x1& = 160 + (rpoly(i).x1 + w \ 2) * DISTtoPLANE1& \ rpoly(i).z1
   y1& = 100 - (rpoly(i).y1 - 32) * DISTtoPLANE2& \ rpoly(i).z1

   x2& = 160 + (rpoly(i).x1 - w \ 2) * DISTtoPLANE1& \ rpoly(i).z1
   y2& = 100 - (rpoly(i).y1 + 16) * DISTtoPLANE2& \ rpoly(i).z1
END IF

IF x1& < x2& THEN SWAP x1&, x2&

    
IF IsOnScreen(x2&, y2&, 0, 0, 0, 0) THEN
   offs& = WALLtexBYTE * 2& * aphase%
   IF wall(i).p = 5 THEN
      copymem DQBmapLayer(MAT1LAYER), offs&, VARSEG(enm(0)), VARPTR(enm(0)), WALLtexBYTE
   END IF
   IF wall(i).p = 6 THEN
      copymem DQBmapLayer(MAT2LAYER), offs&, VARSEG(enm(0)), VARPTR(enm(0)), WALLtexBYTE
   END IF
   IF wall(i).p = 7 THEN
      IF aphase% < 4 THEN
         offs& = CAPTtexBYTE * 2& * aphase%
         copymem DQBmapLayer(CAPTWLAYER), offs&, VARSEG(enm(0)), VARPTR(enm(0)), CAPTtexBYTE
      END IF
      IF aphase% = 4 THEN
         copymem DQBmapLayer(CAPTFLAYER), 0, VARSEG(enm(0)), VARPTR(enm(0)), CAPTtexBYTE
      END IF
      IF aphase% = 5 THEN
         offs& = CAPTtexBYTE * 2&
         copymem DQBmapLayer(CAPTFLAYER), offs&, VARSEG(enm(0)), VARPTR(enm(0)), CAPTtexBYTE
      END IF
      IF aphase% = 6 THEN
         offs& = CAPTtexBYTE * 4&
         copymem DQBmapLayer(CAPTFLAYER), offs&, VARSEG(enm(0)), VARPTR(enm(0)), CAPTtexBYTE
      END IF
      IF aphase% = 7 THEN
         offs& = CAPTtexBYTE * 6&
         copymem DQBmapLayer(CAPTFLAYER), offs&, VARSEG(enm(0)), VARPTR(enm(0)), CAPTtexBYTE
      END IF
      IF aphase% = 8 THEN
         offs& = CAPTtexBYTE * 8&
         copymem DQBmapLayer(CAPTFLAYER), offs&, VARSEG(enm(0)), VARPTR(enm(0)), CAPTtexBYTE
      END IF

   END IF
   DQBsPut 1, x2&, y2&, VARSEG(enm(0)), VARPTR(enm(0)), ABS(x1& - x2&), ABS(y1& - y2&)
END IF
END SUB

SUB RenderFLOOR
   FOR index = slindex% - 1 TO 0 STEP -1
      i = slist(index).i
      IF i < 0 THEN GOTO skip
      IF rpoly(i).z1 <= 0 THEN rpoly(i).z1 = 1
      IF rpoly(i).z2 <= 0 THEN rpoly(i).z2 = 1
      IF rpoly(i).z3 <= 0 THEN rpoly(i).z3 = 1
      IF rpoly(i).z4 <= 0 THEN rpoly(i).z4 = 1

      x1& = 160 + (rpoly(i).x1 * DISTtoPLANE1&) \ (rpoly(i).z1)
      x2& = 160 + (rpoly(i).x2 * DISTtoPLANE1&) \ (rpoly(i).z2)
      x3& = 160 + (rpoly(i).x3 * DISTtoPLANE1&) \ (rpoly(i).z3)
      x4& = 160 + (rpoly(i).x4 * DISTtoPLANE1&) \ (rpoly(i).z4)
   
      y1& = 100 - (rpoly(i).y1 * DISTtoPLANE2&) \ (rpoly(i).z1)
      y2& = 100 - (rpoly(i).y2 * DISTtoPLANE2&) \ (rpoly(i).z2)
      y3& = 100 - (rpoly(i).y3 * DISTtoPLANE2&) \ (rpoly(i).z3)
      y4& = 100 - (rpoly(i).y4 * DISTtoPLANE2&) \ (rpoly(i).z4)

      IF IsOnScreen(x1&, y1&, x2&, y2&, x3&, y3&) THEN
         DQBttri 1, x1&, y1&, x2&, y2&, x3&, y3&, 0, 0, 0, 63, 63, 63, VARSEG(tex(0)), VARPTR(tex(WALLtexBYTE * floor(i).texnum))
      END IF
      IF IsOnScreen(x1&, y1&, x4&, y4&, x3&, y3&) THEN
         DQBttri 1, x1&, y1&, x4&, y4&, x3&, y3&, 0, 0, 63, 0, 63, 63, VARSEG(tex(0)), VARPTR(tex(WALLtexBYTE * floor(i).texnum))
      END IF

skip:
   NEXT index
   slindex% = 0

END SUB

SUB RenderITEM (i AS INTEGER, w AS INTEGER, h AS INTEGER)
IF wall(i).y1 = -64 THEN EXIT SUB
IF rpoly(i).z1 <= 0 THEN rpoly(i).z1 = 1

x1& = 160 + (rpoly(i).x1 + w \ 2) * DISTtoPLANE1& \ rpoly(i).z1
y1& = 100 - (rpoly(i).y1 - h \ 2) * DISTtoPLANE2& \ rpoly(i).z1

x4& = 160 + (rpoly(i).x1 - w \ 2) * DISTtoPLANE1& \ rpoly(i).z1
y4& = 100 - (rpoly(i).y1) * DISTtoPLANE2& \ rpoly(i).z1

DQBsPut 1, x4&, y4&, VARSEG(itm(0)), VARPTR(itm(WALLtexBYTE * (wall(i).p - 2))), ABS(y1& - y4&), ABS(y1& - y4&)
END SUB

SUB RenderSTAT
IF health% > 66 THEN DQBput 1, 0, 160, VARSEG(stt(0)), VARPTR(stt(0))
IF health% <= 66 AND health% > 33 THEN DQBput 1, 0, 160, VARSEG(stt(0)), VARPTR(stt(STATtexBYTE))
IF health% <= 33 THEN DQBput 1, 0, 160, VARSEG(stt(0)), VARPTR(stt(STATtexBYTE * 2))

DQBput 1, 30, 130, VARSEG(itm(WALLtexBYTE * 2)), VARPTR(itm(WALLtexBYTE * 2))

lenh = LEN(STR$(health%)) * 4
DQBprint 1, STR$(health%), 18 - lenh, 150, 1
DQBprint 1, STR$(health%), 17 - lenh, 149, 4

lenh = LEN(STR$(armor%)) * 4
DQBprint 1, STR$(armor%), 60 - lenh, 150, 1
DQBprint 1, STR$(armor%), 59 - lenh, 149, 4

IF save$ <> "" AND savetim < 100 THEN
   DQBprint 1, save$, 0, 0, 1
   DQBprint 1, save$, 1, 1, 4
   savetim = savetim + 1
ELSE
   savetim = 0
   save$ = ""
END IF

END SUB

SUB RenderWALL
DIM i AS INTEGER

   FOR index = slindex% - 1 TO 0 STEP -1
      i = slist(index).i
      IF i < 0 THEN GOTO skip2
      IF wall(i).p = 2 OR wall(i).p = 3 OR wall(i).p = 4 THEN CALL RenderITEM(i, 64, 64): GOTO skip2
      IF wall(i).p > 4 THEN CALL RenderENEM(i): GOTO skip2

      IF rpoly(i).z1 <= 0 THEN rpoly(i).z1 = 1
      IF rpoly(i).z2 <= 0 THEN rpoly(i).z2 = 1
      IF rpoly(i).z3 <= 0 THEN rpoly(i).z3 = 1
      IF rpoly(i).z4 <= 0 THEN rpoly(i).z4 = 1

      x1& = 160 + (rpoly(i).x1 * DISTtoPLANE1&) \ (rpoly(i).z1)
      x2& = 160 + (rpoly(i).x2 * DISTtoPLANE1&) \ (rpoly(i).z2)
      x3& = 160 + (rpoly(i).x3 * DISTtoPLANE1&) \ (rpoly(i).z3)
      x4& = 160 + (rpoly(i).x4 * DISTtoPLANE1&) \ (rpoly(i).z4)
  
      y1& = 100 - (rpoly(i).y1 * DISTtoPLANE2&) \ (rpoly(i).z1)
      y2& = 100 - (rpoly(i).y2 * DISTtoPLANE2&) \ (rpoly(i).z2)
      y3& = 100 - (rpoly(i).y3 * DISTtoPLANE2&) \ (rpoly(i).z3)
      y4& = 100 - (rpoly(i).y4 * DISTtoPLANE2&) \ (rpoly(i).z4)

      IF IsOnScreen(x1&, y1&, x2&, y2&, x3&, y3&) THEN
         DQBttri 1, x1&, y1&, x2&, y2&, x3&, y3&, 0, 0, 0, 63, 63, 63, VARSEG(tex(0)), VARPTR(tex(WALLtexBYTE * wall(i).texnum))
      END IF
      IF IsOnScreen(x1&, y1&, x4&, y4&, x3&, y3&) THEN
         DQBttri 1, x1&, y1&, x4&, y4&, x3&, y3&, 0, 0, 63, 0, 63, 63, VARSEG(tex(0)), VARPTR(tex(WALLtexBYTE * wall(i).texnum))
      END IF

skip2:
   NEXT index
   slindex% = 0


END SUB

SUB RenderWEAP
IF weapanim% = 0 THEN
   copymem DQBmapLayer(WEAPLAYER), 0, VARSEG(wep(0)), VARPTR(wep(0)), WEAPtexBYTE
   DQBput 1, 90, 133, VARSEG(wep(0)), VARPTR(wep(0))
ELSE
   copymem DQBmapLayer(WEAPLAYER), WEAPtexBYTE * 2, VARSEG(wep(0)), VARPTR(wep(0)), WEAPtexBYTE2
   DQBput 1, 86, 118, VARSEG(wep(0)), VARPTR(wep(0))
END IF
END SUB

SUB RotateY (index AS INTEGER)
DIM tempx%
   tempx% = rpoly(index).x1
   rpoly(index).x1 = CLNG((CLNG(rpoly(index).z1) * Cosine%(alpha%) - CLNG(rpoly(index).x1) * Sine%(alpha%))) \ 256
   rpoly(index).z1 = CLNG((CLNG(tempx%) * Cosine%(alpha%) + CLNG(rpoly(index).z1) * Sine%(alpha%))) \ 256
   tempx% = rpoly(index).x2
   rpoly(index).x2 = CLNG((CLNG(rpoly(index).z2) * Cosine%(alpha%) - CLNG(rpoly(index).x2) * Sine%(alpha%))) \ 256
   rpoly(index).z2 = CLNG((CLNG(tempx%) * Cosine%(alpha%) + CLNG(rpoly(index).z2) * Sine%(alpha%))) \ 256
   tempx% = rpoly(index).x3
   rpoly(index).x3 = CLNG((CLNG(rpoly(index).z3) * Cosine%(alpha%) - CLNG(rpoly(index).x3) * Sine%(alpha%))) \ 256
   rpoly(index).z3 = CLNG((CLNG(tempx%) * Cosine%(alpha%) + CLNG(rpoly(index).z3) * Sine%(alpha%))) \ 256
   tempx% = rpoly(index).x4
   rpoly(index).x4 = CLNG((CLNG(rpoly(index).z4) * Cosine%(alpha%) - CLNG(rpoly(index).x4) * Sine%(alpha%))) \ 256
   rpoly(index).z4 = CLNG((CLNG(tempx%) * Cosine%(alpha%) + CLNG(rpoly(index).z4) * Sine%(alpha%))) \ 256
END SUB

SUB SaveState

OPEN "game.cfg" FOR OUTPUT AS #2
   WRITE #2, "yes"
CLOSE #2

OPEN "saves\save.dat" FOR OUTPUT AS #2

   WRITE #2, level$
   WRITE #2, textur$
   WRITE #2, cam.x, cam.z, alpha%, health%, armor%
   WRITE #2, nENEM
   WRITE #2, nITEM
   WRITE #2, nFLOOR
   WRITE #2, nWALL
   WRITE #2, exitdoor%
  
   FOR index = 0 TO nENEM - 1
      WRITE #2, enems(index).in
      WRITE #2, enems(index).hlth
      WRITE #2, enems(index).ai
      WRITE #2, enems(index).move
      WRITE #2, enems(index).stim
   NEXT index
 
   FOR index = 0 TO nFLOOR - 1
      WRITE #2, floor(index).x1
      WRITE #2, floor(index).y1
      WRITE #2, floor(index).z1
      WRITE #2, floor(index).x2
      WRITE #2, floor(index).y2
      WRITE #2, floor(index).z2
      WRITE #2, floor(index).x3
      WRITE #2, floor(index).y3
      WRITE #2, floor(index).z3
      WRITE #2, floor(index).x4
      WRITE #2, floor(index).y4
      WRITE #2, floor(index).z4
      WRITE #2, floor(index).p
      WRITE #2, floor(index).texnum
   NEXT index

   FOR index = 0 TO nWALL - 1
      WRITE #2, wall(index).x1
      WRITE #2, wall(index).y1
      WRITE #2, wall(index).z1
      WRITE #2, wall(index).x2
      WRITE #2, wall(index).y2
      WRITE #2, wall(index).z2
      WRITE #2, wall(index).x3
      WRITE #2, wall(index).y3
      WRITE #2, wall(index).z3
      WRITE #2, wall(index).x4
      WRITE #2, wall(index).y4
      WRITE #2, wall(index).z4
      WRITE #2, wall(index).p
      WRITE #2, wall(index).texnum
   NEXT index
CLOSE #2

DEF SEG = DQBmapLayer(COLLISIONLAYER)
   BSAVE "saves\Save.cmp", 0, 64000
DEF SEG

save$ = "GAME SAVED"
savetim = 0

END SUB

SUB TranslateFLOOR
DIM index AS INTEGER

   FOR index = 0 TO nFLOOR - 1
      AddCAMFLOOR index
      RotateY index
      IF ClipPOLY%(index) = 0 THEN
         slist(slindex%).i = index
         'slist(slindex%).z = (rpoly(index).z1 + rpoly(index).z2 + rpoly(index).z3 + rpoly(index).z4) \ 4
         slindex% = slindex% + 1
      END IF
   NEXT index
END SUB

SUB TranslateWALL
DIM index AS INTEGER

   FOR index = 0 TO nWALL - 1
      IF wall(index).y1 = -34 THEN GOTO skip3DOOR
      AddCAMWALL index
      RotateY index
      IF ClipPOLY%(index) = 0 THEN
         slist(slindex%).i = index
         slist(slindex%).z = (rpoly(index).z1 + rpoly(index).z2 + rpoly(index).z3 + rpoly(index).z4) \ 4
         slindex% = slindex% + 1
      END IF
skip3DOOR:
   NEXT index
   DQBsort VARSEG(slist(0)), VARPTR(slist(0)), slindex%, 4, 0
END SUB

