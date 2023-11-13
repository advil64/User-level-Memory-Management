#include <math.h>
#include "my_vm.h"

// Constants for the number of pages and page frames
#define PHYSICAL_BITMAP_SIZE (MEMSIZE / PGSIZE)
#define VIRTUAL_BITMAP_SIZE (MAX_MEMSIZE / PGSIZE)

// Constants for page table/directory
#define PAGE_TABLE_SIZE (PGSIZE / sizeof(pte_t)) // number of entries that fit on a page
#define PAGE_DIRECTORY_SIZE (PAGE_TABLE_SIZE)    // sqrt of the total number of page tables

// Global variables to store the physical and virtual pages and memory
int *physical_bitmap;
int *virtual_bitmap;
char *physical_memory;

// Global variables to store info about page tables
int num_page_tables;

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
    physical_bitmap = (int *)malloc(sizeof(int) * PHYSICAL_BITMAP_SIZE);
    virtual_bitmap = (int *)malloc(sizeof(int) * VIRTUAL_BITMAP_SIZE);

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





/*
The function takes a virtual address and page directories starting address and
performs translation to return the physical address
@Author - Taj
*/
pte_t *translate(pde_t *pgdir, void *va)
{
    /* Part 1 HINT: Get the Page directory index (1st level) Then get the
     * 2nd-level-page table index using the virtual address.  Using the page
     * directory index and page table index get the physical address.
     *
     * Part 2 HINT: Check the TLB before performing the translation. If
     * translation exists, then you can return physical address from the TLB.
     */

    //step 1. Check the TLB First...
    //needs to be implemented in part 2 of the project first
    /* NOTE: Work on TLB condition with addvith afterwards*/

    //if (check_in_tlb(va)) 
    //{
        // TLB hit: return the physical page from the TLB
      //  return (pte_t*)get_from_tlb(va);
    //}
    //else 
    //{
        // TLB miss: Perform the translation

        // Extract virtual page number
        //The virtual page number = virtual address / page
        unsigned int virtual_page = (unsigned int)va / PGSIZE;

        // Calculate page directory index (1st level)
        unsigned int pd_index = virtual_page / PAGE_TABLE_SIZE;

        // Calculate page table index (2nd level)
        //  calculated by taking the remainder of (virtual page number / number of entries)
        unsigned int pt_index = virtual_page % PAGE_TABLE_SIZE;

        // Check if the page directory entry is valid
        if (pgdir[pd_index] == 0) 
        {
            return NULL;  // fail if not valid
        }

        // Access the page table
        pte_t *page_table = (pte_t*)pgdir[pd_index];

        // Check if the page table entry is valid
        if (page_table[pt_index] == 0) {
            return NULL;  // Invalid translation
        }

        // Translate the virtual address to the physical address
        unsigned int physical_page = page_table[pt_index];
        unsigned int offset = (unsigned int)va % PGSIZE;
        unsigned int physical_address = (physical_page * PGSIZE) + offset;

        // Update TLB with the translation
        put_in_tlb(va, (void*)physical_address);

        // Return the physical address
        return (pte_t*)physical_address;
   // }

    


    //

    // If translation not successful, then return NULL
    //return NULL;
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

    return -1;
}

