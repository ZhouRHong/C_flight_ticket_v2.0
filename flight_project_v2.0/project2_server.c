#include "head.h"

#define PERSON   0
#define SOLDIER  1
#define MEDICAL  2
#define DISABLED 3

#define SIZEOF 200


struct safe_question
{
	char s_question[50];
	char s_answer[50];
};

//参数集，用来给线程传递各种参数
struct p_set
{
	int connfd;//套接字

	struct userinfo *userinfo_head;//用户信息链表头

	struct ticket_library *ticket_library_head;//航班链表头

	struct id_car *id_car_head;//身份证链表头

	struct ticket_library *recycle_head;//回收站链表头

};

//互斥锁
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void rec(int connfd, char *buf);
//发送next函数 让客户端可以到下一步
void next(int connfd, char *buf);
//发送error给客户端，让客户端收到错误信息
void error(int connfd, char *buf);

//票库
struct ticket_library {
	char number[10];//航班号
	char staddress[10];//起点站
	char arraddress[15];//终点站
	char date[15];//班期
	char type[2];//机型
	char stime[10];//起飞时间
	int price;//票价
	int ticket_num;//余票

	struct ticket_library *next;
};

//保存航班票库
void save_flight_fun(struct ticket_library *new);

//用户票链节点
struct user_ticket {
	char number[10];//航班号
	char staddress[10];//起点站
	char arraddress[15];//终点站
	char date[15];//班期
	char type[2];//机型
	char stime[10];//起飞时间
	int price;//票价
	int insurance;//保险


	struct user_ticket *next;
};

//用户信息节点
struct userinfo
{
	char name[10];  //姓名
	char passwd[10];  //密码
	struct safe_question question;//密保
	int age;  //年龄
	char tel[10];  //电话号码
	int status; //身份：普通--0，军人--1，医护人员--2，残疾人--3
	int balance; //余额
	int id_name_flag;//实名认证标志位  未实名为0，实名后为1
	int vip_flag;//标志位   普通用户为0 VIP用户1
	int connfd;//用户连接套接字，下线为-1

	struct user_ticket user_ticket_head;//用户票链头

	struct userinfo *next;
};



//身份证链节点
struct id_car {

	char id_num[10];//身份证号：6位
	char name[10];//名字
	int  status;//身份：普通--0，军人--1，医护人员--2，残疾人--3

	struct id_car *next;
};



//初始化票库链表头
struct ticket_library *init_ticket_library_head()
{
	struct ticket_library *ticket_library_head = NULL;
	ticket_library_head = (struct ticket_library *)malloc(sizeof(struct ticket_library));
	if (ticket_library_head == NULL)
	{
		printf("malloc ticket_library_head error!!!\n");
	}
	ticket_library_head->next = NULL;
	return ticket_library_head;
}




//初始化用户信息头节点
struct userinfo *init_userinfo_head()
{
	struct userinfo *userinfo_head = NULL;
	userinfo_head = (struct userinfo *)malloc(sizeof(struct userinfo));
	if (userinfo_head == NULL)
		printf("malloc1 error\n");

	userinfo_head->next = NULL;
	return userinfo_head;
}



// 新建票链节点
struct user_ticket *new_ticket_node(struct user_ticket *ticket_data)
{
	struct user_ticket *user_ticket_head = NULL;
	user_ticket_head = (struct user_ticket *)malloc(sizeof(struct user_ticket));
	if (user_ticket_head == NULL)
	{
		printf("malloc user_ticket_head error!!!\n");
	}

	*user_ticket_head = *ticket_data;
	user_ticket_head->next = NULL;

	return user_ticket_head;
}


//初始化身份证库链表头
struct id_car *init_id_car_head()
{
	struct id_car *id_car_head = NULL;
	id_car_head = (struct id_car *)malloc(sizeof(struct id_car));
	if (id_car_head == NULL)
	{
		printf("malloc id_car_head error!!!\n");
	}
	id_car_head->next = NULL;
	return id_car_head;
}





void save_userinfo_fun(struct userinfo * new);



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


