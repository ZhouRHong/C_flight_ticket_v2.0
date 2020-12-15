#include "head.h"

#define SIZEOF 200

//接收函数
int rec(int sockfd, char *buf);
//发送函数
void sen(int sockfd, char *buf);

//用来给客户端发送选择信号

void sele(int sockfd, char *i)
{
	char buf[SIZEOF];
	bzero(buf, SIZEOF);
	sprintf(buf, "%s\n", i);
	send(sockfd, buf, SIZEOF, 0);

}

//显示bmp图片
int mmap_show_bmp(const char *bmp_path)
{
	FILE *fp;
	int n, lcd;
	int i, j;
	int x, y;
	int k;
	char bmp_buf[800 * 480 * 3] = {0};
	char lcd_buf[800 * 480 * 4] = {0};
	char show_buf[800 * 480 * 4] = {0};

	/*1. 打开图片 */
	fp = fopen(bmp_path, "r"); //默认在最开头
	if (fp == NULL)
		printf("fopen error!\n");

	/*2. 打开lcd设备 */
	lcd = open("/dev/fb0", O_RDWR);
	if (lcd < 0)
		printf("open fb0 error!\n");

	/*3. 先跳过54个头数据 */
	fseek(fp, 54, SEEK_SET);

	/*4. 将图片的数据读取到缓冲区中 */
	n = fread(bmp_buf, 800 * 480 * 3, 1, fp);
	if (n != 1)
		printf("fread error!\n");

	/*5. 将24位转32位 */
	for (i = 0, j = 0; i < 800 * 480 * 4; i += 4, j += 3)
	{
		lcd_buf[i]   = bmp_buf[j];
		lcd_buf[i + 1] = bmp_buf[j + 1];
		lcd_buf[i + 2] = bmp_buf[j + 2];
		lcd_buf[i + 3] = 0;
	}

	/* 6. 上下颠倒 */
	for (y = 0; y < 480; y++)
	{
		for (x = 0; x < 800 * 4; x++)
		{
			show_buf[800 * 4 * y + x] = lcd_buf[800 * 4 * (479 - y) + x];
		}
	}

	/* 7. 产生一片内存空间，作为映射 */
	char *p = (char *)mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd, 0);
	if (p == (void *) - 1)
		printf("mmap error!\n");

	/* 8. 将数据拷贝到内存上 */
	for (k = 0; k < 800 * 480 * 4; k++)
	{
		memcpy(p + k, &show_buf[k], 1);
	}

	/* 7. 关闭文件 */
	munmap(p, 800 * 480 * 4);
	fclose(fp);
	close(lcd);

	return 0;
}


//显示欢迎界面
void show_welcome_logo()
{
	mmap_show_bmp("./bmp_dir/welcome.bmp");
	printf("======================GEC SYSTEM===================\n");
	printf("                                                   \n");
	printf("                       欢迎使用                     \n");
	printf("                                                   \n");
	printf("===================================================\n");
	printf("正在加载信息...\n");

	sleep(1);

}


//查询所有航班
int check_all_flight(int sockfd)
{
	char buf[SIZEOF];
	printf("===============================================================\n");
	printf("航班号   起点站   终点站   班期      机型   起飞时间  票价   余票\n");
	while (1)
	{
		int rec_value = rec(sockfd, buf);
		// printf("rec_value=%d\n", rec_value);
		// while (rec_value < 0 )
		// {
		// 	rec_value = rec(sockfd, buf);
		// 	printf("rec_value=%d\n", rec_value);
		// }



		if (strcmp(buf, "error") == 0)
		{
			break;
		}
		printf("%s", buf);
	}
	return 0;
}


//目的地查询
int search_flight_location(int sockfd)
{
	char buf[SIZEOF];
	bzero(buf, sizeof(buf));
	printf("请输入你要查询航班的目的地：\n");
	fgets(buf, sizeof(buf), stdin);
	send(sockfd, buf, strlen(buf), 0);
	printf("===============================================================\n");
	printf("航班号   起点站   终点站   班期      机型   起飞时间  票价   余票\n");
	while (1)
	{
		rec(sockfd, buf);
		if (strncmp(buf, "next", 4) == 0)
		{
			break;
		}
		else if (strncmp(buf, "error", 5) == 0)
		{
			printf("buf:%s\n", buf );
			printf("没有此航班！！！\n");
			break;
		}
		printf("%s", buf);
	}


	return 0;
}


//出发日期查询
int search_flight_date(int sockfd)
{
	char buf[SIZEOF];
	printf("请输入你要查询的航班的出发时间：\n");
	sen(sockfd, buf);
	printf("===============================================================\n");
	printf("航班号   起点站   终点站   班期      机型   起飞时间  票价   余票\n");
	while (1)
	{
		rec(sockfd, buf);
		if (strncmp(buf, "next", 4) == 0)
		{
			break;
		}
		else if (strncmp(buf, "error", 5) == 0)
		{
			printf("没有此出发时间的航班！！！\n");
			break;
		}
		printf("%s", buf);
	}

	return 0;
}


//机型查询
int search_flight_type(int sockfd)
{
	char buf[SIZEOF];
	printf("请输入你要查询的航班的机型：\n");
	sen(sockfd, buf);
	printf("===============================================================\n");
	printf("航班号   起点站   终点站   班期      机型   起飞时间  票价   余票\n");
	while (1)
	{
		rec(sockfd, buf);
		if (strncmp(buf, "next", 4) == 0)
		{
			break;
		}
		else if (strncmp(buf, "error", 5) == 0)
		{
			printf("没有此机型的航班！！！\n");
			break;
		}
		printf("%s", buf);
	}

	return 0;
}


//票价查询
int search_flight_price(int sockfd)
{
	char buf[SIZEOF];
	printf("请输入你要查询航班的票价：\n");
	sen(sockfd, buf);
	printf("===============================================================\n");
	printf("航班号   起点站   终点站   班期      机型   起飞时间  票价   余票\n");
	while (1)
	{
		rec(sockfd, buf);
		if (strncmp(buf, "next", 4) == 0)
		{
			break;
		}
		else if (strncmp(buf, "error", 5) == 0)
		{
			printf("没有此票价的航班！！！\n");
			break;
		}
		printf("%s", buf);
	}

	return 0;
}


