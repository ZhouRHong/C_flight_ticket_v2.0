#include "my_head.h"


//录入航班
int entry_flight(int connfd, struct ticket_library * ticket_library_head)
{
	char buf[SIZEOF];

	struct ticket_library *new = NULL;
	struct ticket_library *find = NULL;
	struct ticket_library *p = NULL;
	new = (struct ticket_library *)malloc(sizeof(struct ticket_library));
	if (new == NULL)
	{
		printf("malloc2395 error\n");
	}

	printf("请输入你要录入的航班号（例如-A001-A代表机型0001代表编号）：\n");
	rec(connfd, buf);
	strcpy(new->number, buf);

	for (find = ticket_library_head; find != NULL; find = find->next)
	{
		if (strcmp(new->number, find->number) == 0)
		{
			error(connfd, buf);
			free(new);
			printf("该航班号已被使用请重新输入！！！\n");

			return 0;
		}
	}
	next(connfd, buf);


	printf("请输入起点站:\n");
	rec(connfd, buf);
	strcpy(new->staddress, buf);

	printf("请输入终点站:\n");
	rec(connfd, buf);
	strcpy( new->arraddress, buf);


	printf("请输入班期:\n");
	rec(connfd, buf);
	strcpy(new->date, buf);

	printf("请输入机型:\n");
	rec(connfd, buf);
	strcpy( new->type, buf);

	printf("请输入起飞时间:\n");
	rec(connfd, buf);
	strcpy(new->stime, buf);


	printf("请输入票价：\n");
	rec(connfd, buf);
	new->price = atoi(buf);

	printf("请输入总票数：\n");
	rec(connfd, buf);
	new->ticket_num = atoi(buf);


	new->next = NULL;
	for (p = ticket_library_head; p->next != NULL; p = p->next);
	p->next = new;
	//上锁
	pthread_mutex_lock(&m);

	save_flight_fun(new);
	//解锁
	pthread_mutex_unlock(&m);
}




//删除航班后退票
int delete_ticket_fun(int connfd, char *flight_number, struct userinfo * user_name, struct ticket_library * ticket_library_head)
{
	struct user_ticket *p = NULL;
	struct user_ticket *q = NULL;//p q 用于删除节点
	struct user_ticket *delete_ticket = NULL;// 用于删除节点后遍历保存到票库链
	struct ticket_library *ticket = NULL; //用于查找票库的节点给余票加一
	int i = 0;
	char buf[SIZEOF];


	for (q = &user_name->user_ticket_head, p = user_name->user_ticket_head.next; p != NULL; q = p, p = p->next)
	{
		if (strcmp(p->number , flight_number) == 0) //找到要退票的节点
		{
			next(connfd, buf);
			user_name->balance += p->price + 200 ; //退回包括保险费外所有的钱加200赔偿
			//上锁
			pthread_mutex_lock(&m);
			save_userinfo_fun(user_name);
			//解锁
			pthread_mutex_unlock(&m);
			for (ticket = ticket_library_head->next; ticket != NULL; ticket = ticket->next) //遍历票库
			{
				if (strcmp(ticket->number, flight_number) == 0)
				{
					ticket->ticket_num += 1; //对应的余票加一
					//上锁
					pthread_mutex_lock(&m);
					save_flight_fun(ticket);//save
					//解锁
					pthread_mutex_unlock(&m);
					printf("删除航班成功！！！\n");
				}
			}

			q->next = p->next;
			free(p);
			//上锁
			pthread_mutex_lock(&m);
			save_delete_ticket_fun(user_name);//删除后保存到用户票链
			//解锁
			pthread_mutex_unlock(&m);
			return 0;
		}
		else if (p->next == NULL)
		{
			error(connfd, buf);
			printf("没有用户购买此航班，删除航班成功！！！\n");
			return -1;
		}
	}
}

//寻找所有购买了此航班的人
int find_buy_ticket(int connfd,
                    char *flight_number,
                    struct userinfo * userinfo_head,
                    struct ticket_library * ticket_library_head,
                    struct ticket_library * recycle_head)
{
	struct userinfo *p = NULL; //用来查找用户信息
	struct user_ticket *q = NULL; //用来查找用户买的票

	struct ticket_library *a = NULL;
	struct ticket_library *b = NULL;//a b 用来删除航班；
	char buf[SIZEOF];

