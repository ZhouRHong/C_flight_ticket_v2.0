#include "my_head.h"


//实名认证
int real_name(struct id_car *id_car_head, struct userinfo *user_name)
{
	char real_name[10];//真实姓名
	char id_car_num[10];//身份证号
	int i = 0, j = 0, k = 0;
	int identity;//身份
	struct id_car *find_id;//定义身份证链指针用于遍历身份证库
	char buf[SIZEOF];
	int connfd = user_name->connfd;


	printf("=======正在实名登记========\n");
	printf("请输入您的身份证号：\n");
	rec(connfd, buf);
	strcpy(id_car_num, buf);
	printf("%s\n", id_car_num );
	for (find_id = id_car_head->next; find_id != NULL; find_id = find_id->next)
	{
		if (strcmp(find_id->id_num, id_car_num) == 0)
		{
			next(connfd, buf);
			printf("请输入您的真实姓名：\n");
			rec(connfd, buf);
			printf("%s\n", buf);
			strcpy(real_name, buf);
			//scanf("%s", real_name);
			if (strcmp(find_id->name, real_name) == 0)
			{
				next(connfd, buf);
				printf("请输入您的身份（普通人输入0、军人输入1、医护人员输入2、残疾人输入3）：\n");
				rec(connfd, buf);
				identity = atoi(buf);
				if (find_id->status == identity)
				{
					next(connfd, buf);
					printf("======实名认证成功！！！=======\n");
					user_name->id_name_flag = 1;//认证成功后，用户信息的实名认证标志位置1；
					//上锁
					pthread_mutex_lock(&m);
					save_userinfo_fun(user_name);//保存到文件中
					//解锁
					pthread_mutex_unlock(&m);
					return 0;
				}
				else
				{
					error(connfd, buf);
					printf("身份输入有误（数字）！！！\n");
					return 0;
				}

			}
			else
			{
				error(connfd, buf);
				printf("真实姓名输入有误！！！\n");
				return 0;
			}
			i++;
		}

	}

	if (i == 0)
	{
		error(connfd, buf);
		printf("身份证号输入有误（6位）！！！\n" );
		return 0;
	}

	return 0;
}


//用户票链添加购买的票
void add_user_ticket(int insurance, int price,
                     struct userinfo *user_name,//该用户信息
                     struct ticket_library *find_ticket)//购买的航班信息节点
{
	struct user_ticket *new = NULL;
	struct user_ticket *p = NULL;

	new = (struct user_ticket *)malloc(sizeof(struct user_ticket));
	if (new == NULL)
	{
		printf("malloc 购票链表新节点 错误\n");
	}

	strcpy(new->number, find_ticket->number);
	strcpy(new->staddress, find_ticket->staddress);
	strcpy(new->arraddress, find_ticket->arraddress);
	strcpy(new->date, find_ticket->date);
	strcpy(new->type, find_ticket->type);
	strcpy(new->stime, find_ticket->stime);
	new->price = price;
	new->insurance = insurance;
	new->next = NULL;

	for (p = &user_name->user_ticket_head; p->next != NULL; p = p->next);
	p->next = new;
	printf("=====购票成功======\n");
	//上锁
	pthread_mutex_lock(&m);
	save_user_ticket_fun(new, user_name); //保存用户票链
	//解锁
	pthread_mutex_unlock(&m);
}



//vip购买保险
int vip_buy_insurance(struct userinfo *user_name,
                      struct user_ticket *vip_ticket,//用户票链指针
                      struct ticket_library *find_ticket)//票库指针
{
	int price;//票价
	int insurance;//保险种类
	char buf[SIZEOF];
	int connfd = user_name->connfd;
	printf("==================================================\n");
	printf("请输入你要购买的保险的种类：（0-不购买 1-延误险 2-新冠病毒险）\n");
	rec(connfd, buf);
	insurance = atoi(buf);



