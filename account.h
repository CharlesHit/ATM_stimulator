//
// Created by Charles on 2019-11-29.
//

#ifndef CS3305_3_ACCOUNT_H
#define CS3305_3_ACCOUNT_H

#include <stdlib.h>

enum account_type { business = 0, personal = 1 };

#define business 0
#define personal 1

#define could_overdraft 0
#define cant_overdraft 1

typedef struct
{
	unsigned int id;
	enum account_type type;
	int balance;
	int fee_d;
	int fee_w;
	int fee_t;
	int MAX_trans;
	int remain_trans;
	int over_fee;
	int isOverdraft;
	int over_Draft_fee;
} acc;

acc* init_account( int id, int type, int fd, int fw, int ft, int MAX, int remain, int overAllowed )
{
	acc* a = (acc*) malloc(sizeof(acc));
	a->id = id;
	a->type = type;
	a->fee_d = fd;
	a->fee_w = fw;
	a->fee_t = ft;
	a->MAX_trans = MAX;
	a->remain_trans = remain;
	a->isOverdraft = overAllowed;

	return a;
}

int add ( acc * a, int n, int fee )
{
	if ( a->isOverdraft == cant_overdraft )
		return ERROR;
	if ( a->remain_trans <= 0 )
		fee += a->over_fee;
	if ( a->balance - n - fee >= 0 )
	{
		a->balance = a->balance - n - fee;
		return n + fee;
	} else
	{
		int over_money = a->balance - n - fee;

		int time = -1 * over_money / OVER_PENALTY;
		time++;
		if (( time - 1 ) * OVER_PENALTY == -1 * over_money )
			time--;

		int overfee = time * a->over_Draft_fee;

		if ( over_money - overfee < -OVER_MAX )
			return ERROR;
		else
		{
			a->balance = a->balance - n - overfee;
			a->remain_trans -= 1;
			return n + overfee;
		}
	}
}


#endif //CS3305_3_ACCOUNT_H
