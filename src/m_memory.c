#include "../include/m_memory.h"
#include <stdio.h>
#include <stdlib.h>

#define MEM_SIZE 0x100000
#define BLK_SIZE 0x10

static void *res = '\0';
static void *mem = '\0';
static unsigned int n = 0;

typedef struct blk_s blk_t;


/* If this is turned into a doubly linked list
 * the efficiency of combining would be made constant
 * though the efficiency would still remain linear
 * to maintain a sorted order
*/
struct blk_s
{
	blk_t *next;
	unsigned long bid;
	unsigned long len;
};

static blk_t **open;

/* combines the recently free'd node with its
 * buddy if possible and inserts the free'd space
 * into the list to be re-allocated
*/

static void M_insertcombine(blk_t *new)
{
	blk_t *cur, *prv;
	unsigned long bud, ord;
	bud = new->bid ^ new->len;
	ord = 0;
	while(!((new->len >> ord) & 1))
		ord++;
	if(!open[ord])
	{
		open[ord] = new;
		return;
	}
	cur = open[ord];
	if(cur->bid == bud)
	{
		open[ord] = cur->next;
		cur->next = '\0';
		if(new->bid < cur->bid)
		{
			new->len = new->len << 1;
			cur->len = 0;
		}
		else
		{
			new->len = 0;
			cur->len = cur->len << 1;
			new = cur;
		}
		M_insertcombine(new);
		return;
	}
	if(cur->bid > new->bid)
	{
		new->next = cur;
		open[ord] = new;
		return;
	}
	while(cur->next)
	{
		prv = cur;
		cur = cur->next;
		if(cur->bid == bud)
		{
			prv->next = cur->next;
			cur->next = '\0';
			if(new->bid < cur->bid)
			{
				new->len = new->len << 1;
				cur->len = 0;
			}
			else
			{
				new->len = 0;
				cur->len = cur->len << 1;
				new = cur;
			}
			M_insertcombine(new);
			return;
		}
		if(cur->bid > new->bid)
		{
			new->next = cur;
			prv->next = new;
			return;
		}
	}
	cur->next = new;
}

void M_free(void *addr)
{
	if(addr < mem || addr > mem + MEM_SIZE - BLK_SIZE)
		return;
	blk_t *new;
	unsigned long id;
	id = (unsigned long) (addr - mem) / BLK_SIZE;
	new = (blk_t *) res;
	M_insertcombine(&new[id]);
}

static void M_split(unsigned int ord)
{
	blk_t *dis, *dat;
	unsigned long bud;
	if(ord > n || open[ord-1])
		return;
	if(!open[ord])
	{
		M_split(ord+1);
		if(!open[ord])
			return;
	}
	dis = open[ord];
	open[ord] = dis->next;
	dis->len = dis->len >> 1;
	bud = dis->bid ^ dis->len;
	dat = (blk_t *) res;
	dat = &dat[bud];
	dat->len = dis->len;
	dis->next = dat;
	open[ord-1] = dis;
}

void *M_alloc(unsigned long size)
{
	blk_t *lst;
	if(size > MEM_SIZE)
	{
		printf("Warning: request to allocate too many resources\n");
		return '\0';
	}
	unsigned int ord = 0;
	while((BLK_SIZE << ord) < size && ord < n)
		ord++;
	if(!open[ord])
	{
		M_split(ord + 1);
		if(!open[ord])
			return '\0';
	}
	lst = open[ord];
	open[ord] = lst->next;
	lst->next = '\0';
	return (mem + lst->bid * BLK_SIZE);
}

void M_memoryinit()
{
	unsigned long i;
	blk_t *lst;
	if(mem || res)
		return;
	if(MEM_SIZE < BLK_SIZE)
	{
		printf("ERROR: ATTEMPTING TO RUN WITH LESS THAN ONE BLOCK OF MEMORY\n");
		exit(-1);
	}
	while((MEM_SIZE >> n) > BLK_SIZE)
		n++;
	mem = malloc(MEM_SIZE);
	res = malloc((MEM_SIZE/BLK_SIZE) * sizeof(blk_t) + (n+1) * sizeof(blk_t *));
	open = (blk_t **) (res + (MEM_SIZE/BLK_SIZE) * sizeof(blk_t));
	if(!mem || !res)
	{
		printf("ERROR: COULD NOT ALLOCATE ENOUGH MEMORY\n");
		exit(-1);
	}
	lst = (blk_t *) res;
	for(i = 0; i < (MEM_SIZE/BLK_SIZE); i++)
	{
		lst[i].next = '\0';
		lst[i].bid = i;
		lst[i].len = 0;
	}
	for(i = 0; i < n; i++)
	{
		open[i] = '\0';
	}
	open[i] = lst;
	open[i]->len = (MEM_SIZE/BLK_SIZE);
}

void M_memoryclose()
{
	if(!open[n])
		printf("Warning: some memory still allocated at close\n");
	if(mem)
		free(mem);
	if(res)
		free(res);
	mem = res = open = '\0';
	n = 0;
}
