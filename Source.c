#include <stdio.h>
#include <string.h>

/*
void* memory_alloc(unsigned int size) {
	;
}
int memory_free(void* valid_ptr) {
	;
}
*/

typedef struct hlavicka {					//hlavicka je na zaciatku volnej pamate, obsahuje smernik na prvy volny blok, a smernik na koniec pamate
	struct block* first_free;
	void* end;
}hlavicka;

typedef struct block {
	unsigned int size;				//ak je blok volny, pripocitame k size 1 (inak je size vzdy parne)
	struct block* next;
}block;

hlavicka* start = NULL;				//tento smernik bude vzdy ukazovat na prvy volny blok
/*
int memory_check(block * ptr) {
	printf("\n %d\n", ptr->is_free);
	return 0;
}*/

void memory_init(void* ptr, unsigned int size) {
	start = ptr;
	start->first_free = start + 1;
	start->first_free->size = size - sizeof(hlavicka) - sizeof(block) + 1;
	start->first_free->next = NULL;
	start->end = (char*) start + size;
}

void* memory_alloc(unsigned int required_size) {					
	block* aktualny = start->first_free;
	printf("pustila sa funkcia memory alloc a hlada miesto pre blok velkosti %d\n", required_size);
	if (required_size % 4 == 1) {				//pamat sa vzdy zaokruhli smerom hore na 4
		required_size+= 3;
	}
	else if (required_size % 4 == 2) {				
		required_size += 2;
	}
	else if (required_size % 4 == 3) {				
		required_size += 1;
	}
	while (aktualny != NULL) {
		if (aktualny->size % 2 == 1) {						
			if ((aktualny->size - 1 > required_size + sizeof(block) ) && ((char*)aktualny + required_size + sizeof(block)  < start->end)) {		
				/*nasli sme blok dost velky na to aby sme ho mohli rozdelit a zobrat si iba cast z neho*/
				printf("viac miesta ako treba  (%d), rozdeli sa\n\n\n", aktualny->size);

				struct block* novy;
				novy = (char*)aktualny + required_size + sizeof(block);
				novy->size = aktualny->size - required_size - sizeof(block);
				aktualny->size = required_size;
				novy->next = aktualny->next;
			
				if (start->first_free == aktualny) {
					start->first_free = novy;
				}
				return aktualny + 1;
			}
			else if (aktualny->size - 1 >= required_size) {					
				/*nasli sme blok ktory obsadime cely*/
				printf("presne tolko miesta co treba\n\n\n");	
				aktualny->size = aktualny->size - 1;
				if (start->first_free == aktualny) {
					start->first_free = aktualny->next;
				}
				aktualny->next = NULL;
				return aktualny + 1;
			}
			else {
				printf("nasiel sa blok velkosti %d, co je prilis malo\n", aktualny->size - 1);
			}
		}
		aktualny = aktualny->next;
	}
	printf("nom asi neni miesto\n");
	return NULL;
}

int* memory_free(void* pointer) {							
			// if (memory_check) ... ok, else - pointer nie je z pola
	printf("pustila sa funkcia memory free\n");
	struct block* novy, * aktualny;
	novy = (char*)pointer - sizeof(block);
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
				break;
			}
			aktualny = aktualny->next;
		}
		novy->next = aktualny->next;
		aktualny->next = novy;
	}
	/*tato cast spaja volne bloky co su vedla seba*/
	aktualny = start->first_free;
	while (aktualny->next != NULL) {
		if (aktualny->next == (char*)aktualny + aktualny->size + sizeof(block) - 1) {
			printf("merge\n");
			aktualny->size += aktualny->next->size + sizeof(block) - 1;
			aktualny->next = aktualny->next->next;
		}
		else {
			aktualny = aktualny->next;
		}
	}
	return 0;
}

int main()
{
	int N = 100;
	char region[100];
	memory_init(region, N);

	for (int i = 0; i < N; i++) {
		printf("%d\n", region[i]);
	}
	//memory_check(start);

	char* pointer1 = (char*)memory_alloc(3);
	memset(pointer1, 99, 3);

	char* pointer2 = (char*)memory_alloc(3);
	memset(pointer2, 88, 3);

	char* pointer3 = (char*)memory_alloc(3);
	memset(pointer3, 77, 3);

	char* pointer4 = (char*)memory_alloc(3);
	memset(pointer4, 66, 3);

	char* pointer5 = (char*)memory_alloc(3);
	memset(pointer5, 55, 3);

	for (int i = 0; i < N; i++) {
		printf("%d\n", region[i]);
	}

	memory_free(pointer3);
	memory_free(pointer1);
	memory_free(pointer5);

	for (int i = 0; i < N; i++) {
		printf("%d\n", region[i]);
	}

	char* pointer6 = (char*)memory_alloc(5);
	memset(pointer6, 100, 5);

	for (int i = 0; i < N; i++) {
		printf("%d\n", region[i]);
	}
	/*
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

	for (int i = 0; i < N; i++) {
		printf("%d\n", region[i]);
	}

	char* pointer3 = (char*)memory_alloc(12);
	memset(pointer3, 88, 12);

	for (int i = 0; i < N; i++) {
		printf("%d\n", region[i]);
	}

	char* pointer4 = (char*)memory_alloc(6);
	memset(pointer4, 1, 6);

	for (int i = 0; i < N; i++) {
		printf("%d\n", region[i]);
	}

	char* pointer5 = (char*)memory_alloc(3);
	memset(pointer5, 111, 3);

	for (int i = 0; i < N; i++) {
		printf("%d\n", region[i]);
	}*/

	/*
	char* pointer = (char*)memory_alloc(10);
	if (pointer)
		memset(pointer, 0, 10);
	if (pointer)
		memory_free(pointer);*/
	return 0;
}