//分割用户信息文件
int file_data_to_array(char *file_buf, struct userinfo *user)
{

	char seps[] = ",";  //分隔的字符是什么
	char *tmp;
	tmp = strtok(file_buf, seps);
	strcpy(user->name, tmp);

	tmp = strtok(NULL, seps);
	strcpy(user->passwd, tmp);

	tmp = strtok(NULL, seps);
	user->age = atoi(tmp);

	tmp = strtok(NULL, seps);
	strcpy(user->tel, tmp);

	tmp = strtok(NULL, seps);
	strcpy(user->question.s_question, tmp);

	tmp = strtok(NULL, seps);
	strcpy(user->question.s_answer, tmp);

	tmp = strtok(NULL, seps);
	user->status = atoi(tmp);

	tmp = strtok(NULL, seps);
	user->balance = atoi(tmp);

	tmp = strtok(NULL, seps);
	user->id_name_flag = atoi(tmp);

	tmp = strtok(NULL, seps);
	user->vip_flag = atoi(tmp);

	tmp = strtok(NULL, seps);
	user->connfd = atoi(tmp);


	return 0;
}



//分割库信息文件
int flight_file_data_to_array(char *file_buf, struct ticket_library *new)
{
	char seps[] = ",";  //分隔的字符是什么
	char *tmp;
	tmp = strtok(file_buf, seps);
	strcpy(new->number, tmp);

	tmp = strtok(NULL, seps);
	strcpy(new->staddress, tmp);

	tmp = strtok(NULL, seps);
	strcpy(new->arraddress, tmp);

	tmp = strtok(NULL, seps);
	strcpy(new->date, tmp);

	tmp = strtok(NULL, seps);
	strcpy(new->type, tmp);

	tmp = strtok(NULL, seps);
	strcpy(new->stime, tmp);

	tmp = strtok(NULL, seps);
	new->price = atoi(tmp);

	tmp = strtok(NULL, seps);
	new->ticket_num = atoi(tmp);


	return 0;
}



//分割身份库文件
int flight_id_data_to_array(char *file_buf, struct id_car *id)
{
	char seps[] = ",";  //分隔的字符是什么
	char *tmp;
	tmp = strtok(file_buf, seps);
	strcpy(id->id_num, tmp);

	tmp = strtok(NULL, seps);
	strcpy(id->name, tmp);

	tmp = strtok(NULL, seps);
	id->status = atoi(tmp);

	return 0;
}


int recycle_file_data_to_array(char *file_buf, struct ticket_library *new)
{
	char seps[] = ",";  //分隔的字符是什么
	char *tmp;
	tmp = strtok(file_buf, seps);
	strcpy(new->number, tmp);

	tmp = strtok(NULL, seps);
	strcpy(new->staddress, tmp);

	tmp = strtok(NULL, seps);
	strcpy(new->arraddress, tmp);

	tmp = strtok(NULL, seps);
	strcpy(new->date, tmp);

	tmp = strtok(NULL, seps);
	strcpy(new->type, tmp);

	tmp = strtok(NULL, seps);
	strcpy(new->stime, tmp);

	tmp = strtok(NULL, seps);
	new->price = atoi(tmp);

	tmp = strtok(NULL, seps);
	new->ticket_num = atoi(tmp);

}


//读取加载票链文件
int read_user_ticket_file(struct userinfo *user)
{
	struct user_ticket *new;
	char file_path[50] = {0};
	sprintf(file_path, "./user_ticket/%s.txt", user->tel);

	FILE *fp = NULL;

	fp = fopen(file_path, "r");
	if (fp == NULL)
	{
		//此用户票链不存在
		return 0;
	}


	int i = 0;
	while (1)
	{

		new = (struct user_ticket *) malloc(sizeof(struct user_ticket));

		i = fscanf(fp, "%s %s %s %s %s %s %d %d\n",
		           new->number,
		           new->staddress,
		           new->arraddress,
		           new->date,
		           new->type,
		           new->stime,
		           &new->price,
		           &new->insurance);

		if (i == EOF)
		{
			free(new);
			break;
		}

		new->next = user->user_ticket_head.next;
		user->user_ticket_head.next = new;

	}
	fclose(fp);

}


//读取加载用户文件
int read_file(FILE *fp, struct userinfo *userinfo_head)
{

	char file_buf[200] = {0};
	fread(file_buf, 200, 1, fp);

	struct userinfo *user = NULL;
	user = (struct userinfo *)malloc(sizeof(struct userinfo));
	if (user == NULL)
	{
		printf("malloc user error\n");
	}

	file_data_to_array(file_buf, user);
	read_user_ticket_file(user);

	user->next = userinfo_head->next;
	userinfo_head->next = user;



	return 0;
}