//条件查询
int search_flight(int fd, int sockfd)
{
	system("clear");
	int y = 0;

	struct input_event buf;

	while (1)
	{
		mmap_show_bmp("./bmp_dir/search_flight.bmp");
		read(fd, &buf, sizeof(buf));

		if (buf.type == 3 && buf.code == 1)
		{
			y = buf.value;
		}

		if (y < 120)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)

			{
				sele(sockfd, "1");
				search_flight_location(sockfd);
			}

		}
		if ( y < 240 && y > 120)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)

			{
				sele(sockfd, "2");
				search_flight_date(sockfd);

			}

		}
		if ( y < 360 && y > 240)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "3");
				search_flight_type(sockfd);

			}

		}
		if ( y < 480 && y > 360)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "4");
				search_flight_price(sockfd);

			}

		}
		if (y > 480)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "5");
				return 0;

			}

		}
	}
	/*system("clear");
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
		bzero(buf, sizeof(buf));
		fgets(buf, sizeof(buf), stdin);
		send(sockfd, buf, strlen(buf), 0);
		if (strlen(buf) > 2)
		{
			printf("输入有误，请重新输入！！！\n");
			continue;
		}


		if (strncmp(buf, "1", 1) == 0)
		{
			search_flight_location(sockfd);
			continue;
		}


		if (strncmp(buf, "2", 1) == 0)
		{
			search_flight_date(sockfd);
			continue;

		}


		if (strncmp(buf, "3", 1) == 0)
		{
			search_flight_type(sockfd);
			continue;

		}

		if (strncmp(buf, "4", 1) == 0)
		{
			search_flight_price(sockfd);
			continue;
		}

		if (strncmp(buf, "5", 1) == 0)
		{
			return 0;

		}
	}*/

}




//查询航班界面
int check_flight(int fd, int sockfd)
{

	int x = 0, y = 0;

	struct input_event buf;

	while (1)
	{
		mmap_show_bmp("bmp_dir/check_flight.bmp");
		/*	printf("======================GEC SYSTEM===================\n");
			printf("                                                   \n");
			printf("                       1 查询所有航班               \n");
			printf("                       2 条件查询                   \n");
			printf("                       3 返回                      \n");
			printf("                                                   \n");
			printf("===================================================\n");
			printf("请输入您的选项：\n");*/

		read(fd, &buf, sizeof(buf));

		if (buf.type == 3 && buf.code == 0)
		{
			x = buf.value;
		}
		if (buf.type == 3 && buf.code == 1)
		{
			y = buf.value;
		}

		if ( y < 200)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "1");
				check_all_flight(sockfd);
			}

		}
		if (y < 400 && y > 200)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "2");
				search_flight(fd, sockfd);//条件查询

			}

		}
		if (y > 400)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "3");
				return 0;

			}
		}
	}

	/*char buf[SIZEOF];
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
		bzero(buf, sizeof(buf));
		fgets(buf, sizeof(buf), stdin);
		send(sockfd, buf, strlen(buf), 0);
		if (strlen(buf) > 2)
		{
			printf("输入有误，请重新输入！！！\n");
			continue;
		}
		if (strncmp(buf, "1", 1) == 0)
		{
			check_all_flight(sockfd);
			continue;
		}

		if (strncmp(buf, "2", 1) == 0)
		{
			search_flight(sockfd);//条件查询
			continue;

		}

		if (strncmp(buf, "3", 1) == 0)
		{
			return 0;
		}

	}*/
	return 0;
}


//快速查询
int check_fast(int fd, int sockfd)
{
	system("clear");
	char buf[SIZEOF];

	printf("====================================================================\n");
	printf("航班号有：\n");
	while (1)
	{
		rec(sockfd, buf);
		if (strncmp(buf, "error", 5) == 0)
		{
			break;
		}
		printf("%s\t", buf);
	}


	printf("\n请输入您要查询的航班号：\n");

	sen(sockfd, buf);

	printf("===============================================================\n");
	printf("航班号   起点站   终点站   班期      机型   起飞时间  票价   余票\n");
	while (1)
	{
		rec(sockfd, buf);
		if (strncmp(buf, "next", 4) == 0)
		{
			break;
		}
		else if (strncmp(buf, "error", 5) == 0)
		{
			printf("没有此航班！！！\n");
			break;
		}
		printf("%s", buf);
	}

	return 0;
}

//实名认证
int real_name(int sockfd)
{
	char buf[SIZEOF];

	printf("=======正在实名登记========\n");
	printf("请输入您的身份证号：\n");
	sen(sockfd, buf);

	rec(sockfd, buf);
	if (strncmp(buf, "error", 5) == 0)
	{
		printf("身份证号输入有误（6位）！！！\n" );
		return 0;
	}

	printf("请输入您的真实姓名：\n");
	sen(sockfd, buf);

	rec(sockfd, buf);
	if (strncmp(buf, "error", 5) == 0)
	{
		printf("真实姓名输入有误！！！\n");
		return 0;
	}

	printf("请输入您的身份（普通人输入0、军人输入1、医护人员输入2、残疾人输入3）：\n");
	sen(sockfd, buf);
	rec(sockfd, buf);
	if (strncmp(buf, "error", 5) == 0)
	{
		printf("身份输入有误（数字）！！！\n");
		return 0;
	}

	printf("========实名认证成功！！！=======\n");

	return 0;
}



//普通用户购买保险
int buy_insurance(int sockfd)//票库指针
{
	char buf[SIZEOF];
	printf("==================================================\n");
	printf("请输入你要购买的保险的种类：（0-不购买 1-延误险 2-新冠病毒险）\n");
	sen(sockfd, buf);

	rec(sockfd, buf);
	if (strncmp("next", buf, 4) == 0) //没买保险
	{
		rec(sockfd, buf);
		if (strncmp("next", buf, 5) == 0)//普通人身份
		{
			rec(sockfd, buf);
			if (strncmp("next", buf, 5) == 0)//购票
			{
				rec(sockfd, buf);
				if (strncmp("next", buf, 5) == 0)//成功购票
				{

					printf("购票成功！！！\n");
					return 0;
				}
				else
				{
					printf("%s\n", buf);
				}

			} else//余额不足
			{
				printf("%s\n", buf );
			}

		}
		else if (strncmp("error", buf, 5) == 0)//特殊身份
		{
			rec(sockfd, buf);
			if (strncmp("next", buf, 5) == 0)//购票
			{
				rec(sockfd, buf);
				if (strncmp("next", buf, 5) == 0)//成功购票
				{
					printf("成功购票！！！\n");
				}
				else//票卖完了
				{
					printf("%s\n", buf);
					return 0;
				}

			}
			else
			{
				printf("%s\n", buf);
				return 0;
			}

		}

	}
	else if (strncmp("error", buf, 5) == 0)//购买了保险的
	{
		rec(sockfd, buf);
		if (strncmp("next", buf, 5) == 0)//普通人身份
		{
			rec(sockfd, buf);
			if (strncmp("next", buf, 5) == 0)//购票
			{
				rec(sockfd, buf);
				if (strncmp("next", buf, 5) == 0)//成功购票
				{

					printf("购票成功！！！\n");
					return 0;
				}
				else if (strncmp("error", buf, 5) == 0) //余额不足
				{
					printf("此航班票已卖完！！！\n");
				}

			} else//余额不足
			{
				printf("%s\n", buf );
			}

		}
		else if (strncmp("error", buf, 5) == 0)//特殊身份
		{
			rec(sockfd, buf);
			if (strncmp("next", buf, 5) == 0)//购票
			{
				rec(sockfd, buf);
				if (strncmp("next", buf, 5) == 0)//成功购票
				{
					printf("成功购票！！！\n");
				}
				else//票卖完了
				{
					printf("%s\n", buf);
					return 0;
				}

			}
			else
			{
				printf("%s\n", buf);
				return 0;
			}

		}
	}

	return 0;
}


