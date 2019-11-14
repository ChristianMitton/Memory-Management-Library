#ifndef MY_VM_H_INCLUDED
#define MY_VM_H_INCLUDED
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

//Assume the address space is 32 bits, so the max memory size is 4GB
//Page size is 4KB

//Add any important includes here which you may need

/* 
 ------------------------------------------------------------------------------
 ------------------------------ Defines ---------------------------------------
 ------------------------------------------------------------------------------
*/
// Size of a page, 2^12, number of bytes per page
#define PGSIZE 4096

// Maximum size of your virtual memory
#define MAX_MEMSIZE 4*1024*1024*1024 //multiplying the address space and the total pgs, same as 4096*(2^20)

// Size of your physical memory
#define MEMSIZE 1024*1024*1024

/* 
 ------------------------------------------------------------------------------
 ------------------------------ Personal Defines ------------------------------
 ------------------------------------------------------------------------------
*/

//#define TOTAL_PAGES 1048576 //2^20
#define PG_DIR_SIZE 1024 //or 2^10
#define PG_TABLE_SIZE 1024 //or 2^10

// Represents a page table entry
typedef unsigned long pte_t;

// Represents a page directory entry
typedef unsigned long pde_t;

/* 
 ------------------------------------------------------------------------------
 ------------------------------ Structs ---------------------------------------
 ------------------------------------------------------------------------------
*/

#define TLB_SIZE 

//Structure to represents TLB
struct tlb {

    //Assume your TLB is a direct mapped TLB of TBL_SIZE (entries)
    // You must also define wth TBL_SIZE in this file.
    //Assume each bucket to be 4 bytes
};
struct tlb tlb_store;

/* 
 ------------------------------------------------------------------------------
 ------------------------------ Personal Structs ------------------------------
 ------------------------------------------------------------------------------
*/

//array of these in 
typedef struct addressSpace{
	int offset;
	bool available;
	int index;
} addressSpace;

//array of these in physicalPage array
typedef struct physicalPage {
	int pageNumber;
	bool available;
	int vpn;
	//pointer to array of addressSpaces:
	addressSpace *addressSpace;
} physicalPage;

typedef struct directoryBitMap{
	pte_t *pageTable;
} directoryBitMap;

typedef struct pageDirectoryEntry {
	//pde_t numberConvertedFromBits;
	struct pageTableEntry *pgTable;
} pageDirectoryEntry;

typedef struct pageTableEntry {
	//pte_t numberConvertedFromBits;
	int indexInPhysicalMemory;
} pageTableEntry;

/* 
 ------------------------------------------------------------------------------
 ------------------------------ Function Headers ------------------------------
 ------------------------------------------------------------------------------
*/

void SetPhysicalMem();
pte_t * Translate(pde_t *pgdir, void *va);
int PageMap(pde_t *pgdir, void *va, void* pa);
bool check_in_tlb(void *va);
void put_in_tlb(void *va, void *pa);
void *get_next_avail(int num_pages);
void *m_alloc(unsigned int num_bytes);
void a_free(void *va, int size);
void PutVal(void *va, void *val, int size);
void GetVal(void *va, void *val, int size);
void MatMult(void *mat1, void *mat2, int size, void *answer);

/* 
 ------------------------------------------------------------------------------
 ------------------------------ Personal Function Headers ---------------------
 ------------------------------------------------------------------------------
*/
char **createMemoryPage(int row, int col);
unsigned long convertBitToNum(unsigned long number, unsigned long numBits, unsigned long position);
int personalTranslate(pageDirectoryEntry *pgdir, void *va);
void personalPageMap(pageDirectoryEntry *pgdir, void *va);
int personal_get_next_avail(int num_pages);


#endif
