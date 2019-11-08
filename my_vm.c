#include "my_vm.h"
#include <stdlib.h>
#include <stdio.h>

//	make clean; make; cd benchmark/; make clean; make; ./test; cd ..

//addressSpace **physicalMemory;
char **physicalMemory;

/*
		PhysicalBitMap     	PhysicalMem

0	vpn|	[ppn] 			[ppn+offse] [ppn+offset] [ppn+offset] .... [ppn+offset]
1	vpn|	[ppn]			[ppn+offse] [ppn+offset] [ppn+offset] .... [ppn+offset]
2	vpn|	[ppn] 			[ppn+offse] [ppn+offset] [ppn+offset] .... [ppn+offset]

*/
 
//addressSpace **virtualMemory;
bitmap *physicalMemBitMap;
bitmap *virtualMemBitMap;

pageDirectory *pgDir;
pageTable *pgTable;
//pde_t *pgDir;
//pte_t *pgTable;

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

		//change physical mem to array of char'ßs

		//int pageTableSize = (MEMSIZE/PGSIZE);
		int physicalMemorySize = (MEMSIZE/PGSIZE);

		//printf("Creating....\n");

		printf("Creating physical memory, size will be 2d array of %d btyes....\n", physicalMemorySize);
		physicalMemory = createMemoryPage(physicalMemorySize, PGSIZE);

	    	printf("Creating physicalMemBitMap, size will be array of %d bytes....\n", physicalMemorySize);
		physicalMemBitMap = malloc(physicalMemorySize * sizeof(bitmap));

	    	printf("Creating virtualMemBitMap, size will be array of %d bytes....\n", physicalMemorySize);
		virtualMemBitMap = malloc(physicalMemorySize * sizeof(bitmap));

	    	printf("Creating pgDir array, where each index is a pte_t, will contain 1024 pde_t's....\n");
		//pgDir = malloc(PG_DIR_SIZE * sizeof(pde_t));
		pgDir = malloc(PG_DIR_SIZE * sizeof(pageDirectory));
		
		printf("Creating pgTable in setPhysMem, will contain 1024 pte_t's....\n");
		//pgTable = malloc(PG_TABLE_SIZE * sizeof(pte_t));
		pgTable = malloc(PG_TABLE_SIZE * sizeof(pageTable));

		printf("Done\n");

}

/*
addressSpace **createMemoryPage(int row, int col) {
	addressSpace** matrix = (addressSpace**) malloc(row * sizeof(addressSpace *));
	int i;	
	for(i = 0; i < row; i++) {
		matrix[i] = (addressSpace*) malloc(col * sizeof(addressSpace ));
	}
	return matrix;
	
}
*/

char **createMemoryPage(int row, int col) {
	char **matrix = malloc(row * sizeof(char *));
	int i = 0;
	for(i = 0; i < row; i++){
		matrix[i] = malloc(col * sizeof(char));
	}
	return matrix;
}
/*
addressSpace *createAddressSpace(){
	
	addressSpace *newAddressSpace = malloc(sizeof(addressSpace));
	newAddressSpace->offset = -1;
	newAddressSpace->available = false;
	newAddressSpace->index = 0;

	return newAddressSpace;
}
*/
/*
 --------------------------------------------------------------------------------
 -------------------------------- Translate -------------------------------------
 --------------------------------------------------------------------------------
*/

/*
	_ _ _ _ _ _ _ _ __ _  _ _ _ _
	| pg num  ||   byte in pg    |
*/

/*
The function takes a virtual address and page directories starting address and
performs translation to return the physical address
*/
unsigned long *Translate(pde_t *pgdir, void *va) {
    //HINT: Get the Page directory index (1st level) Then get the
    //2nd-level-page table index using the virtual address.  Using the page
    //directory index and page table index get the physical address

	bool exsitsInDir = false;
	bool exsitsInPgTable = false;

	printf("In translate\n");
	unsigned long pgDirectoryEntry;
	unsigned long vpn;
	unsigned long offset;

	int i, row, col;
	
	potentialPgDirectoryEntry = convertBitToNum(*pgdir, 10, 1);
	vpn = convertBitToNum(*pgdir, 10, 11);
	offset = convertBitToNum(*pgdir, 12, 22);
	
	//Check if pgDirectoryEntry is within directory
	for(i = 0; i < PG_DIR_SIZE; i++){
		pde_t currentDirEntry = pgDir[i]->numberConvertedFromBits;

		if(currentDirEntry == potentialPgDirectoryEntry){
			exsitsInDir = true;

			//search pgTable of currentDirectory
			pageTable *currentDirEntryPgTable = pgDir[i]->pgTable;
			
			
			
		} else {
			
		}
	}

/*
	

	physicalAddress = pgTableBitMap[vpn];

	addressLocation = physicalAddress + offset;

	pointer = physicalMem[physicalAddress][addressLocation];
	//it'd have to allocate x num of bytes, regardless of how much/ how little they ask for

	return pointer which points to allocated mem;

  */  


    return NULL; 

	/*
	int row, col;
	int physicalMemorySize = (MEMSIZE/PGSIZE)/sizeof(addressSpace);

	for(row = 0; row < physicalMemorySize; row++){
		for(col = 0; col < PGSIZE; col++){
			if
		}
	}
	*/

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
int PageMap(pde_t *pgdir, void *va, void *pa) {

    /*HINT: Similar to Translate(), find the page directory (1st level)
    and page table (2nd-level) indices. If no mapping exists, set the
    virtual to physical mapping */
	    	//printf("In pageMap\n");
    return -1;
}



/*
 --------------------------------------------------------------------------------
 -------------------------------- get_next_avail --------------------------------
 --------------------------------------------------------------------------------
*/
/*Function that gets the next available page
*/
void *get_next_avail(int num_pages) {
 	    	//printf("In get_next_avail\n");
    //Use virtual address bitmap to find the next free page
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

   /* HINT: If the page directory is not initialized, then initialize the
   page directory. Next, using get_next_avail(), check if there are free pages. If
   free pages are available, set the bitmaps and map a new page. Note, you will 
   have to mark which physical pages are used. */

	printf("in m_alloc--\n");

    return NULL;
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
	    	//printf("In a_free\n");
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

    /* HINT: Using the virtual address and Translate(), find the physical page. Copy
       the contents of "val" to a physical page. NOTE: The "size" value can be larger
       than one page. Therefore, you may have to find multiple pages using Translate()
       function.*/
	    	//printf("In put val\n");
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
	    	//printf("In getVal\n");

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

    /* Hint: You will index as [i * size + j] where  "i, j" are the indices of the
    matrix accessed. Similar to the code in test.c, you will use GetVal() to
    load each element and perform multiplication. Take a look at test.c! In addition to 
    getting the values from two matrices, you will perform multiplication and 
    store the result to the "answer array"*/
		    	//printf("In matmulti\n");

       
}
