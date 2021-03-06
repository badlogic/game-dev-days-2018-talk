' ---------------------------------------------------------------------------
'                         DirectQB Datafile Encoder
'                                version 1.0
'               by Angelo Mottola - Enhanced Creations 1998-99
' ---------------------------------------------------------------------------
'
' Use this program to create DQB-compatible datafiles to store your own data.
' Data is compressed using a simple RLE based algorithm; this means low
' compression rates, but I choose it due to its ease of implementation. You
' can also crypt the data, by giving an unique password of up to 8 characters:
' this password will not be stored into the datafile itself, so people will
' not be able to read your stuff without it.
' Datafiles can contain:
' - fonts: the font files must have been generated by DirectQB Tools or by
'     any other program that's able to save them into a DQB-compatible format.
' - images: they can be images of any size saved into BSV, PCX or BMP format.
' - sounds: must be WAV files.
' - palettes: taken either from a PAL file (the common 768 bytes palette file)
'     or from a BSV, PCX or BMP image. If you specify a BSV image as the
'     source file for your palette, be sure that the BSV file has the palette
'     attached at its end; DQBsaveImage automatically makes this attachment
'     when you save an image into the BSV format.
' - blender maps: must be files saved with DQBsaveBMap.
' - cursors: mouse cursor files saved by DQB Tools or any other program able
'     to store a cursor into a DQB-compatible format.
' - user data: any kind of binary file data.
'
' It is suggested that you compile this program and always run the EXE
' version; run it without command line arguments to get a brief help on how
' the program works.
'
' The following example of working infofile will build the TEST.BIN datafile,
' holding the image stored into MYPICT.PCX, two sounds from SAMPLE01.WAV and
' SAMPLE02.WAV, and font SMALL.FNT: all these files are supposed to be into
' the current directory, as no path is specified. The datafile will be also
' crypted using password "TEST"
'
' OUTFILE=TEST.BIN
' PASSWORD=TEST
' IMAGE:MYPICT=MYPICT.PCX
' SOUND:SOUND1=SAMPLE01.WAV
' SOUND:SOUND2=SAMPLE02.WAV
' FONT:SMALL=SMALL.FNT
'
' ---------------------------------------------------------------------------

'$INCLUDE: 'DIRECTQB.BI'
DEFINT A-Z
DECLARE SUB Init ()
DECLARE SUB Usage ()
DECLARE SUB CheckInfo ()
DECLARE SUB CreateDataFile ()
DECLARE SUB CreateIncludeFile ()
DECLARE SUB AddPacket (index)
DECLARE SUB ShowError (errorline, errormsg$)
DECLARE SUB PutByte (code)
DECLARE SUB Encode (pNum, pFile$, pType, pId$, NumBytes AS LONG, DataSeg, DataOff)
DECLARE FUNCTION CheckPacket (pId, p$)
DECLARE FUNCTION FixFilename$ (f$)

CONST VERSION$ = "1.1", BUFLEN = 4096

DIM SHARED InfoFile AS STRING, NumPackets, OutFile AS STRING, BIfile AS STRING
DIM SHARED Password AS STRING, PasswordPtr, PackID(255) AS STRING * 8
DIM SHARED PackType(255), PackFile(255) AS STRING, PackOff(255) AS LONG
DIM SHARED CurPack AS STRING, Buffer(32767), WriteBuf AS STRING * BUFLEN
DIM SHARED WritePtr, TotalBytes AS LONG, EncodedBytes AS LONG
DIM SHARED PacketBytes AS LONG, StartTimer AS DOUBLE

ON ERROR GOTO ErrorHandler

d& = SETMEM(-66000)
IF DQBinit(0, 0, 0) THEN DQBclose: END
Init
CheckInfo
CreateDataFile
PRINT
PRINT USING "Datafile successfully created in ###.## seconds."; TIMER - StartTimer#
PRINT "Total compression rate:";
PRINT STR$(INT(((TotalBytes - EncodedBytes) / TotalBytes) * 100)) + "%"
PRINT
CreateIncludeFile

DQBclose
d& = SETMEM(66000)
END