	if (insurance == 0)//没保险的
	{
		next(connfd, buf);
		if (user_name->status == 0)//判断身份
		{
			next(connfd, buf);
			if (user_name->balance < 0.8 * find_ticket->price) //判断余额是否足够
			{
				bzero(buf, SIZEOF);
				sprintf(buf, "余额不足，请充值！！！\n");
				send(connfd, buf, SIZEOF, 0);
				return -1;
			}
			else
			{
				next(connfd, buf);
				if (find_ticket->ticket_num > 0)//判断票有没有卖完
				{
					next(connfd, buf);
					price = 0.8 * find_ticket->price;
					printf("price:%d\n", price );
					user_name->balance = user_name->balance - price; //用户信息减去余额
					//上锁
					pthread_mutex_lock(&m);
					save_userinfo_fun(user_name);
					//解锁
					pthread_mutex_unlock(&m);

					find_ticket->ticket_num -= 1; //票库对应的航班余票减一

					//上锁
					pthread_mutex_lock(&m);

					save_flight_fun(find_ticket);//保存此时的余票
					//解锁
					pthread_mutex_unlock(&m);

					add_user_ticket(insurance, price, user_name, find_ticket); //用户票链添加购买的票
				}
				else
				{
					bzero(buf, SIZEOF);
					sprintf(buf, "此航班票已卖完！！！\n");
					send(connfd, buf, SIZEOF, 0);
					return 0;
				}

			}
		}
		else//特殊身份
		{
			error(connfd, buf);

			if (user_name->balance < 0.5 * 0.8 * find_ticket->price) //判断余额是否足够
			{
				bzero(buf, SIZEOF);
				sprintf(buf, "余额不足，请充值！！！\n");
				send(connfd, buf, SIZEOF, 0);
				return 0;
			}
			else
			{
				next(connfd, buf);

				if (find_ticket->ticket_num > 0)
				{
					next(connfd, buf);
					price = 0.5 * 0.8 * find_ticket->price;

					user_name->balance = user_name->balance - price;//用户信息减去余额

					//上锁
					pthread_mutex_lock(&m);
					save_userinfo_fun(user_name);
					//解锁
					pthread_mutex_unlock(&m);


					find_ticket->ticket_num -= 1; //票库对应的航班余票减一

					//上锁
					pthread_mutex_lock(&m);
					save_flight_fun(find_ticket);//保存此时的余票
					//解锁
					pthread_mutex_unlock(&m);

					add_user_ticket(insurance, price, user_name, find_ticket); //用户票链添加购买的票
				}
				else
				{
					bzero(buf, SIZEOF);
					sprintf(buf, "此航班票已卖完！！！\n");
					send(connfd, buf, SIZEOF, 0);
					return 0;
				}
			}
		}
	}
	else//买了保险的 30保费
	{
		error(connfd, buf);
		if (user_name->status == 0)//判断身份 普通
		{
			next(connfd, buf);
			if (user_name->balance < 0.8 * find_ticket->price + 30) //判断余额是否足够
			{
				bzero(buf, SIZEOF);
				sprintf(buf, "余额不足，请充值！！！\n");
				send(connfd, buf, SIZEOF, 0);
				return -1;
			}
			else
			{
				next(connfd, buf);
				if (find_ticket->ticket_num > 0)//判断票有没有卖完
				{
					next(connfd, buf);
					price = 0.8 * find_ticket->price + 30;
					user_name->balance = user_name->balance - price; //用户信息减去余额

					//上锁
					pthread_mutex_lock(&m);
					save_userinfo_fun(user_name);
					//解锁
					pthread_mutex_unlock(&m);

					find_ticket->ticket_num -= 1; //票库对应的航班余票减一

					//上锁
					pthread_mutex_lock(&m);
					save_flight_fun(find_ticket);//保存此时的余票
					//解锁
					pthread_mutex_unlock(&m);

					add_user_ticket(insurance, price, user_name, find_ticket); //用户票链添加购买的票
				}
				else
				{
					bzero(buf, SIZEOF);
					sprintf(buf, "此航班票已卖完！！！\n");
					send(connfd, buf, SIZEOF, 0);
					return 0;
				}
			}
		}
		else//半价身份
		{
			error(connfd, buf);
			if (user_name->balance < 0.5 * 0.8 * find_ticket->price + 30) //判断余额是否足够
			{
				bzero(buf, SIZEOF);
				sprintf(buf, "余额不足，请充值！！！\n");
				send(connfd, buf, SIZEOF, 0);
				return 0;
			}
			else
			{
				next(connfd, buf);

				if (find_ticket->ticket_num > 0)
				{
					next(connfd, buf);
					price = 0.5 * 0.8 * find_ticket->price + 30;
					user_name->balance = user_name->balance - price;//用户信息减去余额

					//上锁
					pthread_mutex_lock(&m);
					save_userinfo_fun(user_name);
					//解锁
					pthread_mutex_unlock(&m);

					find_ticket->ticket_num -= 1; //票库对应的航班余票减一


					//上锁
					pthread_mutex_lock(&m);
					save_flight_fun(find_ticket);//保存此时的余票
					//解锁
					pthread_mutex_unlock(&m);

					add_user_ticket(insurance, price, user_name, find_ticket); //用户票链添加购买的票
				}
				else
				{
					bzero(buf, SIZEOF);
					sprintf(buf, "此航班票已卖完！！！\n");
					send(connfd, buf, SIZEOF, 0);
					return 0;

				}
			}
		}
	}
	return 0;
}



