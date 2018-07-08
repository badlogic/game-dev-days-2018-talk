.model medium, basic
.386
.stack 100h
.code


Public CopyMem
CopyMem Proc SourceSEG:word, SourceOFF:dword, DestSEG:word, DestOFF:word, leng: word
   push ds
   push si

   mov esi, SourceOFF
   mov ax, SourceSEG
   mov ds, ax

   mov di, DestOFF
   mov ax, DestSEG
   mov es, ax

   mov cx, leng
   rep movsw

   pop si
   pop ds
   ret
CopyMem endp
END
