#include "my_head.h"

//显示欢迎界面
void show_welcome_logo()
{

	printf("======================GEC SYSTEM===================\n");
	printf("                                                   \n");
	printf("                       欢迎使用                     \n");
	printf("                                                   \n");
	printf("===================================================\n");
	printf("正在加载信息...\n");

}


//显示主界面
int main_meum(int connfd,
              struct userinfo * userinfo_head,
              struct ticket_library * ticket_library_head,
              struct id_car * id_car_head,
              struct ticket_library * recycle_head
             )
{

	char buf[SIZEOF];
	while (1)
	{
		printf("======================GEC SYSTEM===================\n");
		printf("                                                   \n");
		printf("                      1 选择登陆                    \n");
		printf("                      2 注册                        \n");
		printf("                      3 忘记密码                    \n");
		printf("                      4 退出                        \n");
		printf("                                                   \n");
		printf("===================================================\n");
		printf("请在客户端输入您的选项：\n");

		rec(connfd, buf);
		printf("buf=%s\n", buf);

		if (strcmp(buf, "1") == 0)
		{
			//选择登陆
			select_login_fun( connfd, userinfo_head, ticket_library_head, id_car_head, recycle_head);
			continue;

		}


		if (strcmp(buf, "2") == 0)
		{
			//注册
			register_fun(connfd, userinfo_head);
			continue;

		}

		//找回密码
		if (strcmp(buf, "3") == 0)
		{
			find_passwd(connfd, userinfo_head);
			continue;
		}


		if (strcmp(buf, "4") == 0)
		{
			return 0;
		}

	}
	return 0;
}



//选择登陆界面
int select_login_fun(int connfd,
                     struct userinfo * userinfo_head,
                     struct ticket_library * ticket_library_head,
                     struct id_car * id_car_head,
                     struct ticket_library * recycle_head
                    )
{
	char buf[SIZEOF];
	while (1)
	{
		printf("======================GEC SYSTEM===================\n");
		printf("                                                     \n");
		printf("                       1 普通用户登陆                 \n");
		printf("                       2 管理员登陆                   \n");
		printf("                       3 返回                         \n");
		printf("                                                     \n");
		printf("===================================================\n");
		printf("请在客户端输入您的选项：\n");
		rec(connfd, buf);

		//普通用户登陆
		if (strcmp(buf, "1") == 0)
		{
			user_login_fun( connfd, userinfo_head, ticket_library_head, id_car_head);
		}

		//管理员登陆
		if (strcmp(buf, "2") == 0)
		{
			admin_fun(connfd, userinfo_head, ticket_library_head, recycle_head); //管理员登陆
		}
		if (strcmp(buf , "3") == 0)
		{
			return 0;
		}

	}
}



//注册界面
int register_fun(int connfd, struct userinfo * userinfo_head)
{
	system("clear");

	struct userinfo *new = NULL;
	struct userinfo *find = NULL;
	struct userinfo *p = NULL;
	char buf[SIZEOF];
	new = (struct userinfo *)malloc(sizeof(struct userinfo));
	if (new == NULL)
	{
		printf("malloc2 error\n");
	}
	printf("请输入您的用户名：\n");
	bzero(buf, SIZEOF);
	rec(connfd, buf);
	printf("%s\n", buf);
	strcpy(new->name, buf );

	printf("请输入您的密码:\n");
	bzero(buf, SIZEOF);
	rec(connfd, buf);
	printf("%s\n", buf);
	strcpy(new->passwd, buf );

	printf("请输入您的年龄:\n");
	bzero(buf, SIZEOF);
	rec(connfd, buf);
	printf("%s\n", buf);
	new->age = atoi(buf);

	printf("请输入您的电话:\n");
	bzero(buf, SIZEOF);
	rec(connfd, buf);
	printf("%s\n", buf);
	strcpy(new->tel, buf);
	for (find = userinfo_head; find != NULL; find = find->next)
	{
		if (strcmp(new->tel, find->tel) == 0)
		{
			printf("该电话已经注册！请重新注册！\n");
			bzero(buf, SIZEOF);
			free(new);
			error(connfd, buf);
			return 0;
		}
	}
	next(connfd, buf);


	printf("请输入您的密保问题:\n");
	bzero(buf, SIZEOF);
	rec(connfd, buf);
	printf("%s\n", buf);
	strcpy(new->question.s_question, buf);

	printf("请输入您的密保答案:\n");
	bzero(buf, SIZEOF);
	rec(connfd, buf);
	printf("%s\n", buf);
	strcpy( new->question.s_answer, buf);

	printf("请输入您的身份（普通人输入0、军人输入1、医护人员输入2、残疾人输入3）:\n");
	bzero(buf, SIZEOF);
	rec(connfd, buf);
	printf("%s\n", buf);
	new->status = atoi(buf);

	if (new->status >= 4)
	{
		bzero(buf, SIZEOF);
		free(new);
		sprintf(buf, "error");
		send(connfd, buf, SIZEOF, 0);
		return 0;
	}
	bzero(buf, SIZEOF);
	sprintf(buf, "next");
	send(connfd, buf, SIZEOF, 0);

	new->balance = 0;

	new->id_name_flag = 0;

	new->vip_flag = 0;

	new->connfd = -1;
	for (find = userinfo_head; find != NULL; find = find->next)
	{
		if (strcmp(new->tel, find->tel) == 0)
		{
			printf("该电话已经注册！请重新注册！\n");
			bzero(buf, SIZEOF);
			free(new);
			error(connfd, buf);
			return 0;
		}
	}
	next(connfd, buf);
	//上锁
	pthread_mutex_lock(&m);
	new->next = NULL;
	for (p = userinfo_head; p->next != NULL; p = p->next);
	p->next = new;

	save_userinfo_fun(new);
	//解锁
	pthread_mutex_unlock(&m);

}