//普通用户购买保险
int buy_insurance(struct userinfo *user_name,
                  struct user_ticket *vip_ticket,//用户票链指针
                  struct ticket_library *find_ticket)//票库指针
{
	int price;//票价
	int insurance;//保险种类
	char buf[SIZEOF];
	int connfd = user_name->connfd;
	printf("==================================================\n");
	printf("请输入你要购买的保险的种类：（0-不购买 1-延误险 2-新冠病毒险）\n");
	rec(connfd, buf);
	insurance = atoi(buf);


	if (insurance == 0)//买了保险的
	{
		next(connfd, buf);
		if (user_name->status == 0)//判断身份
		{
			next(connfd, buf);
			if (user_name->balance <  find_ticket->price ) //判断余额是否足够
			{
				bzero(buf, SIZEOF);
				sprintf(buf, "余额不足，请充值！！！\n");
				send(connfd, buf, SIZEOF, 0);
				return -1;
			}
			else
			{
				next(connfd, buf);
				if (find_ticket->ticket_num > 0)//判断票有没有卖完
				{
					next(connfd, buf);
					price = find_ticket->price ;
					user_name->balance = user_name->balance - price; //用户信息减去余额
					save_userinfo_fun(user_name);
					find_ticket->ticket_num -= 1; //票库对应的航班余票减一
					save_flight_fun(find_ticket);//保存此时的余票
					add_user_ticket(insurance, price, user_name, find_ticket); //用户票链添加购买的票
				}
				else
				{
					bzero(buf, SIZEOF);
					sprintf(buf, "此航班票已卖完！！！\n");
					send(connfd, buf, SIZEOF, 0);
					return 0;
				}

			}
		}
		else
		{
			error(connfd, buf);

			if (user_name->balance < 0.5 * find_ticket->price) //判断余额是否足够
			{
				bzero(buf, SIZEOF);
				sprintf(buf, "余额不足，请充值！！！\n");
				send(connfd, buf, SIZEOF, 0);
				return 0;
			}
			else
			{
				next(connfd, buf);

				if (find_ticket->ticket_num > 0)
				{
					price = 0.5 *  find_ticket->price;
					user_name->balance = user_name->balance - price;//用户信息减去余额
					//上锁
					pthread_mutex_lock(&m);
					save_userinfo_fun(user_name);
					//解锁
					pthread_mutex_unlock(&m);


					find_ticket->ticket_num -= 1; //票库对应的航班余票减一

					//上锁
					pthread_mutex_lock(&m);
					save_flight_fun(find_ticket);//保存此时的余票
					//解锁
					pthread_mutex_unlock(&m);

					add_user_ticket(insurance, price, user_name, find_ticket); //用户票链添加购买的票
				}
				else
				{
					bzero(buf, SIZEOF);
					sprintf(buf, "此航班票已卖完！！！\n");
					send(connfd, buf, SIZEOF, 0);
				}

			}
		}


	}
	else//没买保险的 保险费为30
	{
		error(connfd, buf);
		if (user_name->status == 0)//判断身份
		{
			next(connfd, buf);
			if (user_name->balance <  find_ticket->price + 30) //判断余额是否足够  保险费为30
			{
				bzero(buf, SIZEOF);
				sprintf(buf, "余额不足，请充值！！！\n");
				send(connfd, buf, SIZEOF, 0);
				return -1;
			}
			else
			{
				next(connfd, buf);
				if (find_ticket->ticket_num > 0)//判断票有没有卖完
				{
					next(connfd, buf);

					price = find_ticket->price + 30 ;
					user_name->balance = user_name->balance - price; //用户信息减去余额
					//上锁
					pthread_mutex_lock(&m);
					save_userinfo_fun(user_name);
					//解锁
					pthread_mutex_unlock(&m);

					find_ticket->ticket_num -= 1; //票库对应的航班余票减一

					//上锁
					pthread_mutex_lock(&m);
					save_flight_fun(find_ticket);//保存此时的余票
					//解锁
					pthread_mutex_unlock(&m);

					add_user_ticket(insurance, price, user_name, find_ticket); //用户票链添加购买的票
				}
				else
				{
					bzero(buf, SIZEOF);
					sprintf(buf, "此航班票已卖完！！！\n");
					send(connfd, buf, SIZEOF, 0);
				}

			}
		}
		else
		{
			error(connfd, buf);
			if (user_name->balance < 0.5 * find_ticket->price + 30) //判断余额是否足够
			{
				bzero(buf, SIZEOF);
				sprintf(buf, "余额不足，请充值！！！\n");
				send(connfd, buf, SIZEOF, 0);
				return 0;
			}
			else
			{
				next(connfd, buf);
				if (find_ticket->ticket_num > 0)
				{
					next(connfd, buf);
					price = 0.5 *  find_ticket->price + 30;
					user_name->balance = user_name->balance - price;//用户信息减去余额
					//上锁
					pthread_mutex_lock(&m);
					save_userinfo_fun(user_name);
					//解锁
					pthread_mutex_unlock(&m);

					find_ticket->ticket_num -= 1; //票库对应的航班余票减一


					//上锁
					pthread_mutex_lock(&m);
					save_flight_fun(find_ticket);//保存此时的余票
					//解锁
					pthread_mutex_unlock(&m);

					add_user_ticket(insurance, price, user_name, find_ticket); //用户票链添加购买的票
				}
				else
				{
					bzero(buf, SIZEOF);
					sprintf(buf, "此航班票已卖完！！！\n");
					send(connfd, buf, SIZEOF, 0);
					return 0;

				}
			}
		}
	}
	return 0;
	printf("普通用户\n");
}