	for (p = userinfo_head->next; p != NULL; p = p->next)//遍历所有用户
	{

		for (q = p->user_ticket_head.next; q != NULL; q = q->next)//遍历所有用户的所有票
		{
			if (strcmp(q->number, flight_number) == 0)//找到所有购买了此航班的票
			{
				next(connfd, buf);
				delete_ticket_fun(connfd, flight_number, p, ticket_library_head); //退掉删除的票并赔偿200元

				for (a = ticket_library_head, b = ticket_library_head->next; b != NULL; a = b, b = b->next)
				{

					if (strcmp(b->number , flight_number) == 0)
					{
						//把要删除的航班移动到回收站
						char path[50] = {0};
						sprintf(path, "mv ./flight_data/%s.txt ./recycle", b->number);
						DIR *dp = opendir("./flight_data");
						if (dp == NULL)
							printf("打开目录失败!\n");

						closedir(dp);

						a->next = b->next;//把找到的要删除的航班的节点断开接到上一个
						b->next = recycle_head->next;//把断开的节点头插到回收链
						recycle_head->next = b;
						//上锁
						pthread_mutex_lock(&m);
						save_delete_ticket_fun(p);//删除后保存到用户票链
						//解锁
						pthread_mutex_unlock(&m);
						return 0;
					}

				}
				return -1;

			}
			else if (q->next == NULL)
			{
				error(connfd, buf);
				if (p->next != NULL)
				{
					break;
				}

				for (a = ticket_library_head, b = ticket_library_head->next; b != NULL; a = b, b = b->next)
				{

					if (strcmp(b->number , flight_number) == 0)
					{
						//把要删除的航班移动到回收站
						char path[50] = {0};
						sprintf(path, "mv ./flight_data/%s.txt ./recycle", b->number);
						DIR *dp = opendir("./flight_data");
						if (dp == NULL)
							printf("打开目录失败!\n");
						system(path);
						closedir(dp);

						a->next = b->next;//把找到的要删除的航班的节点断开接到上一个
						b->next = recycle_head->next;//把断开的节点头插到回收链
						recycle_head->next = b;


						//上锁
						pthread_mutex_lock(&m);
						save_delete_ticket_fun(p);//删除后保存到用户票链
						//解锁
						pthread_mutex_unlock(&m);

						return 0;
					}
				}
				return -1;

			}

		}
	}

	return 0;
}


//删除航班
int  delete_flight(int connfd,
                   struct userinfo * userinfo_head,
                   struct ticket_library * ticket_library_head,
                   struct ticket_library * recycle_head)
{
	system("clear");
	int i = 0;
	int choise;
	char flight_number[10];
	struct ticket_library *find_flight = NULL; //用于遍历所有航班票库
	char buf[SIZEOF];


	check_all_flight(connfd, ticket_library_head); //先显示所有航班

	printf("请输入你要删除的航班:\n");
	rec(connfd, buf);
	strcpy(flight_number, buf);

	//遍历航班票库
	for (find_flight = ticket_library_head->next; find_flight != NULL; find_flight = find_flight->next)
	{
		if (strcmp(flight_number, find_flight->number) == 0)
		{
			bzero(buf, SIZEOF);
			sprintf(buf, "%s     %s       %s       %s   %s     %s  %d     %d\n",
			        find_flight->number,
			        find_flight->staddress,
			        find_flight->arraddress,
			        find_flight->date,
			        find_flight->type,
			        find_flight->stime,
			        find_flight->price,
			        find_flight->ticket_num
			       );
			send(connfd, buf, SIZEOF, 0);

			printf("请确认是否要删除此航班（0-确认 1-取消）:\n");
label2:
			rec(connfd, buf);

			if (strcmp(buf, "0") == 0)
			{
				next(connfd, buf);
				find_buy_ticket(connfd, flight_number, userinfo_head, ticket_library_head, recycle_head);
				return 0;
			}
			else if (strcmp(buf, "1") == 0)
			{
				bzero(buf, SIZEOF);
				sprintf(buf, "%s", "return");
				send(connfd, "return", SIZEOF, 0);
				return -1;
			}
			else
			{
				error(connfd, buf);
				printf("输入错误,请重新输入!!!\n");
				goto label2;
			}

			i++;
		}

	}
	if (i == 0)
	{
		error(connfd, buf);
		printf("没有找到此航班！！！\n");
		return 0;
	}

	return 0;
}