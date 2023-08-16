#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include "image_processing.cpp"

using namespace std;

void Dilation(int n, int filter_size, short* resimadres_org);
void Erosion(int n, int filter_size, short* resimadres_org);

int main(void) {
	int M, N, Q, i, j, filter_size;
	bool type;
	int efile;
	char org_resim[100], dil_resim[] = "dilated.pgm", ero_resim[] = "eroded.pgm";
	do {
		printf("Orijinal resmin yolunu (path) giriniz:\n-> ");
		scanf("%s", &org_resim);
		system("CLS");
		efile = readImageHeader(org_resim, N, M, Q, type);
	} while (efile > 1);
	int** resim_org = resimOku(org_resim);

	printf("Orjinal Resim Yolu: \t\t\t%s\n", org_resim);

	short *resimdizi_org = (short*) malloc(N*M * sizeof(short));

	for (i = 0; i < N; i++)
		for (j = 0; j < M; j++)
			resimdizi_org[i*N + j] = (short)resim_org[i][j];

	int menu;
	printf("Yapmak istediginiz islemi giriniz...\n");
	printf("1-) Dilation\n");
	printf("2-) Erosion\n");
	printf("3-) Cikis\n> ");
	scanf("%d", &menu);
	printf("Filtre boyutunu giriniz: ");
	scanf("%d", &filter_size);

	switch (menu){
		case 1:
			Dilation(N*M, filter_size, resimdizi_org);
			resimYaz(dil_resim, resimdizi_org, N, M, Q);
			break;
		case 2:
			Erosion(N*M, filter_size, resimdizi_org);
			resimYaz(ero_resim, resimdizi_org, N, M, Q);
			break;
		case 3:
			system("EXIT");
			break;
		default:
			system("EXIT");
			break;
	}

	system("PAUSE");
	return 0;
}