//普通用户购票
int usr_buy_ticket(int sockfd)
{
	system("clear");
	char buf[SIZEOF];

	printf("请输入您要购买的航班号\n");
	sen(sockfd, buf);

	rec(sockfd, buf);
	if (strncmp("error", buf, 5) == 0)
	{
		printf("您已购买了此航班的票！！！\n");
		return -1;
	}

	rec(sockfd, buf);
	if (strncmp("error", buf, 5) == 0)
	{
		printf("没有此航班！！！\n");
		return -1;
	}

	rec(sockfd, buf);
	printf("%s\n", buf);
	printf("请确认是否购买！（0-确认 1-取消）\n");
	sen(sockfd, buf);

	rec(sockfd, buf);
	if (strncmp("error", buf, 5) == 0)
	{
		printf("没有此航班！！！\n");
		return -1;
	}
	else if (strncmp("return", buf, 6) == 0)
	{
		return 0;
	}
	else if (strncmp("1error", buf, 6) == 0)
	{
		printf("输入有误！！！\n");
		return 0;
	}

	buy_insurance(sockfd);


	return 0;
}



//vip购买保险
int vip_buy_insurance(int sockfd)//票库指针
{
	char buf[SIZEOF];
	printf("==================================================\n");
	printf("请输入你要购买的保险的种类：（0-不购买 1-延误险 2-新冠病毒险）\n");
	sen(sockfd, buf);

	rec(sockfd, buf);
	if (strncmp("next", buf, 4) == 0) //没买保险
	{
		rec(sockfd, buf);
		if (strncmp("next", buf, 5) == 0)//普通人身份
		{
			rec(sockfd, buf);
			if (strncmp("next", buf, 5) == 0)//购票
			{
				rec(sockfd, buf);
				if (strncmp("next", buf, 5) == 0)//成功购票
				{

					printf("购票成功！！！\n");
					return 0;
				}
				else
				{
					printf("%s\n", buf);
				}

			} else//余额不足
			{
				printf("%s\n", buf );
			}

		}
		else if (strncmp("error", buf, 5) == 0)//特殊身份
		{
			rec(sockfd, buf);
			if (strncmp("next", buf, 5) == 0)//购票
			{
				rec(sockfd, buf);
				if (strncmp("next", buf, 5) == 0)//成功购票
				{
					printf("成功购票！！！\n");
				}
				else//票卖完了
				{
					printf("%s\n", buf);
					return 0;
				}

			}
			else
			{
				printf("%s\n", buf);
				return 0;
			}

		}

	}
	else if (strncmp("error", buf, 5) == 0)//购买了保险的
	{
		rec(sockfd, buf);
		if (strncmp("next", buf, 5) == 0)//普通人身份
		{
			rec(sockfd, buf);
			if (strncmp("next", buf, 5) == 0)//购票
			{
				rec(sockfd, buf);
				if (strncmp("next", buf, 5) == 0)//成功购票
				{

					printf("购票成功！！！\n");
					return 0;
				}
				else if (strncmp("error", buf, 5) == 0) //余额不足
				{
					printf("此航班票已卖完！！！\n");
				}

			} else//余额不足
			{
				printf("%s\n", buf );
			}

		}
		else if (strncmp("error", buf, 5) == 0)//特殊身份
		{
			rec(sockfd, buf);
			if (strncmp("next", buf, 5) == 0)//购票
			{
				rec(sockfd, buf);
				if (strncmp("next", buf, 5) == 0)//成功购票
				{
					printf("成功购票！！！\n");
				}
				else//票卖完了
				{
					printf("%s\n", buf);
					return 0;
				}

			}
			else
			{
				printf("%s\n", buf);
				return 0;
			}

		}
	}

	return 0;
}





//vip购票
int vip_buy_ticket(int sockfd)
{
	system("clear");
	char buf[SIZEOF];

	rec(sockfd, buf);
	if (strncmp("error", buf, 5) == 0)
	{
		printf("您好，您还不是VIP用户，请选择普通用户进行购票！\n");
		return -1;
	}

	printf("尊敬的VIP用户，您购票享受8折优惠\n");

	printf("请输入您要购买的航班号\n");
	sen(sockfd, buf);

	rec(sockfd, buf);
	if (strncmp("error", buf, 5) == 0)
	{
		printf("您已购买了此航班的票！！！\n");
		return -1;
	}

	rec(sockfd, buf);
	if (strncmp("error", buf, 5) == 0)
	{
		printf("没有此航班！！！\n");
		return -1;
	}

	rec(sockfd, buf);
	printf("%s\n", buf);
	printf("请确认是否购买！（0-确认 1-取消）\n");
	sen(sockfd, buf);

	rec(sockfd, buf);
	if (strncmp("error", buf, 5) == 0)
	{
		printf("没有此航班！！！\n");
		return -1;
	}
	else if (strncmp("return", buf, 6) == 0)
	{
		return 0;
	}
	else if (strncmp("1error", buf, 6) == 0)
	{
		printf("输入有误！！！\n");
		return 0;
	}

	vip_buy_insurance(sockfd);


	return 0;
}



//购票入口
int buy_ticket(int fd, int sockfd)

