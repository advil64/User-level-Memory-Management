#include <math.h>
#include "my_vm.h"

// memory is page-addressed
#define PHYSICAL_BITMAP_SIZE (MEMSIZE)
#define VIRTUAL_BITMAP_SIZE (MAX_MEMSIZE)

// Constants for page table/directory
#define PAGE_TABLE_SIZE (PGSIZE / sizeof(pte_t))                         // number of entries that fit on a page
#define PAGE_DIRECTORY_SIZE (pow(2, (log2(MAX_MEMSIZE) - log2(PGSIZE)))) // Page directory has an address for each table

// Global sizes
int page_dir_off;
int page_tbl_off;
pde_t directory_start;

// Global variables to store the physical and virtual pages and memory
char *physical_bitmap;
char *virtual_bitmap;
char *physical_memory;

/*
Function responsible for allocating and setting your physical memory
@Author - Advith
*/
void set_physical_mem()
{

    // Allocate physical memory using mmap or malloc; this is the total size of
    // your memory you are simulating
    physical_memory = (char *)malloc(MEMSIZE);

    if (physical_memory == NULL)
    {
        perror("Failed to allocate physical memory");
        exit(1);
    }

    // HINT: Also calculate the number of physical and virtual pages and allocate
    // virtual and physical bitmaps and initialize them
    physical_bitmap = (char *)malloc(sizeof(char) * PHYSICAL_BITMAP_SIZE);
    virtual_bitmap = (char *)malloc(sizeof(char) * VIRTUAL_BITMAP_SIZE);

    if (physical_bitmap == NULL || virtual_bitmap == NULL)
    {
        perror("Failed to allocate bitmaps");
        free(physical_memory); // Clean up allocated memory
        exit(1);
    }

    // Initialize the bitmaps
    for (int i = 0; i < PHYSICAL_BITMAP_SIZE; i++)
    {
        physical_bitmap[i] = 0; // Mark all physical pages as unallocated
    }

    for (int i = 0; i < VIRTUAL_BITMAP_SIZE; i++)
    {
        virtual_bitmap[i] = 0; // Mark all virtual pages as unallocated
    }

    // calculate offsets
    page_dir_off = log2(PAGE_DIRECTORY_SIZE);
    page_tbl_off = log2(PAGE_TABLE_SIZE);
}

/*
The function takes a virtual address and page directories starting address and
performs translation to return the physical address
@Author - Advith
@Author - Advith
*/
pte_t translate(pde_t pgdir, void *va)
pte_t translate(pde_t pgdir, void *va)
{
    /* Part 1 HINT: Get the Page directory index (1st level) Then get the
     * 2nd-level-page table index using the virtual address.  Using the page
     * directory index and page table index get the physical address.
     */
    long curr_add = (long)va;
    int directory_entry = curr_add >> page_tbl_off;

    int mask = (1 << 10) - 1;
    int table_entry = curr_add & mask;

    // page directory has not been set yet
    if (physical_memory[pgdir + directory_entry] == -1)
    {
        // Accessing an invalid entry
        return -1;
    }

    pde_t pg_tbl = physical_memory[pgdir + directory_entry];

    if (physical_memory[pg_tbl * PGSIZE + table_entry] == -1)
    {
        // Accessing an invalid entry
        return -1;
    }

    // Part 2 HINT: Check the TLB before performing the translation. If
    // translation exists, then you can return physical address from the TLB.

    return (pte_t)(pg_tbl * PGSIZE + table_entry);
}

/* The function copies data pointed by "val" to physical
 * memory pages using virtual address (va)
 * The function returns 0 if the put is successfull and -1 otherwise.
 * @Author - Advith
 */
int put_value(void *va, void *val, int size)
{

    /* HINT: Using the virtual address and translate(), find the physical page. Copy
     * the contents of "val" to a physical page. NOTE: The "size" value can be larger
     * than one page. Therefore, you may have to find multiple pages using translate()
     * function.
     */

    // Check if the virtual address is valid
    if (va == NULL)
    {
        return -1; // Invalid virtual address
    }

    // Calculate the number of pages needed to store the data
    int pages_needed = (size / PGSIZE) + 1;

    // Loop through each page
    for (int i = 0; i < pages_needed; i++)
    {
        // Use translate() to find the physical page corresponding to the virtual address
        pte_t pt_index = translate(directory_start, va + i);

        // Copy data from the source buffer to the physical page
        if (size < PGSIZE)
        {
            memcpy((int*)(&physical_memory[pt_index]), val, size);
        }
        else
        {
            memcpy((int*)(&physical_memory[pt_index]), val, PGSIZE);
            size -= PGSIZE;
        }
    }

    return 0; // Successful data copy
}

