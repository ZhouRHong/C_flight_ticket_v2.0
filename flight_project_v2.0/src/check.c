#include "my_head.h"


//查询所有航班
int check_all_flight(int connfd, struct ticket_library *ticket_library_head)
{
	char buf[SIZEOF];
	struct ticket_library *check = NULL;
	for (check = ticket_library_head->next; check != NULL; check = check->next)
	{
		bzero(buf, SIZEOF);
		sprintf(buf, "%s     %s       %s       %s   %s     %s  %d     %d\n",
		        check->number,
		        check->staddress,
		        check->arraddress,
		        check->date,
		        check->type,
		        check->stime,
		        check->price,
		        check->ticket_num
		       );
		int send_value = send(connfd, buf, SIZEOF, 0);
		printf("send=%d\n", send_value);
		while (send_value < 0 )
		{
			send_value = send(connfd, buf, SIZEOF, 0);
		}
		//send(connfd, buf, SIZEOF, 0);
		usleep(10000);
	}
	error(connfd, buf);
	printf("buf=%s\n", buf);
	return 0;
}


//票价查询
int search_flight_price( int connfd, struct userinfo *userinfo_head, struct ticket_library *ticket_library_head)
{
	system("clear");
	int i = 0, price = 0;
	struct ticket_library *pr = NULL;
	char buf[SIZEOF];
	rec(connfd, buf);
	price = atoi(buf);


	printf("===============================================================\n");
	printf("航班号   起点站   终点站   班期      机型   起飞时间  票价   余票\n");
	for (pr = ticket_library_head->next; pr != NULL; pr = pr->next)
	{

		if (price >= pr->price)
		{
			i++;
			bzero(buf, SIZEOF);
			sprintf(buf, "%s     %s       %s       %s   %s     %s  %d     %d\n",
			        pr->number,
			        pr->staddress,
			        pr->arraddress,
			        pr->date,
			        pr->type,
			        pr->stime,
			        pr->price,
			        pr->ticket_num
			       );
			send(connfd, buf, SIZEOF, 0);
			usleep(10000);
		}

	}
	if (i == 0)
	{
		printf("没有此价格的航班！！！\n");
		error(connfd, buf);
		return -1;
	}
	next(connfd, buf);


	return 0;
}



//机型查询
int search_flight_type(int connfd, struct userinfo *userinfo_head, struct ticket_library *ticket_library_head)
{
	int i = 0;
	struct ticket_library *ty = NULL;
	char type[5];
	char buf[SIZEOF];
	rec(connfd, buf);
	strcpy(type, buf);

	for (ty = ticket_library_head->next; ty != NULL; ty = ty->next)
	{
		if (strcmp(ty->type, type) == 0)
		{
			i++;
			bzero(buf, SIZEOF);
			sprintf(buf, "%s     %s       %s       %s   %s     %s  %d     %d\n",
			        ty->number,
			        ty->staddress,
			        ty->arraddress,
			        ty->date,
			        ty->type,
			        ty->stime,
			        ty->price,
			        ty->ticket_num
			       );
			send(connfd, buf, SIZEOF, 0);
			usleep(10000);
		}

	}
	if (i == 0)
	{
		printf("没有此机型的航班！！！\n");
		error(connfd, buf);
		return -1;
	}
	next(connfd, buf);
	return 0;
}



//出发日期查询
int search_flight_date(int connfd, struct userinfo *userinfo_head, struct ticket_library *ticket_library_head)
{
	int i = 0;
	struct ticket_library *da = NULL;
	char date[10];
	printf("请输入航班的出发时间：\n");
	char buf[SIZEOF];
	rec(connfd, buf);
	printf("班期:%s\n", buf);
	strcpy(date, buf);

	for (da = ticket_library_head->next; da != NULL; da = da->next)
	{
		if (strcmp(da->date, date) == 0)
		{
			i++;
			bzero(buf, SIZEOF);
			sprintf(buf, "%s     %s       %s       %s   %s     %s  %d     %d\n",
			        da->number,
			        da->staddress,
			        da->arraddress,
			        da->date,
			        da->type,
			        da->stime,
			        da->price,
			        da->ticket_num
			       );
			send(connfd, buf, SIZEOF, 0);
			usleep(10000);
		}

	}
	if (i == 0)
	{
		printf("没有此出发时间的航班！！！\n");
		error(connfd, buf);
		return -1;
	}
	next(connfd, buf);

	return 0;
}


//目的地查询
int search_flight_location(int connfd, struct userinfo *userinfo_head, struct ticket_library *ticket_library_head)
{
	int i = 0;
	struct ticket_library *loca = NULL;
	char location[5];
	char buf[SIZEOF];
	printf("请输入航班的目的地：\n");
	rec(connfd, buf);
	strcpy(location, buf);

	for (loca = ticket_library_head->next; loca != NULL; loca = loca->next)
	{
		if (strcmp(loca->staddress, location) == 0)
		{
			i++;
			bzero(buf, SIZEOF);
			sprintf(buf, "%s     %s       %s       %s   %s     %s  %d     %d\n",
			        loca->number,
			        loca->staddress,
			        loca->arraddress,
			        loca->date,
			        loca->type,
			        loca->stime,
			        loca->price,
			        loca->ticket_num
			       );
			send(connfd, buf, SIZEOF, 0);
			usleep(10000);

		}

	}
	if (i == 0)
	{
		printf("没有此目的地的航班！！！\n");
		error(connfd, buf);
		return -1;
	}
	next(connfd, buf);

	return 0;
}

