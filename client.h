//
// Created by Charles on 2019-11-29.
//

#ifndef CS3305_3_CLIENT_H
#define CS3305_3_CLIENT_H

#define MAX_TRANS 1024
#define MAX_CLIENT 1024
#define ERROR 1021
#define OVER_PENALTY_PERIOD 500
#define OVER_MAX -5000
#define thread_cli_generate() rand()

#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "account.h"

// used in transaction_with_exclusive_lock
pthread_mutex_t lock;

acc * init_acc ( int id, const char * type, int fd, int fw, int ft, int MAX, int over_fee, int overAllowed,
                 int over_Draft_fee )
{
	acc * a = ( acc * ) malloc(sizeof(acc));
	a->id = id;
	if ( type[0] == 'b' )
		a->type = 0;
	else
		a->type = 1;
	a->fee_d = fd;
	a->fee_w = fw;
	a->fee_t = ft;
	a->MAX_trans = MAX;
	a->over_fee = over_fee;
	a->remain_trans = MAX;
	a->isOverdraft = overAllowed;
	a->over_Draft_fee = over_Draft_fee;

	return a;
}

void print_acc ( acc * a )
{
	printf("\naccount %d, type: %d:\n  - f %d t %d w %d\n  - max trans %d over_fee %d\n  could over? %d penalty: %d",
	       a->id, a->type, a->fee_d, a->fee_t, a->fee_w, a->MAX_trans, a->over_fee, a->isOverdraft, a->over_Draft_fee);
}

FILE * fp;
void print_acc_reduced ( acc * a )
{
	char type[16];
	switch ( a->type )
	{
		case 0:
			memcpy(type, "business", 9);
			break;
		case 1:
			memcpy(type, "personal", 9);
			break;
	}
	printf("a%d type %s %d\n", a->id, type, a->balance);
	fprintf(fp, "a%d type %s %d\n", a->id, type, a->balance);
}

// transaction
enum transaction_type { d = 0, w = 1, t = 2, not_applicable = ERROR };
typedef struct
{
	enum transaction_type type;
	unsigned int acc_1;
	unsigned int acc_2;
	int num;
} tra;

tra * init_tra ( int type, int acc_1, int acc_2, int num )
{
	tra * t = ( tra * ) malloc(sizeof(tra));
	t->type = type;
	t->acc_1 = acc_1;
	t->acc_2 = acc_2;
	t->num = num;

	return t;
}

// client
typedef struct
{
	unsigned int id;
	unsigned int cur; // cur transactions
	unsigned int n; // cur transaction
	tra t[MAX_TRANS];
} cli;
// the clis array and the accs array
cli * clis[MAX_CLIENT];
int index_clients = 0;
acc * accs[MAX_CLIENT];

cli * init_cli ( int i )
{
	cli * c = ( cli * ) malloc(sizeof(cli));
	c->id = i;
	c->n = 0;
	c->cur = 0;
	tra * NULL_tra = ( tra * ) malloc(sizeof(tra));
	NULL_tra->num = 0;
	NULL_tra->acc_1 = ERROR;
	NULL_tra->acc_2 = ERROR;
	NULL_tra->type = not_applicable;
	for ( unsigned int j = 0; j < MAX_TRANS; j++ )
	{
		c->t[j] = *NULL_tra;
	}
	return c;
}

void print_cli ( cli * c )
{
	printf("\nclient %d,\n", c->id);

	// traverse the transaction array inside a client
	for ( unsigned int i = 0; i < c->n; i++ )
		if ( c->t[i].acc_2 != ERROR )
			printf("  * %d: %d --%d-> %d\n", ( int ) c->t[i].type, c->t[i].acc_1, c->t[i].num, c->t[i].acc_2);
		else
			printf("  * %d: %d -> %d\n", ( int ) c->t[i].type, c->t[i].acc_1, c->t[i].num);
}

// client array
void init_clis ( int n )
{
	for ( int i = 0; i < n; i++ )
	{
		cli * c = init_cli(i);
		clis[i] = c;
	}
}

// client and account's interaction
void trans_add ( cli * c, char type, int acc_id_1, int acc_id_2, int amount )
{
	if ( type == 'd' )
		c->t[c->n].type = d;
	else if ( type == 'w' )
		c->t[c->n].type = w;
	else if ( type == 't' )
		c->t[c->n].type = t;

	c->t[c->n].num = amount;
	c->t[c->n].acc_1 = acc_id_1;
	c->t[c->n].acc_2 = acc_id_2;

	c->n += 1;
}

void print_trans ( tra transac )
{
	if ( transac.type == t )
		printf("  * %d: %d --%d-> %d\n", ( int ) transac.type, transac.acc_1, transac.num, transac.acc_2);
	else
		printf("  * %d: %d -> %d\n", ( int ) transac.type, transac.acc_1, transac.num);
}

int FINISHED_TRANS = 0;
int TOTAL_TRANS = 0;

void * transaction_with_exclusive_lock ( cli * c, unsigned cur )
{
	if ( cur >= c->n || FINISHED_TRANS > TOTAL_TRANS )
		return ( void * ) 0;
	// printf("client: %d, %d / %d, finished %d\n", c->id, c->n, c->cur+1, FINISHED_TRANS);
	tra trans = c->t[cur];

	acc * acc_1 = accs[trans.acc_1];
	acc * acc_2 = accs[trans.acc_2];
	int money = trans.num;

	int fee;

	if ( trans.type == d )
	{
		fee = acc_1->fee_d;
		add(acc_1, money, fee, 0);
	} else if ( trans.type == w )
	{
		fee = acc_1->fee_w;
		add(acc_1, -1 * money, fee, 0);
	} else if ( trans.type == t )
	{
		fee = acc_1->fee_t;
		int fee2 = acc_2->fee_t;
		if ( add(acc_1, -1 * money, fee, 0) != ERROR )
			add(acc_2, money, fee2, 0);
	}
	c->id == 0? c->id : FINISHED_TRANS++;
	return ( void * ) 0;
}

#endif //CS3305_3_CLIENT_H
