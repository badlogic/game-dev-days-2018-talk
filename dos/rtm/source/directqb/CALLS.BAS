' This file contains function overrides for the DirectQB Library version 1.61
' Created by the DirectQB Library Manager v1.2 on 07-10-2018
' If you've successfully built the library, you can now delete this file...
DEFINT A-Z
DECLARE SUB xDQBerror (BYVAL MsgSeg, BYVAL MsgOff)
DECLARE FUNCTION DQBerror$ ()
DECLARE FUNCTION DQBid$ ()
DECLARE FUNCTION xDQBloadImage (BYVAL Layer, BYVAL x, BYVAL y, BYVAL FileSeg, BYVAL FileOff, BYVAL PalSeg, BYVAL PalOff, imgWidth AS INTEGER, imgHeight AS INTEGER)
DECLARE FUNCTION DQBloadImage (Layer AS INTEGER, x AS INTEGER, y AS INTEGER, FileName AS STRING, Pal AS STRING, imgWidth AS INTEGER, imgHeight AS INTEGER)
DECLARE FUNCTION xDQBsaveImage (BYVAL Layer, BYVAL x1, BYVAL y1, BYVAL x2, BYVAL y2, BYVAL FileSeg, BYVAL FileOff, BYVAL PalSeg, BYVAL PalOff, BYVAL Format)
DECLARE FUNCTION DQBsaveImage (Layer AS INTEGER, x1 AS INTEGER, y1 AS INTEGER, x2 AS INTEGER, y2 AS INTEGER, FileName AS STRING, Pal AS STRING, Format AS INTEGER)
DECLARE FUNCTION xDQBplayFLI (BYVAL FileSeg, BYVAL FileOff, BYVAL BufSeg, BYVAL KeyStop, BYVAL LoopFlag)
DECLARE FUNCTION DQBplayFLI (FileName AS STRING, BufLayer AS INTEGER, KeyStop AS INTEGER, LoopFlag AS INTEGER)
DECLARE FUNCTION xDQBopenFLI (BYVAL FileSeg, BYVAL FileOff, Frames AS INTEGER, Speed AS INTEGER)
DECLARE FUNCTION DQBopenFLI (FileName AS STRING, Frames AS INTEGER, Speed AS INTEGER)
DECLARE FUNCTION xDQBfindPalCol (BYVAL PalSeg, BYVAL PalOff, BYVAL Red, BYVAL Green, BYVAL Blue)
DECLARE FUNCTION DQBfindPalCol (Pal AS STRING, Red AS INTEGER, Green AS INTEGER, Blue AS INTEGER)
DECLARE SUB xDQBsetPal (BYVAL PalSeg, BYVAL PalOff)
DECLARE SUB DQBsetPal (Pal AS STRING)
DECLARE SUB xDQBgetPal (BYVAL PalSeg, BYVAL PalOff)
DECLARE SUB DQBgetPal (Pal AS STRING)
DECLARE SUB xDQBfadeIn (BYVAL PalSeg, BYVAL PalOff)
DECLARE SUB DQBfadeIn (Pal AS STRING)
DECLARE SUB xDQBfadeStepIn (BYVAL PalSeg, BYVAL PalOff)
DECLARE SUB DQBfadeStepIn (Pal AS STRING)
DECLARE FUNCTION xDQBlen (BYVAL TextSeg, BYVAL TextOff)
DECLARE FUNCTION DQBlen (Text AS STRING)
DECLARE FUNCTION xDQBloadFont (BYVAL FileSeg, BYVAL FileOff)
DECLARE FUNCTION DQBloadFont (FileName AS STRING)
DECLARE SUB xDQBprint (BYVAL Layer, BYVAL TextSeg, BYVAL TextOff, BYVAL x, BYVAL y, BYVAL Col, BYVAL Style)
DECLARE SUB DQBprint (Layer AS INTEGER, Text AS STRING, x AS INTEGER, y AS INTEGER, Col AS INTEGER)
DECLARE SUB DQBprints (Layer AS INTEGER, Text AS STRING, x AS INTEGER, y AS INTEGER, Col AS INTEGER, Style AS INTEGER)
DECLARE SUB xDQBsetFont (BYVAL FontSeg, BYVAL FontOff)
DECLARE SUB DQBsetFont (Font AS STRING)
DECLARE FUNCTION xDQBloadBMap (BYVAL BMap, BYVAL FileSeg, BYVAL FileOff)
DECLARE FUNCTION DQBloadBMap (BMap AS INTEGER, FileName AS STRING)
DECLARE FUNCTION xDQBsaveBMap (BYVAL BMap, BYVAL FileSeg, BYVAL FileOff)
DECLARE FUNCTION DQBsaveBMap (BMap AS INTEGER, FileName AS STRING)
DECLARE FUNCTION xDQBinkey ()
DECLARE FUNCTION DQBinkey$ ()
DECLARE SUB xDQBsetMouseShape (BYVAL hotX, BYVAL hotY, BYVAL ShapeSeg, BYVAL ShapeOff)
DECLARE SUB DQBsetMouseShape (hotX AS INTEGER, hotY AS INTEGER, Shape AS STRING)
FUNCTION DQBerror$
DIM ErrorMsg AS STRING * 32
xDQBerror VARSEG(ErrorMsg), VARPTR(ErrorMsg)
DQBerror$ = RTRIM$(ErrorMsg)
END FUNCTION
FUNCTION DQBid$
DQBid$ = "DirectQB v1.61 by Angelo Mottola - Enhanced Creations 1998-99"
END FUNCTION
FUNCTION DQBloadImage (Layer, x, y, FileName AS STRING, Pal AS STRING, imgWidth AS INTEGER, imgHeight AS INTEGER)
DIM NewName AS STRING
NewName = FileName + CHR$(0)
DQBloadImage = xDQBloadImage(Layer, x, y, VARSEG(NewName), SADD(NewName), VARSEG(Pal), SADD(Pal), imgWidth, imgHeight)
END FUNCTION
FUNCTION DQBsaveImage (Layer AS INTEGER, x1 AS INTEGER, y1 AS INTEGER, x2 AS INTEGER, y2 AS INTEGER, FileName AS STRING, Pal AS STRING, Format AS INTEGER)
DIM NewName AS STRING
NewName = FileName + CHR$(0)
DQBsaveImage = xDQBsaveImage(Layer, x1, y1, x2, y2, VARSEG(NewName), SADD(NewName), VARSEG(Pal), SADD(Pal), Format)
END FUNCTION
FUNCTION DQBplayFLI (FileName AS STRING, BufLayer AS INTEGER, KeyStop AS INTEGER, LoopFlag AS INTEGER)
DIM NewName AS STRING
NewName = FileName + CHR$(0)
DQBplayFLI = xDQBplayFLI(VARSEG(NewName), SADD(NewName), BufLayer, KeyStop, LoopFlag)
END FUNCTION
FUNCTION DQBopenFLI (FileName AS STRING, Frames AS INTEGER, Speed AS INTEGER)
DIM NewName AS STRING
NewName = FileName + CHR$(0)
DQBopenFLI = xDQBopenFLI(VARSEG(NewName), SADD(NewName), Frames, Speed)
END FUNCTION
FUNCTION DQBfindPalCol (Pal AS STRING, Red AS INTEGER, Green AS INTEGER, Blue AS INTEGER)
DQBfindPalCol = xDQBfindPalCol(VARSEG(Pal), SADD(Pal), Red, Green, Blue)
END FUNCTION
SUB DQBsetPal (Pal AS STRING)
xDQBsetPal VARSEG(Pal), SADD(Pal)
END SUB
SUB DQBgetPal (Pal AS STRING)
xDQBgetPal VARSEG(Pal), SADD(Pal)
END SUB
SUB DQBfadeIn (Pal AS STRING)
xDQBfadeIn VARSEG(Pal), SADD(Pal)
END SUB
SUB DQBfadeStepIn (Pal AS STRING)
xDQBfadeStepIn VARSEG(Pal), SADD(Pal)
END SUB
FUNCTION DQBlen (Text AS STRING)
DIM NewText AS STRING
NewText = Text + CHR$(0)
DQBlen = xDQBlen(VARSEG(NewText), SADD(NewText))
END FUNCTION
FUNCTION DQBloadFont (FileName AS STRING)
DIM NewName AS STRING
NewName = FileName + CHR$(0)
DQBloadFont = xDQBloadFont(VARSEG(NewName), SADD(NewName))
END FUNCTION
SUB DQBprint (Layer AS INTEGER, Text AS STRING, x AS INTEGER, y AS INTEGER, Col AS INTEGER)
DIM NewText AS STRING
NewText = Text + CHR$(0)
xDQBprint Layer, VARSEG(NewText), SADD(NewText), x, y, Col, &HFF
END SUB
SUB DQBprints (Layer AS INTEGER, Text AS STRING, x AS INTEGER, y AS INTEGER, Col AS INTEGER, Style AS INTEGER)
DIM NewText AS STRING
NewText = Text + CHR$(0)
xDQBprint Layer, VARSEG(NewText), SADD(NewText), x, y, Col, Style
END SUB
SUB DQBsetFont (Font AS STRING)
xDQBsetFont VARSEG(Font), SADD(Font)
END SUB
FUNCTION DQBloadBMap (BMap AS INTEGER, FileName AS STRING)
DIM NewName AS STRING
NewName = FileName + CHR$(0)
DQBloadBMap = xDQBloadBMap(BMap, VARSEG(NewName), SADD(NewName))
END FUNCTION
FUNCTION DQBsaveBMap (BMap AS INTEGER, FileName AS STRING)
DIM NewName AS STRING
NewName = FileName + CHR$(0)
DQBsaveBMap = xDQBsaveBMap(BMap, VARSEG(NewName), SADD(NewName))
END FUNCTION
FUNCTION DQBinkey$
DIM Char AS STRING * 1
Char = CHR$(xDQBinkey)
IF Char = CHR$(0) THEN DQBinkey$ = "" ELSE DQBinkey$ = Char
END FUNCTION
SUB DQBsetMouseShape (hotX AS INTEGER, hotY AS INTEGER, Shape AS STRING)
xDQBsetMouseShape hotX, hotY, VARSEG(Shape), SADD(Shape)
END SUB