ErrorHandler:
CLOSE
KILL OutFile
PRINT : PRINT "Error: ";
SELECT CASE ERR
CASE 53: PRINT "Packet file not found (" + CurPack + ")"
CASE 57: PRINT "I/O device fault"
CASE 61: PRINT "Disk full"
CASE 64: PRINT "Bad packet file name"
CASE 70: PRINT "Permission denied"
CASE 71: PRINT "Disk not ready"
CASE 72: PRINT "Internal disk error"
CASE 75: PRINT "Cannot access packet path (" + CurPack + ")"
CASE 76: PRINT "Packet path not found (" + CurPack + ")"
CASE 77: PRINT "Error loading packet " + CurPack
CASE 78: PRINT "User packet " + CurPack + " exceeds 64K length limit"
CASE ELSE: PRINT "Internal program error, code " + STR$(ERR)
END SELECT
PRINT "Program aborted"
DQBclose
END

' DQB datafile format
'
' offset  length  description
' 0       7       File id: must be "DQBPaCk"
' 7       1       Number of packets
' 8       1024    256 packet absolute offset references
' 1032    ?       Actual packets data

REM $DYNAMIC
SUB AddPacket (index)
'
DIM Pal AS STRING * 768, wid, hei, t AS LONG
CurPack = PackFile(index)
DQBclearLayer B0
OPEN CurPack FOR BINARY AS #2
l& = LOF(2): IF l& = 0 THEN CLOSE #2: KILL CurPack: ERROR 53
PasswordPtr = 1
LOCATE CSRLIN, 1: PRINT "Accessing data...";
SELECT CASE PackType(index)
CASE 1
  a$ = SPACE$(2305): GET #2, , a$
  DEF SEG = DQBmapLayer(B0)
  FOR e = 0 TO 2304
    POKE e, ASC(MID$(a$, e + 1, 1))
  NEXT e
  a$ = "FNT�": Encode 0, "", 0, "", 4, VARSEG(a$), SADD(a$)
  Encode index, FixFilename$(CurPack), 1, PackID(index), 2305, DQBmapLayer(B0), 0
CASE 2
  CLOSE #2
  IF DQBloadImage(B0, 0, 0, CurPack, Pal, wid, hei) THEN ERROR 77
  DQBget B0, 0, 0, wid - 1, hei - 1, VARSEG(Buffer(0)), VARPTR(Buffer(0))
  a$ = "IMG�" + MKI$(wid) + MKI$(hei): Encode 0, "", 0, "", 8, VARSEG(a$), SADD(a$)
  w& = wid: h& = hei: l& = w& * h&
  Encode index, FixFilename$(CurPack), 2, PackID(index), l&, VARSEG(Buffer(0)), VARPTR(Buffer(0)) + 4
CASE 3
  IF LOF(2) < 45 THEN ERROR 77
  l& = LOF(2) - 44
  DEF SEG = VARSEG(Buffer(0))
  FOR e& = 1 TO l&
    a$ = " ": GET #2, , a$: POKE VARPTR(Buffer(0)) + (e& - 1), ASC(a$)
  NEXT e&
  a$ = "SND�" + MKL$(l&): Encode 0, "", 0, "", 8, VARSEG(a$), SADD(a$)
  Encode index, FixFilename$(CurPack), 3, PackID(index), l&, VARSEG(Buffer(0)), VARPTR(Buffer(0))
CASE 4
  a$ = "PAL�": Encode 0, "", 0, "", 4, VARSEG(a$), SADD(a$)
  IF l& = 768 THEN
    GET #2, , Pal: CLOSE #2
    Encode index, FixFilename$(CurPack), 4, PackID(index), 768, VARSEG(Pal), VARPTR(Pal)
  ELSE
    CLOSE #2
    IF DQBloadImage(B0, 0, 0, CurPack, Pal, wid, hei) THEN ERROR 77
    Encode index, FixFilename$(CurPack), 4, PackID(index), 768, VARSEG(Pal), VARPTR(Pal)
  END IF