{
	int x = 0, y = 0;
	struct input_event buf;
	mmap_show_bmp("bmp_dir/buy_ticket.bmp");
	char buf1[SIZEOF];


	rec(sockfd, buf1);
	if (strncmp(buf1, "error", 5) == 0)
	{
		real_name(sockfd); //实名认证
	}
	else if (strncmp(buf1, "next", 4) == 0)
	{
		while (1)
		{
			mmap_show_bmp("bmp_dir/buy_ticket2.bmp");
			read(fd, &buf, sizeof(buf));

			if (buf.type == 3 && buf.code == 0)
			{
				x = buf.value;
			}
			if (buf.type == 3 && buf.code == 1)
			{
				y = buf.value;
			}


			if (y < 200)
			{
				if (buf.type == 1 && buf.code == 330 && buf.value == 0)
				{
					sele(sockfd, "1");
					vip_buy_ticket(sockfd);
				}
			}
			if (y > 200 && y < 400)
			{
				if (buf.type == 1 && buf.code == 330 && buf.value == 0)
				{
					sele(sockfd, "2");
					usr_buy_ticket(sockfd);
				}
			}
			if (y > 400)
			{
				if (buf.type == 1 && buf.code == 330 && buf.value == 0)
				{
					sele(sockfd, "3");
					return 0;
				}
			}
		}


		/*while (1)
		{
			printf("======================GEC SYSTEM===================\n");
			printf("                                                   \n");
			printf("                      1 VIP购票                    \n");
			printf("                      2 普通用户购票                 \n");
			printf("                      3 返回                       \n");
			printf("                                                   \n");
			printf("===================================================\n");
			printf("请在客户端输入您的选项：\n");
			sen(sockfd, buf);
			if (strlen(buf) > 2)
			{
				printf("输入有误，请重新输入！！！\n");
				continue;
			}

			if (strncmp(buf, "1", 1) == 0)
			{
				vip_buy_ticket(sockfd);
				continue;

			}
			if (strncmp(buf, "2", 1) == 0)
			{
				usr_buy_ticket(sockfd);
				continue;

			}
			if (strncmp(buf, "3", 1) == 0)
			{
				return 0;

			}
		}*/
	}
	return 0;

}


//充值
int top_up(int fd, int sockfd)
{
	char buf[SIZEOF];
	printf("=========================================================\n");
	printf("=========              充值活动！！！                ======\n");
	printf("=========             充值100送20                   ======\n");
	printf("=========            充值500送200                   ======\n");
	printf("=========         一次充值5000成为终身VIP            ======\n");
	printf("==========================================================\n");

	printf("请输入你要充值的金额：");
	sen(sockfd, buf);

	rec(sockfd, buf);
	if (strncmp("error", buf, 5) == 0)
	{
		printf("充值错误！！！\n");
		return -1;
	}

	rec(sockfd, buf);
	printf("%s\n", buf);

	rec(sockfd, buf);
	if (strncmp("error", buf, 5) == 0)
	{
		printf("恭喜您成为了终身VIP!!!\n");
		return -2;
	}

	return 0;
}



//订单查询
int check_order(int fd, int sockfd)
{
	system("clear");
	printf("===============================================================\n");
	printf("航班号   起点站   终点站   班期      机型   起飞时间  价格   保险种类（0-无 1-延误险 2-新冠险）\n");
	char buf[SIZEOF];
	while (1)
	{
		rec(sockfd , buf);
		if (strncmp("error", buf, 5) == 0)
		{
			break;
		}
		printf("%s\n", buf );
	}

	return 0;
}


//修改密码
int change_passwd(int sockfd)
{
	system("clear");
	char buf[SIZEOF];

	printf("请输入你的旧密码：\n");
	sen(sockfd, buf);

	rec(sockfd, buf);
	if (strncmp("error", buf, 5) == 0)
	{
		printf("密码错误！！！\n");
		return -1;
	}

	printf("请输入您的新密码：\n");
	sen(sockfd, buf);

	printf("二次输入：\n");
	sen(sockfd, buf);

	rec(sockfd, buf);
	if (strncmp("error", buf, 5) == 0)
	{
		printf("两次密码不一致！！！\n");
	}

	printf("=========修改密码成功========\n");
	return 0;


}


//修改年龄
int change_age(int sockfd)
{
	char buf[SIZEOF];
	printf("请输入新的年龄：\n");
	sen(sockfd, buf);

	rec(sockfd, buf);
	if (strncmp("error", buf, 5) == 0)
	{
		printf("年龄输入错误！！！\n");
		return -1;
	}

	printf("年龄修改成功！！！\n");
	return 0;

}


int	change_user_data(int fd, int sockfd)
{

	int x = 0, y = 0;
	struct input_event buf;

	while (1)
	{
		mmap_show_bmp("bmp_dir/chang_fun.bmp");
		read(fd, &buf, sizeof(buf));


		if (buf.type == 3 && buf.code == 1)
		{
			y = buf.value;
		}

		if ( y < 200)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "1");
				change_passwd(sockfd);//修改密码
			}
		}
		if ( y < 400 && y > 200)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "2");
				change_age(sockfd);//修改年龄
			}
		}
		if (  y > 400)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{	sele(sockfd, "3");
				return 0;
			}
		}
	}

	/*char buf[SIZEOF];
	while (1)
	{
		printf("======================GEC SYSTEM===================\n");
		printf("                                                   \n");
		printf("                      1 修改密码                       \n");
		printf("                      2 修改年龄                       \n");
		printf("                      3 返回                        \n");
		printf("                                                   \n");
		printf("===================================================\n");
		printf("请输入您的选项：\n");
		sen(sockfd, buf);
		if (strlen(buf) > 2)
		{
			printf("输入有误，请重新输入！！！\n");
			continue;
		}

		if (strncmp(buf, "1", 1) == 0)
		{

			change_passwd(sockfd);//修改密码

		}
		if (strncmp(buf, "2", 1) == 0)
		{

			change_age(sockfd);//修改年龄

		}
		if (strncmp(buf, "3", 1) == 0)
		{

			return 0;

		}
	}*/
}



//退票
int return_ticket(int sockfd) //退票
{
	system("clear");
	char buf[SIZEOF];
	printf("下面是您的订单：\n");
	printf("===============================================================\n");
	printf("航班号   起点站   终点站   班期      机型   起飞时间  价格   保险种类（0-无 1-延误险 2-新冠险）\n");

	while (1)
	{
		rec(sockfd, buf);
		if (strncmp("error", buf, 5) == 0)
		{
			break;
		}
		printf("%s", buf );
	}
	printf("请输入您要退的航班号：\n");
	sen(sockfd, buf);

	rec(sockfd, buf);
	if (strncmp("next", buf, 4) == 0)//买了保险的
	{
		rec(sockfd, buf);
		if (strncmp("next", buf, 4) == 0)//找到票
		{
			rec(sockfd, buf);
			if (strncmp("next", buf, 4) == 0)//特殊身份
			{
				printf("退票成功！！！\n");
				return 0;
			}
			else if (strncmp("error", buf, 5) == 0)//普通人
			{
				printf("退票成功！！！\n");
				return 0;
			}
		}
		else if (strncmp("error", buf, 5) == 0)//没有此票
		{
			printf("您没有购买此航班！！！!\n");
			return -1;
		}
	}
	else if (strncmp("error", buf, 5) == 0)//没买保险的
	{
		rec(sockfd, buf);
		if (strncmp("next", buf, 4) == 0)//找到票
		{
			rec(sockfd, buf);
			if (strncmp("next", buf, 4) == 0)//特殊身份
			{
				printf("退票成功！！！\n");
				return 0;
			}
			else if (strncmp("error", buf, 5) == 0)//普通人
			{
				printf("退票成功！！！\n");
				return 0;
			}
		}
		else if (strncmp("error", buf, 5) == 0)//没有此票
		{
			printf("您没有购买此航班！！！!\n");
			return -1;
		}
	}

	return 0;
}



