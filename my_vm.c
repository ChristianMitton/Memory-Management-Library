#include "my_vm.h"
#include <stdlib.h>
#include <stdio.h>
#include<pthread.h>
#include<pthread.h> 
#include <string.h>

/*	
make clean; make; cd benchmark/; make clean; make; ./test; cd ..
*/


/*
		-index-		|-------------------------PhysicalMem--------------------------|

vpn--> 		0 = ppn		[ppn + offset] [ppn + offset] [ppn + offset] .... [ppn + offset]
vpn--> 		1 = ppn		[ppn + offset] [ppn + offset] [ppn + offset] .... [ppn + offset]
vpn--> 		2 = ppn 	[ppn + offset] [ppn + offset] [ppn + offset] .... [ppn + offset]

*/

//Physical Memory and it's bitmap
char **physicalMemory;
int *physicalMemBitMap;

//Page directory and it's bit map
pde_t *pgDir;
directoryBitMap *dirBitMap;

//Mutex lock
pthread_mutex_t lock;

/*
 --------------------------------------------------------------------------------
 -------------------------------- SetPhysicalMem --------------------------------
 --------------------------------------------------------------------------------
*/
/*
Function responsible for allocating and setting your physical memory 
*/
void SetPhysicalMem() {

	//Allocate physical memory using mmap or malloc; this is the total size of
	//your memory you are simulating

	//HINT: Also calculate the number of physical and virtual pages and allocate
	 //virtual and physical bitmaps and initialize them

	pthread_mutex_lock(&lock);

	int numPages = (MEMSIZE/PGSIZE);
	int i = 0;

	//Initialize Physical Memory
	physicalMemory = createMemoryPage(numPages, PGSIZE);

	physicalMemBitMap = malloc(numPages * sizeof(int));
	for(i = 0; i < numPages; i++){
		//set all entries in bit map to -1, thus marking all pages in physical memory as free
		physicalMemBitMap[i] = -1;
	}
	
	pthread_mutex_unlock(&lock);
}

char **createMemoryPage(int row, int col) {
	pthread_mutex_lock(&lock);
	char **matrix = malloc(row * sizeof(char *));
	int i = 0;
	for(i = 0; i < row; i++){
		matrix[i] = malloc(col * sizeof(char));
	}	
	pthread_mutex_unlock(&lock);	
	
	return matrix;
}

/*
 --------------------------------------------------------------------------------
 -------------------------------- Translate -------------------------------------
 --------------------------------------------------------------------------------
*/

/*	_ _ _ _ _ _ _ _ _ _ _ _ _ _ __ _  _ _ _ _ _ _ _ _ _ _ _ _
	|      directoryIndex	   ||   vpn   ||    offset      |
		   10 bits	      10 bits	    12 bits

	dir = index within directory

	vpn = virtual page number/ index within page table

		- ppn = index of page table to go to

	offset = address in memory to go to

		- since using a 2-d array, address location would be physicalMemory[ppn][offset]
	
*/

/*
The function takes a virtual address and page directories starting address and
performs translation to return the physical address
*/
pte_t *Translate(pde_t *pgdir, void *va) {
	//HINT: Get the Page directory index (1st level) Then get the
    	//2nd-level-page table index using the virtual address.  Using the page
    	//directory index and page table index get the physical address

	pthread_mutex_lock(&lock);	
	
	unsigned long virtualAddress = (unsigned long) va;
	
	unsigned long directoryIndex, vpn, offset;
	
	directoryIndex = convertBitToNum(virtualAddress, 10, 1);
	vpn = convertBitToNum(virtualAddress, 10, 11);
	offset = convertBitToNum(virtualAddress, 12, 22);

	pde_t pgdirEntry = pgdir[directoryIndex];

	//get the pageTable that the pgdirEntry Points to
	pte_t *pageTable = dirBitMap[pgdirEntry].pageTable;
	
	//Get the vpn from the pageTable. This is the page of the va's physical address
	pte_t *indexOfPhysicalAddress = &pageTable[vpn];

	pthread_mutex_unlock(&lock);

	//return page in physical address
	return indexOfPhysicalAddress;


}

unsigned long convertBitToNum(unsigned long number, unsigned long numBits, unsigned long position) { 
    return (((1 << numBits) - 1) & (number >> (position - 1))); 
} 