CASE 5
  a$ = SPACE$(6): GET #2, , a$
  IF MID$(a$, 1, 4) <> "BMap" THEN ERROR 77
  t = ASC(MID$(a$, 6, 1)) - ASC(MID$(a$, 5, 1)) + 1
  DEF SEG = VARSEG(Buffer(0))
  FOR e& = 1 TO (t * 256)
    a$ = SPACE$(1): GET #2, , a$
    POKE VARPTR(Buffer(0)) + (e& - 1), ASC(a$)
  NEXT e&
  a$ = "BMA�" + CHR$(t): Encode 0, "", 0, "", 5, VARSEG(a$), SADD(a$)
  Encode index, FixFilename$(CurPack), 5, PackID(index), (t * 256), VARSEG(Buffer(0)), VARPTR(Buffer(0))
CASE 6
  IF l& <> 64 THEN ERROR 77
  a$ = SPACE$(64): GET #2, , a$
  a$ = "CUR�" + a$
  Encode index, FixFilename$(CurPack), 6, PackID(index), 68, VARSEG(a$), SADD(a$)
CASE 7
  IF l& > 65535 THEN ERROR 78
  DEF SEG = VARSEG(Buffer(0))
  FOR e& = 1 TO l&
    a$ = SPACE$(1): GET #2, , a$
    POKE VARPTR(Buffer(0)) + (e& - 1), ASC(a$)
  NEXT e&
  a$ = "USR�" + MKL$(l&): Encode 0, "", 0, "", 8, VARSEG(a$), SADD(a$)
  Encode index, FixFilename$(CurPack), 7, PackID(index), l&, VARSEG(Buffer(0)), VARPTR(Buffer(0))
CASE ELSE
  PRINT LTRIM$(STR$(index)) + "/" + LTRIM$(STR$(NumPackets)) + " Unknown packet skipped"
  CLOSE #2: EXIT SUB
END SELECT
CLOSE #2
PRINT LTRIM$(STR$(INT(((l& - PacketBytes) / l&) * 100))) + "% rate"

END SUB

SUB CheckInfo
'
DIM ok, Pass, Outf, BIf, l(999) AS STRING, NumLines
NumPackets = 0: NumLines = 0
PRINT "Processing " + InfoFile + "..."
' Load file in memory
OPEN InfoFile FOR INPUT AS #1
WHILE NOT EOF(1)
  LINE INPUT #1, lin$
  l(NumLines) = LTRIM$(RTRIM$(UCASE$(lin$)))
  NumLines = NumLines + 1
  IF NumLines = 1000 THEN ShowError 0, "Infofile is too big!"
WEND
CLOSE #1
NumLines = NumLines - 1
' First pass
FOR i = 0 TO NumLines
  WHILE INSTR(l(i), " ") > 0
    s = INSTR(l(i), " ")
    l(i) = LEFT$(l(i), s - 1) + MID$(l(i), s + 1)
  WEND
NEXT i
' Second pass
Check = 0
FOR i = 0 TO NumLines
  ok = 1
  IF LEFT$(l(i), 8) = "OUTFILE=" THEN Outf = Outf + 1: ok = 0: OutFile = MID$(l(i), 9)
  IF LEFT$(l(i), 12) = "INCLUDEFILE=" THEN BIf = BIf + 1: ok = 0: BIfile = MID$(l(i), 13)
  IF LEFT$(l(i), 9) = "PASSWORD=" THEN Pass = Pass + 1: ok = 0: Password = MID$(l(i), 10): IF Password = "" THEN ok = 2
  IF LEFT$(l(i), 5) = "FONT:" THEN ok = CheckPacket(1, MID$(l(i), 6))
  IF LEFT$(l(i), 6) = "IMAGE:" THEN ok = CheckPacket(2, MID$(l(i), 7))
  IF LEFT$(l(i), 6) = "SOUND:" THEN ok = CheckPacket(3, MID$(l(i), 7))
  IF LEFT$(l(i), 8) = "PALETTE:" THEN ok = CheckPacket(4, MID$(l(i), 9))
  IF LEFT$(l(i), 5) = "BMAP:" THEN ok = CheckPacket(5, MID$(l(i), 6))
  IF LEFT$(l(i), 7) = "CURSOR:" THEN ok = CheckPacket(6, MID$(l(i), 8))
  IF LEFT$(l(i), 5) = "USER:" THEN ok = CheckPacket(7, MID$(l(i), 6))
  IF LEFT$(l(i), 1) = "#" OR l(i) = "" THEN ok = 0
  IF ok = 1 THEN ShowError i + 1, "Unknown identifier"
  IF ok = 2 THEN ShowError i + 1, "Missing parameter"
  IF ok = 3 THEN ShowError i + 1, "Packet ID exceeds 8 characters length limit"
