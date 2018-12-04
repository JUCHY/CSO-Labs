/*
 * mm.c -- implement your malloc functions here. See mm-naive.c for an example implementation.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"
#define NLISTS  3
#define MIN_OS_ALLOC_SZ 1024



//header struct
typedef struct
{
	unsigned long size_and_status;
	unsigned long padding;	
}header;
//freeheader for freelit, with next freelist and previous freelist pointer
typedef struct free_hdr
{
	header common_header;
	struct free_hdr *next;
	struct free_hdr *prev;
}free_hdr;

//gets status of block from header
bool
get_status(header* h){
	return h->size_and_status & 0x1L;
}
//gets size of block from header
size_t
get_size(header* h){
	return h->size_and_status & ~ (0x1L);
}
//sets the size and status of header
void
set_size_status(header *h, size_t sz, bool status){
	h->size_and_status = sz | status;
}
//sets just the status of header
void
set_status(header *h, bool status){
	size_t sz = get_size(h);
	set_size_status(h, sz, status);
}
//sets just the size of header
void
set_size(header *h, size_t sz){
	bool status = get_status(h);
	set_size_status(h, sz, status);
}
//gets the footer of block
header*
get_footer_from_header(header *h){
	return (header *)((char*)h + get_size(h) - sizeof(header));
}

//initializes free_block
void
init_free_chunk(free_hdr *h, size_t sz){
	set_size_status(&(h->common_header), sz, false);
	h-> prev = h->next = NULL;
	set_size_status(get_footer_from_header(&(h->common_header)), sz, false);
}
//gets header from payload ptr
header *
get_header_from_payload(void* ptr){
	return (header*)((char*)ptr - sizeof(header));
}

//get the prev_header in heap
header *
get_prev_header(header *h){
	header *footer = (header *)((char*)h - sizeof(header));
	size_t chunksize = get_size(footer);
	return (header *)((char *)h - chunksize);
}

//gets the next header in heap
header *
get_next_header(header *h){

	return (header *)((char *)h + get_size(h));
}
free_hdr* freelist;

//initializes heap
free_hdr* 
get_block_from_OS(size_t sz){

	free_hdr *h = mem_sbrk(sz);
	init_free_chunk(h, sz);//init header and footer
	return h;

}

/* 
 * mm_init initializes the malloc package.
 */
int mm_init(void)
{
	freelist=get_block_from_OS(MIN_OS_ALLOC_SZ);
	return 0;
}

//deletes block from freelist
void delete(free_hdr **head, free_hdr *node){
	assert(node>mem_heap_lo());
	assert(node<mem_heap_hi());

	if(node->prev && node->prev->next && node ->next){//node is not the first node in the list
		assert(node->prev!=NULL);
		assert(node->next!=NULL);
		assert(node->prev->next!=NULL);
	
		//printf("%d/n", node);
		node->prev->next = node->next;
		if(node->next){
			node-> next->prev = node ->prev;
		}
	}
		else{ //delete the first node in the list
			*head = node->next;
			assert(*head<mem_heap_hi());
			assert(node>mem_heap_lo());
			if(node->next){
				node->next->prev = NULL;
			}
		}


	}


//searches for first freeblock that fits size
free_hdr *
first_fit(size_t sz){
	free_hdr *first_fit = NULL;
	free_hdr *n = freelist;
	while(n<mem_heap_hi()&& n>mem_heap_lo()){

		if(get_size(&(n->common_header))>=sz){
			if(((void*)n-mem_heap_hi()>32)){
				n = n->next;
				continue;
			}
			delete(&freelist, n);
			first_fit = n;
			break;
		}

		n = n->next;
	}
	return first_fit;
}



//inserts block into freelist
void
insert(free_hdr **head, free_hdr *node){
	assert(node<mem_heap_hi());
	assert(node>mem_heap_lo());

	if(*head)
		(*head)->prev = node;
	assert(*head<mem_heap_hi());
	node->next = *head;
	*head = node; //node becomes the new head
}

//splits large freeblock into two.
free_hdr*
split(free_hdr* h,size_t sz){
	size_t freesize = get_size((header *)h);
	if(sz == ((void*)h - mem_heap_hi())){
		//printf("checktrue\n");
	}
	else{
		//printf("checkfalse\n");
	}
	if(freesize>sz && freesize-sz > 48){
		assert(is_aligned(freesize-sz));
		assert(freesize>sz);
		set_size_status((header *)h, sz, true);
		header *footer = get_footer_from_header((header *)h);
		assert(footer<mem_heap_hi());
		set_size_status(footer, sz, true);
		header *next_block= get_next_header((header *)h);
		assert(next_block<mem_heap_hi());
		init_free_chunk(next_block, freesize-sz);
		return (free_hdr*)((char*)h+sz);
	}
	else{
		//printf("check")
		return NULL;
	}
}