void Dilation(int n, int filter_size, short* resim_org) {
	__asm {
		xor di, di
		mov eax, dword ptr[filter_size]
		dec eax
		shr eax, 1
		mov ecx, eax
		// LOOP1 y ekseni
		L1 :
		// Görsel kenar boyutunu al
		cvtsi2sd  xmm0, dword ptr[n]
			sqrtsd    xmm0, xmm0
			cvtsd2si  edx, xmm0
			mov eax, dword ptr[filter_size]
			dec eax
			shr eax, 1
			sub edx, eax
			cmp ecx, edx
			ja L1DONE

			mov eax, dword ptr[filter_size]
			dec eax
			shr eax, 1
			// LOOP2 x ekseni
			L2:
		// görsel kenar boyutunu al
		cvtsi2sd  xmm0, dword ptr[n]
			sqrtsd    xmm0, xmm0
			cvtsd2si  edx, xmm0
			push eax
			mov eax, dword ptr[filter_size]
			dec eax
			shr eax, 1
			sub edx, eax
			pop eax
			cmp eax, edx
			ja L2DONE

			// kernelin baþlangýcýný bulur
			push eax
			mov eax, dword ptr[filter_size]
			dec eax
			shr eax, 1

			push edx
			push ecx

			xor edx, edx
			mov ecx, -1
			mul ecx

			pop ecx
			pop edx

			mov ebx, eax

			pop eax

			mov si, 0
			// LOOP 3
			// EBX'te index saklanýr
			// kernelin bitiþini bulur
		L3: push eax

			mov eax, dword ptr[filter_size]
			dec eax
			shr eax, 1

			cmp ebx, eax

			pop eax

			jg L3DONE

			// kernelin baþlangýcýný bulur
			push eax
			mov eax, dword ptr[filter_size]
			dec eax
			shr eax, 1

			push edx
			push ecx

			xor edx, edx
			mov ecx, -1
			mul ecx

			pop ecx
			pop edx

			mov edx, eax

			pop eax

			// LOOP4 
			// EDX'te index saklanýr
			// kernelin bitiþini bulur
		L4: push eax

			mov eax, dword ptr[filter_size]
			dec eax
			shr eax, 1

			cmp edx, eax

			pop eax

			jg L4DONE

			// Karþýlaþtýrma yapar maximumu tutar
			push ebx
			push eax


			// Görselin kenarýný bulur
			cvtsi2sd  xmm0, dword ptr[n]
			sqrtsd    xmm0, xmm0
			cvtsd2si  eax, xmm0

			push edx
			xor edx, edx
			push ecx
			add ecx, ebx
			mul ecx
			pop ecx
			mov ebx, eax	// x eksenindeki konumu
			pop edx
			pop eax

			push edx
			push eax
			add eax, ebx	// x ve y konumlarý toplandý
			add eax, edx
			xor edx, edx
			push ecx
			mov ecx, 2
			mul ecx			// short olduðunda 2 ile çarpýldý
			pop ecx

			mov ebx, resim_org
			add ebx, eax	// resimin bulunduðu adrese konum verisi eklendi

			mov ax, word ptr[ebx]

			// veri bir önceki max ile karþýlaþtýrýldý
			cmp si, ax
			jae ATLA
			mov si, ax
			ATLA :

		pop eax
			pop edx

			pop ebx


			inc edx
			jmp L4
			L4DONE :


		inc ebx
		jmp L3
		L3DONE :

		// Elde edilen veriyi stacke yükle
		push si

		inc eax
		jmp L2
		L2DONE :


		inc ecx
		jmp L1
		L1DONE :

		
		// Resim_org deðiþkenin bitiþi ebxe aktarýlýr
		mov ebx, resim_org
		mov eax, dword ptr[n]
		shl eax, 1
		add ebx, eax


		// Veriyi deðiþkene aktar
		// Y eksen döngüsü
		xor eax, eax
		Y1 :
		cvtsi2sd  xmm0, dword ptr[n]
		sqrtsd    xmm0, xmm0
		cvtsd2si  ecx, xmm0
		push eax
		mov eax, dword ptr[filter_size]
		dec eax
		sub ecx, eax
		pop eax
		cmp eax, ecx
		ja Y1SON

		xor ecx, ecx
		// X eksen döngüsü
		X1 :
		push edx
		cvtsi2sd  xmm0, dword ptr[n]
		sqrtsd    xmm0, xmm0
		cvtsd2si  edx, xmm0
		push eax
		mov eax, dword ptr[filter_size]
		dec eax
		sub edx, eax
		pop eax
		cmp ecx, edx
		pop edx

		ja X1SON

		pop dx
		MOV WORD PTR[EBX], dx
		push eax
		mov ax, WORD PTR[EBX]
		pop eax

		sub EBX, 2

		inc ecx
		JMP X1
		X1SON :
		push eax
		mov eax, dword ptr[filter_size]
		sub eax, 2
		shl eax, 1
		sub EBX, eax
		pop eax

		inc eax
		JMP Y1

		Y1SON :
	}
	printf("\nDilation islemi sonucunda resim \"dilated.pgm\" ismiyle olusturuldu...\n");
}