/*
 --------------------------------------------------------------------------------
 -------------------------------- PageMap ---------------------------------------
 --------------------------------------------------------------------------------
*/
/*
The function takes a page directory address, virtual address, physical address
as an argument, and sets a page table entry. This function will walk the page
directory to see if there is an existing mapping for a virtual address. If the
virtual address is not present, then a new entry will be added
*/
int PageMap(pde_t *pgdir, void *va, void* pa){
	/*HINT: Similar to Translate(), find the page directory (1st level)
 	and page table (2nd-level) indices. If no mapping exists, set the
    	virtual to physical mapping */
	pthread_mutex_lock(&lock);	

	unsigned long virtualAddress = (unsigned long) va;
	unsigned long physicalAddress = (unsigned long) pa;
	unsigned long directoryIndex, vpn, offset;
	
	directoryIndex = convertBitToNum(virtualAddress, 10, 1);
	vpn = convertBitToNum(virtualAddress, 10, 11);
	offset = convertBitToNum(virtualAddress, 12, 22);

	int i, indexOfNewPageTable;

	pde_t pgdirEntry = pgdir[directoryIndex];
	pte_t *pageTable = dirBitMap[pgdirEntry].pageTable;

	// if there is no page table for current directory entry, make one,
	// NOTE: (assumes pgdir is not null, and has an entry at directoryIndex)
	if(pageTable == NULL){
		//create table for entry in dir
		dirBitMap[pgdirEntry].pageTable = malloc(PG_TABLE_SIZE * sizeof(pte_t *));
		//set all entries in newly created page table to -1, marking them as empty
		for(i = 0; i < PG_TABLE_SIZE; i++){
			dirBitMap[pgdirEntry].pageTable[i] = -1;		
		}

		/* and then set the vpn within the newly created page table to point 
		to specified space in mem (aka pa/physical address) */
		dirBitMap[pgdirEntry].pageTable[vpn] = physicalAddress;

		//mark the space in physicalMem as being occupied in it's bitmap
		physicalMemBitMap[physicalAddress] = 1;

		pthread_mutex_unlock(&lock);

		//Wasn't specified what we should return, so returning the physical page number as an int
		return (int) physicalAddress;

	} else {
		//if the table exsits, but the entry in it is empty, set it to pa/physical address
		if(pageTable[vpn] == -1){
			dirBitMap[pgdirEntry].pageTable[vpn] = physicalAddress;

			//mark the space in physicalMem as being occupied in it's bitmap
			physicalMemBitMap[physicalAddress] = 1;

			pthread_mutex_unlock(&lock);	

			//Wasn't specified what we should return, so returning the physical page number as an int
			return (int) physicalAddress;
		}
	}

}
/*
 --------------------------------------------------------------------------------
 -------------------------------- get_next_avail --------------------------------
 --------------------------------------------------------------------------------
*/
/*Function that gets the next available page
*/
void *get_next_avail(int num_pages) {
	//Use virtual address bitmap to find the next free page
	pthread_mutex_lock(&lock);
	int i = 0;

	for(i = 0; i < num_pages; i++){
		//Search physicalMemBitMap (aka virtual address bitmap), if there is a free space, return it
		if(physicalMemBitMap[i] == -1){
			pthread_mutex_unlock(&lock);
			return (void *)i;
		}
	}
	pthread_mutex_unlock(&lock);
	return (void *)-1;
}

/*
 --------------------------------------------------------------------------------
 -------------------------------- m_alloc --------------------------------
 --------------------------------------------------------------------------------
*/
/* Function responsible for allocating pages
and used by the benchmark
*/
void *m_alloc(unsigned int num_bytes) {

	//HINT: If the physical memory is not yet initialized, then allocate and initialize.

	/* 
	HINT: If the page directory is not initialized, then initialize the
	page directory. Next, using get_next_avail(), check if there are free pages. If
	free pages are available, set the bitmaps and map a new page. Note, you will 
	have to mark which physical pages are used. 
	*/

	pthread_mutex_lock(&lock);

	//if the physical memory is not initialized, allocate and initialize
	if(physicalMemory == NULL){
		SetPhysicalMem();
	}

	//if the page directory is not initialized, intialize it.
	if(pgDir == NULL){
		pgDir = malloc(PG_DIR_SIZE * sizeof(pde_t));
		dirBitMap = malloc(PG_DIR_SIZE * sizeof(directoryBitMap));
	}

	//if next free space is available, get it
	int freeSpace = (int)get_next_avail(MEMSIZE/PGSIZE);

	//set index in physical memory bitmap to be occupied:
	physicalMemBitMap[freeSpace] = 1;

	//return memory address to user
	void *addressInMemory = &physicalMemory[freeSpace];
	pthread_mutex_unlock(&lock);
	return addressInMemory;
		
}