NEXT i
IF Outf = 0 THEN PRINT "No outfile file specified!": DQBclose: END
IF Outf > 1 THEN PRINT "Multiple output file definitions": DQBclose: END
IF BIf = 0 THEN PRINT "No include file specified!": DQBclose: END
IF BIf > 1 THEN PRINT "Multiple include file definitions": DQBclose: END
IF Pass > 1 THEN PRINT "Multiple password definitions": DQBclose: END
IF NumPackets = 0 THEN PRINT "No packets specified!": DQBclose: END
IF NumPackets > 256 THEN PRINT "Cannot store more than 256 packets into the same datafile!": DQBclose: END
IF INSTR(OutFile, ".") > 0 AND LEN(OutFile) > 12 THEN PRINT "Bad output filename specified": DQBclose: END
IF INSTR(OutFile, ".") = 0 AND LEN(OutFile) > 8 THEN PRINT "Bad output filename specified": DQBclose: END
IF LEN(Password) > 8 THEN PRINT "Password exceeds 8 characters length limit": DQBclose: END
NumPackets = NumPackets - 1
FOR i = 0 TO NumPackets
  FOR ii = 0 TO NumPackets
    IF PackID(i) = PackID(ii) AND i <> ii THEN
      PRINT "Packet id ";
      p$ = ""
      FOR a = 1 TO 8
        IF MID$(PackID(i), a, 1) = CHR$(0) THEN EXIT FOR
        p$ = p$ + MID$(PackID(i), a, 1)
      NEXT a
      PRINT CHR$(34) + p$ + CHR$(34) + " is used more than once"
      DQBclose
      END
    END IF
  NEXT ii
NEXT i
IF Pass = 0 THEN PRINT "Warning: no datafile password specified": Password = ""
IF INSTR(OutFile, ".") = 0 THEN PRINT "Assuming default extension for output file": OutFile = OutFile + ".BIN"
IF INSTR(BIfile, ".") = 0 THEN PRINT "Assuming default extension for include file": BIfile = BIfile + ".BI"
Password = Password + CHR$(0)

END SUB

FUNCTION CheckPacket (pId, p$)
'
DIM fixedID AS STRING * 8
sign = INSTR(p$, "=")
IF sign = 0 THEN CheckPacket = 2: EXIT FUNCTION
id$ = LEFT$(p$, sign - 1)
IF id$ = "" THEN CheckPacket = 2: EXIT FUNCTION
IF LEN(id$) > 8 THEN CheckPacket = 3: EXIT FUNCTION
path$ = MID$(p$, sign + 1)
IF path$ = "" THEN CheckPacket = 2: EXIT FUNCTION
fixedID = STRING$(8, 0)
MID$(fixedID, 1, LEN(id$)) = id$
PackID(NumPackets) = fixedID
PackFile(NumPackets) = path$
PackType(NumPackets) = pId
NumPackets = NumPackets + 1
CheckPacket = 0

END FUNCTION

SUB CreateDataFile
'
TotalBytes = 0: WritePtr = 1
OPEN OutFile FOR BINARY AS #1
IF LOF(1) > 0 THEN
  CLOSE #1: KILL OutFile: OPEN OutFile FOR BINARY AS #1
  PRINT "Warning: overwriting " + OutFile
ELSE
  PRINT "Creating " + OutFile + "..."
END IF
PRINT LTRIM$(STR$(NumPackets + 1)) + " packets to be encoded"
PRINT
StartTimer = TIMER
a$ = "DQBPaCk" + CHR$(NumPackets): PUT #1, , a$
FOR i = 0 TO 255: a$ = STRING$(4, 0): PUT #1, , a$: NEXT i
FOR MainLoop = 0 TO NumPackets
  l& = LOF(1) + WritePtr - 1
  PUT #1, (9 + (MainLoop * 4)), l&
  l& = LOF(1)
  SEEK #1, l& + 1
  AddPacket MainLoop
NEXT MainLoop
PutByte -1
EncodedBytes = LOF(1)
CLOSE #1

END SUB

