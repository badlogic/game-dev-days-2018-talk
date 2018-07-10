.model medium, basic
.386
.stack 100h
.code

; Copies length bytes from the source to the destination.
public copymem
copymem proc sourceSeg:word, sourceOff:word, destSeg:word, destOff:word, len: word
   push ds
   push si

   mov si, sourceOff
   mov ax, sourceSeg
   mov ds, ax

   mov di, destOff
   mov ax, destSeg
   mov es, ax

   mov cx, len
   rep movsw

   pop si
   pop ds
   ret
copymem endp

; Translates the array of quads by the x/y/z
Public translatePolygons
translatePolygons Proc polysInSeg:word, polysInOff:word, polysOutSeg:word, polysOutOff:word, x:word, y:word, z:word, numPolies: word
    push ds
    push si

    mov ds, polysInSeg
    mov si, polysInOff
    mov es, polysOutSeg
    mov di, polysOutOff

    mov cx, numPolies
loop_polys:
    ; x1, x2, x3, x4
    mov dx, x
    mov ax, ds:[si]
    add ax, dx
    mov es:[di], ax

    mov ax, ds:[si + 2]
    add ax, dx
    mov es:[di + 2], ax

    mov ax, ds:[si + 4]
    add ax, dx
    mov es:[di + 4], ax

    mov ax, ds:[si + 6]
    add ax, dx
    mov es:[di + 6], ax

    ; y1, y2, y3, y4
    mov dx, y
    mov ax, ds:[si + 8]
    add ax, dx
    mov es:[di + 8], ax

    mov ax, ds:[si + 10]
    add ax, dx
    mov es:[di + 10], ax

    mov ax, ds:[si + 12]
    add ax, dx
    mov es:[di + 12], ax

    mov ax, ds:[si + 14]
    add ax, dx
    mov es:[di + 14], ax

    ; z1, z2, z3, z4
    mov dx, z
    mov ax, ds:[si + 16]
    add ax, dx
    mov es:[di + 16], ax

    mov ax, ds:[si + 18]
    add ax, dx
    mov es:[di + 18], ax

    mov ax, ds:[si + 20]
    add ax, dx
    mov es:[di + 20], ax

    mov ax, ds:[si + 22]
    add ax, dx
    mov es:[di + 22], ax

    ; next polygon is 36 bytes away
    mov ax, 28
    add di, ax
    add si, ax

    dec cx
    cmp cx, 0
    jne loop_polys

    pop si
    pop ds
    ret
translatePolygons endp

Public rotatePolygons
rotatePolygons proc polysSeg:word, polysOff:word, cosine:word, sine:word, numPolies: word

    push ds
    push si

    mov ds, polysSeg
    mov si, polysOff

    mov cx, numPolies

loop_polys:
    push cx

    movsx eax, cosine
    movsx ebx, sine

    ; x1, z1
    movsx ecx, ds:[si]
    movsx edx, ds:[si + 16]

    ; x1' = z1 * cos
    mov edi, edx
    imul edi, eax
    push edi

    ; x1' = (z1 * cos - x1 * sin) / 256
    mov edi, ecx
    imul edi, ebx
    pop edx
    sub edx, edi
    sar edx, 8
    mov ds:[si], dx

    ; next polygon is 36 bytes away
    mov ax, 28
    add si, ax

    pop cx
    dec cx
    cmp cx, 0
    jne loop_polys

    pop si
    pop ds
    ret
rotatePolygons endp

end
