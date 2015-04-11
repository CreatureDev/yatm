#include "../include/l_memory.h"

#include <stdlib.h>

#define MEM_SIZE 0x200000
#define BLK_SIZE 0x20

typedef struct blk_s blk_t;

struct blk_s
{
	blk_t *next;
	unsigned long bid;
	unsigned long len;
};

static void *res = '\0';
static void *mem = '\0';
static unsigned int n = 0;
static blk_t **open;

static void L_split(unsigned int ord)
{
	blk_t *dis, *dat;
	unsigned long bud;
	if(ord > n || open[ord-1])
		return;
	if(!open[ord])
	{
		L_split(ord+1);
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
	dat->next = '\0';
	open[ord-1] = dis;
}

void *L_alloc(unsigned long size)
{
	blk_t *lst;
	unsigned int ord = 0;
	if(size > MEM_SIZE)
		return 0;
	while((BLK_SIZE << ord) < size && ord < n)
		ord++;
	if(!open[ord])
	{
		L_split(ord+1);
		if(!open[ord])
			return 0;
	}
	lst = open[ord];
	open[ord] = lst->next;
	lst->next = '\0';
	return (mem + lst->bid * BLK_SIZE);
}

static void L_insertcombine(blk_t *new)
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
		L_insertcombine(new);
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
			L_insertcombine(new);
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

void L_free(void *ptr)
{
	blk_t *new;
	unsigned long id;
	if(ptr < mem || ptr > mem + MEM_SIZE - BLK_SIZE)
		return;
	id = (unsigned long) (ptr - mem) / BLK_SIZE;
	new = (blk_t *) res;
	L_insertcombine(&new[id]);
}

void L_memoryinit()
{
	unsigned long i;
	blk_t *lst;
	if(res || mem)
		return;
	while((MEM_SIZE >> n) > BLK_SIZE)
		n++;
	mem = malloc(MEM_SIZE);
	res = malloc((MEM_SIZE / BLK_SIZE) * sizeof(blk_t) + (n+1) * sizeof(blk_t *));
	open = (blk_t **) (res + (MEM_SIZE/BLK_SIZE) * sizeof(blk_t));
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
	open[i]->next = '\0';
}

void L_memoryclose()
{
	if(res)
		free(res);
	if(mem)
		free(mem);
	res = mem = open = '\0';
	n = 0;
}

