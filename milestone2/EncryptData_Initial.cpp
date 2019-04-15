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

		mov esi, gptrPasswordHash	//gPtrPasswordHash
		xor eax, eax		//set eax to 0
		mov ah, [esi]		//gPasswordHas[0] is in ah
		mov al, [esi + 1]	//gPasswordHash[1] is in al


		mov edi, gptrKey	//store the gptrKey into edi register

		xor ebx, ebx		//clear the ebx register
		xor ecx, ecx		//clear the ecx register

		mov ecx, dataLength	//store the dataLength into the counter register
		xor edx, edx		//clear the edx register
		cmp ecx, 0		//check to see if counter equals 0
		sub ecx, 1		//decrement the counter 
		jbe LENGTH_ZERO		//jump to the LENGTH_ZERO function
		mov esi, data		//store data into esi register
		//mov dl, [edi + eax]	//store new value into dl register
	START_OF_LOOP :
			mov dl, [esi + ebx]	//store new value into al register
			xor dl, [edi+eax]	//
			push edx
			call Dstep		//call Dstep
			call Estep		//call Estep
			call Bstep		//call Bstep
			call Astep		//call Astep
			call Cstep		//call Cstep
			pop edx
			mov[esi + ebx], dl	//store dl into esi register index by ebx
			
			add ebx, 1		//increment ebx
			cmp ebx, ecx		//compare counter to ebx
			ja  END			//if equal, jump to END
			jmp START_OF_LOOP	//else jump to START_OF_LOOP
Dstep :
			push ebp
			mov ebp, esp
			push eax
			xor eax, eax
			mov al, dl
			ror al, 3
			mov dl, al
			mov byte ptr [ebp+8], al
			pop eax
			mov esp, ebp
			pop ebp
			ret
Estep :
			push ebp
			mov ebp, esp
			push eax
			push ebx
			//push edi
			xor ebx, ebx
			xor eax, eax
			lea ebx, gEncodeTable
			mov al, dl
			mov ah, [ebx + eax]	//
			mov dl, ah
			mov byte ptr[ebp + 8], dl
			pop ebx
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
			mov byte ptr[ebp + 8], al
			pop eax
			mov esp, ebp
			pop ebp
			ret					//return to previous position
Astep :
			push ebp
			mov ebp, esp
			push eax
			push ebx
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
			pop ebx
			pop eax
			mov esp, ebp
			pop ebp
			ret			//return to previous position
Cstep:
			push ebp
			mov ebp, esp
			push eax
			push ebx
			push ecx
			xor ecx, ecx
			xor eax, eax
			xor edx, edx
			xor ebx, ebx
			mov al, byte ptr[ebp+8]	//move esi into eax
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
			sub ebx, 1 //decrement ebx by 1
			jmp EXIT	//jump to exit
END :
		xor ebx, ebx	//clear ebx register
EXIT :
		mov resulti, ebx   //store ebx into resulti
	}
	return resulti;	//return data
} // encryptData
