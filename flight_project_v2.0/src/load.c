#include "my_head.h"

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