//vip购票
int vip_buy_ticket(struct userinfo *user_name, struct ticket_library *ticket_library_head)
{
	system("clear");
	char vip_ticket_number[10];
	struct user_ticket *vip_ticket;//定义寻找用户票的指针
	struct ticket_library *find_ticket;//定义票库指针
	int choise;
	int i = 0, k = 0;
	char buf[SIZEOF];
	int connfd = user_name->connfd;

	//先判断用户是不是VIP用户

	if (user_name->vip_flag == 0)
	{
		error(connfd, buf);
		printf("您好，您还不是VIP用户，请选择普通用户进行购票！\n");
		return -1;
	}
	next(connfd, buf);

	printf("尊敬的VIP用户，您购票享受8折优惠\n");
	printf("请输入您要购买的航班号\n");
	rec(connfd, buf);
	strcpy(vip_ticket_number, buf);


	for (vip_ticket = user_name->user_ticket_head.next; vip_ticket != NULL; vip_ticket = vip_ticket->next) //遍历user_name的票链
	{

		if (strcmp(vip_ticket_number, vip_ticket->number) == 0) //如果票链中存在则不可以购买，退出！
		{
			error(connfd, buf);
			printf("您已购买了此航班的票！！！\n");
			return -1;

		}
		//k++;
	}
	next(connfd, buf);
	for (find_ticket = ticket_library_head->next; find_ticket != NULL; find_ticket = find_ticket->next)//遍历票库中的航班
	{

		if (strcmp(vip_ticket_number, find_ticket->number) == 0)//找到要购买的航班号并打印
		{
			next(connfd, buf);
			bzero(buf, SIZEOF);
			sprintf(buf, "%s     %s       %s       %s   %s     %s  %d     %d\n",
			        find_ticket->number,
			        find_ticket->staddress,
			        find_ticket->arraddress,
			        find_ticket->date,
			        find_ticket->type,
			        find_ticket->stime,
			        find_ticket->price,
			        find_ticket->ticket_num
			       );
			send(connfd, buf, SIZEOF, 0);
			printf("请确认是否购买！（0-确认 1-取消）\n");
			rec(connfd, buf);
			choise = atoi(buf);
			if (choise == 0)
			{
				next(connfd, buf);
				vip_buy_insurance(user_name, vip_ticket, find_ticket); //判断是否购买保险，减去对应余额
				return 0;
			}
			else if (choise == 1)
			{
				send(connfd, "return", 6, 0);
				return 0;
			}
			else
			{
				send(connfd, "1error", 6, 0);
				printf("输入有误！！！\n");
			}
			i++;

		}
	}

	if (i == 0)
	{
		printf("没有此航班！！！\n");
		error(connfd, buf);
		return -1;
	}

	return 0;
}