//重新对应购买保险
int change_ticket_insurance(int sockfd)
{
	char buf[SIZEOF];

	rec(sockfd, buf);
	if (strncmp("next", buf, 4) == 0)//没买保险
	{
		rec(sockfd, buf);
		if (strncmp("next", buf, 4) == 0)//普通人
		{
			rec(sockfd, buf);
			if (strncmp("next", buf, 4) == 0)//余额不足
			{
				printf("余额不足以改签，请充值后重新购票！！！\n");
				return 0;
			}
			else if (strncmp("error", buf, 5) == 0)//余额足够
			{
				rec(sockfd, buf);
				if (strncmp("next", buf, 4) == 0)//票没卖完
				{
					printf("改签成功！！！\n");
					return 0;
				}
				else if (strncmp("error", buf, 5) == 0)//票卖完了
				{
					printf("此航班票已卖完无法改签,票额已经退回,请重新购票！！！\n");
					return 0;
				}
			}
		}
		else if (strncmp("error", buf, 5) == 0)//特殊身份
		{
			rec(sockfd, buf);
			if (strncmp("next", buf, 4) == 0)//余额不足
			{
				printf("余额不足以改签，请充值后重新购票！！！\n");
				return 0;
			}
			else if (strncmp("error", buf, 5) == 0)//余额足够
			{
				rec(sockfd, buf);
				if (strncmp("next", buf, 4) == 0)//票没卖完
				{
					printf("改签成功！！！\n");
					return 0;
				}
				else if (strncmp("error", buf, 5) == 0)//票卖完了
				{
					printf("此航班票已卖完无法改签,票额已经退回,请重新购票！！！\n");
					return 0;
				}
			}
		}
	}
	else if (strncmp("error", buf, 5) == 0)//买了保险
	{
		rec(sockfd, buf);
		if (strncmp("next", buf, 4) == 0)//普通人
		{
			rec(sockfd, buf);
			if (strncmp("next", buf, 4) == 0)//余额不足
			{
				printf("余额不足以改签，请充值后重新购票！！！\n");
				return 0;
			}
			else if (strncmp("error", buf, 5) == 0)//余额足够
			{
				rec(sockfd, buf);
				if (strncmp("next", buf, 4) == 0)//票没卖完
				{
					printf("改签成功！！！\n");
					return 0;
				}
				else if (strncmp("error", buf, 5) == 0)//票卖完了
				{
					printf("此航班票已卖完无法改签,票额已经退回,请重新购票！！！\n");
					return 0;
				}
			}
		}
		else if (strncmp("error", buf, 5) == 0)//特殊身份
		{
			rec(sockfd, buf);
			if (strncmp("next", buf, 4) == 0)//余额不足
			{
				printf("余额不足以改签，请充值后重新购票！！！\n");
				return 0;
			}
			else if (strncmp("error", buf, 5) == 0)//余额足够
			{
				rec(sockfd, buf);
				if (strncmp("next", buf, 4) == 0)//票没卖完
				{
					printf("改签成功！！！\n");
					return 0;
				}
				else if (strncmp("error", buf, 5) == 0)//票卖完了
				{
					printf("此航班票已卖完无法改签,票额已经退回,请重新购票！！！\n");
					return 0;
				}
			}
		}
	}

	return 0;
}



//购买改签后的票
int change_ticket_fun(int sockfd)
{

	char buf[SIZEOF];

	rec(sockfd, buf);
	if (strncmp("error", buf, 5) == 0)
	{
		printf("您已购买了此航班的票,无法改签到此航班！！！\n");
		return -1;
	}

	rec(sockfd, buf);
	if (strncmp("error", buf, 5) == 0)
	{
		printf("无法改签到同一航班，已经把票款退回，请重新购买！！！\n");
		return -1;
	}

	rec(sockfd, buf);
	if (strncmp("error", buf, 5) == 0)
	{
		printf("没有此航班！！！\n");
		return -1;
	}
	printf("===============================================================\n");
	printf("航班号   起点站   终点站   班期      机型   起飞时间  价格   余票\n");
	printf("%s\n", buf );
	change_ticket_insurance(sockfd); //判断是否购买保险，减去对应余额

	return 0;
}




// 改签
int change_ticket(int sockfd)
{
	system("clear");
	char buf[SIZEOF];

	printf("下面是您的订单：\n");
	printf("===============================================================\n");
	printf("航班号   起点站   终点站   班期      机型   起飞时间  价格   保险种类（0-无 1-延误险 2-新冠险）\n");

	while (1)
	{
		rec(sockfd, buf);
		if (strncmp("error", buf, 5) == 0)
		{
			break;
		}
		printf("%s", buf );
	}


	printf("====请注意改签后的票将不享受VIP优惠！！！====\n");
	printf("请输入您要改签的航班号：\n");
	sen(sockfd, buf);

	rec(sockfd, buf);
	if (strncmp("error", buf, 5) == 0)
	{
		printf("您没有购买此航班！！！\n");
		return 0;
	}

	printf("请输入您要改签到的航班号：\n");
	sen(sockfd, buf);

	change_ticket_fun(sockfd); //购买改签后的票

	return 0;
}




//退单与改签
int refund_and_change(int fd, int sockfd)
{

	int x = 0, y = 0;

	struct input_event buf;

	while (1)
	{
		mmap_show_bmp("bmp_dir/refund.bmp");
		read(fd, &buf, sizeof(buf));


		if (buf.type == 3 && buf.code == 1)
		{
			y = buf.value;
		}

		if ( y < 200)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "1");
				return_ticket( sockfd);//退票

			}
		}
		if ( y > 200 && y < 400)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "2");
				change_ticket( sockfd);//改签
			}
		}
		if ( y > 400)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "3");
				return 0;
			}
		}


	}

	/*char buf[SIZEOF];
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

		sen(sockfd, buf);
		if (strlen(buf) > 2)
		{
			printf("输入有误，请重新输入！！！\n");
			continue;
		}

		if (strncmp(buf, "1", 1) == 0)
		{
			return_ticket(sockfd);//退票

		}
		if (strncmp(buf, "2", 1) == 0)
		{
			change_ticket(sockfd);//改签
		}
		if (strncmp(buf, "3", 1) == 0)
		{
			return 0;
		}

	}*/
	return 0;
}

