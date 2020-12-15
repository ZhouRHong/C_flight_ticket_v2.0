#include "my_head.h"


/*void up_remind(struct userinfo * user_name, struct userinfo * userinfo_head)//上线提醒。
{
	char buf[SIZEOF];
	struct userinfo *p = NULL;

	for (p = userinfo_head->next; p != NULL; p = p->next)
	{
		if (p->connfd != -1 && p->connfd != user_name->connfd)
		{
			// //发送登陆的用户名过去
			// bzero(buf, SIZEOF);
			// sprintf(buf, "%s", user_name->name);
			// send(p->connfd, buf, SIZEOF, 0);

			bzero(buf, SIZEOF);
			sprintf(buf, "账户：%s 上线了！！！", user_name->name);
			send(p->connfd, buf, SIZEOF, 0);
			usleep(10000);
		}

	}

}
*/

/*void du_remind(struct userinfo * user_name, struct userinfo * userinfo_head) //下线提醒。
{
	char buf[SIZEOF];
	struct userinfo *p = NULL;

	for (p = userinfo_head->next; p != NULL; p = p->next)
	{
		if (p->connfd != -1)
		{
			//发送退出信号过去
			bzero(buf, SIZEOF);
			sprintf(buf, "%s", "exit");
			send(user_name->connfd, buf, SIZEOF, 0);
			usleep(10000);

			bzero(buf, SIZEOF);
			sprintf(buf, "账户：%s 下线了！！！", user_name->name);
			send(p->connfd, buf, SIZEOF, 0);
			usleep(10000);
		}

	}
}
*/


//发送next函数 让客户端可以到下一步
void next(int connfd, char *buf)
{
	bzero(buf, SIZEOF);
	sprintf(buf, "next");
	send(connfd, buf, SIZEOF, 0);
}


//发送error给客户端，让客户端收到错误信息
void error(int connfd, char *buf)
{
	bzero(buf, SIZEOF);
	sprintf(buf, "error");
	send(connfd, buf, SIZEOF, 0);
}


//接收函数
void rec(int connfd, char *buf)
{
	bzero(buf, SIZEOF);
	recv(connfd, buf, SIZEOF, 0);
	// char *p = buf;
	// for (;; p++)
	// {
	// 	if (*p == '\n')
	// 	{
	// 		*p = '\0';
	// 		break;
	// 	}
	// }
	buf[strlen(buf) - 1] = '\0';
}



//线程处理
void *func(void *arg)
{
	int connfd;

	struct p_set p_s = *(struct p_set *)arg;

	struct userinfo *userinfo_head = NULL; //用户信息链表头

	struct ticket_library *ticket_library_head = NULL; //航班链表头

	struct id_car *id_car_head = NULL; //身份证链表头

	struct ticket_library *recycle_head = NULL; //回收站链表头

	connfd = p_s.connfd;
	userinfo_head = p_s.userinfo_head;
	ticket_library_head = p_s.ticket_library_head;
	id_car_head = p_s.id_car_head;
	recycle_head = p_s.recycle_head;

	char buf[SIZEOF];
	main_meum(connfd, userinfo_head, ticket_library_head, id_car_head, recycle_head); //主界面
	printf("线程退出。\n");
	pthread_exit(NULL);
}

