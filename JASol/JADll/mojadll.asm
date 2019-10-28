.CODE

DllEntry PROC hInstDLL: DWORD, reason: DWORD, reserved1: DWORD

mov eax, 1
ret

DllEntry ENDP

AsmVal proc
mov eax, 213
ret
AsmVal endp

END