/*Given a virtual address, this function copies the contents of the page to val
@Author - Advith*/
void get_value(void *va, void *val, int size)
{

    /* HINT: put the values pointed to by "va" inside the physical memory at given
     * "val" address. Assume you can access "val" directly by derefencing them.
     */
    // Calculate the number of pages needed to store the data
    int pages = (size / PGSIZE) + 1;

    // Loop through each page
    for (int i = 0; i < pages; i++)
    {
        // Use translate() to find the physical page corresponding to the virtual address
        pte_t pt_index = translate(directory_start, va + i);
        int temp = 0;

        // Copy data from the physical page to the source buffer
        if (size < PGSIZE)
        {
            memcpy(val, (int*)&physical_memory[pt_index], size);
        }
        else
        {
            memcpy(val, (int*)&physical_memory[pt_index], PGSIZE);
            size -= PGSIZE;
        }
    }
}

/*
The function takes a page directory address, virtual address, physical address
as an argument, and sets a page table entry. This function will walk the page
directory to see if there is an existing mapping for a virtual address. If the
virtual address is not present, then a new entry will be added
@Author - Taj
*/
int page_map(pde_t *pgdir, void *va, void *pa)
{

    /*HINT: Similar to translate(), find the page directory (1st level)
    and page table (2nd-level) indices. If no mapping exists, set the
    virtual to physical mapping */


    // loop to walk the page directory:
        // if address found
            // return 1 since everything is set up

    // Out of the loop means the page directory 

    

    return -1;
}

/*Function that gets the next available virtual address
@Author - Advith
*/
void *get_next_avail(int num_pages)
{
    bool is_contiguous = true;
    // Use virtual address bitmap to find the next free page
    for (int i = 1; i < VIRTUAL_BITMAP_SIZE; i++)
    {
        is_contiguous = true;
        for (int j = i; j < num_pages+i; j++)
        {
            if (virtual_bitmap[j] != 0)
            {
                is_contiguous = false;
            }
        }
        if (is_contiguous)
        {
            return (void *)(i);
        }
    }
    return NULL;
}

/*Function that gets the next available physical
@Author - Advith
*/
long get_next_page()
{
    for (pde_t i = 0; i < PHYSICAL_BITMAP_SIZE; i++)
    {
        if (physical_bitmap[i] == 0)
        {
            return i;
        }
    }

    perror("Ran out of physical memory");
    // TODO clean up allocated memory
    exit(1);
}

/* Function responsible for allocating pages
and used by the benchmark
@Author - Advith
*/
void *t_malloc(unsigned int num_bytes)
{

    /*
     * HINT: If the physical memory is not yet initialized, then allocate and initialize.
     */
    if (physical_memory == NULL)
    {
        set_physical_mem();
    }

    /*
     * HINT: If the page directory is not initialized, then initialize the
     * page directory.
     */

    if (physical_bitmap[0] == 0)
    {
        // Page directory has not been initialized first page is for the directory
        physical_bitmap[0] = 1;
        directory_start = (pde_t)0; // page table starts at address 0 of the memory

        // set all the directory values to -1
        for (int i = 0; i < PGSIZE; i++)
        {
            physical_memory[directory_start + i] = -1;
        }
    }

    /* Next, using get_next_avail(), check if there are free pages. If
     * free pages are available, set the bitmaps and map a new page. Note, you will
     * have to mark which physical pages are used.
     */
    int pages_needed = (num_bytes / PGSIZE) + 1;
    void *virtual_address = get_next_avail(pages_needed);
    if (virtual_address == NULL)
    {
        perror("Ran out of memory");
        exit(1);
    }

    for (int i = 0; i < pages_needed; i++)
    {
        long curr_add = (long)(virtual_address) + i;
        virtual_bitmap[curr_add] = 1;
        // Allocate the corresponding physical memory to the virtual memory
        int directory_entry = curr_add >> page_tbl_off;

        int mask = (1 << 10) - 1;
        int table_entry = curr_add & mask;

        // page directory has not been set yet
        if (physical_memory[directory_start + directory_entry] == -1)
        {
            pde_t page_idx = get_next_page(); // for the page table
            physical_bitmap[page_idx] = 1;
            physical_memory[directory_start + directory_entry] = page_idx;

            // set all the page values to -1
            for (int i = 0; i < PGSIZE; i++)
            {
                physical_memory[page_idx * PGSIZE + i] = -1;
            }
        }

        pde_t pg_tbl = physical_memory[directory_start + directory_entry];
        if (physical_memory[pg_tbl * PGSIZE + table_entry] == -1)
        {
            pte_t val_idx = get_next_page();
            physical_bitmap[val_idx] = 1;
            physical_memory[pg_tbl * PGSIZE + table_entry] = val_idx;

            // set all the page values to -1
            for (int i = 0; i < PGSIZE; i++)
            {
                physical_memory[val_idx * PGSIZE + i] = -1;
            }
        }
    }

    return virtual_address;
}

