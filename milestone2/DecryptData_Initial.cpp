// DecryptData.cpp
//
// THis file uses the input data and key information to decrypt the input data
//

#include "Main.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// code to decrypt the data as specified by the project assignment
int decryptData(char *data, int dataLength)
{
	int resulti = 0;

	gdebug1 = 0;					// a couple of global variables that could be used for debugging
	gdebug2 = 0;					// also can have a breakpoint in C code

	// You can not declare any local variables in C, but should use resulti to indicate any errors
	// Set up the stack frame and assign variables in assembly if you need to do so
	// access the parameters BEFORE setting up your own stack frame
	// Also, you cannot use a lot of global variables - work with registers

	__asm {
		mov esi, gptrPasswordHash
		xor eax, eax
		mov al, byte ptr[esi]
		shl ax, 8
		xor ecx, ecx
		mov cl, byte ptr[esi+1]
		//mov ah, [esi]
		//mov al, [esi + 1]
		add ax, cx

		mov edi, gptrKey
		xor ebx, ebx
		xor ecx, ecx
		xor edx, edx
		mov ecx, dataLength

		cmp ecx, 0
		sub ecx, 1
		mov esi, data		//store data into esi register
		jbe LENGTH_ZERO
START_OF_LOOP:
			mov dl, [esi + ebx]
			push edx
			call Cstep		//call Dstep
			call Astep		//call Estep
			call Bstep		//call Bstep
			call Estep		//call Astep
			call Dstep		//call Cstep
			
			xor dl, [edi+eax]//syarting index keyfile
			mov [esi+ebx], dl
			pop edx
			add ebx, 1
			cmp ebx, ecx
			ja  END
			jmp START_OF_LOOP
Dstep :
			push ebp
			mov ebp, esp
			push eax
			xor eax, eax
			mov al, dl
			rol al, 3
			mov dl, al
			mov byte ptr[ebp + 8], al
			pop eax
			mov esp, ebp
			pop ebp
			ret
Estep :
			push ebp
			mov ebp, esp
			push eax
			push ecx
			xor eax, eax
			xor ecx, ebx
			lea ecx, gDecodeTable
			mov al, dl
			mov dl, [ecx + eax]	//
			mov byte ptr[ebp + 8], bl
			pop ecx
			pop eax
			mov esp, ebp
			pop ebp
			ret
Bstep :
			push ebp
			mov ebp, esp
			push eax
			xor eax, eax
			mov al, dl	//store hex value into al
			xor al, 01001001b   //invert bits 0,3,6
			mov dl, al	//insert new value back into place 
			mov byte ptr[ebp + 8], dl
			pop eax
			mov esp, ebp
			pop ebp
			ret					//return to previous position
Astep :
			push ebp
			mov ebp, esp
			push eax
			push ecx
			mov eax, 0
			mov ecx, 0		//clear ecx
			mov al, byte ptr[ebp + 8]	//move & zero extend value at ebp+8 into eax
			jmp reverse_bit			//jump to LoopBit function
	reverse_bit :
			shl al, 1		//shift al left by 1
			rcr cl, 1		//rotate and carry cl by 1
			cmp ah, 7
			je reverse_comp     // if counter is 7 end the loop
			inc ah     // else increment count
			jmp reverse_bit    // and keep looping
	reverse_comp :
			mov dl, cl	//move cl into ebp+8
			mov byte ptr[ebp + 8], cl
			pop ecx
			pop eax
			mov esp, ebp
			pop ebp
			ret			//return to previous position
Cstep :
			push ebp
			mov ebp, esp
			push eax
			push ebx
			push ecx
			xor ecx, ecx
			xor eax, eax
			xor edx, edx
			xor ebx, ebx
			mov eax, [ebp + 8]		//move esi into eax
			jmp Looper1		//jump to Looper1 function
	Looper1 :
			shr eax, 1	//shift eax right by 1
			rcr ecx, 1	//rotate right and carry ecx by 1
			add bl, 1	//increment ebx by 1
			cmp bl, 1	//compare ebx to 1
			ja MOVE		//if true jump to MOVE
			jmp Looper1	//else jump to Looper1
	MOVE :
			mov edx, ecx	//move ecx into edx
			mov bl, 0
			mov ecx, 0	//store 0 into ecx
			jmp SHIFT_OTHER	//jump to SHIFT_OTHER function
	SHIFT_OTHER :
			shr eax, 1	//shift eax to the right 1
			rcr ecx, 1	//rotate right and carry edi by 1
			add bl, 1	//increment edi by 1
			cmp bl, 1	//compare edi to 1
			ja OVER		//if true, jump to OVER function
			jmp SHIFT_OTHER	//else jump to SHIFT_OTHER
	OVER :
			shr ecx, 2	//shift ecx to the right by 2
			add ecx, edx	//increment ecx by edx
			xchg edx, ecx	//xchg the register values
			mov eax, [ebp + 8]	//store esi into eax
			mov bl, 0
			mov ecx, 0	//store 0 in ecx
			shr eax, 4	//shift eax right by 4
			jmp Loop2	//jump to Loop2 function
	Loop2 :
			shr eax, 1	//shift eax to the right by 1
			rcr ecx, 1	//rotate right and carry ecx by 1
			add bl, 1	//increment edi by 1
			cmp bl, 1	//compare edi to 1
			ja EXIT2	//if true jump to EXIT2
			jmp Loop2	//else jump to Loop2
	EXIT2 :
			mov ebx, ecx	//move ecx into ebx
			mov ecx, 0	//set ecx to 0
			mov bl, 0	//set edi to 0
			jmp Loop3	//jump to Loop3
	Loop3 :
			shr eax, 1	//shift eax to the right by 1+
			rcr ecx, 1	//rotate right and carry ecx by 1
			add bl, 1	//increment edi by 1
			cmp bl, 1	//compare edi to 1
			ja END3		//jump above END3
			jmp Loop3	//else jump to Loop3
	END3 :
			ror ecx, 2	//rotate ecx right by 2
			mov bl, 0
			add ecx, ebx	//increment ecx by ebx
			shr edx, 16	//shift edx to the right by 16
			shr edx, 12	//shift edx to the right by 12
			shr ecx, 16	//shift ecx to the right by 16
			shr ecx, 8	//shift ecx to the right by 8
			add ecx, edx	//increment ecx by edx
			mov dl, cl	//move al into ebp+
			mov byte ptr[ebp + 8], cl
			pop ecx
			pop ebx
			pop eax
			mov esp, ebp
			pop ebp
			ret
LENGTH_ZERO :
			sub ebx, 1
			jmp EXIT

END :
		xor ebx, ebx
EXIT :
		mov resulti, ebx
	}
	return resulti;
} // decryptData
