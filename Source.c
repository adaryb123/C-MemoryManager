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
	//unsigned short is_free;
	struct block* next;
	//struct block* prev;
}block;

block* start = NULL;
/*
int memory_check(block * ptr) {
	printf("\n %d\n", ptr->is_free);
	return 0;
}*/

void memory_init(void* ptr, unsigned int size) {
	start = ptr;
	start->size = size - sizeof(int) + 1;
	start->next = NULL;
	//start->prev = NULL;
}

void* memory_alloc(unsigned int required_size) {
	block *aktualny = start;
	printf("pustila sa funkcia memory alloc a hlada miesto pre blok velkosti %d\n",required_size);
	if (required_size % 2 == 1) {				//pamat sa vzdy zaokruhli smerom hore na parne cislo
		required_size++;
	}

	while (aktualny != NULL) {
		if (aktualny->size % 2 == 1) {						//nasli sme blok s rovnakou velkostou
			if (aktualny->size - 1 == required_size){
				printf("presne tolko miesta co treba\n\n\n");
				aktualny->size = aktualny->size - 1;
				aktualny->next = NULL;
				/*
				if (aktualny->next != NULL) {
					aktualny->next->prev = aktualny->prev;
				}
				if (aktualny->prev != NULL) {
					aktualny->prev->next = aktualny->next;
				}*/

				return (char*)aktualny + sizeof(int);
			}
			else if(aktualny->size > required_size) {			//nasli sme vacsi blok, zobereme z neho cast
				printf("viac miesta ako treba  (%d), rozdeli sa\n\n\n",aktualny->size);

				struct block* novy;
				novy = (char*)aktualny + required_size + sizeof(int);
				novy->size = aktualny->size - required_size - sizeof(int);
				aktualny->size = required_size;
				novy->next = aktualny->next;
				/*
				novy->next = aktualny->next;
				novy->prev = aktualny->prev;
				if (aktualny->next) {
					aktualny->next->prev = novy;
				}
				if (aktualny->prev) {
					aktualny->prev->next = novy;
				}*/
				
				if (start == aktualny) {
					start = novy;
				}

				return (char*)aktualny + sizeof(int);
			}
			else {
				printf("nasiel sa blok velkosti %d, co je prilis malo\n",aktualny->size - 1);
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

	struct block * hlavicka, *temp;
	hlavicka = (char*)pointer - sizeof(int);
	hlavicka->size += 1;

	temp = start;
	hlavicka->next = temp;
	start = hlavicka;

	//a tu este merge
	temp = start;
	while (temp != NULL && temp->next != NULL) {
		printf("\n%d %d \n", (char*)temp + temp->size - 1, (char*)temp->next - sizeof(int));
		if ((char*)temp + temp->size - 1 == (char*)temp->next - sizeof(int)) {
			printf("nasli sa dva volne bloky pri sebe, spoja sa\n");
			temp->size += temp->next->size - 1 + sizeof(int);
			temp->next = temp->next->next;

			// a este ak sme menili start tak updatnut
			if (start == temp->next) {
				start = temp;
			}
		}
		temp = temp->next;
	}

}

int main()
{
	char region[50];
	memory_init(region, 50);

	for (int i = 0; i < 50; i++) {
		printf("%d\n", region[i]);
	}
	//memory_check(start);

	char* pointer = (char*)memory_alloc(3);
	memset(pointer, 66,3);

	for (int i = 0; i < 50; i++) {
		printf("%d\n", region[i]);
	}

	char* pointer2 = (char*)memory_alloc(22);
	memset(pointer2, 77, 22);

	for (int i = 0; i < 50; i++) {
		printf("%d\n", region[i]);
	}
	memory_free(pointer2);
	memory_free(pointer);

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

	/*
	char* pointer = (char*)memory_alloc(10);
	if (pointer)
		memset(pointer, 0, 10);
	if (pointer)
		memory_free(pointer);*/
	return 0;
}