/* Responsible for releasing one or more memory pages using virtual address (va)
@Author - Advith
*/
void t_free(void *va, int size)
{

    /* Part 1: Free the page table entries starting from this virtual address
     * (va). Also mark the pages free in the bitmap. Perform free only if the
     * memory from "va" to va+size is valid.
     *
     * Part 2: Also, remove the translation from the TLB
     */
}

/*Given a virtual address, this function copies the contents of the page to val
@Author - Advith*/
void get_value(void *va, void *val, int size)
{

    /* HINT: put the values pointed to by "va" inside the physical memory at given
     * "val" address. Assume you can access "val" directly by derefencing them.
     */
    
}

/*
This function receives two matrices mat1 and mat2 as an argument with size
argument representing the number of rows and columns. After performing matrix
multiplication, copy the result to answer.
*/
void mat_mult(void *mat1, void *mat2, int size, void *answer) {

    /* Hint: You will index as [i * size + j] where  "i, j" are the indices of the
     * matrix accessed. Similar to the code in test.c, you will use get_value() to
     * load each element and perform multiplication. Take a look at test.c! In addition to 
     * getting the values from two matrices, you will perform multiplication and 
     * store the result to the "answer array"
     */
    int x, y, val_size = sizeof(int);
    int i, j, k;
    for (i = 0; i < size; i++) {
        for(j = 0; j < size; j++) {
            unsigned int a, b, c = 0;
            for (k = 0; k < size; k++) {
                int address_a = (unsigned int)mat1 + ((i * size * sizeof(int))) + (k * sizeof(int));
                int address_b = (unsigned int)mat2 + ((k * size * sizeof(int))) + (j * sizeof(int));
                get_value( (void *)address_a, &a, sizeof(int));
                get_value( (void *)address_b, &b, sizeof(int));
                // printf("Values at the index: %d, %d, %d, %d, %d\n", 
                //     a, b, size, (i * size + k), (k * size + j));
                c += (a * b);
            }
            int address_c = (unsigned int)answer + ((i * size * sizeof(int))) + (j * sizeof(int));
            // printf("This is the c: %d, address: %x!\n", c, address_c);
            put_value((void *)address_c, (void *)&c, sizeof(int));
        }
    }
}

/*
 * Part 2: Add a virtual to physical page translation to the TLB.
 * Feel free to extend the function arguments or return type.
 */
int add_TLB(void *va, void *pa)
{

    /*Part 2 HINT: Add a virtual to physical page translation to the TLB */

    return -1;
}

/*
 * Part 2: Check TLB for a valid translation.
 * Returns the physical page address.
 * Feel free to extend this function and change the return type.
 */
pte_t *check_TLB(void *va)
{

    /* Part 2: TLB lookup code here */

    /*This function should return a pte_t pointer*/
}

/*
 * Part 2: Print TLB miss rate.
 * Feel free to extend the function arguments or return type.
 */
void print_TLB_missrate()
{
    double miss_rate = 0;

    /*Part 2 Code here to calculate and print the TLB miss rate*/

    fprintf(stderr, "TLB miss rate %lf \n", miss_rate);
}