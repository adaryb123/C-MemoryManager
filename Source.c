#include <stdio.h>


typedef struct hlavicka {					//hlavicka je na zaciatku volnej pamate, obsahuje smernik na prvy volny blok, a smernik na koniec pamate
	struct block* first_free;
	void* end;
}hlavicka;

typedef struct block {
	unsigned int size;				//ak je blok volny, pripocitame k size 1 (inak je size vzdy parne)
	struct block* next;
}block;

hlavicka* start = NULL;				//tento smernik bude vzdy ukazovat na prvy volny blok

void memory_init(void* ptr, unsigned int size) {
	start = ptr;
	start->first_free = start + 1;
	start->first_free->size = size - sizeof(hlavicka) - sizeof(int) + 1;
	start->first_free->next = NULL;
	start->end = (char*) start + size;
}

void* memory_alloc(unsigned int required_size) {					
	block* aktualny = start->first_free;
	//printf("pustila sa funkcia memory alloc a hlada miesto pre blok velkosti %d\n", required_size);
	if (required_size % 8 != 0){
		required_size += (8 - required_size % 8);
	}
	while (aktualny != NULL) {
		if (aktualny->size % 2 == 1) {						
			if ((aktualny->size - 1 > required_size + sizeof(int)) && ((char*)aktualny + sizeof(int) + required_size + sizeof(block) < start->end)) {
				/*nasli sme blok dost velky na to aby sme ho mohli rozdelit a zobrat si iba cast z neho*/
				//printf("viac miesta ako treba  (%d), rozdeli sa\n\n\n", aktualny->size);

				struct block* novy;
				novy = (char*)aktualny + required_size + sizeof(int);
				novy->size = aktualny->size - required_size - sizeof(int);
				aktualny->size = required_size;
				novy->next = aktualny->next;
			
				if (start->first_free == aktualny) {
					start->first_free = novy;
				}
				return (char*)aktualny + sizeof(int);
			}
			else if (aktualny->size - 1 >= required_size) {					
				/*nasli sme blok ktory obsadime cely*/
				//printf("presne tolko miesta co treba\n\n\n");	
				aktualny->size = aktualny->size - 1;
				if (start->first_free == aktualny) {
					start->first_free = aktualny->next;
				}
				aktualny->next = NULL;
				return (char*)aktualny + sizeof(int);
			}
			else {
				//printf("nasiel sa blok velkosti %d, co je prilis malo\n", aktualny->size - 1);
			}
		}
		aktualny = aktualny->next;
	}
	//printf("nie je miesto\n");
	return NULL;
}

int memory_check(void* pointer) {
	//printf("spusta sa funkcia memory check\n");
	if (pointer <= start || pointer > start->end) {
		//printf("smernik nie je z vyhradeneho pola pamate\n");
		return 0;
	}
	//printf("smernik je ok\n");
	return 1;
}

int memory_free(void* pointer) {			
	//printf("pustila sa funkcia memory free\n");
	if (memory_check(pointer) == 0) {
		return 1;
	}
	//printf("pustila sa funkcia memory free\n");
	struct block* novy, * aktualny;
	novy = (char*)pointer - sizeof(int);
	novy->size += 1;
	/* ulozime uvolneny blok do zoznamu podla adresy */
	if (novy < start->first_free || start->first_free == NULL) {
		novy->next = start->first_free;
		start->first_free = novy;
	}
	else {
		aktualny = start->first_free;
		while (aktualny->next != NULL) {
			if (novy > aktualny && novy < aktualny->next) {
				novy->next = aktualny->next;
				aktualny->next = novy;
				break;
			}
			aktualny = aktualny->next;
		}
	}
	/*tato cast spaja volne bloky co su vedla seba*/
	aktualny = start->first_free;
	while (aktualny->next) {
		//printf("%d\n",aktualny->next);
		if (aktualny->next == (char*)aktualny + aktualny->size + sizeof(int) - 1) {
			//printf("merge\n");
			aktualny->size += aktualny->next->size + sizeof(int) - 1;
			aktualny->next = aktualny->next->next;
		}
		else {
			aktualny = aktualny->next;
			//printf("e\n");
		}
	}
	pointer = NULL;
	return 0;
}
/*
int main()
{
	int N = 100;
	char region[100];
	memory_init(region, N);

	for (int i = 0; i < N; i++) {
		printf("%d\n", region[i]);
	}
	//memory_check(start);

	int* pointer1 = (int*)memory_alloc(3*sizeof(int));
	for (int i = 0; i < 3; i++) {
		pointer1[i] = 99;
	}
	
	char* pointer2 = (char*)memory_alloc(22);
	memset(pointer2, 77, 22);

	for (int i = 0; i < N; i++) {
		printf("%d\n", region[i]);
	}
	memory_free(pointer1);

	for (int i = 0; i < N; i++) {
		printf("%d\n", region[i]);
	}

	memory_free(pointer2);
	memory_free(pointer2);

	for (int i = 0; i < N; i++) {
		printf("%d\n", region[i]);
	}

	char* pointer3 = (char*)memory_alloc(32);
	memset(pointer3, 98, 32);

	for (int i = 0; i < N; i++) {
		printf("%d\n", region[i]);
	}

	memory_free(pointer2);

	for (int i = 0; i < N; i++) {
		printf("%d\n", region[i]);
	}
	

	/*
	char* pointer = (char*)memory_alloc(10);
	if (pointer)
		memset(pointer, 0, 10);
	if (pointer)
		memory_free(pointer);*/
	//return 0;
//}*/
