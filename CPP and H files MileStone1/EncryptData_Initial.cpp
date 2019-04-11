// EncryptData.cpp
//
// This file uses the input data and key information to encrypt the input data
//

#include "Main.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// code to encrypt the data as specified by the project assignment
int encryptData(char *data, int dataLength)
{
	int resulti = 0;

	gdebug1 = 0;					// a couple of global variables that could be used for debugging
	gdebug2 = 0;					// also can have a breakpoint in C code

	// You can not declare any local variables in C, but should use resulti to indicate any errors
	// Set up the stack frame and assign variables in assembly if you need to do so
	// access the parameters BEFORE setting up your own stack frame
	// Also, you cannot use a lot of global variables - work with registers

	__asm {
			lea esi, gPasswordHash
			xor eax, eax
			mov ah, [esi]
			mov al, [esi + 1]

			lea edi, gkey
			mov esi, data
			xor ebx, ebx
			xor edx, edx
			mov dl, [edi + eax]
			xor ecx, ecx
			mov ecx, dataLength
			cmp ecx, 0
			jbe LENGTH_ZERO
			sub ecx, 1

	START_OF_LOOP :
			mov al, [esi + ebx]
			xor al, dl
			mov[esi + ebx], al

			add ebx, 1
			cmp ebx, ecx
			je  END
			jmp START_OF_LOOP

	LENGTH_ZERO :
			mov ebx, -1
			jmp EXIT

	END :
			xor ebx, ebx

	EXIT :
			mov resulti, ebx
	}

	return resulti;
} // encryptData