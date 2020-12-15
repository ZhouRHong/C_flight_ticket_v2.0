#include "my_head.h"

//充值
int top_up(struct userinfo *user_name)
{
	int balance = 0;
	char buf[SIZEOF];
	int connfd = user_name->connfd;
	printf("=========================================================\n");
	printf("=========              充值活动！！！                ======\n");
	printf("=========             充值100送20                   ======\n");
	printf("=========            充值500送200                   ======\n");
	printf("=========         一次充值5000成为终身VIP            ======\n");
	printf("==========================================================\n");

	printf("请输入你要充值的金额：");
	rec(connfd, buf);
	balance = atoi(buf);
	if (balance >= 100 && balance < 500)
	{
		next(connfd, buf);
		bzero(buf, SIZEOF);
		user_name->balance = user_name->balance + balance + 20;
		sprintf(buf, "充值成功！！！\n余额为%d", user_name->balance);
		send(connfd, buf, SIZEOF, 0);
		next(connfd, buf);

	}
	else if (balance  >= 500 && balance < 5000)
	{
		next(connfd, buf);
		bzero(buf, SIZEOF);
		user_name->balance = user_name->balance + balance + 200;
		sprintf(buf, "充值成功！！！\n余额为%d", user_name->balance);
		send(connfd, buf, SIZEOF, 0);
		next(connfd, buf);
	}
	else if (balance >= 5000)
	{
		next(connfd, buf);
		bzero(buf, SIZEOF);
		user_name->balance = user_name->balance + balance + 200;
		sprintf(buf, "充值成功！！！\n余额为%d", user_name->balance);
		send(connfd, buf, SIZEOF, 0);
		if (user_name->vip_flag != 1)
		{
			user_name->vip_flag = 1;//vip标志位置1，成为会员
			printf("恭喜您成为了终身VIP!!!\n");
			error(connfd, buf);
		}
		next(connfd, buf);


	}
	else if (balance < 0)
	{
		error(connfd, buf);
		printf("充值错误！！！\n");
		return 0;
	}
	else
	{
		next(connfd, buf);
		bzero(buf, SIZEOF);
		user_name->balance += balance;
		sprintf(buf, "充值成功！！！\n余额为%d", user_name->balance);
		send(connfd, buf, SIZEOF, 0);
		next(connfd, buf);
	}
	//上锁
	pthread_mutex_lock(&m);
	save_userinfo_fun(user_name);
	//解锁
	pthread_mutex_unlock(&m);

	return 0;
}

//修改密码
int change_passwd(struct userinfo *user_name)
{
	char passwd_buf[10];
	char user_name_passwd1[10];
	char user_name_passwd2[10];
	char buf[SIZEOF];
	int connfd = user_name->connfd;

	printf("请输入你的旧密码：\n");
	rec(connfd, buf);
	strcpy(passwd_buf, buf);

	if (strcmp(user_name->passwd, passwd_buf) == 0)
	{
		next(connfd, buf);
		printf("请输入你的新密码：\n");
		rec(connfd, buf);
		strcpy(user_name_passwd1, buf);

		printf("二次输入：\n");
		rec(connfd, buf);
		strcpy(user_name_passwd2, buf);

		if (strcmp(user_name_passwd1, user_name_passwd2) == 0)
		{
			next(connfd, buf);
			strcpy(user_name->passwd, user_name_passwd1);
			//上锁
			pthread_mutex_lock(&m);
			save_userinfo_fun(user_name);
			//解锁
			pthread_mutex_unlock(&m);
			printf("=========修改密码成功========\n");
			return 0;
		}
		else
		{
			error(connfd, buf);
			printf("两次密码不一致！！！\n");
			return -2;
		}
	}
	else
	{
		error(connfd, buf);
		printf("密码错误！！！\n");
		return -1;
	}

}

