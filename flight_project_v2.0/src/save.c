#include "my_head.h"

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