//普通用户登陆成功界面
int login_success(struct userinfo * user_name,
                  struct userinfo * userinfo_head,
                  struct ticket_library * ticket_library_head,
                  struct id_car * id_car_head
                 )
{

	/*up_remind(user_name, userinfo_head); //上线提醒。*/
	char buf[SIZEOF];
	while (1)
	{
		printf("======================GEC SYSTEM===================\n");
		printf("                                                   \n");
		printf("                      1 查询航班                    \n");
		printf("                      2 快速查询                    \n");
		printf("                      3 购票                       \n");
		printf("                      4 订单查询                    \n");
		printf("                      5 退票与改签                  \n");
		printf("                      6 余额查询                    \n");
		printf("                      7 充值                       \n");
		printf("                      8 修改个人信息                 \n");
		printf("                      9 返回                        \n");
		printf("                                                   \n");
		printf("===================================================\n");
		printf("请在客户端输入您的选项：\n");
		rec(user_name->connfd, buf);

		if (strcmp(buf, "1") == 0)
		{
			check_flight( user_name->connfd, userinfo_head, ticket_library_head);
			continue;
		}


		//快速查询
		if (strcmp(buf, "2") == 0)
		{
			check_fast( user_name->connfd, userinfo_head, ticket_library_head);
			continue;
		}


		//购票
		if (strcmp(buf, "3") == 0)
		{
			buy_ticket(user_name, ticket_library_head, id_car_head);
			continue;
		}


		//订单查询
		if (strcmp(buf, "4") == 0)
		{
			check_order(user_name);
		}



		//退单与改签
		if (strcmp(buf, "5") == 0)
		{
			refund_and_change(user_name, ticket_library_head);
		}



		//余额查询
		if (strcmp(buf, "6") == 0)
		{
			bzero(buf, SIZEOF);
			sprintf(buf, "%d", user_name->balance);
			send(user_name->connfd , buf, SIZEOF, 0);
			printf("====================您的余额为：%d======================\n", user_name->balance);

		}



		//余额充值
		if (strcmp(buf, "7") == 0)
		{
			top_up(user_name);
		}



		//修改个人信息
		if (strcmp(buf, "8") == 0)
		{
			change_user_data(user_name);
		}


		//返回
		if (strcmp(buf, "9") == 0)
		{
			/*du_remind(user_name, userinfo_head);//下线提醒。*/
			//上锁
			pthread_mutex_lock(&m);
			user_name->connfd = -1;
			save_userinfo_fun(user_name);
			//解锁
			pthread_mutex_unlock(&m);

			return 0;
		}
	}

	return 0;
}

//修改个人信息界面
int	change_user_data(struct userinfo *user_name)
{

	char buf[SIZEOF];
	int connfd = user_name->connfd;
	while (1)
	{
		printf("======================GEC SYSTEM===================\n");
		printf("                                                   \n");
		printf("                      1 修改密码                       \n");
		printf("                      2 修改年龄                       \n");
		printf("                      3 返回                        \n");
		printf("                                                   \n");
		printf("===================================================\n");
		printf("请在客户端输入您的选项：\n");

		rec(connfd, buf);

		if (strcmp(buf, "1") == 0)
		{

			change_passwd(user_name);//修改密码

		}
		if (strcmp(buf, "2") == 0)
		{

			change_age(user_name);//修改年龄

		}
		if (strcmp(buf, "3") == 0)
		{

			return 0;

		}
	}
}