/*
 --------------------------------------------------------------------------------
 -------------------------------- a_free --------------------------------
 --------------------------------------------------------------------------------
*/
/* Responsible for releasing one or more memory pages using virtual address (va)
*/
void a_free(void *va, int size) {
	
	//Free the page table entries starting from this virtual address (va)
	// Also mark the pages free in the bitmap
	//Only free if the memory from "va" to va+size is valid
	pthread_mutex_lock(&lock);

	unsigned long virtualAddress = (unsigned long) va;
	unsigned long directoryIndex, vpn, offset;
	
	directoryIndex = convertBitToNum(virtualAddress, 10, 1);
	vpn = convertBitToNum(virtualAddress, 10, 11);
	offset = convertBitToNum(virtualAddress, 12, 22);

	//get physicalMemIndex of va
	pte_t physicalMemIndex = (pte_t) Translate(pgDir, va);

	//free it in bitmap
	physicalMemBitMap[physicalMemIndex] = -1;
	
	//zero out all the bits
	int i;
	for(i = 0; i < PGSIZE; i++){
		memset(physicalMemory[physicalMemIndex], 0, i*sizeof(char));
		//memset(temp[i],0,1*sizeof(char));
	}	
	
	pthread_mutex_unlock(&lock);	
}


/*
 --------------------------------------------------------------------------------
 -------------------------------- PutVal --------------------------------
 --------------------------------------------------------------------------------
*/
/* The function copies data pointed by "val" to physical
 * memory pages using virtual address (va)
*/
void PutVal(void *va, void *val, int size) {

	/* 
	HINT: Using the virtual address and Translate(), find the physical page. Copy
	the contents of "val" to a physical page. NOTE: The "size" value can be larger
	than one page. Therefore, you may have to find multiple pages using Translate()
	function.
	*/
	pthread_mutex_lock(&lock);

	pte_t *indexOfPhysicalAddress = Translate(pgDir, va);

	//physicalMemory[*indexOfPhysicalAddress];

	int i;
	for(i = 0; i < size; i++){
		
		//printf("i: %d\n",i);
		//memcpy(physicalMemory[0][i], val, size);
		memcpy(physicalMemory[(int)indexOfPhysicalAddress], val, size);
	}
	
	pthread_mutex_unlock(&lock);
	    	
}


/*
 --------------------------------------------------------------------------------
 -------------------------------- GetVal --------------------------------
 --------------------------------------------------------------------------------
*/
/*Given a virtual address, this function copies the contents of the page to val*/
void GetVal(void *va, void *val, int size) {

	/* HINT: put the values pointed to by "va" inside the physical memory at given
	"val" address. Assume you can access "val" directly by derefencing them.
	If you are implementing TLB,  always check first the presence of translation
	in TLB before proceeding forward */

	pthread_mutex_lock(&lock);

	//put values pointed to by va inside physical mem at val address
	pte_t indexInPhysicalMem = (pte_t ) Translate(pgDir, va);
	
	val = &physicalMemory[indexInPhysicalMem];
	pthread_mutex_unlock(&lock);	

	int num = 1;

}


/*
 --------------------------------------------------------------------------------
 -------------------------------- MatMult --------------------------------
 --------------------------------------------------------------------------------
*/
/*
This function receives two matrices mat1 and mat2 as an argument with size
argument representing the number of rows and columns. After performing matrix
multiplication, copy the result to answer.
*/
void MatMult(void *mat1, void *mat2, int size, void *answer) {

	/* 
	Hint: You will index as [i * size + j] where  "i, j" are the indices of the
	matrix accessed. Similar to the code in test.c, you will use GetVal() to
	load each element and perform multiplication. Take a look at test.c! In addition to 
	getting the values from two matrices, you will perform multiplication and 
	store the result to the "answer array"
	*/
	pthread_mutex_lock(&lock);
	int *ans = (int *) answer;
	int *matrix1 = (int *) mat1;
	int *matrix2 = (int *) mat2;
	
	int i,j,k;
	for(i = 0; i < size; i++){
		for(j = 0; j < size; j++){
			for(k = 0; k < size; k++){
				ans[i*size + j] = matrix1[i*size + k] * matrix2[k*size + j];
			} 
		}
	}

	answer = &ans;

	pthread_mutex_unlock(&lock);
}

/*
	------------------- INTEGER MATRIX MULTIPLICATION -------------------

int** multiply(int** matrix1, int row1, int col1, int** matrix2, int row2, int col2){
	int ** result = createMatrix(row1,col2);
	//multiplication
	for(int i = 0; i < row1; i++) {
		for(int j = 0; j < col2; j++) {
			result[i][j] = 0;
			for(int k = 0; k < row2; k++){
				result[i][j] += matrix1[i][k] * matrix2[k][j];
			}
		}
	}
	//printMatrix(result,row1,row1);
	return result;
}

int** createMatrix(int row, int col) {
	int** matrix = (int**) malloc(row * sizeof(double *));
	for(int i = 0; i < row; i++) {
		matrix[i] = (double*) malloc((col+1)*sizeof(double));
	}
	return matrix;
}
*/