//读取加载票库文件
int read_flight_file(FILE *fp, struct ticket_library *ticket_library_head)
{
	char file_buf[200] = {0};
	fread(file_buf, 200, 1, fp);

	struct ticket_library *new = NULL;
	new = (struct ticket_library *)malloc(sizeof(struct ticket_library));
	if (new == NULL)
	{
		printf("malloc new error\n");
	}

	flight_file_data_to_array(file_buf, new);


	new->next = ticket_library_head->next;
	ticket_library_head->next = new;


	return 0;
}




//读取加载回收站信息
int read_recycle_file(FILE *fp, struct ticket_library *recycle_head)
{
	char file_buf[200] = {0};
	fread(file_buf, 200, 1, fp);

	struct ticket_library *new = NULL;
	new = (struct ticket_library *)malloc(sizeof(struct ticket_library));
	if (new == NULL)
	{
		printf("malloc new error\n");
	}

	recycle_file_data_to_array(file_buf, new);


	new->next = recycle_head->next;
	recycle_head->next = new;


	return 0;
}

//读取加载身份库库文件
int read_id_file(FILE *fp, struct id_car *id_car_head)
{

	char file_buf[50] = {0};
	fread(file_buf, 50, 1, fp);

	struct id_car *id = NULL;
	id = (struct id_car *)malloc(sizeof(struct id_car));
	if (id == NULL)
	{
		printf("malloc id error\n");
	}

	flight_id_data_to_array(file_buf, id);

	id->next = id_car_head->next;
	id_car_head->next = id;


	return 0;
}


//加载用户信息
int loading_information(struct userinfo *userinfo_head)
{
	DIR *dp = opendir("./usr_data");
	if (dp == NULL)
		printf("打开目录失败!\n");

	struct dirent *ep = NULL;


	while (1)
	{
		ep = readdir(dp);
		if (ep == NULL)
		{
			break;
		}

		if (ep->d_name[0] == '.')
		{
			continue;
		}

		char file_path[50] = {0};
		sprintf(file_path, "./usr_data/%s", ep->d_name);

		FILE *fp = NULL;
		fp = fopen(file_path, "r");

		if (fp == NULL)
		{
			printf("此用户不存在，打开文件失败！！！\n");
			return -1;
		}

		read_file(fp, userinfo_head);
		fclose(fp);

	}
	closedir(dp);


	return 0;
}

//开机加载用户票链
int loading_ticket_library(struct ticket_library *ticket_library_head)
{
	DIR *dp = opendir("./flight_data");
	if (dp == NULL)
		printf("打开目录失败!\n");

	struct dirent *ep = NULL;

	while (1)
	{
		ep = readdir(dp);
		if (ep == NULL)
		{
			break;
		}

		if (ep->d_name[0] == '.')
		{
			continue;
		}

		char file_path[50] = {0};
		sprintf(file_path, "./flight_data/%s", ep->d_name);
		FILE *fp = NULL;
		fp = fopen(file_path, "r");

		if (fp == NULL)
		{
			printf("此用户不存在，打开文件失败！！！\n");
			return -1;
		}

		read_flight_file(fp, ticket_library_head);
		fclose(fp);

	}
	closedir(dp);


	return 0;
}



//开机加载身份库信息
int loading_id_library(struct id_car *id_car_head)
{
	DIR *dp = opendir("./id_library");
	if (dp == NULL)
		printf("打开目录失败!\n");

	struct dirent *ep = NULL;

	while (1)
	{
		ep = readdir(dp);

		if (ep == NULL)
		{
			break;
		}

		if (ep->d_name[0] == '.')
		{
			continue;
		}

		char file_path[50] = {0};
		sprintf(file_path, "./id_library/%s", ep->d_name);
		FILE *fp = NULL;
		fp = fopen(file_path, "r");

		if (fp == NULL)
		{
			printf("此身份证不存在，打开文件失败！！！\n");
			return -1;
		}

		read_id_file(fp, id_car_head);
		fclose(fp);

	}
	closedir(dp);
}



