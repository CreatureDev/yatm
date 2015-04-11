#include "../include/v_memory.h"
#include <stdlib.h>
#include <stdio.h>

#define MEM_SIZE 0x400000
#define BLK_SIZE 0x40
#define FAKE_BLK 0x1


/* This file manages the memory that is used
 * by the visual system. It does not directly
 * manage the internals of the memory, it simply
 * fulfilles the requests for memory from other
 * processes within the visual system.
 *
 * It handles CPU memory only and is not related
 * to the management of programs, textures or buffers.
*/

static void *mem = '\0';
static void *res = '\0';
static unsigned int n = 0;

typedef struct blk_s blk_t;

struct blk_s
{
	blk_t *next;
	blk_t *prev;
	unsigned long bid;
	unsigned long len;
};

static blk_t **open;

static void V_split(unsigned int ord)
{
	blk_t *blk, *bud;
	unsigned long buid;
	if(ord > n || open[ord-1])
		return;
	if(!open[ord])
	{
		V_split(ord+1);
		if(!open[ord])
			return;
	}
	blk = open[ord];
	if(blk->next == (blk_t *) FAKE_BLK)
		open[ord] = '\0';
	else
	{
		open[ord] = blk->next;
		blk->next->prev = (blk_t *) FAKE_BLK;
	}
	blk->len = blk->len >> 1;
	buid = blk->bid ^ blk->len;
	bud = (blk_t *) res;
	bud = &bud[buid];
	blk->next = bud;
	bud->prev = blk;
	bud->next = (blk_t *) FAKE_BLK;
	bud->len = blk->len;
	open[ord-1] = blk;
}

void *V_alloc(unsigned long size)
{
	blk_t *blk;
	if(size > MEM_SIZE)
		return '\0';
	unsigned int ord = 0;
	while((BLK_SIZE << ord) < size && ord < n)
		ord++;
	if(!open[ord])
	{
		V_split(ord+1);
		if(!open[ord])
			return '\0';
	}
	blk = open[ord];
	if(blk->next == (blk_t *) FAKE_BLK)
	{
		open[ord] = '\0';
	}
	else
	{
		open[ord] = blk->next;
		blk->next->prev = (blk_t *) FAKE_BLK;
	}
	blk->next = blk->prev = '\0';
	return (mem + BLK_SIZE * blk->bid);
}

static void V_insertcombine(blk_t *new)
{
	unsigned long buid;
	blk_t *bud = (blk_t *) res;
	buid = new->bid ^ new->len;
	bud = &bud[buid];
	if(bud->len == new->len && bud->next && bud->prev)
	{	
		if(bud->next == bud->prev)
		{
			buid = 0;
			while((BLK_SIZE << buid) < new->len)
				buid++;
			open[buid] = '\0';
		}
		else
		if(bud->prev == (blk_t *) FAKE_BLK)
		{
			buid = 0;
			while((BLK_SIZE << buid) < new->len)
				buid++;
			open[buid] = bud->next;
			open[buid]->prev = (blk_t *) FAKE_BLK;
		}
		else
		if(bud->next == (blk_t *) FAKE_BLK)
		{
			bud->prev->next = bud->next;
		}
		else
		{
			bud->prev->next = bud->next;
			bud->next->prev = bud->prev;
		}
		if(bud->bid < new->bid)
		{
			new->len = 0;
			bud->len = bud->len << 1;
			V_insertcombine(bud);
		}
		else
		{
			bud->len = 0;
			new->len = new->len << 1;
			V_insertcombine(new);
		}
	}
	else
	{
		buid = 0;
		while((BLK_SIZE << buid) < new->len)
			buid++;
		bud = open[buid];
		if(!bud)
		{
			open[buid] = new;
			new->next = new->prev = (blk_t *) FAKE_BLK;
			return;
		}
		while(bud->bid < new->bid && bud->next != (blk_t *) FAKE_BLK)
			bud = bud->next;
		if(bud->bid < new->bid)
		{
			new->next = bud->next;
			bud->next = new;
			new->prev = bud;
		}
		else
		if(bud->prev == (blk_t *) FAKE_BLK)
		{
			open[buid] = new;
			new->prev = bud->prev;
			new->next = bud;
			bud->prev = new;
		}
		else
		{
			new->prev = bud->prev;
			new->prev->next = bud->prev = new;
			new->next = bud;
		}
	}
}

void V_free(void *addr)
{
	unsigned long id;
	blk_t *lst = (blk_t *) res;
	if(addr < mem || addr > mem + MEM_SIZE - BLK_SIZE)
		return;
	id = (unsigned long) (addr - mem) / BLK_SIZE;
	V_insertcombine(&lst[id]);
}

void V_memoryinit()
{	
	unsigned long i;
	blk_t *lst;
	if(mem || res)
		return;
	i = (MEM_SIZE/BLK_SIZE) * sizeof(blk_t) + (n+1) * sizeof(blk_t *);
	mem = malloc(MEM_SIZE);
	while((MEM_SIZE >> n) > BLK_SIZE)
		n++;
	res = malloc((MEM_SIZE/BLK_SIZE) * sizeof(blk_t) + (n+1) * sizeof(blk_t *));
	open = (blk_t **) (res + (MEM_SIZE/BLK_SIZE) * sizeof(blk_t));
	lst = (blk_t *) res;
	for(i = 0; i < (MEM_SIZE/BLK_SIZE); i++)
	{
		lst[i].next = lst[i].prev = '\0';
		lst[i].bid = i;
		lst[i].len = 0;
	}
	for(i = 0; i < n; i++)
	{
		open[i] = '\0';
	}
	open[i] = lst;
	open[i]->len = (MEM_SIZE/BLK_SIZE);
	open[i]->next = open[i]->prev = (blk_t *) FAKE_BLK;
}


void V_memoryclose()
{
	if(mem)
		free(mem);
	if(res)
		free(res);
	mem = res = open = '\0';
	n = 0;
}