REM $STATIC
SUB CreateIncludeFile
'
OPEN BIfile FOR BINARY AS #1
IF LOF(1) > 0 THEN o = 1 ELSE o = 0
CLOSE #1
OPEN BIfile FOR OUTPUT AS #1
PRINT #1, "' ---------------------------------------------------------------------------"
PRINT #1, "' Include file for " + OutFile + " packet references"
PRINT #1, "' Created by the DirectQB DataFile Encoder version " + VERSION$ + " on " + DATE$
PRINT #1, "' ---------------------------------------------------------------------------"
PRINT #1, ""
FOR i = 0 TO NumPackets
  a$ = PackID(i)
  IF INSTR(a$, CHR$(0)) > 0 THEN a$ = LEFT$(a$, INSTR(a$, CHR$(0)) - 1)
  IF LEN(a$) < 8 THEN a$ = a$ + SPACE$(8 - LEN(a$))
  PRINT #1, "CONST " + a$ + " =" + STR$(i) + "   ' ";
  SELECT CASE PackType(i)
  CASE 1: PRINT #1, "Font"
  CASE 2: PRINT #1, "Image"
  CASE 3: PRINT #1, "Sound"
  CASE 4: PRINT #1, "Palette"
  CASE 5: PRINT #1, "Blender map"
  CASE 6: PRINT #1, "Mouse cursor"
  CASE 7: PRINT #1, "User data"
  END SELECT
NEXT i
CLOSE #1
PRINT BIfile;
IF o THEN PRINT " overwritten." ELSE PRINT " created."

END SUB

REM $DYNAMIC
SUB Encode (pNum, Pack$, pType, pId$, NumBytes AS LONG, DataSeg, DataOff)
'
p$ = ""
FOR i = 1 TO 8
  IF MID$(pId$, i, 1) = CHR$(0) THEN EXIT FOR
  p$ = p$ + MID$(pId$, i, 1)
NEXT i
TotalBytes = TotalBytes + NumBytes
DEF SEG = DataSeg
DIM curPtr AS LONG
curPtr = 0: PacketBytes = 0
DO
  byte = PEEK(DataOff + curPtr)
  IF byte = 255 THEN
    FOR e& = 1 TO 255
      IF byte <> PEEK(DataOff + curPtr + e&) OR curPtr + e& > NumBytes THEN EXIT FOR
    NEXT e&
    curPtr = curPtr + e&
    rep = e&
    PutByte 255: PutByte rep: PutByte byte
  ELSE
    IF byte = PEEK(DataOff + curPtr + 1) THEN
      FOR e& = 1 TO 254
        IF byte <> PEEK(DataOff + curPtr + e&) OR curPtr + e& > NumBytes THEN EXIT FOR
      NEXT e&
      curPtr = curPtr + e&
      rep = e&
      PutByte 255: PutByte rep: PutByte byte
    ELSE
      PutByte byte
      curPtr = curPtr + 1
    END IF
  END IF
  IF Pack$ <> "" THEN
    LOCATE CSRLIN, 1: PRINT "Encoding " + LTRIM$(STR$(pNum + 1)) + "/" + LTRIM$(STR$(NumPackets + 1)) + ": ";
    SELECT CASE pType
    CASE 1: PRINT "font";
    CASE 2: PRINT "image";
    CASE 3: PRINT "sound";
    CASE 4: PRINT "palette";
    CASE 5: PRINT "blender map";
    CASE 6: PRINT "mouse cursor";
    CASE 7: PRINT "user data";
    END SELECT
    PRINT " from " + Pack$ + " as " + CHR$(34) + p$ + CHR$(34) + " -" + STR$(INT((curPtr / NumBytes) * 100)) + "% done";
  END IF
LOOP WHILE curPtr < NumBytes
IF Pack$ <> "" THEN
  LOCATE CSRLIN, 1: PRINT "Encoding " + LTRIM$(STR$(pNum + 1)) + "/" + LTRIM$(STR$(NumPackets + 1)) + ": ";
  SELECT CASE pType
  CASE 1: PRINT "font";
  CASE 2: PRINT "image";
  CASE 3: PRINT "sound";
  CASE 4: PRINT "palette";
  CASE 5: PRINT "blender map";
  CASE 6: PRINT "mouse cursor";
  CASE 7: PRINT "user data";
  END SELECT
  PRINT " from " + Pack$ + " as " + CHR$(34) + p$ + CHR$(34) + " - done, ";
