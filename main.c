#include <stdio.h>
#include <string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "client.h"

int deposit_queue[1024];
int index_deposit = 0;
int index_accs = 0;


char str[64];

void read_by_line ()
{
	char string_to_id[64];
	while ( str[0] == 'a' )
	{
		// a1 type business d 0 w 5 t 10 transactions 20 2 overdraft Y 40
		int id = str[1] - '0';
		scanf("%s", str); // type
		char account_type[64];
		scanf("%s", account_type); // business
		scanf("%s", str); // d
		scanf("%s", str);
		int fd = atoi(str); // 0
		scanf("%s", str); // w
		scanf("%s", str);
		int wd = atoi(str); // 5
		scanf("%s", str); // t
		scanf("%s", str);
		int td = atoi(str); // 10
		scanf("%s", str); // transactions
		scanf("%s", str);
		int trans = atoi(str); // 20
		scanf("%s", str);
		int overfee = atoi(str); // 2
		scanf("%s", str); // overdraft
		scanf("%s", str); // Y
		int isOver;
		int over_draft_fee;
		if ( str[0] == 'Y' )
		{
			isOver = 1;
			scanf("%s", str);
			over_draft_fee = atoi(str); // 40
		} else if ( str[0] == 'N' )
		{
			isOver = 0;
			over_draft_fee = 0;
		}
		// acc * init_acc ( int id, const char* type, int fd, int fw, int ft, int MAX, int over_fee, int overAllowed, int over_Draft_fee )
		acc * a = init_acc(id, account_type, fd, wd, td, trans, overfee, isOver, over_draft_fee);
		accs[id] = a;
		id > index_accs? index_accs = id : id;
		scanf("%s", str);
	}

	while (str[0] == 'd')
	{
		char c = '0'; char d = '0';
		while (c!='\n' && d!='\n')
		{
			scanf("%s%c", str, &c);
			if(c == 0)
				break;
			scanf("%s%c", str, &d);
			if(d == 0)
				break;
			int id = atoi(memcpy(string_to_id, str + 1, sizeof(str)));
			if(id == 0)
				break;
			scanf("%s%c", str, &c);
			int value = atoi(str);

			deposit_queue[index_deposit++] = id;
			deposit_queue[index_deposit++] = value;
		}
		scanf("%s", str);
	}

	while (str[0] == 'c')
	{
		int id = atoi(memcpy(string_to_id, str + 1, sizeof(str)));
		// update id
		id > index_clients ? index_clients = id : id;
		char c;
		scanf("%s%c", str, &c);
		while (c!='\n')
		{
			if(str[0] == 'd')
			{
				scanf("%s%c", str, &c);
				memcpy(string_to_id, str + 1, sizeof(str));
				int acc_id = atoi(string_to_id);
				scanf("%s%c", str, &c);
				int amount = atoi(str);
				trans_add(clis[id], 'd', acc_id, -1, amount);
			}

			else if(str[0] == 'w')
			{
				scanf("%s%c", str, &c);
				memcpy(string_to_id, str + 1, sizeof(str));
				int acc_id = atoi(string_to_id);
				scanf("%s%c", str, &c);
				int amount = atoi(str);
				trans_add(clis[id], 'w', acc_id, -1, amount);
			}

			else if(str[0] == 't')
			{
				scanf("%s%c", str, &c);
				int acc_id = atoi(memcpy(string_to_id, str + 1, sizeof(str)));
				scanf("%s%c", str, &c);
				int acc_id_2 = atoi(memcpy(string_to_id, str + 1, sizeof(str)));
				scanf("%s%c", str, &c);
				int amount = atoi(str);
				trans_add(clis[id], 't', acc_id, acc_id_2, amount);
			}
			if(c == 0 || c == '\n')
				break;
			else
				scanf("%s%c", str, &c);
		}
		scanf("%s", str);
	}
}

int main ()
{

	init_clis(MAX_CLIENT);

	scanf("%s", str);
	read_by_line ();

//	for(int i = 0; i < index_deposit; i++)
//		printf("%d ", deposit_queue[i]);
//
//	for(int i = 1; i < index_accs; i++)
//		print_acc(accs[i]);
//
//	for(int i = 1; i < index_clients + 1; i++)
//		print_cli(clis[i]);

	for(int i = 1; i < index_accs; i++)
		print_acc_reduced(accs[i]);

}

