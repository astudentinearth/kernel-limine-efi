
global outl
global inl

outl:
    mov dx, di
    mov eax, esi
    out dx, eax
    ret


inl:
    mov dx, di
    in eax, dx
    ret