//开机加载回收链信息
int loading_recycle(struct ticket_library *recycle_head)
{
	DIR *dp = opendir("./recycle");
	if (dp == NULL)
		printf("打开目录失败!\n");

	struct dirent *ep = NULL;

	while (1)
	{
		ep = readdir(dp);

		if (ep == NULL)
		{
			break;
		}

		if (ep->d_name[0] == '.')
		{
			continue;
		}

		char file_path[50] = {0};
		sprintf(file_path, "./recycle/%s", ep->d_name);
		FILE *fp = NULL;
		fp = fopen(file_path, "r");

		if (fp == NULL)
		{
			printf("此身份证不存在，打开文件失败！！！\n");
			return -1;
		}

		read_recycle_file(fp, recycle_head);
		fclose(fp);

	}
	closedir(dp);
}





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

//保存用户票链
void save_user_ticket_fun(struct user_ticket *new, struct userinfo *user_name)
{
	//1. 拼接路径
	char path[50] = {0};
	sprintf(path, "./user_ticket/%s.txt", user_name->tel);
	printf("%s\n", path );


	//2. 打开文件。
	FILE *fp = NULL;
	fp = fopen(path, "a");
	if (fp == NULL)
		printf("fopen1184 error!\n");

	//3. 将内容保存到文件中
	fprintf(fp, "%s %s %s %s %s %s %d %d\n",
	        new->number,
	        new->staddress,
	        new->arraddress,
	        new->date,
	        new->type,
	        new->stime,
	        new->price,
	        new->insurance);
	//5. 关闭文件
	fclose(fp);
	printf("保存用户票单成功\n");

	return;
}


//删除节点后保存用户票链
void save_delete_ticket_fun(struct userinfo *user_name)
{
	//1. 拼接路径
	char path[50] = {0};
	struct user_ticket *ticket = NULL;
	sprintf(path, "./user_ticket/%s.txt", user_name->tel);

	//2. 打开文件。
	FILE *fp = NULL;
	fp = fopen(path, "w");
	if (fp == NULL)
		printf("fopen1184 error!\n");


	for (ticket = user_name->user_ticket_head.next; ticket != NULL; ticket = ticket->next)
	{
		//3. 将内容保存到文件中
		fprintf(fp, "%s %s %s %s %s %s %d %d\n",
		        ticket->number,
		        ticket->staddress,
		        ticket->arraddress,
		        ticket->date,
		        ticket->type,
		        ticket->stime,
		        ticket->price,
		        ticket->insurance);
	}

	//5. 关闭文件
	fclose(fp);
	printf("保存用户票单成功\n");

	return;
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


void up_remind(struct userinfo * user_name, struct userinfo * userinfo_head)//上线提醒。
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


//保存航班
void save_flight_fun(struct ticket_library * new)
{
	//1. 拼接路径
	char path[50] = {0};
	sprintf(path, "./flight_data/%s.txt", new->number);
	printf("%s\n", path );

	//2. 拼接内容
	char file_data[500] = {0};
	sprintf(file_data, "%s,%s,%s,%s,%s,%s,%d,%d", new->number,
	        new->staddress,
	        new->arraddress,
	        new->date,
	        new->type,
	        new->stime,
	        new->price,
	        new->ticket_num
	       );
	//3. 打开文件。
	FILE *fp = NULL;
	fp = fopen(path, "w");
	system("pwd");
	if (fp == NULL)
		printf("fopen  ./flight_data/%s.txt  error!\n", new->number);

	//4. 将内容保存到文件中
	fwrite(file_data, 100, 1, fp);

	//5. 关闭文件
	fclose(fp);
	printf("票库已经更新！\n");

	return;
}



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


//保存注册信息函数
void save_userinfo_fun(struct userinfo * new)
{
	//1. 拼接路径
	char path[50] = {0};
	sprintf(path, "./usr_data/%s.txt", new->name);

	//2. 拼接内容
	char file_data[SIZEOF] = {0};
	sprintf(file_data, "%s,%s,%d,%s,%s,%s,%d,%d,%d,%d,%d", new->name,
	        new->passwd,
	        new->age,
	        new->tel,
	        new->question.s_question,
	        new->question.s_answer,
	        new->status,
	        new->balance,
	        new->id_name_flag,
	        new->vip_flag,
	        -1);
	//3. 打开文件。
	FILE *fp = NULL;
	fp = fopen(path, "w");
	if (fp == NULL)
		printf("fopen save error!\n");

	//4. 将内容保存到文件中
	fwrite(file_data, SIZEOF, 1, fp);

	printf("======数据保存成功======\n");

	//5. 关闭文件
	fclose(fp);

	return;

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


