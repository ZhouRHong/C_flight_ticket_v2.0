#include "my_head.h"

//打印回收站内容
int recycle_fun(int connfd, struct ticket_library * recycle_head)
{
	system("clear");
	struct ticket_library *p;
	int i = 0;
	char buf[SIZEOF];

	for (p = recycle_head->next; p != NULL; p = p->next)
	{
		i++;
		bzero(buf, SIZEOF);
		printf("===============================================================\n");
		printf("航班号   起点站   终点站   班期      机型   起飞时间  票价   余票\n");
		sprintf(buf, "%s     %s       %s       %s   %s     %s  %d     %d\n",
		        p->number,
		        p->staddress,
		        p->arraddress,
		        p->date,
		        p->type,
		        p->stime,
		        p->price,
		        p->ticket_num
		       );
		send(connfd, buf, SIZEOF, 0);
		usleep(10000);
	}
	if (i == 0)
	{
		error(connfd, buf);
		printf("回收站没有东西。\n");
		return 0;
	}
	bzero(buf, SIZEOF);
	strcpy(buf, "break");
	send(connfd, buf, SIZEOF, 0);
	return 0;
}


//生效保险
int effect_insurance(int connfd, struct userinfo * userinfo_head)
{
	int insurance = 0;
	char number[10];
	struct userinfo *p = NULL; //用来查找用户信息
	struct user_ticket *q = NULL; //用来查找用户买的票
	char buf[SIZEOF];

	system("clear");
	printf("请输入你要生效的航班：\n" );
	rec(connfd, buf);
	strcpy(number, buf);
	printf("请输入你要生效的的险种（1-延误险-赔购买此票总费用的双倍，2-新冠险-赔购买此票总费用的10倍）：\n");
	rec(connfd, buf);
	insurance = atoi(buf);


	for (p = userinfo_head->next; p != NULL; p = p->next)//遍历所有用户
	{

		for (q = p->user_ticket_head.next; q != NULL; q = q->next)//遍历所有用户的所有票
		{
			if (strcmp(q->number, number) == 0)
			{
				if (q->insurance == 1) //找到购买了延误险的用户
				{
					p->balance += q->price * 2; //赔购买此票总费用的双倍
					q->insurance = 0;//陪了后保险标志位为0
					//上锁
					pthread_mutex_lock(&m);
					save_delete_ticket_fun(p);
					save_userinfo_fun(p);
					//解锁
					pthread_mutex_unlock(&m);

				}
				else if (q->insurance == 2)
				{
					p->balance += q->price * 10; //赔购买此票总费用的十倍
					q->insurance = 0;//陪了后保险标志位为0
					//上锁
					pthread_mutex_lock(&m);
					save_delete_ticket_fun(p);
					save_userinfo_fun(p);
					//解锁
					pthread_mutex_unlock(&m);

				}
			}

		}
	}
	printf("保险生效成功！！！\n");
	return 0;
}


//管理员界面
int admin_login_fun(int connfd,
                    struct userinfo * userinfo_head,
                    struct ticket_library * ticket_library_head,
                    struct ticket_library * recycle_head)
{
	system("clear");
	char buf[SIZEOF];
	while (1)
	{

		printf("======================GEC SYSTEM===================\n");
		printf("                                                   \n");
		printf("                      1 查询所有航班                \n");
		printf("                      2 录入航班                    \n");
		printf("                      3 删除航班                    \n");
		printf("                      4 航班回收站                  \n");
		printf("                      5 生效保险                    \n");
		printf("                      6 退出登陆                    \n");
		printf("                                                   \n");
		printf("===================================================\n");
		printf("请在客户端输入您的选项：\n");
		rec(connfd, buf);

		//查看所有航班
		if (strcmp(buf, "1") == 0)
		{
			check_all_flight(connfd, ticket_library_head);
		}


		//录入航班
		if (strcmp(buf, "2") == 0)
		{
			entry_flight(connfd, ticket_library_head);
		}


		//删除航班
		if (strcmp(buf, "3") == 0)
		{
			delete_flight(connfd, userinfo_head, ticket_library_head, recycle_head);
		}


		//航班回收站
		if (strcmp(buf, "4") == 0)
		{
			recycle_fun(connfd, recycle_head); //打印回收站内容
		}



		//生效保险
		if (strcmp(buf, "5") == 0)
		{
			effect_insurance(connfd, userinfo_head);
		}



		//退出登陆
		if (strcmp(buf, "6") == 0)
		{
			printf("管理员退出登陆\n");
			return 0;
		}

	}
	return 0;
}


//判断管理员账号密码
int admin_fun(int connfd,
              struct userinfo * userinfo_head,
              struct ticket_library * ticket_library_head,
              struct ticket_library * recycle_head)
{
	system("clear");
	char admin[5];
	char passwd[6];
	char buf[SIZEOF];
	printf("请输入您的管理员账号：\n");
	rec(connfd, buf);
	strcpy(admin, buf);
	if (strcmp(admin, "root") == 0)
	{
		next(connfd, buf);
		printf("请输入您的密码：\n");
		rec(connfd, buf);
		strcpy(passwd, buf);
		if (strcmp(passwd, "123123") == 0)
		{
			next(connfd, buf);
			admin_login_fun( connfd, userinfo_head, ticket_library_head, recycle_head);
		}
		else
		{
			error(connfd, buf);
			printf("密码输入错误！！！\n");
		}
	}
	else
	{
		error(connfd, buf);
		printf("账号输入有误！！！\n");
	}

	return 0;
}