/*void *remind(void *arg)
{
	char buf[SIZEOF];
	int sockfd = *(int *)arg;
	while (1)
	{
		rec(sockfd, buf);
		if (strncmp(buf, "账户：", 3) == 0)
		{
			printf("%s\n", buf);
		}
		else if (strncmp(buf, "exit", 4) == 0)
		{
			break;
		}
	}
	pthread_exit(NULL);
}
*/
//普通用户登陆成功界面
int login_success(int fd, int sockfd)
{
	system("clear");
	int x = 0, y = 0;
	struct input_event buf;
	char buf1[SIZEOF];
	/*pthread_t t2;
	pthread_create(&t2, NULL, remind, (void *)&sockfd);*/

	while (1)
	{
		mmap_show_bmp("bmp_dir/login_success.bmp");

		read(fd, &buf, sizeof(buf));

		if (buf.type == 3 && buf.code == 0)
		{
			x = buf.value;
		}
		if (buf.type == 3 && buf.code == 1)
		{
			y = buf.value;
		}

		if (x < 340 && y < 200)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "1");
				check_flight( fd, sockfd);
			}

		}


		//快速查询
		if (x > 340 && x < 680 && y < 200)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "2");
				check_fast( fd, sockfd);
			}

		}


		//购票
		if (x > 680 &&  y < 200)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "3");
				buy_ticket(fd, sockfd);
			}
		}


		//订单查询
		if (x < 340 && y > 200 && y < 400)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "4");
				check_order(fd, sockfd);
			}
		}



		//退单与改签
		if (x < 680 && x > 340 && y > 200 && y < 400)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "5");
				refund_and_change(fd, sockfd);
			}
		}



		//余额查询
		if (x > 680 && y > 200 && y < 400)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "6");
				int i = 0;
				rec(sockfd, buf1);
				i = atoi(buf1);
				printf("====================您的余额为：%d======================\n", i);
			}
		}



		//余额充值
		if (x < 340 && y > 400 )
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "7");
				top_up(fd, sockfd);
			}

		}



		//修改个人信息
		if (x < 680 && x > 340 && y > 400 )
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "8");
				change_user_data(fd, sockfd);
			}

		}


		//返回
		if (x > 680 && y > 400)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "9");
				/*pthread_join(t2, NULL);*/
				return 0;

			}

		}
	}

	/*char buf[SIZEOF];
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
		printf("请输入您的选项：\n");
		sen(sockfd, buf);
		if (strlen(buf) > 2)
		{
			printf("输入有误，请重新输入！！！\n");
			continue;
		}
		if (strncmp(buf, "1", 1) == 0)
		{
			check_flight(sockfd);
			continue;
		}


		//快速查询
		if (strncmp(buf, "2", 1) == 0)
		{
			check_fast(sockfd);
			continue;
		}


		//购票
		if (strncmp(buf, "3", 1) == 0)
		{
			buy_ticket(sockfd);
			continue;
		}


		//订单查询
		if (strncmp(buf, "4", 1) == 0)
		{
			check_order(sockfd);
		}



		//退单与改签
		if (strncmp(buf, "5", 1) == 0)
		{
			refund_and_change( sockfd);
		}



		//余额查询
		if (strncmp(buf, "6", 1) == 0)
		{	int i = 0;
			rec(sockfd, buf);
			i = atoi(buf);
			printf("====================您的余额为：%d======================\n", i);

		}



		//余额充值
		if (strncmp(buf, "7", 1) == 0)
		{
			top_up(sockfd);
		}



		//修改个人信息
		if (strncmp(buf, "8", 1) == 0)
		{
			change_user_data(sockfd);
		}


		//返回
		if (strncmp(buf, "9", 1) == 0)
		{
			return 0;
		}
	}*/

}


//用户登陆
int user_login_fun(int fd, int sockfd)
{
	mmap_show_bmp("./bmp_dir/login.bmp");
	char buf[SIZEOF];

	printf("请输入您的用户名：\n");
	sen(sockfd, buf);

	bzero(buf, sizeof(buf));
	rec(sockfd, buf);

	if (strncmp("error", buf, 5) == 0)
	{
		printf("该账号不存在!!!\n");
		return 0;
	}

	printf("请输入您的密码：\n");
	sen(sockfd, buf);

	bzero(buf, sizeof(buf));
	rec(sockfd, buf);
	if (strncmp("error", buf, 5) == 0)
	{
		printf("密码错误！！！\n");
		return 0;
	}

	bzero(buf, sizeof(buf));
	rec(sockfd, buf);
	printf("126:%s\n", buf);
	if (strncmp("error", buf, 5) == 0)
	{
		printf("您的账号已登陆，不能重复登陆！！！\n");
		return 0;
	}
	printf("登陆成功！！！\n");
	login_success(fd, sockfd);
	//把套接字存入用户信息节点
	return 0;
}


//录入航班
int entry_flight(int sockfd)
{
	char buf[SIZEOF];



	printf("请输入你要录入的航班号（例如-A001-A代表机型0001代表编号）：\n");
	sen(sockfd, buf);

	rec(sockfd, buf);
	if (strncmp("error", buf, 4) == 0)
	{
		printf("该航班号已被使用请重新输入！！！\n");
		return 0;
	}


	printf("请输入起点站:\n");
	sen(sockfd, buf);


	printf("请输入终点站:\n");
	sen(sockfd, buf);



	printf("请输入班期:\n");
	sen(sockfd, buf);


	printf("请输入机型:\n");
	sen(sockfd, buf);


	printf("请输入起飞时间:\n");
	sen(sockfd, buf);



	printf("请输入票价：\n");
	sen(sockfd, buf);


	printf("请输入总票数：\n");
	sen(sockfd, buf);

	printf("录入航班成功！！！\n");

	return 0;
}




//删除航班后退票
int delete_ticket_fun(int sockfd)
{
	char buf[SIZEOF];
	rec(sockfd, buf);
	if (strncmp("error", buf, 5) == 0)
	{
		printf("没有用户购买此航班，删除航班成功！！！\n");
		return 0;
	}
	printf("删除航班成功！！！\n");
	return 0;
}



int find_buy_ticket(int sockfd)
{
	char buf[SIZEOF];
	rec(sockfd, buf);
	if (strncmp("error", buf, 5) == 0)
	{
		printf("删除航班成功！！！\n");
		return 0;
	}
	delete_ticket_fun(sockfd);

	return 0;
}