END IF

END SUB

FUNCTION FixFilename$ (f$)
'
file$ = f$
WHILE INSTR(file$, "\") > 0
  file$ = MID$(file$, INSTR(file$, "\") + 1)
WEND
FixFilename$ = file$

END FUNCTION

SUB Init
'
PRINT "DirectQB Datafile Encoder version " + VERSION$
PRINT "by Angelo Mottola - Enhanced Creations 1998-99"
PRINT
IF DQBsetBaseLayer(B0) = 0 THEN DQBclose: PRINT "Error: " + DQBerror$: END
InfoFile = COMMAND$
IF InfoFile = "" THEN Usage
IF INSTR(InfoFile, ".") = 0 THEN InfoFile = InfoFile + ".NFO"
IF LEN(InfoFile) > 12 THEN
  PRINT "Bad filename": DQBclose: END
END IF
OPEN InfoFile FOR BINARY AS #1
fl& = LOF(1): CLOSE #1
IF fl& = 0 THEN KILL InfoFile: PRINT "InfoFile not found": DQBclose: END

END SUB

SUB PutByte (code)
'
IF code <> -1 THEN
  enc = code + ASC(MID$(Password, PasswordPtr, 1))
  IF enc > 255 THEN enc = enc - 256
  PasswordPtr = PasswordPtr + 1: IF PasswordPtr >= LEN(Password) THEN PasswordPtr = 1
  c$ = CHR$(enc)
 
'  PUT #1, , c$
 
  MID$(WriteBuf, WritePtr, 1) = c$
  WritePtr = WritePtr + 1: PacketBytes = PacketBytes + 1
  IF WritePtr > BUFLEN THEN PUT #1, , WriteBuf: WritePtr = 1
ELSE
  IF WritePtr = 1 THEN EXIT SUB
  c$ = LEFT$(WriteBuf, WritePtr - 1)
  PUT #1, , c$
END IF
END SUB

SUB ShowError (errorline, errormsg$)
'
CLOSE
DQBclose
PRINT UCASE$(InfoFile) + " (Line" + STR$(errorline) + "): " + errormsg$
END
END SUB

SUB Usage
'
PRINT "Usage: DQBENC <InfoFile>"
PRINT "where " + CHR$(34) + "InfoFile" + CHR$(34) + " is a plain text file (" + CHR$(34) + ".NFO" + CHR$(34) + " extension assumed by default)"
PRINT "holding the right informations on how to build the datafile."
PRINT "- You can add comment lines by entering a " + CHR$(34) + "#" + CHR$(34) + " as the first character of them"
PRINT "- All words and symbols are not case-sensitive"
PRINT "- Spaces between words are not required"
PRINT "InfoFile file structure:"
PRINT "� Required fields:"
PRINT "  OUTFILE = <OutputFileName>"
PRINT "     Sets the name of the output file; " + CHR$(34) + ".BIN" + CHR$(34) + " extension assumed by default"
PRINT "  INCLUDEFILE = <IncludeFileName>"
PRINT "     Sets the name of the QB include file; " + CHR$(34) + ".BI" + CHR$(34) + " extension assumed by default"
PRINT "  At least one packet information (see below)"
PRINT "� Optional fields:"
PRINT "  PASSWORD = <Password>"
PRINT "     " + CHR$(34) + "Password" + CHR$(34) + " must be a string of up to 8 characters"
PRINT "� Packet information structure:"
PRINT "  <PacketType> : <PacketID> = <FilePath>"
PRINT "   " + CHR$(34) + "PacketType" + CHR$(34) + " can be FONT, IMAGE, SOUND, PALETTE, CURSOR, BMAP or USER"
PRINT "   " + CHR$(34) + "PacketID" + CHR$(34) + " is an unique ID string of up to 8 characters"
PRINT "   " + CHR$(34) + "FilePath" + CHR$(34) + " is the full source file path and extension"
PRINT "Creates a datafile and a correspondent include file with packet ID references;"
PRINT "$INCLUDE this file in your programs and use constants to refer to your packets."
DQBclose
END
END SUB

