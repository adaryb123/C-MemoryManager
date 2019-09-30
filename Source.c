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
	struct block* prev;
}block;

block* start = NULL;
/*
int memory_check(block * ptr) {
	printf("\n %d\n", ptr->is_free);
	return 0;
}*/

void memory_init(void* ptr, unsigned int size) {
	start = ptr;
	start->size = size - sizeof(struct block) + 1;
	start->next = NULL;
	start->prev = NULL;
}

void zober_cast_pamete_z_velkeho_bloku(struct block* dostatocne_miesto, unsigned int size) {
	struct block* novy;
	novy = dostatocne_miesto  + size;

	novy->size = dostatocne_miesto->size - size + 1;
	novy->next = dostatocne_miesto->next;
	novy->prev = dostatocne_miesto->prev;

	if (dostatocne_miesto->next) {
		dostatocne_miesto->next->prev = novy;
	}
	if (dostatocne_miesto->prev) {
		dostatocne_miesto->prev->next = novy;
	}

	dostatocne_miesto->size = size - 1;
	dostatocne_miesto->next = novy;

	if (start == dostatocne_miesto) {
		start = novy;
	}
}

void* memory_alloc(unsigned int required_size) {
	block *aktualny = start;
	printf("pustila sa funkcia memory alloc\n");
	if (required_size % 2 == 1) {
		required_size++;
	}

	while (aktualny != NULL) {
		if (aktualny->size % 2 == 1) {
			if (aktualny->size - 1 == required_size){
				printf("presne tolko miesta co treba\n\n\n");
				aktualny->size = aktualny->size - 1;					
				if (aktualny->next != NULL) {
					aktualny->next->prev = aktualny->prev;
				}
				if (aktualny->prev != NULL) {
					aktualny->prev->next = aktualny->next;
				}
				return aktualny;
			}
			else if(aktualny->size > required_size) {				
				printf("viac miesta ako treba, rozdeli sa\n\n\n");

				struct block* temp_prev = aktualny->prev;
				struct block* temp_next = aktualny->next;

				struct block* novy;
				novy = (char*)aktualny + (required_size + sizeof(struct block));

				novy->size = aktualny->size - required_size - sizeof(struct block);

				aktualny->next = temp_next;
				aktualny->prev = temp_prev;

				novy->next = aktualny->next;
				novy->prev = aktualny->prev;

				if (aktualny->next) {
					aktualny->next->prev = novy;
				}
				if (aktualny->prev) {
					aktualny->prev->next = novy;
				}

				aktualny->size = required_size;
				aktualny->next = novy;

				if (start == aktualny) {
					start = novy;
				}

				return (aktualny + 1);
			}
			else {
				;
			}
		}
		aktualny = aktualny->next;
	}
	printf("nom asi neni miesto\n");
	return NULL;
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
	memset(pointer, 6,3);

	for (int i = 0; i < 50; i++) {
		printf("%d\n", region[i]);
	}

	char* pointer2 = (char*)memory_alloc(5);
	memset(pointer2, 7, 5);

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