//删除航班
int  delete_flight(int sockfd)
{
	system("clear");
	char buf[SIZEOF];


	while (1)
	{
		rec(sockfd, buf);
		if (strncmp("error", buf, 5) == 0)
		{
			break;
		}
		printf("%s", buf);
	}


	printf("请输入你要删除的航班:\n");
	sen(sockfd, buf);

	rec(sockfd, buf);
	if (strncmp("error", buf, 5) == 0)
	{
		printf("没有找到此航班！！！\n");
		return 0;
	}

	printf("请确认是否要删除此航班（0-确认 1-取消）:\n");
label2:
	sen(sockfd, buf);

	rec(sockfd, buf);
	if (strncmp("return", buf, 6) == 0)
	{
		return 0;
	}
	else if (strncmp("error", buf, 5) == 0)
	{
		printf("输入错误,请重新输入!!!\n");
		goto label2;
	}

	find_buy_ticket(sockfd);

	return 0;
}




//打印回收站内容
int recycle_fun(int sockfd)
{
	system("clear");

	char buf[SIZEOF];
	rec(sockfd, buf);
	if (strncmp(buf, "error", 5) == 0)
	{
		printf("回收站没有东西。\n");
		return 0;
	}

	printf("===============================================================\n");
	printf("航班号   起点站   终点站   班期      机型   起飞时间  票价   余票\n");
	while (1)
	{
		printf("%s", buf);
		rec(sockfd, buf);

		if (strncmp(buf, "break", 5) == 0)
		{
			break;
		}


	}

	return 0;
}



//生效保险
int effect_insurance(int sockfd)
{
	char buf[SIZEOF];
	printf("请输入你要生效的航班：\n" );
	sen(sockfd, buf);

	printf("请输入你要生效的的险种（1-延误险-赔购买此票总费用的双倍，2-新冠险-赔购买此票总费用的10倍）：\n");
	sen(sockfd, buf);
	printf("保险生效成功！！！\n");
	return 0;
}


//管理员界面
int admin_login_fun(int fd, int sockfd)
{
	system("clear");
	int x = 0, y = 0;
	struct input_event buf;
	while (1)
	{
		mmap_show_bmp("./bmp_dir/admin.bmp");
		read(fd, &buf, sizeof(buf));
		if (buf.type == 3 && buf.code == 0)
		{
			x = buf.value;
		}
		if (buf.type == 3 && buf.code == 1)
		{
			y = buf.value;
		}

		//查看所有航班
		if (x < 500 && y < 200)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "1");
				check_all_flight(sockfd);
			}

		}


		//录入航班
		if (x > 500 && y < 200)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "2");
				entry_flight(sockfd);
			}

		}


		//删除航班
		if (x < 500 && y > 200 && y < 400)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "3");
				delete_flight( sockfd);
			}

		}


		//航班回收站
		if (x > 500 && y > 200 && y < 400)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "4");
				recycle_fun( sockfd);//打印回收站内容

			}

		}



		//生效保险
		if (x < 500 && y > 400)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "5");
				effect_insurance(sockfd);

			}

		}



		//退出登陆
		if (x > 500 && y > 400)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "6");
				printf("管理员退出登陆\n");
				return 0;

			}

		}

	}

	/*char buf[SIZEOF];
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
		sen(sockfd, buf);

		if (strlen(buf) > 2)
		{
			printf("输入有误，请重新输入！！！\n");
			continue;
		}

		//查看所有航班
		if (strncmp(buf, "1", 1) == 0)
		{
			check_all_flight(sockfd);
		}


		//录入航班
		if (strncmp(buf, "2", 1) == 0)
		{
			entry_flight(sockfd);
		}


		//删除航班
		if (strncmp(buf, "3", 1) == 0)
		{
			delete_flight(sockfd);
		}


		//航班回收站
		if (strncmp(buf, "4", 1) == 0)
		{
			recycle_fun(sockfd);//打印回收站内容
		}



		//生效保险
		if (strncmp(buf, "5", 1) == 0)
		{
			effect_insurance(sockfd);
		}



		//退出登陆
		if (strncmp(buf, "6", 1) == 0)
		{
			printf("管理员退出登陆\n");
			return 0;
		}

	}*/
}


//判断管理员账号密码
int admin_fun(int fd, int sockfd)
{
	mmap_show_bmp("./bmp_dir/login.bmp");
	system("clear");
	char buf[SIZEOF];
	printf("请输入您的管理员账号：\n");
	sen(sockfd, buf);

	rec(sockfd, buf);
	if (strcmp("error", buf) == 0)
	{
		printf("账号输入有误！！！\n");
		return 0;
	}

	printf("请输入您的密码：\n");
	sen(sockfd, buf);

	rec(sockfd, buf);
	if (strcmp("error", buf) == 0)
	{
		printf("密码输入错误！！！\n");
		return 0;
	}

	admin_login_fun(fd, sockfd);

	return 0;
}


//选择登陆界面
int select_login_fun(int fd, int sockfd)
{
	//char buf[SIZEOF];
	system("clear");
	int x, y;


	struct input_event buf;
	while (1)
	{
		mmap_show_bmp("./bmp_dir/select_login.bmp");

		read(fd, &buf, sizeof(buf));
		if (buf.type == 3 && buf.code == 1)
		{
			y = buf.value;
		}

		//普通用户登陆
		if (y < 200)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "1");
				user_login_fun(fd, sockfd);
			}

		}


		//管理员登陆
		if (y > 200 && y < 400)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "2");
				admin_fun(fd, sockfd); //管理员登陆
			}

		}
		if (y > 400)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "3");
				return 0;

			}

		}

	}

	/*while (1)
	{
		printf("======================GEC SYSTEM===================\n");
		printf("                                                     \n");
		printf("                       1 普通用户登陆                 \n");
		printf("                       2 管理员登陆                   \n");
		printf("                       3 返回                         \n");
		printf("                                                     \n");
		printf("===================================================\n");
		printf("请输入您的选项：\n");
		bzero(buf, sizeof(buf));
		fgets(buf, sizeof(buf), stdin);
		send(sockfd, buf, strlen(buf), 0);
		if (strlen(buf) > 2)
		{
			printf("输入有误，请重新输入！！！\n");
			continue;
		}
		//普通用户登陆
		if (strncmp(buf, "1", 1) == 0)
		{
			user_login_fun(sockfd);
			continue;
		}

		//管理员登陆
		if (strncmp(buf, "2", 1) == 0)
		{
			admin_fun(sockfd); //管理员登陆
		}
		if (strncmp(buf, "3", 1) == 0)
		{

			printf("%s, 二级返回\n", buf);
			return 0;
		}

	}*/
	return -1;
}