/*Function that gets the next available page
@Author - Advith
*/
void *get_next_avail(int num_pages)
{
    bool is_contiguous = true;
    // Use virtual address bitmap to find the next free page
    for (int i = 0; i < VIRTUAL_BITMAP_SIZE; i++)
    {
        for (int j = i; j < num_pages; j++)
        {
            if (virtual_bitmap[j] != 0)
            {
                is_contiguous = false;
            }
        }
        if (is_contiguous)
        {
            for (int j = i; j < num_pages; j++)
            {
                if (virtual_bitmap[j] != 0)
                {
                    is_contiguous = false;
                }
            }
            return (void *)(i+1); // first one is the pointer to the page directory
        }
    }
    return NULL;
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
    num_page_tables = pow(2, (log2(MAX_MEMSIZE) - log2(PGSIZE)));

    if (virtual_bitmap[0] == 0)
    {
        // Page directory has not been initialized loop through physical memory and set table addresses
        for (int i = 0; i < PAGE_DIRECTORY_SIZE; i++)
        {
            physical_bitmap[i] = 1;
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

    // TODO Need to now translate this to extract the page directory num and table num, then get the offset and mark the physical pages in the bitmap
    // First right shift the address the correct number of bits then do more shifting stuff to get the indices

    return NULL;
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

/* The function copies data pointed by "val" to physical
 * memory pages using virtual address (va)
 * The function returns 0 if the put is successfull and -1 otherwise.
 * @Author - Taj
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
        return -1;  // Invalid virtual address
    }

    // Calculate the number of pages needed to store the data
    int num_pages = (size / PGSIZE) + ((size % PGSIZE) != 0);

    // Loop through each page
    for (int i = 0; i < num_pages; i++) 
    {
        // Calculate indices for the two-level page table
        unsigned int virtual_page = (unsigned int)(va + (i * PGSIZE)) / PGSIZE;
        unsigned int pd_index = virtual_page / PAGE_TABLE_SIZE;
        unsigned int pt_index = virtual_page % PAGE_TABLE_SIZE;

        // Use translate() to find the physical page corresponding to the virtual address
        pte_t *page_table = translate(page_directory, va + (i * PGSIZE));
        
        // Check if the translation was successful
        if (page_table == NULL) 
        {
            return -1;  // Translation failed
        }

        // Check if the page table entry is valid
        if (page_table[pt_index] == 0) 
        {
            return -1;  // Invalid translation
        }

        // Calculate the physical address
        unsigned int physical_page = page_table[pt_index];
        unsigned int offset = (unsigned int)(va + (i * PGSIZE)) % PGSIZE;
        unsigned int physical_address = (physical_page * PGSIZE) + offset;

        // Copy data from the source buffer to the physical page
        memcpy((void *)physical_address, val + (i * PGSIZE), PGSIZE);
    }

    return 0;  // Successful data copy

    /*return -1 if put_value failed and 0 if put is successfull*/
}

/*Given a virtual address, this function copies the contents of the page to val
@Author - Taj*/
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
@Author - Taj
*/
void mat_mult(void *mat1, void *mat2, int size, void *answer)
{

    /* Hint: You will index as [i * size + j] where  "i, j" are the indices of the
     * matrix accessed. Similar to the code in test.c, you will use get_value() to
     * load each element and perform multiplication. Take a look at test.c! In addition to
     * getting the values from two matrices, you will perform multiplication and
     * store the result to the "answer array"
     */
    int x, y, val_size = sizeof(int);
    /*
    int i, j, k;
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            unsigned int a, b, c = 0;
            for (k = 0; k < size; k++)
            {
                int address_a = (unsigned int)mat1 + ((i * size * sizeof(int))) + (k * sizeof(int));
                int address_b = (unsigned int)mat2 + ((k * size * sizeof(int))) + (j * sizeof(int));
                get_value((void *)address_a, &a, sizeof(int));
                get_value((void *)address_b, &b, sizeof(int));
                // printf("Values at the index: %d, %d, %d, %d, %d\n",
                //     a, b, size, (i * size + k), (k * size + j));
                c += (a * b);
            }
            int address_c = (unsigned int)answer + ((i * size * sizeof(int))) + (j * sizeof(int));
            // printf("This is the c: %d, address: %x!\n", c, address_c);
            put_value((void *)address_c, (void *)&c, sizeof(int));
        }
    }
    */

    int i, j, k;

    for (i = 0; i < size; i++) 
    {
        for (j = 0; j < size; j++) 
        {
            unsigned int c = 0;

            for (k = 0; k < size; k++) {
                // Calculate addresses for elements in matrices mat1 and mat2
                int address_a = (unsigned int)mat1 + ((i * size * sizeof(int))) + (k * sizeof(int));
                int address_b = (unsigned int)mat2 + ((k * size * sizeof(int))) + (j * sizeof(int));

                // Use get_value to retrieve values from matrices mat1 and mat2
                unsigned int a, b;
                get_value((void *)address_a, &a, sizeof(int));
                get_value((void *)address_b, &b, sizeof(int));

                // Perform matrix multiplication
                c += (a * b);
            }

            // Calculate address for the corresponding element in the answer matrix
            int address_c = (unsigned int)answer + ((i * size * sizeof(int))) + (j * sizeof(int));

            // Store the result of multiplication in the answer matrix using put_value
            put_value((void *)address_c, (void *)&c, sizeof(int));
        }
    }
}