/* 
 * mm_malloc allocates a memory block of size bytes
 */
void *
mm_malloc(size_t s)
{
	size_t csz = align(s)+2*sizeof(header); //min chunk size required
	free_hdr *n = first_fit(csz);
	//printf("first fit\n");
	if(!n){
		n = get_block_from_OS(csz>MIN_OS_ALLOC_SZ?csz:MIN_OS_ALLOC_SZ);
		set_status((header *)n, true);
		//printf("%d\n", get_size((header *)n));
		//printf("test \n");
		return (char *)n + sizeof(header);
	}

	free_hdr *newchunk = split(n,csz);
	if(!(csz>MIN_OS_ALLOC_SZ) && newchunk < mem_heap_hi() && newchunk>mem_heap_lo()){
		insert(&freelist, newchunk);
		//printf("%d freesize\n",get_size((header *)newchunk));
	}
	if(newchunk < mem_heap_hi() && newchunk>mem_heap_lo()){
		set_status((header *)n, true);
		set_status(get_footer_from_header((header*)n), true);
	}
	//printf("test \n");
	return (char *)n+sizeof(header);
}

//coalesce two free_blocks into
//didn't actually use since couldn't get it to work, brought erros
free_hdr *
coalesce(free_hdr *me, free_hdr *other){
	delete(&freelist, other);
	int sum = get_size(&(me->common_header))+get_size(&(other->common_header));
	free_hdr *h = me<other? me:other;
	set_size_status((header*)h, sum, false);
	set_size_status(get_footer_from_header((header *)h), sum, false);
	h->next = h->prev = NULL;
	return h;
}
/*
 * mm_free frees the previously allocated memory block
 */
void mm_free(void *ptr)
{
	header *h = get_header_from_payload(ptr);
	init_free_chunk((free_hdr *)h, get_size(h));
	/*header *next = get_next_header(h);
	assert(h == &(((free_hdr*)h)->common_header));
	assert(h == get_next_header(get_prev_header(h)));
	if(!get_status(next) && next < mem_heap_hi() && next > mem_heap_lo()){
		assert(next<mem_heap_hi());
		h = coalesce((free_hdr*)h, (free_hdr *)next);
	}
	header *prev = get_prev_header(h);
	if(!get_status(prev)&& prev < mem_heap_hi() && prev > mem_heap_lo())
		h= coalesce((free_hdr *)h, (free_hdr*)prev);*/

	insert(&freelist, (free_hdr *)h);
	
}	

/*
 * mm_realloc changes the size of the memory block pointed to by ptr to size bytes.  
 * The contents will be unchanged in the range from the start of the region up to the minimum of   
 * the  old  and  new sizes.  If the new size is larger than the old size, the added memory will   
 * not be initialized.  If ptr is NULL, then the call is equivalent  to  malloc(size).
 * if size is equal to zero, and ptr is not NULL, then the call is equivalent to free(ptr).
 */
void *mm_realloc(void *ptr, size_t size)
{
	if(size>0){
		if(ptr==NULL){
			return malloc(size);
		}
		header *h = get_header_from_payload(ptr);
		size_t copySize =get_size(h) - sizeof(header);
		if(size<copySize){
			copySize = size;
			set_size(h,copySize);
			header* foot = get_footer_from_header(h);
			set_size_status(foot, copySize, true);

		}
		if(size>copySize){
			void* newptr = malloc(size);
			memcpy(newptr, ptr, copySize);

		}
		if(size == copySize){
			return ptr;
		}

	}
	else if(ptr!=NULL){
		mm_free(ptr);
	}
	return NULL;
	// Your code here 
}


/*
 * mm_checkheap checks the integrity of the heap and helps with debugging
 */
void mm_checkheap(int verbose_level) 
{
	//Prints out all freelists and their size, and checks if they're allocated correctly(footer and header)
	free_hdr *n = freelist;
	while(n!=NULL){
		size_t headersz = get_size((header *)n);
		size_t footersz = get_size(get_footer_from_header((header *)n));
		//bool szstatus = (headersz == footersz);
		bool headerstatus = get_status((header *)n)==false;
		bool freeblockstatus = false;
		if(n>=mem_heap_lo() && n<=mem_heap_hi()){
			freeblockstatus = true;
		}
		printf("Header status is set to the correct status: %d Size of freeblock according to header:%d Size of freeblock according to footer %d Freeblock is within memheap: %d\n", headerstatus, headersz, footersz,freeblockstatus );

		n = n->next;
	}
}