//普通用户购票
int usr_buy_ticket(struct userinfo *user_name, struct ticket_library *ticket_library_head)
{
	system("clear");
	char usr_ticket_number[10];
	struct user_ticket *usr_ticket;//定义寻找用户票的指针
	struct ticket_library *find_ticket;//定义票库指针
	int choise;
	int i = 0, k = 0;
	char buf[SIZEOF];
	int connfd = user_name->connfd;


	printf("请输入您要购买的航班号\n");
	rec(connfd, buf);
	strcpy(usr_ticket_number, buf);

	for (usr_ticket = user_name->user_ticket_head.next; usr_ticket != NULL; usr_ticket = usr_ticket->next) //遍历user_name的票链
	{

		if (strcmp(usr_ticket_number, usr_ticket->number) == 0) //如果票链中存在则不可以购买，退出！
		{
			error(connfd, buf);
			printf("您已购买了此航班的票！！！\n");
			return -1;

		}

	}
	next(connfd, buf);
	for (find_ticket = ticket_library_head->next; find_ticket != NULL; find_ticket = find_ticket->next)//遍历票库中的航班
	{

		if (strcmp(usr_ticket_number, find_ticket->number) == 0)//找到要购买的航班号并打印
		{
			next(connfd, buf);
			bzero(buf, SIZEOF);
			sprintf(buf, "%s     %s       %s       %s   %s     %s  %d     %d\n",
			        find_ticket->number,
			        find_ticket->staddress,
			        find_ticket->arraddress,
			        find_ticket->date,
			        find_ticket->type,
			        find_ticket->stime,
			        find_ticket->price,
			        find_ticket->ticket_num
			       );
			send(connfd, buf, SIZEOF, 0);
			printf("请确认是否购买！（0-确认 1-取消）\n");
			rec(connfd, buf);
			choise = atoi(buf);
			if (choise == 0)
			{
				next(connfd, buf);
				buy_insurance(user_name, usr_ticket, find_ticket); //判断是否购买保险，减去对应余额
				return 0;
			}
			else if (choise == 1)
			{
				send(connfd, "return", 6, 0);
				return 0;
			}
			else
			{
				send(connfd, "1error", 6, 0);
				printf("输入有误！！！\n");
			}
			i++;


		}

	}


	if (i == 0)
	{
		printf("没有此航班！！！\n");
		error(connfd, buf);
		return -1;
	}

	return 0;
}


//购票入口
int buy_ticket(struct userinfo * user_name,
               struct ticket_library * ticket_library_head,
               struct id_car * id_car_head)

{
	char buf[SIZEOF];
	int connfd = user_name->connfd;
	if (user_name->id_name_flag == 0)//判断实名标志位
	{
		error(connfd, buf);
		real_name(id_car_head, user_name); //实名认证
	}
	else
	{
		next(connfd, buf);
		while (1)
		{
			printf("======================GEC SYSTEM===================\n");
			printf("                                                   \n");
			printf("                      1 VIP购票                    \n");
			printf("                      2 普通用户购票                 \n");
			printf("                      3 返回                       \n");
			printf("                                                   \n");
			printf("===================================================\n");
			printf("请在客户端输入您的选项：\n");

			rec(connfd, buf);
			if (strcmp(buf, "1") == 0)
			{
				vip_buy_ticket(user_name, ticket_library_head);
				continue;

			}
			if (strcmp(buf, "2") == 0)
			{
				usr_buy_ticket(user_name, ticket_library_head);
				continue;

			}
			if (strcmp(buf, "3") == 0)
			{
				return 0;

			}
		}

	}
	return 0;

}