void Erosion(int n, int filter_size, short* resim_org) {
	short* yeni_resim = (short*)malloc(sizeof(short) * n);
	__asm {
		xor di, di
		mov eax, dword ptr[filter_size]
		dec eax
		shr eax, 1
		mov ecx, eax
		// LOOP1 y ekseni
		L1 :
		// Görsel kenar boyutunu al
		cvtsi2sd  xmm0, dword ptr[n]
			sqrtsd    xmm0, xmm0
			cvtsd2si  edx, xmm0
			mov eax, dword ptr[filter_size]
			dec eax
			shr eax, 1
			sub edx, eax
			cmp ecx, edx
			ja L1DONE

			mov eax, dword ptr[filter_size]
			dec eax
			shr eax, 1
			// LOOP2 x ekseni
			L2:
		// görsel kenar boyutunu al
		cvtsi2sd  xmm0, dword ptr[n]
			sqrtsd    xmm0, xmm0
			cvtsd2si  edx, xmm0
			push eax
			mov eax, dword ptr[filter_size]
			dec eax
			shr eax, 1
			sub edx, eax
			pop eax
			cmp eax, edx
			ja L2DONE

			// kernelin baþlangýcýný bulur
			push eax
			mov eax, dword ptr[filter_size]
			dec eax
			shr eax, 1

			push edx
			push ecx

			xor edx, edx
			mov ecx, -1
			mul ecx

			pop ecx
			pop edx

			mov ebx, eax

			pop eax

			mov si, 255
			// LOOP 3
			// EBX'te index saklanýr
			// kernelin bitiþini bulur
		L3: push eax

			mov eax, dword ptr[filter_size]
			dec eax
			shr eax, 1

			cmp ebx, eax

			pop eax

			jg L3DONE

			// kernelin baþlangýcýný bulur
			push eax
			mov eax, dword ptr[filter_size]
			dec eax
			shr eax, 1

			push edx
			push ecx

			xor edx, edx
			mov ecx, -1
			mul ecx

			pop ecx
			pop edx

			mov edx, eax

			pop eax

			// LOOP4 
			// EDX'te index saklanýr
			// kernelin bitiþini bulur
		L4: push eax

			mov eax, dword ptr[filter_size]
			dec eax
			shr eax, 1

			cmp edx, eax

			pop eax

			jg L4DONE

			// Karþýlaþtýrma yapar maximumu tutar
			push ebx
			push eax


			// Görselin kenarýný bulur
			cvtsi2sd  xmm0, dword ptr[n]
			sqrtsd    xmm0, xmm0
			cvtsd2si  eax, xmm0

			push edx
			xor edx, edx
			push ecx
			add ecx, ebx
			mul ecx
			pop ecx
			mov ebx, eax	// x eksenindeki konumu
			pop edx
			pop eax

			push edx
			push eax
			add eax, ebx	// x ve y konumlarý toplandý
			add eax, edx
			xor edx, edx
			push ecx
			mov ecx, 2
			mul ecx			// short olduðunda 2 ile çarpýldý
			pop ecx

			mov ebx, resim_org
			add ebx, eax	// resimin bulunduðu adrese konum verisi eklendi

			mov ax, word ptr[ebx]

			// veri bir önceki min ile karþýlaþtýrýldý
			cmp ax, si
			jae ATLA
			mov si, ax
			ATLA :

		pop eax
			pop edx

			pop ebx


			inc edx
			jmp L4
			L4DONE :


		inc ebx
			jmp L3
			L3DONE :

		// Elde edilen veriyi stacke yükle
		push si

			inc eax
			jmp L2
			L2DONE :


		inc ecx
			jmp L1
			L1DONE :

		// Veriyi deðiþkene aktar
		mov ebx, resim_org
			mov eax, dword ptr[n]
			shl eax, 1
			add ebx, eax

			xor eax, eax
			Y1 :
		cvtsi2sd  xmm0, dword ptr[n]
			sqrtsd    xmm0, xmm0
			cvtsd2si  ecx, xmm0
			push eax
			mov eax, dword ptr[filter_size]
			dec eax
			sub ecx, eax
			pop eax
			cmp eax, ecx
			ja Y1SON

			xor ecx, ecx
			X1 :
		push edx
			cvtsi2sd  xmm0, dword ptr[n]
			sqrtsd    xmm0, xmm0
			cvtsd2si  edx, xmm0
			push eax
			mov eax, dword ptr[filter_size]
			dec eax
			sub edx, eax
			pop eax
			cmp ecx, edx
			pop edx

			ja X1SON

			pop dx
			MOV WORD PTR[EBX], dx
			push eax
			mov ax, WORD PTR[EBX]
			pop eax

			sub EBX, 2

			inc ecx
			JMP X1
			X1SON :
		push eax
			mov eax, dword ptr[filter_size]
			sub eax, 2
			shl eax, 1
			sub EBX, eax
			pop eax

			inc eax
			JMP Y1

			Y1SON :
	}
	printf("\nErosion islemi sonucunda resim \"eroded.pgm\" ismiyle olusturuldu...\n");
}
