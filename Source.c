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

typedef struct block {
	unsigned int size;				//ak je blok volny, pripocitame k size 1 (inak je size vzdy parne)
	struct block* next;
}block;

block* start = NULL;				//tento smernik bude vzdy ukazovat na prvy volny blok
/*
int memory_check(block * ptr) {
	printf("\n %d\n", ptr->is_free);
	return 0;
}*/

void memory_init(void* ptr, unsigned int size) {
	start = ptr;
	start->size = size - sizeof(int) + 1;
	start->next = NULL;
}

void* memory_alloc(unsigned int required_size) {
	block* aktualny = start;
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
		if (aktualny->size % 2 == 1) {						//nasli sme blok s rovnakou velkostou
			if (aktualny->size - 1 == required_size) {
				printf("presne tolko miesta co treba\n\n\n");
				aktualny->size = aktualny->size - 1;
				start = aktualny->next;
				aktualny->next = NULL;
				return (char*)aktualny + sizeof(int);
			}
			else if (aktualny->size > required_size) {			//nasli sme vacsi blok, zobereme z neho cast
				printf("viac miesta ako treba  (%d), rozdeli sa\n\n\n", aktualny->size);

				struct block* novy;
				novy = (char*)aktualny + required_size + sizeof(int);
				novy->size = aktualny->size - required_size - sizeof(int);
				aktualny->size = required_size;
				novy->next = aktualny->next;
						//tu treba osetrit ze ten novy sa nevytvori ak tam uz neni miesto lebo tam je dalsi blok(pripadne koniec pamate)
				if (start == aktualny) {
					start = novy;
				}
				return (char*)aktualny + sizeof(int);
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

int* memory_free(void* pointer) {							// co ak freeneme jediny blok ktory je?
			// if (memory_check) ... ok, else - pointer nie je z pola
	printf("pustila sa funkcia memory free\n");
	struct block* novy, * aktualny;
	novy = (char*)pointer - sizeof(int);
	novy->size += 1;
	/* ulozime uvolneny blok do zoznamu podla adresy */
	if (novy < start) {
		novy->next = start;
		start = novy;
	}
	else {
		aktualny = start;
		while (aktualny->next != NULL) {
			if (novy > aktualny && novy < aktualny->next) {
				break;
			}
		}
		novy->next = aktualny->next;
		aktualny->next = novy;
	}
	/*tato cast spaja volne bloky co su vedla seba*/
	aktualny = start;
	while (aktualny->next != NULL) {
		if (aktualny->next == (char*)aktualny + aktualny->size + sizeof(int) - 1) {
			printf("merge\n");
			aktualny->size += aktualny->next->size + sizeof(int) - 1;
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
	char region[50];
	memory_init(region, 50);

	for (int i = 0; i < 50; i++) {
		printf("%d\n", region[i]);
	}
	//memory_check(start);

	char* pointer1 = (char*)memory_alloc(3);
	memset(pointer1, 66, 3);

	for (int i = 0; i < 50; i++) {
		printf("%d\n", region[i]);
	}

	char* pointer2 = (char*)memory_alloc(22);
	memset(pointer2, 77, 22);

	for (int i = 0; i < 50; i++) {
		printf("%d\n", region[i]);
	}
	memory_free(pointer1);

	for (int i = 0; i < 50; i++) {
		printf("%d\n", region[i]);
	}

	memory_free(pointer2);

	for (int i = 0; i < 50; i++) {
		printf("%d\n", region[i]);
	}

	char* pointer3 = (char*)memory_alloc(12);
	memset(pointer3, 88, 12);

	for (int i = 0; i < 50; i++) {
		printf("%d\n", region[i]);
	}

	for (int i = 0; i < 50; i++) {
		printf("%d\n", region[i]);
	}

	char* pointer4 = (char*)memory_alloc(6);
	memset(pointer4, 1, 6);

	for (int i = 0; i < 50; i++) {
		printf("%d\n", region[i]);
	}

	char* pointer5 = (char*)memory_alloc(3);
	memset(pointer5, 111, 3);

	for (int i = 0; i < 50; i++) {
		printf("%d\n", region[i]);
	}

	/*
	char* pointer = (char*)memory_alloc(10);
	if (pointer)
		memset(pointer, 0, 10);
	if (pointer)
		memory_free(pointer);*/
	return 0;
}
