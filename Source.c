#include <stdio.h>


typedef struct head {					//hlavicka je na zaciatku volnej pamate, obsahuje smernik na prvy volny blok, a smernik na koniec pamate
	struct block* first_free;
	void* end;
}head;

typedef struct block {			//hlavicka kazdeho bloku obsahuje velkost bloku a smernik na nasledujuci blok
	unsigned int size;				//ak je blok volny, pripocitame k size 1 (inak je size vzdy parne)
	struct block* next;			//ak blok obsadime, smernik na dalsi uz potrebovat nebudeme, takze data mozeme vpisovat aj sem
}block;

head* start = NULL;				//globalny smernik bude ukazovat na hlavicku pamate

void memory_init(void* memory, unsigned int memory_size) {
	start = memory;
	start->first_free = start + 1;
	start->first_free->size = memory_size - sizeof(head) - sizeof(int) + 1;
	start->first_free->next = NULL;
	start->end = (char*) start + memory_size;
}

void* memory_alloc(unsigned int required_size) {					
	block* current = start->first_free , *prev = NULL;
	if (required_size % 8 != 0){		//velkost za vzdy zaukruhli smerom hore na najblizsi nasobok 8
		required_size += (8 - required_size % 8);
	}
	while (current != NULL) {		//prehladavame volne bloky, kym nenajdeme taky co vyhovuje
		if ((current->size - 1 > required_size + sizeof(int)) && ((char*)current + sizeof(int) + required_size + sizeof(block) < start->end)) {
			/*Prva moznost je ze sme nasli blok dost velky na to aby sme ho mohli rozdelit a zobrat si iba cast z neho.
			Podmienka zaroven kontoluje aj to, ze ci je blok dost velky na to, aby,ked ho rozdelime, bolo miesto aj pre hlavicku noveho volneho bloku, ktory vznikne za obsadenym*/

			block* new_free;
			new_free = (char*)current + required_size + sizeof(int);
			new_free->size = current->size - required_size - sizeof(int);
			current->size = required_size;
			new_free->next = current->next;

			if (start->first_free == current) {
				start->first_free = new_free;
			}
			if (prev != NULL) {
				prev->next = new_free;
			}

			return (char*)current + sizeof(int);
			}
		else if (current->size - 1 >= required_size) {
			/*Druha moznost je, ze sme nasli blok rovnaky alebo iba o trochu vacsi ako je pozadovana velksot,takze ho obsadime cely*/
			current->size = current->size - 1;
			if (start->first_free == current) {
				start->first_free = current->next;
			}
			if (prev != NULL) {
				prev->next = current->next;
			}
			return (char*)current + sizeof(int);
		}
		/* ak bol volny blok mensi ako nasa pozadovana velkost, pokracujeme v hladani*/
		prev = current;
		current = current->next;
	}
	/*Ak neexistuje volny blok co by vyhovoval, vratime NULL*/
	return NULL;
}

int memory_check(void* pointer) {
	if (pointer <= start || pointer > start->end) {		//smernik je validny ak sa nachadza medzi smernikom na zaciatok pamate a smernikom na koniec pamate
		return 0;
	}
	return 1;
}

int memory_free(void* pointer) {
	if (memory_check(pointer) == 0) {
		return 1;
	}
	block* new_free, * current, * prev = NULL;
	new_free = (char*)pointer - sizeof(int);
	new_free->size += 1;
	/* ulozime uvolneny blok do zoznamu podla adresy v pamati. Takto bude zoznam vzdy usporiadany podla poradia v akom su bloky v pamati ulozene */
	if (new_free < start->first_free || start->first_free == NULL) {
		new_free->next = start->first_free;
		start->first_free = new_free;
	}
	else {
		current = start->first_free;
		if (current->next == NULL) {
			if (new_free > current) {
				current->next = new_free;
				new_free->next = NULL;
				prev = current;
			}
			else {
				start->first_free = new_free;
				new_free->next = current;
			}
		}
		else {
			while (current->next != NULL) {
				if (new_free > current && new_free < current->next) {
					new_free->next = current->next;
					current->next = new_free;
					prev = current;
					break;
				}
				current = current->next;
			}
		}
	}
	/*Ak sa uvolneny blok nachadza vedla ineho volneho bloku, spoji sa s nim*/
	if (new_free->next != NULL && (new_free->next == (char*)new_free + new_free->size + sizeof(int) - 1)){		//spoji sa s nasledujucim
		new_free->size += new_free->next->size + sizeof(int) - 1;
		new_free->next = new_free->next->next;
	}
	if (prev != NULL && (prev->next == (char*)prev + prev->size + sizeof(int) - 1)){			//spoji sa s predchadzajucim		
	prev->size += prev->next->size + sizeof(int) - 1;
	prev->next = prev->next->next;
	}
	return 0;
}