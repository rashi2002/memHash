#include "hash.h"


struct HashTable *createTable( struct memsys *memsys,
								unsigned int capacity,
								unsigned int width,int(*hash)( void*, int),
								int(*compar)(const void*, const void*) )
{
	int data;
	struct HashTable *new = NULL;
	int set =MEMNULL;
	new = malloc(sizeof(struct HashTable));
	if(new==NULL){
		fprintf(stderr, "ERROR: Malloc failed, no table allocated\n");
		exit(0);
	}
	new->capacity = capacity;
	new->width = width;
	new->hash = hash;
	new->compar = compar;
	new->nel = 0;


	data = memmalloc(memsys, capacity*sizeof(int));
	if(data==MEMNULL){
		fprintf(stderr, "ERROR: Memmalloc failed, no data allocated\n");
		
	}

	//initializing all the values to memnull
	for (int i=0; i<capacity; i++){
		setval(memsys,&set, sizeof(int), data+ sizeof(int)*i );
	}

	new->data = data;
	return new;
}

void addElement( struct memsys *memsys, struct HashTable *table, int addr )
{
	void* value = malloc(table->width);
	int index,i ,address;

	if(table->nel==table->capacity){
		fprintf(stderr, "ERROR: The table is completely filled\n");
		exit(0);
	}

	getval(memsys,value, table->width, addr );
	index = table->hash(value, (table->capacity));
	getval(memsys, &address, sizeof(int),(table->data)+sizeof(int)*index );
	
	i=index;

	while(address!=MEMNULL){
		i++;
		if(i==table->capacity){
			i=0;
		}
	    getval(memsys, &address, sizeof(int),(table->data)+sizeof(int)*i );
		if(i==index){
			break;
		}
	}
	setval(memsys, &addr,sizeof(int), (table->data)+i*sizeof(int) );
	table->nel = table->nel +1;
	free(value);
}

int getElement( struct memsys *memsys, struct HashTable *table, void*key ){
	int index, address, i;
	void* value = malloc(table->width);

	index = table->hash(key, table->capacity);
	getval(memsys, &address, sizeof(int), table->data+ index*sizeof(int));

	if(address==MEMNULL){
		free(value);
		return MEMNULL;
	}

	getval(memsys, value, table->width, address);


	
	if(table->compar(value, key)==0){
		free(value);
		return address;
	}
	else{
		i=index;
		while(i<table->capacity){
			i++;
			getval(memsys, &address, sizeof(int), (table->data)+i*sizeof(int));
			if(address!=MEMNULL){
				getval(memsys, value, table->width, address);
				if(table->compar(value, key)==0){
					free(value);
					return address;
				}
			}
			else{
				free(value);
				return MEMNULL;
			}

			
		}
			i=0;
			while(i<index){
				getval(memsys, &address, sizeof(int), (table->data)+i*sizeof(int));
				i++;
				if(address!=MEMNULL){
					getval(memsys, value, table->width, address);
					if(table->compar(value, key)==0){
						free(value);
						return address;
					}

				}
				else{
					free(value);
					return MEMNULL;
				}
			}
	}
	free(value);
	return MEMNULL;
}


void freeTable( struct memsys *memsys, struct HashTable *table )
{
	memfree(memsys, table->data);
	free(table);
}

int hashAccuracy( struct memsys *memsys, struct HashTable *table )
{
	int calculatedIndex, index, difference ,total=0, address =0;
	void*value = malloc(table->width);

	for(index = 0; index<table->capacity; index++){

		getval(memsys, &address, sizeof(int), (table->data)+index*sizeof(int));
		if(address!=MEMNULL){
			getval(memsys, value, table->width, address);
			calculatedIndex = table->hash(value, (table->capacity));
			difference = index - calculatedIndex;
			if(difference>=0){
				total += difference;
			}
			else{
				total+=index -(table->capacity)+calculatedIndex;
			}

		}
		
	}
	free(value);
	return total;
}