//修改年龄
int change_age(struct userinfo *user_name)
{
	int age_buf;
	char buf[SIZEOF];
	int connfd = user_name->connfd;
	printf("请输入新的年龄：\n");
	rec(connfd, buf);
	age_buf = atoi(buf);

	if (age_buf <= user_name->age)
	{
		error(connfd, buf);
		printf("年龄输入错误！！！\n");
		return -1;
	} else
	{
		next(connfd, buf);
		user_name->age = age_buf;

	}
	//上锁
	pthread_mutex_lock(&m);
	save_userinfo_fun(user_name);
	//解锁
	pthread_mutex_unlock(&m);
	printf("年龄修改成功！！！\n");
	return 0;
}


//用户登陆
int user_login_fun(int connfd,
                   struct userinfo * userinfo_head,
                   struct ticket_library * ticket_library_head,
                   struct id_car * id_car_head
                  )
{

	//1. 请求用户输入需要登录的用户名
	char u_name[10] = {0};
	char user_passwd[10] = {0};
	char buf[SIZEOF];

	printf("请输入您的用户名：\n");
	bzero(buf, SIZEOF);
	rec(connfd, buf);
	strcpy(u_name, buf);
	int i = 0;

	//2. 检查这个想登录的用户注册过了没有。（在注册链表中寻找这个用户名）
	struct userinfo *user_name = NULL;

	for (user_name = userinfo_head->next; user_name != NULL; user_name = user_name->next)
	{


		//查找有没有此用户
		if (strcmp(user_name->name, u_name) == 0)
		{
			next(connfd, buf);

			printf("请输入您的密码：\n");
			bzero(buf, SIZEOF);
			rec(connfd, buf);
			strcpy(user_passwd, buf);
			if (strcmp(user_name->passwd, user_passwd) == 0)
			{
				next(connfd, buf);

				if (user_name->connfd == -1)
				{
					next(connfd, buf);

					//上锁
					pthread_mutex_lock(&m);
					user_name->connfd = connfd;//把套接字存入用户信息节点
					//解锁
					pthread_mutex_unlock(&m);
					//保存到链表。
					printf("登陆成功！！！\n");

					login_success(user_name, userinfo_head, ticket_library_head, id_car_head
					             );

				}
				else
				{
					error(connfd, buf);
					printf("您的账号已登陆！！！\n");
					return -3;
				}

			} else
			{
				error(connfd, buf);
				printf("密码错误！！！\n");
				return -2;
			}
			i++;
		}
	}
	if (i == 0)
	{
		error(connfd, buf);
		printf("该账号不存在!!!\n");
	}


	return -4;
}



//找回密码
int find_passwd(int connfd, struct userinfo * userinfo_head)
{
	system("clear");
	char find_passwd_name[20];
	char new_passwd1[10];
	char new_passwd2[10];
	char answer_buf[50];
	char buf[SIZEOF];

	printf("请输入您要修改密码的账户：\n");
	rec(connfd, buf);
	strcpy(find_passwd_name, buf);
	struct userinfo *change = NULL;
	//遍历链表看有没有这个账户
	for (change = userinfo_head->next; change != NULL; change = change->next)
	{
		if (strcmp(change->name, find_passwd_name) == 0)
		{
			next(connfd, buf);
			bzero(buf, SIZEOF);
			printf("密保问题：%s\n", change->question.s_question);
			send(connfd, change->question.s_question, SIZEOF, 0);
			printf("请输入您的回答：\n");
			rec(connfd, buf);
			strcpy(answer_buf, buf );
			if (strcmp(change->question.s_answer, answer_buf) == 0)
			{
				next(connfd, buf);
				printf("请输入您的新密码：\n");
				rec(connfd, buf);
				strcpy(new_passwd1, buf);
				printf("二次输入：\n");
				rec(connfd, buf);
				strcpy(new_passwd2,  buf);
				if (strcmp(new_passwd1, new_passwd2) == 0)
				{
					next(connfd, buf);
					strcpy(change->passwd, new_passwd1);
					printf("===修改密码成功！！！===\n");
					return 0;
				}
				else
				{
					printf("两次密码不一致！！！\n");
					error(connfd, buf);
					return -1;
				}
			}
			else
			{

				printf("密保错误！！！\n");
				error(connfd, buf);
				return -2;
			}
		}
	}
	printf("该用户未注册！！！\n");
	error(connfd, buf);
	return 0;

}
