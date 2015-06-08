#include "weaponid.h"
#include "iddrawsurface.h"
#include <stdio.h>

char WeaponArray[4432000+0x20000];
char UnitArray[0x2846A*10];

int UPTR = (int)UnitArray;
int UPTR2 = (int)&UPTR;

int PTR = (int)WeaponArray;
int PTR2 = (int)&PTR;

void Func1();

void IncreaseWeaponID()
{
  //weaponarray, 0x115 bytes for each element 0x115*16000 = 4432000 bytes

	memset(WeaponArray, 0x0, 4432000);
   memset(UnitArray, 0x0, 0x2846A*10);

  char nop[10];
  memset(nop, 0x90, 10);

  /*
  //uvervrites the unit array
  int Adress = 0x511de8;
  int BasePTR;
  ReadProcessMemory(GetCurrentProcess(), (void*)Adress, (void*)&BasePTR, sizeof(int), NULL);
  //ReadProcessMemory(TAProc, (void*)(BasePTR+0x1b8e+0x3c), (void*)&UnitPTR, sizeof(int), NULL);
  WriteProcessMemory(GetCurrentProcess(), (void*)(BasePTR+0x1439B), &UPTR, 4, NULL);
  WriteProcessMemory(GetCurrentProcess(), (void*)0x0042AA11, nop, 6, NULL);
  WriteProcessMemory(GetCurrentProcess(), (void*)0x0042AA98, nop, 6, NULL);
  */


  //wep
  WriteProcessMemory(GetCurrentProcess(), (void*)0x0049E5CC, &PTR2, 4, NULL); ///
  WriteProcessMemory(GetCurrentProcess(), (void*)0x0042CDCE, &PTR2, 4, NULL); ///
  WriteProcessMemory(GetCurrentProcess(), (void*)0x0042E31E, &PTR2, 4, NULL); ///
  WriteProcessMemory(GetCurrentProcess(), (void*)0x0042E46A, &PTR2, 4, NULL); ///

  WriteProcessMemory(GetCurrentProcess(), (void*)0x0042F3AC, &PTR2, 4, NULL); //
  WriteProcessMemory(GetCurrentProcess(), (void*)0x00437CF9, &PTR2, 4, NULL); //
  WriteProcessMemory(GetCurrentProcess(), (void*)0x00437D15, &PTR2, 4, NULL); //
  WriteProcessMemory(GetCurrentProcess(), (void*)0x00455476, &PTR2, 4, NULL); //

  WriteProcessMemory(GetCurrentProcess(), (void*)0x0042ED48, &PTR2, 4, NULL);
  WriteProcessMemory(GetCurrentProcess(), (void*)0x0042ED70, &PTR2, 4, NULL);
  WriteProcessMemory(GetCurrentProcess(), (void*)0x0042F360, &PTR2, 4, NULL);
  WriteProcessMemory(GetCurrentProcess(), (void*)0x0042F378, &PTR2, 4, NULL);

  int onk = 0x747589;
  WriteProcessMemory(GetCurrentProcess(), (void*)0x0042ED67, &onk, 4, NULL);
  onk = 0x90909090;
  WriteProcessMemory(GetCurrentProcess(), (void*)0x0042ED6A, &onk, 4, NULL);


  //in recieve 0xd packet function
  //jump to unused space (short jump)
  onk = 0xceeb;
  WriteProcessMemory(GetCurrentProcess(), (void*)0x0049D291, &onk, 2, NULL);
  //move edi,PTR2
  onk = 0xbf;
  WriteProcessMemory(GetCurrentProcess(), (void*)0x0049D261, &onk, 1, NULL);
  WriteProcessMemory(GetCurrentProcess(), (void*)0x0049D262, &PTR2, 4, NULL);
  //return (jmp 0x49D293)
  onk = 0x2beb;
  WriteProcessMemory(GetCurrentProcess(), (void*)0x0049D266, &onk, 2, NULL);

  onk = 0xBB;
  WriteProcessMemory(GetCurrentProcess(), (void*)0x0049D3D8, &onk, 1, NULL);
  onk = (int)Func1;
  WriteProcessMemory(GetCurrentProcess(), (void*)0x0049D3D9, &onk, 4, NULL);
  onk = 0xe3ff;
  WriteProcessMemory(GetCurrentProcess(), (void*)0x0049D3DD, &onk, 2, NULL);
  onk = 0x90909090;
  WriteProcessMemory(GetCurrentProcess(), (void*)0x0049D3DF, &onk, 2, NULL);


  int Count = 0x43A080;
  WriteProcessMemory(GetCurrentProcess(), (void*)0x0042E33F, &Count, 4, NULL);
  WriteProcessMemory(GetCurrentProcess(), (void*)0x0042F433, &Count, 4, NULL);
  WriteProcessMemory(GetCurrentProcess(), (void*)0x0049E5ED, &Count, 4, NULL);


  //more units
  //WriteProcessMemory(GetCurrentProcess(), (void*)0x0042F3F4, nop, 5, NULL);  //nops a memory cleanup call
  char alocunitarray[14] = {0x89, 0x9A, 0x8F, 0x43, 0x01, 0x00, 0x90, 0x90, 0x90, 0xBD, 0x24, 0x2C, 0x19, 0x00};
  //WriteProcessMemory(GetCurrentProcess(), (void*)0x0042AA72, alocunitarray, 14, NULL);  //change instruction to create larger unit array

/*  FILE *file;
  file = fopen("C:\\temp\\taddrawlog.txt", "a");

  char Mo[10];

  itoa(*((int*)0x0049E5CC), Mo, 10);
  fwrite(Mo, strlen(Mo), 1, file);
  fwrite("\n", 1, 1, file);

  itoa((int)PTR+0x2cec+0x115*1234, Mo, 10);
  fwrite(Mo, strlen(Mo), 1, file);
  fwrite("\n", 1, 1, file);

  itoa(*(((int*)0x00511DE8+0x2CF3)), Mo, 10);
  fwrite(Mo, strlen(Mo), 1, file);
  fwrite("\n", 1, 1, file);

  fclose(file);*/
}

void Func1()
{
	__asm{
   //push ebx;
   mov ebx, PTR2;
   mov dl, [ebx+0x2A42];
   pop ebx;
   cmp     [ecx+0x3A], dl;
   mov ebx, 0x49D3E1;
   jmp ebx;
   }
}