//退票
int return_ticket(struct userinfo *user_name, struct ticket_library *ticket_library_head) //退票
{
	system("clear");
	char number[10];
	int insurance = 0;
	char buf[SIZEOF];
	int connfd = user_name->connfd;
	struct user_ticket *check = NULL;//用于遍历票链
	struct user_ticket *p = NULL;
	struct user_ticket *q = NULL;//p q 用于删除节点
	struct user_ticket *delete_ticket = NULL;// 用于删除节点后遍历保存到票库链
	struct ticket_library *ticket = NULL; //用于查找票库的节点给余票加一
	printf("下面是您的订单：\n");
	printf("===============================================================\n");
	printf("航班号   起点站   终点站   班期      机型   起飞时间  价格   保险种类（0-无 1-延误险 2-新冠险）\n");

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
	error(connfd, buf);

	printf("请输入您要退的航班号：\n");
	rec(connfd, buf);
	strcpy(number, buf);

	for (check = user_name->user_ticket_head.next; check != NULL; check = check->next)
	{

		if (strcmp(check->number , number) == 0)
		{
			insurance = check->insurance;//找到用户为此航班买的保险用来判断
		}

	}

	if (insurance > 0) //买了保险的
	{
		next(connfd, buf);
		for (q = &user_name->user_ticket_head, p = user_name->user_ticket_head.next; p != NULL; q = p, p = p->next)
		{

			if (strcmp(p->number , number) == 0) //找到要退票的节点
			{
				next(connfd, buf);
				if (user_name->status > 0) //判断是否为特殊身份
				{
					next(connfd, buf);
					user_name->balance += p->price - 30; //特殊用户退回除了保险费外所有的钱

					//上锁
					pthread_mutex_lock(&m);
					save_userinfo_fun(user_name);
					//解锁
					pthread_mutex_unlock(&m);

					for (ticket = ticket_library_head->next; ticket != NULL; ticket = ticket->next) //遍历票库
					{
						if (strcmp(ticket->number, number) == 0)
						{
							ticket->ticket_num += 1; //对应的余票加一
							//上锁
							pthread_mutex_lock(&m);
							save_flight_fun(ticket);//save
							//解锁
							pthread_mutex_unlock(&m);
						}
					}
					printf("退票成功！！！\n");

				}
				else//普通人，扣除5%的手续费
				{
					error(connfd, buf);
					user_name->balance += 0.95 * p->price - 30;
					//上锁
					pthread_mutex_lock(&m);
					save_userinfo_fun(user_name);
					//解锁
					pthread_mutex_unlock(&m);
					for (ticket = ticket_library_head->next; ticket != NULL; ticket = ticket->next) //遍历票库
					{
						if (strcmp(ticket->number, number) == 0)
						{
							ticket->ticket_num += 1; //对应的余票加一
							//上锁
							pthread_mutex_lock(&m);
							save_flight_fun(ticket);//save
							//解锁
							pthread_mutex_unlock(&m);
						}
					}
					printf("退票成功！！！\n");
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
		}
		error(connfd, buf);
		printf("您没有购买此航班1！！！!\n");
	}
	else//没买保险的
	{
		error(connfd, buf);
		for (q = &user_name->user_ticket_head, p = user_name->user_ticket_head.next; p != NULL; q = p, p = p->next)
		{


			if (strcmp(p->number , number) == 0) //找到要退票的节点
			{
				next(connfd, buf);
				if (user_name->status > 0) //判断是否为特殊身份
				{
					next(connfd, buf);
					user_name->balance += p->price ; //特殊用户退回所有的钱
					//上锁
					pthread_mutex_lock(&m);
					save_userinfo_fun(user_name);
					//解锁
					pthread_mutex_unlock(&m);

					for (ticket = ticket_library_head->next; ticket != NULL; ticket = ticket->next) //遍历票库
					{
						if (strcmp(ticket->number, number) == 0)
						{
							ticket->ticket_num += 1; //对应的余票加一
							//上锁
							pthread_mutex_lock(&m);
							save_flight_fun(ticket);//save
							//解锁
							pthread_mutex_unlock(&m);
						}
					}
					printf("退票成功！！！\n");

				}
				else//普通人，扣除5%的手续费
				{
					error(connfd, buf);
					user_name->balance += 0.95 * p->price ;
					save_userinfo_fun(user_name);
					for (ticket = ticket_library_head->next; ticket != NULL; ticket = ticket->next) //遍历票库
					{
						if (strcmp(ticket->number, number) == 0)
						{
							ticket->ticket_num += 1; //对应的余票加一
							//上锁
							pthread_mutex_lock(&m);
							save_flight_fun(ticket);//save
							//解锁
							pthread_mutex_unlock(&m);
						}
					}
					printf("退票成功！！！\n");
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
		}
		error(connfd, buf);
		printf("您没有购买此航班2！！！!\n");
	}


	return -1;
}


//重新对应购买保险
int change_ticket_insurance(struct user_ticket *p, struct userinfo *user_name, struct ticket_library *find_ticket)
{
	int price;//票价
	char buf[SIZEOF];
	int connfd = user_name->connfd;

	if (p->insurance == 0)//没买保险的
	{
		next(connfd, buf);
		if (user_name->status == 0)//判断身份
		{
			next(connfd, buf);
			if (user_name->balance < find_ticket->price) //判断余额是否足够
			{
				next(connfd, buf);
				printf("余额不足以改签，请充值后重新购票！！！\n");
				return 0;

			}
			else
			{
				error(connfd, buf);
				if (find_ticket->ticket_num > 0)
				{
					next(connfd, buf);
					price = find_ticket->price;
					user_name->balance -= price; //用户信息减去余额

					//上锁
					pthread_mutex_lock(&m);
					save_userinfo_fun(user_name);
					//解锁
					pthread_mutex_unlock(&m);

					find_ticket->ticket_num -= 1; //票库对应的航班余票减一

					//上锁
					pthread_mutex_lock(&m);
					save_flight_fun(find_ticket);//保存此时的余票
					//解锁
					pthread_mutex_unlock(&m);

					add_user_ticket(p->insurance, price, user_name, find_ticket); //用户票链添加购买的票
				}
				else
				{
					error(connfd, buf);
					printf("此航班票已卖完无法改签,票额已经退回,请重新购票！！！\n");
					return 0;
				}

			}


		}
		else//特殊身份
		{
			error(connfd, buf);
			if (user_name->balance < 0.5 * find_ticket->price) //判断余额是否足够
			{
				next(connfd, buf);
				printf("余额不足以改签，请充值后重新购票！！！\n");
				return 0;
			}
			else
			{
				error(connfd, buf);
				if (find_ticket->ticket_num > 0)
				{
					next(connfd, buf);
					price = 0.5 * find_ticket->price;
					user_name->balance -= price;//用户信息减去余额

					//上锁
					pthread_mutex_lock(&m);
					save_userinfo_fun(user_name);
					//解锁
					pthread_mutex_unlock(&m);

					find_ticket->ticket_num -= 1; //票库对应的航班余票减一

					//上锁
					pthread_mutex_lock(&m);
					save_flight_fun(find_ticket);//保存此时的余票
					//解锁
					pthread_mutex_unlock(&m);

					add_user_ticket(p->insurance, price, user_name, find_ticket); //用户票链添加购买的票
				}
				else
				{
					error(connfd, buf);
					printf("此航班票已卖完无法改签,票额已经退回,请重新购票！！！\n");
					return 0;
				}
			}
		}
	}
	else//买了保险的 30保费
	{
		error(connfd, buf);
		if (user_name->status == 0)//判断身份 普通
		{
			next(connfd, buf);
			if (user_name->balance < find_ticket->price + 30) //判断余额是否足够
			{
				next(connfd, buf);
				printf("余额不足以改签，请充值后重新购票！！！\n");
				return -1;
			}
			else
			{
				error(connfd, buf);
				if (find_ticket->ticket_num > 0)//判断票有没有卖完
				{
					next(connfd, buf);
					price = find_ticket->price + 30;
					user_name->balance -= price; //用户信息减去余额

					//上锁
					pthread_mutex_lock(&m);
					save_userinfo_fun(user_name);
					//解锁
					pthread_mutex_unlock(&m);
					find_ticket->ticket_num -= 1; //票库对应的航班余票减一
					//上锁
					pthread_mutex_lock(&m);
					save_flight_fun(find_ticket);//保存此时的余票
					//解锁
					pthread_mutex_unlock(&m);
					add_user_ticket(p->insurance, price, user_name, find_ticket); //用户票链添加购买的票
				}
				else
				{
					error(connfd, buf);
					printf("此航班票已卖完无法改签,票额已经退回,请重新购票！！！\n");
					return 0;
				}
			}
		}
		else//半价身份
		{
			error(connfd, buf);
			printf("user_name->balance:%d\n", user_name->balance);
			if (user_name->balance < 0.5 * find_ticket->price + 30) //判断余额是否足够
			{
				next(connfd, buf);
				printf("余额不足以改签，请充值后重新购票！！！\n");
				return 0;
			}
			else
			{
				error(connfd, buf);
				if (find_ticket->ticket_num > 0)
				{
					next(connfd, buf);
					price = 0.5 * find_ticket->price + 30;
					user_name->balance = user_name->balance - price;//用户信息减去余额

					//上锁
					pthread_mutex_lock(&m);
					save_userinfo_fun(user_name);
					//解锁
					pthread_mutex_unlock(&m);
					find_ticket->ticket_num -= 1; //票库对应的航班余票减一
					//上锁
					pthread_mutex_lock(&m);
					save_flight_fun(find_ticket);//保存此时的余票
					//解锁
					pthread_mutex_unlock(&m);
					add_user_ticket(p->insurance, price, user_name, find_ticket); //用户票链添加购买的票
				}
				else
				{
					error(connfd, buf);
					printf("此航班票已卖完无法改签,票额已经退回,请重新购票！！！\n");
					return 0;
				}
			}
		}
	}
	return 0;
}



//购买改签后的票
int change_ticket_fun(struct user_ticket *p, char *change_num, struct userinfo *user_name, struct ticket_library *ticket_library_head)
{
	struct user_ticket *user_ticket;//定义寻找用户票的指针
	struct ticket_library *find_ticket;//定义票库指针
	int choise;
	int i = 0;
	char buf[SIZEOF];
	int connfd = user_name->connfd;


	for (user_ticket = user_name->user_ticket_head.next; user_ticket != NULL; user_ticket = user_ticket->next) //遍历user_name的票链
	{

		if (strcmp(change_num, user_ticket->number) == 0 ) //如果票链中存在则不可以改签，退出！
		{
			error(connfd, buf);
			printf("您已购买了此航班的票,无法改签到此航班！！！\n");
			return -1;

		}

	}

	next(connfd, buf);

	if (strcmp(change_num, p->number) == 0)
	{
		error(connfd, buf);
		printf("无法改签到同一航班，已经把票款退回，请重新购买！！！\n");
		return -1;
	}
	next(connfd, buf);

	for (find_ticket = ticket_library_head->next; find_ticket != NULL; find_ticket = find_ticket->next)//遍历票库中的航班
	{

		if (strcmp(change_num, find_ticket->number) == 0)//找到要改签的航班号并打印
		{
			bzero(buf, SIZEOF);
			sprintf(buf, "%s     %s       %s       %s   %s     %s  %d     %d\n",
			        find_ticket->number,
			        find_ticket->staddress,
			        find_ticket->arraddress,
			        find_ticket->date,
			        find_ticket->type,
			        find_ticket->stime,
			        find_ticket->price,
			        find_ticket->ticket_num
			       );
			send(user_name->connfd, buf, SIZEOF, 0);
			change_ticket_insurance(p, user_name, find_ticket); //判断是否购买保险，减去对应余额
			i++;

		}


	}
	if (i == 0)
	{
		error(connfd, buf);
		printf("没有此航班！！！\n");
		return -1;
	}

	return 0;
}



// 改签
int change_ticket(struct userinfo *user_name, struct ticket_library *ticket_library_head)
{
	system("clear");
	char number[10];
	char change_num[10];
	char buf[SIZEOF];
	int connfd = user_name->connfd;
	struct user_ticket *check = NULL;//用于遍历票链
	struct user_ticket *p = NULL;
	struct user_ticket *q = NULL;//p q 用于删除节点
	struct user_ticket *delete_ticket = NULL;// 用于删除节点后遍历保存到票库链
	struct ticket_library *ticket = NULL; //用于查找票库的节点给余票加一

	printf("下面是您的订单：\n");
	printf("===============================================================\n");
	printf("航班号   起点站   终点站   班期      机型   起飞时间  价格   保险种类（0-无 1-延误险 2-新冠险）\n");

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
	error(connfd, buf);
	printf("====请注意改签后的票将不享受VIP优惠！！！====\n");

	printf("请输入您要改签的航班号：\n");
	rec(connfd, buf);
	strcpy(number, buf);
	for (q = &user_name->user_ticket_head, p = user_name->user_ticket_head.next; p != NULL; q = p, p = p->next)
	{
		if (strcmp(p->number , number) == 0) //找到要退票的节点
		{
			next(connfd, buf);
			user_name->balance += p->price ; //退回所有的钱
			//上锁
			pthread_mutex_lock(&m);
			save_userinfo_fun(user_name);
			//解锁
			pthread_mutex_unlock(&m);

			for (ticket = ticket_library_head->next; ticket != NULL; ticket = ticket->next) //遍历票库
			{
				if (strcmp(ticket->number, number) == 0)
				{
					ticket->ticket_num += 1; //对应的余票加一
					//上锁
					pthread_mutex_lock(&m);
					save_flight_fun(ticket);//save
					//解锁
					pthread_mutex_unlock(&m);
				}
			}

			q->next = p->next;
			printf("请输入您要改签到的航班号：\n");
			rec(connfd, buf);
			strcpy(change_num, buf);
			change_ticket_fun(p, change_num, user_name, ticket_library_head); //购买改签后的票
			free(p);
			//上锁
			pthread_mutex_lock(&m);
			save_delete_ticket_fun(user_name);//删除后保存到用户票链
			//解锁
			pthread_mutex_unlock(&m);
			return 0;
		}
	}
	error(connfd, buf);
	printf("您没有购买此航班！！！\n");
	return 0;
}


//退单与改签
int refund_and_change(struct userinfo * user_name, struct ticket_library * ticket_library_head)
{

	char buf[SIZEOF];
	int connfd = user_name->connfd;
	while (1)
	{
		printf("======================GEC SYSTEM===================\n");
		printf("                                                   \n");
		printf("                      1 退票                       \n");
		printf("                      2 改签                       \n");
		printf("                      3 返回                        \n");
		printf("                                                   \n");
		printf("===================================================\n");
		printf("请在客户端输入您的选项：\n");

		rec(connfd, buf);
		if (strcmp(buf, "1") == 0)
		{
			return_ticket( user_name, ticket_library_head);//退票

		}
		if (strcmp(buf, "2") == 0)
		{
			change_ticket(user_name, ticket_library_head);//改签
		}
		if (strcmp(buf, "3") == 0)
		{
			return 0;
		}


	}
	return 0;
}