//条件查询
int search_flight(int connfd, struct userinfo *userinfo_head, struct ticket_library *ticket_library_head)
{
	system("clear");
	char buf[SIZEOF];
	while (1)
	{
		printf("======================GEC SYSTEM===================\n");
		printf("                                                   \n");
		printf("                       1 目的地查询                 \n");
		printf("                       2 班期查询                   \n");
		printf("                       3 机型查询                   \n");
		printf("                       4 价格查询                   \n");
		printf("                       5 返回                       \n");
		printf("                                                   \n");
		printf("===================================================\n");
		printf("请输入您的选项：\n");
		rec(connfd, buf);

		if (strcmp(buf, "1") == 0)
		{
			search_flight_location(connfd, userinfo_head, ticket_library_head);
			continue;
		}

		if (strcmp(buf, "2") == 0)
		{
			search_flight_date(connfd, userinfo_head, ticket_library_head);
			continue;

		}

		if (strcmp(buf, "3") == 0)
		{
			search_flight_type(connfd,  userinfo_head, ticket_library_head);
			continue;

		}

		if (strcmp(buf, "4") == 0)
		{
			search_flight_price( connfd, userinfo_head, ticket_library_head);
			continue;

		}

		if (strcmp(buf, "5") == 0)
		{
			return 0;

		}
	}

}



//查询航班界面
int check_flight(int connfd, struct userinfo * userinfo_head, struct ticket_library *ticket_library_head)
{
	char buf[SIZEOF];
	while (1)
	{
		printf("======================GEC SYSTEM===================\n");
		printf("                                                   \n");
		printf("                       1 查询所有航班               \n");
		printf("                       2 条件查询                   \n");
		printf("                       3 返回                       \n");
		printf("                                                   \n");
		printf("===================================================\n");
		printf("请输入您的选项：\n");
		rec(connfd, buf);

		if (strcmp(buf, "1") == 0)
		{
			check_all_flight(connfd, ticket_library_head);
			continue;
		}

		if (strcmp(buf, "2") == 0)
		{
			search_flight(connfd, userinfo_head, ticket_library_head); //条件查询
			continue;

		}

		if (strcmp(buf, "3") == 0)
		{
			return 0;
		}

	}
	return 0;
}



//快速查询
int check_fast(int connfd, struct userinfo *userinfo_head, struct ticket_library *ticket_library_head)
{
	system("clear");
	int i = 0;
	struct ticket_library *num = NULL;
	char number[5];
	char buf[SIZEOF];

	for (num = ticket_library_head->next; num != NULL; num = num->next)
	{
		bzero(buf, SIZEOF);
		sprintf(buf, "%s", num->number);
		send(connfd, buf, SIZEOF, 0);
		usleep(10000);
	}
	error(connfd, buf);

	printf("请输入您要查询的航班号：\n");
	rec(connfd, buf);
	printf("航班号：%s\n", buf);
	strcpy(number, buf);

	for (num = ticket_library_head->next; num != NULL; num = num->next)
	{
		if (strcmp(num->number, number) == 0)
		{
			i++;
			bzero(buf, SIZEOF);
			sprintf(buf, "%s     %s       %s       %s   %s     %s  %d     %d\n",
			        num->number,
			        num->staddress,
			        num->arraddress,
			        num->date,
			        num->type,
			        num->stime,
			        num->price,
			        num->ticket_num
			       );
			send(connfd, buf, SIZEOF, 0);
			usleep(10000);
		}

	}
	if (i == 0)
	{
		error(connfd, buf);
		return -1;
	}
	next(connfd, buf);


	return 0;
}


//订单查询
int check_order(struct userinfo *user_name)
{
	system("clear");
	printf("===============================================================\n");
	printf("航班号   起点站   终点站   班期      机型   起飞时间  价格   保险种类（0-无 1-延误险 2-新冠险）\n");
	struct user_ticket *check = NULL;
	char buf[SIZEOF];
	for (check = user_name->user_ticket_head.next; check != NULL; check = check->next)
	{
		bzero(buf, SIZEOF);
		sprintf(buf, "%s     %s       %s       %s   %s     %s  %d     %d\n",
		        check->number,
		        check->staddress,
		        check->arraddress,
		        check->date,
		        check->type,
		        check->stime,
		        check->price,
		        check->insurance
		       );
		send(user_name->connfd, buf, SIZEOF, 0);
	}
	error(user_name->connfd, buf);

	return 0;
}
