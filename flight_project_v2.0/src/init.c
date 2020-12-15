#include "my_head.h"

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

