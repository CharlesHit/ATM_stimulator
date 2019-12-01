//
// Created by Charles on 2019-11-29.
//

#ifndef CS3305_3_ACCOUNT_H
#define CS3305_3_ACCOUNT_H

#include <stdlib.h>
#include <math.h>

enum account_type { business = 0, personal = 1 };

#define business 0
#define personal 1

#define cant_overdraft 0

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

acc * init_account ( int id, int type, int fd, int fw, int ft, int MAX, int remain, int overAllowed )
{
	acc * a = ( acc * ) malloc(sizeof(acc));
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

/*
 * add and over_draft_helper
 *
 * Do addition on an account, with money n and corresponding fee
 *
 * Always cooperate with transaction_with_exclusive_lock(), since that function want to add
 *   money or withdraw and transfer money
 *
 * add's responsibility is
 * 1. do with fee and overfee
 * 2. do with over draft fee
 *
 * the pre_fee is used to evaluate the case if I have been delete this amount of money, will I still be good?
 * Majorly in the over_draft calculation case
 *
 *
 * over_draft_helper's responsibility is:
 *
 * we know sometimes the over_draft_fee may overflow, and sometimes when you add the over_draft_fee
 *  the whole amount is < -5000. You need to check.
 *
 *  you have to make a helper function and do it RECURSIVELY!!! Why?
 *
 */
int over_draft_helper ( acc * a, int over_draft, int pre_amount )
{
	int previous = a->balance + pre_amount;
	int over = previous - over_draft;

	int time_1;
	if ( previous >= 0 )
		time_1 = 0;
	else
		time_1 = floor(1.0 * previous / OVER_PENALTY_PERIOD);
	int time_2 = floor(1.0 * over / OVER_PENALTY_PERIOD);
	if (over < OVER_MAX)
		return ERROR;
	else
		if ( time_1 != time_2 )
		{
			int new_over_d = ( time_1 - time_2 ) * a->over_Draft_fee + over_draft;
			// You have to do it RECURSIVELY! Why?
			return over_draft_helper(a, new_over_d, over);
		}
		else
			return over_draft;
}

int add ( acc * a, int n, int fee, int pre_amount )
{
	if ( n == 0 )
		return 0;
	// if it reached the allowed maximum transaction, you have to pay over fee
	if ( a->remain_trans <= 0 )
		fee += a->over_fee;

	// if everything is fine, no negative, do it
	if ( a->balance + n - fee + pre_amount >= 0 )
	{
		a->balance = a->balance + n - fee + pre_amount;
		a->remain_trans--;
		return n - fee + pre_amount;
	} else
	{
		// if cannot over draft, stop
		if ( a->isOverdraft == cant_overdraft || a->balance + n - fee + pre_amount < OVER_MAX )
			return ERROR;

		int over_money = a->balance + n - fee + pre_amount;

		int time_1;
		if ( a->balance >= 0 )
			time_1 = 0;
		else
			time_1 = floor(1.0 * a->balance / OVER_PENALTY_PERIOD);
		int time_2 = floor(1.0 * over_money / OVER_PENALTY_PERIOD);

		int over_draft_fee = 0;
		if ( time_1 != time_2 )
		{
			over_draft_fee = ( time_1 - time_2 ) *
			                 a->over_Draft_fee; // time_1 = 0, time_2 = -1; notice time 1 is always less than time 2
		}
		// then calculate the overdraft fee.
		// we use over_draft_helper to check the over_draft_fee
		if (( over_draft_fee
				= over_draft_helper(a, over_draft_fee, n - fee))
		            != ERROR )
		{
			a->balance = a->balance + n - fee - over_draft_fee + pre_amount;
		} else
			return ERROR;
		a->remain_trans--;
		return n - fee - over_draft_fee + pre_amount;
	}
}


#endif //CS3305_3_ACCOUNT_H