//注册界面
int register_fun(int sockfd)
{
	system("clear");


	char buf[SIZEOF];
	printf("请输入您的用户名：\n");
	sen(sockfd, buf);

	printf("请输入您的密码:\n");
	sen(sockfd, buf);

	printf("请输入您的年龄:\n");
	sen(sockfd, buf);

	printf("请输入您的电话:\n");
	sen(sockfd, buf);
	rec(sockfd, buf);

	if (strncmp(buf, "error", 5) == 0)
	{
		printf("该电话已经注册！！！\n");
		return 0;
	}

	printf("请输入您的密保问题:\n");
	sen(sockfd, buf);

	printf("请输入您的密保答案:\n");
	sen(sockfd, buf);

	printf("请输入您的身份（普通人输入0、军人输入1、医护人员输入2、残疾人输入3）:\n");
	sen(sockfd, buf);

	rec(sockfd, buf);

	if (strcmp("error", buf) == 0)
	{
		printf("输入格式错误!!!\n");
		return 0;
	}

	rec(sockfd, buf);

	if (strncmp(buf, "error", 5) == 0)
	{
		printf("手慢了哦！！！\n");
		return 0;
	}

	printf("注册成功！！\n");
}


//接收函数
int rec(int sockfd, char *buf)
{
	bzero(buf, SIZEOF);
	return recv(sockfd, buf, SIZEOF, 0);
	//buf[strlen(buf) - 1] = '\0';
}

void sen(int sockfd, char *buf)
{
	bzero(buf, SIZEOF);
	fgets(buf, SIZEOF, stdin);
	send(sockfd, buf, SIZEOF, 0);
}

//找回密码
int find_passwd(int sockfd)
{
	system("clear");
	char buf[SIZEOF];

	printf("请输入您要修改密码的账户：\n");
	sen(sockfd, buf);

	rec(sockfd, buf);
	if (strcmp("error", buf) == 0)
	{
		printf("没有此账户!!!\n");
		return 0;
	}

	rec(sockfd, buf);
	printf("密保问题：%s\n", buf);

	printf("请输入你的回答：\n");
	sen(sockfd, buf);

	rec(sockfd, buf);
	if (strcmp("error", buf) == 0)
	{
		printf("密保错误！！！\n");
		return 0;
	}

	printf("请输入您的新密码：\n");
	sen(sockfd, buf);

	printf("二次输入：\n");
	sen(sockfd, buf);

	rec(sockfd, buf);
	if (strcmp("error", buf) == 0)
	{
		printf("两次密码不一致！！！\n");
		return 0;
	}

	printf("===修改密码成功！！！===\n");

	return 0;

}

//退出系统界面
void show_black_color()
{
	int lcd;

	int *p = NULL;
	lcd = open("/dev/fb0", O_RDWR);

	if (lcd < 0)
	{
		printf("open lcd error!\n");
	}



	p = (int*)mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd, 0);
	if (p == (void *) - 1)
	{
		printf("mmap error\n");
	}

	int black_color = 0x00000000;


	mmap_show_bmp("./bmp_dir/exit.bmp");

	sleep(3);
	for (int i = 0; i < 480; ++i)
	{
		for (int a = i * 800; a < i * 800 + 800; a++)
		{
			memcpy(p + a, &black_color, 4);
		}
	}


	munmap(p, 800 * 480 * 4);
	close(lcd);
}



//主函数
void *main_meum(void *arg)
{
	int sockfd = *(int *)arg;

	system("clear");
	int fd;
	fd = open("/dev/input/event0", O_RDONLY);
	if (fd < 0)
	{
		printf("open touch error!\n");
	}
	struct input_event buf;

	int x = 0, y = 0;


	while (1)
	{
		mmap_show_bmp("./bmp_dir/main_meum.bmp");

		read(fd, &buf, sizeof(buf));

		if (buf.type == 3 && buf.code == 1)
		{
			y = buf.value;
		}

		if (y < 150)
		{
			//选择登陆
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)

			{
				sele(sockfd, "1");
				select_login_fun(fd, sockfd);
				continue;

			}

		}


		if (y < 300 && y > 150)
		{
			//注册
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "2");
				register_fun(sockfd);
				continue;
			}
		}

		//找回密码
		if (y < 450 && y > 300)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "3");
				find_passwd(sockfd);
				continue;
			}
		}


		if (y > 450)
		{
			if (buf.type == 1 && buf.code == 330 && buf.value == 0)
			{
				sele(sockfd, "4");
				show_black_color();
				return 0;
			}

		}

	}
	close(fd);

	/*while (1)
	{
		printf("======================GEC SYSTEM===================\n");
		printf("                                                   \n");
		printf("                      1 选择登陆                    \n");
		printf("                      2 注册                        \n");
		printf("                      3 忘记密码                    \n");
		printf("                      4 退出                        \n");
		printf("                                                   \n");
		printf("===================================================\n");
		printf("请输入您的选项：\n");
		sen(sockfd, buf);
		if (strlen(buf) > 2)
		{
			printf("输入有误，请重新输入！！！\n");
			continue;
		}
		if (strncmp(buf, "1", 1) == 0)
		{
			//选择登陆
			select_login_fun(sockfd);
			continue;
		}
		if (strncmp(buf, "2", 1) == 0)
		{
			//注册
			register_fun(sockfd);
			continue;

		}

		//找回密码
		if (strncmp(buf, "3", 1) == 0)
		{
			find_passwd(sockfd);
			continue;

		}


		if (strncmp(buf, "4", 1) == 0)
		{

			printf("一级退出\n");
			return 0;

		}

		if (buf)
		{
			printf("输入有误，请重新输入！！！\n");
			continue;
		}
	}*/
	return 0;
}


int main(int argc, char const *argv[])
{
	show_welcome_logo();//欢迎界面

	//1. 创建TCP套接字
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("sockfd = %d\n", sockfd);


	//2. 准备服务端的IP地址。
	struct sockaddr_in srvaddr;
	socklen_t len = sizeof(srvaddr);
	bzero(&srvaddr, len);

	srvaddr.sin_family = AF_INET;  //地址族
	srvaddr.sin_port = htons(atoi(argv[2]));  //端口号
	inet_pton(AF_INET, argv[1], &srvaddr.sin_addr); //设置好Rose的IP地址
	pthread_t t1;
	//3. 发起连接
	//connect调用之前： sockfd  -> 待连接套接字
	int ret = connect(sockfd, (struct sockaddr *)&srvaddr, len);
	if (ret == 0)
	{
		printf("connect success!\n");
		pthread_create(&t1, NULL, main_meum, (void *)&sockfd);
	} else
	{
		printf("连接失败！！！\n");
		printf("请检查网络连接!!!\n");
		printf("请检查IP地址，端口号是否输入有误！！！\n");
	}

	pthread_join(t1, NULL);


	close(sockfd);


	return 0;
}