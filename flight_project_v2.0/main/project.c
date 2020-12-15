#include "my_head.h"
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
//主函数
int main(int argc, char const * argv[])
{

	show_welcome_logo();//欢迎界面

	struct ticket_library *ticket_library_head = NULL; //定义航班票库链表头
	ticket_library_head = init_ticket_library_head(); //初始化航班链表头

	struct userinfo *userinfo_head = NULL;//定义用户信息链表头
	userinfo_head = init_userinfo_head();//初始化用户信息链表头

	struct user_ticket *user_ticket_head ;

	struct id_car * id_car_head = NULL; //定义身份证链头
	id_car_head = init_id_car_head();//初始化身份证链头

	struct ticket_library *recycle_head;//定义回收站链表头
	recycle_head = init_ticket_library_head();//初始化回收站链头


	loading_ticket_library(ticket_library_head);//开机加载票库信息

	loading_information(userinfo_head); //开机加载用户信息

	loading_id_library(id_car_head);//开机加载身份库信息

	loading_recycle(recycle_head);//开机加载身份库信息

	//1. 创建一个TCP套接字
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	//2. 绑定IP地址，端口号
	struct sockaddr_in srvaddr;
	socklen_t len = sizeof(srvaddr);
	bzero(&srvaddr, len);

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(atoi(argv[1]));
	srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(sockfd, (struct sockaddr *)&srvaddr, len);

	//3. 设置监听套接字
	listen(sockfd, 5);

	//sockfd  -> 默认是阻塞属性

	//4. 添加非阻塞属性给sockfd
	int state;
	state = fcntl(sockfd, F_GETFL); //state就是sockfd当前的属性。
	state |= O_NONBLOCK;           //state就是新属性了
	fcntl(sockfd, F_SETFL, state);

	//sockfd  -> 具有非阻塞属性

	//5. 等待客户端连接
	struct sockaddr_in cliaddr;
	bzero(&cliaddr, len);
	int connfd;

	//用来传递各种参数
	struct p_set p_s;

	//最多100个线程
	pthread_t tid[100];

	char buf[100];

	int i = 0;

	while (1)
	{

		connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
		if (connfd > 0) //有人连接,就会返回成功
		{
			printf("connfd = %d\n", connfd);
			printf("new connection:%s\n", inet_ntoa(cliaddr.sin_addr));
			p_s.connfd = connfd;
			p_s.userinfo_head = userinfo_head;
			p_s.ticket_library_head = ticket_library_head;
			p_s.id_car_head = id_car_head;
			p_s.recycle_head = recycle_head;
			pthread_create(&tid[i], NULL, func, (void *)&p_s);
			i++;
		}

		//等待所有客户机退出后关闭服务器停机维护
		if (strncmp(buf, "quit", 4) == 0)
		{
			break;
		}

		//提示服务器满载荷状态
		if (i > 100)
		{
			printf("服务器已满载荷!！！\n");
			break;
		}
	}


	//接合线程
	for (int a = 0; a < i; a++)
	{
		pthread_join(tid[a], NULL);
	}


	//销毁读写锁
	pthread_mutex_destroy(&m);

	close(sockfd);
	close(connfd);
	return 0;
}
