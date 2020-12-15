#ifndef  _MY_HEAD_H_
#define  _MY_HEAD_H_

#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <syslog.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include  <sys/shm.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PERSON   0
#define SOLDIER  1
#define MEDICAL  2
#define DISABLED 3
#define SIZEOF 200


//互斥锁
extern pthread_mutex_t m;


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


//接收函数
void rec(int connfd, char *buf);
//发送next函数 让客户端可以到下一步
void next(int connfd, char *buf);
//发送error给客户端，让客户端收到错误信息
void error(int connfd, char *buf);
//线程处理
void *func(void *arg);
//分割用户信息文件
int file_data_to_array(char *file_buf, struct userinfo *user);
//分割库信息文件
int flight_file_data_to_array(char *file_buf, struct ticket_library *new);
//分割身份库文件
int flight_id_data_to_array(char *file_buf, struct id_car *id);
//分割回收站文件
int recycle_file_data_to_array(char *file_buf, struct ticket_library *new);
//读取加载票链文件
int read_user_ticket_file(struct userinfo *user);
//读取加载用户文件
int read_file(FILE *fp, struct userinfo *userinfo_head);
//读取加载票库文件
int read_flight_file(FILE *fp, struct ticket_library *ticket_library_head);
//读取加载回收站信息
int read_recycle_file(FILE *fp, struct ticket_library *recycle_head);
//读取加载身份库库文件
int read_id_file(FILE *fp, struct id_car *id_car_head);
//加载用户信息
int loading_information(struct userinfo *userinfo_head);
//开机加载用户票链
int loading_ticket_library(struct ticket_library *ticket_library_head);
//开机加载身份库信息
int loading_id_library(struct id_car *id_car_head);
//开机加载回收链信息
int loading_recycle(struct ticket_library *recycle_head);
//初始化票库链表头
struct ticket_library *init_ticket_library_head();
//初始化用户信息头节点
struct userinfo *init_userinfo_head();
//新建票链节点
struct user_ticket *new_ticket_node(struct user_ticket *ticket_data);
//初始化身份证库链表头
struct id_car *init_id_car_head();
//显示bmp图片
int mmap_show_bmp(const char *bmp_path);
//退出系统界面
void show_black_color();
//显示欢迎界面
void show_welcome_logo();
//显示主界面
int main_meum(int connfd, struct userinfo * userinfo_head,
              struct ticket_library * ticket_library_head,
              struct id_car * id_car_head,
              struct ticket_library *recycle_head
             );
//选择登陆界面
int select_login_fun(int connfd, struct userinfo * userinfo_head,
                     struct ticket_library * ticket_library_head,
                     struct id_car * id_car_head,
                     struct ticket_library *recycle_head
                    );
//注册界面
int register_fun(int connfd, struct userinfo * userinfo_head);
//查询所有航班
int check_all_flight(int connfd, struct ticket_library *ticket_library_head);
//票价查询
int search_flight_price(int connfd, struct userinfo *userinfo_head,
                        struct ticket_library *ticket_library_head);
//机型查询
int search_flight_type(int connfd, struct userinfo *userinfo_head,
                       struct ticket_library *ticket_library_head);
//出发日期查询
int search_flight_date(int connfd, struct userinfo *userinfo_head,
                       struct ticket_library *ticket_library_head);
//目的地查询
int search_flight_location(int connfd, struct userinfo *userinfo_head,
                           struct ticket_library *ticket_library_head);
//条件查询
int search_flight(int connfd, struct userinfo *userinfo_head,
                  struct ticket_library *ticket_library_head);
//查询航班界面
int check_flight(int connfd, struct userinfo * userinfo_head,
                 struct ticket_library *ticket_library_head);
//快速查询
int check_fast(int connfd, struct userinfo *userinfo_head,
               struct ticket_library *ticket_library_head);
//订单查询
int check_order(struct userinfo *user_name);
//充值
int top_up(struct userinfo *user_name);
//实名认证
int real_name(struct id_car *id_car_head, struct userinfo *user_name);
//修改密码
int change_passwd(struct userinfo *user_name);
//修改年龄
int change_age(struct userinfo *user_name);
//修改用户信息
int change_user_data( struct userinfo *user_name);
//普通用户登陆成功界面
int login_success(
  struct userinfo * user_name,
  struct userinfo * userinfo_head,
  struct ticket_library * ticket_library_head,
  struct id_car * id_car_head
);
//用户登陆
int user_login_fun(int connfd, struct userinfo * userinfo_head,
                   struct ticket_library * ticket_library_head,
                   struct id_car * id_car_head
                  );
//找回密码
int find_passwd(int connfd, struct userinfo * userinfo_head);
//打印回收站内容
int recycle_fun(int connfd, struct ticket_library *recycle_head);
//生效保险
int effect_insurance(int connfd, struct userinfo *userinfo_head);
//管理员界面
int admin_login_fun(int connfd, struct userinfo *userinfo_head,
                    struct ticket_library *ticket_library_head,
                    struct ticket_library *recycle_head);
//判断管理员账号密码
int admin_fun(int connfd, struct userinfo *userinfo_head,
              struct ticket_library *ticket_library_head,
              struct ticket_library *recycle_head);
//录入航班
int entry_flight(int connfd, struct ticket_library *ticket_library_head);
//删除航班后退票
int delete_ticket_fun(int connfd, char *flight_number,
                      struct userinfo *user_name,
                      struct ticket_library *ticket_library_head);
//寻找所有购买了此航班的人
int find_buy_ticket(int connfd, char *flight_number,
                    struct userinfo *userinfo_head,
                    struct ticket_library *ticket_library_head,
                    struct ticket_library *recycle_head);
//删除航班
int  delete_flight(int connfd, struct userinfo *userinfo_head,
                   struct ticket_library *ticket_library_head,
                   struct ticket_library *recycle_head);
//保存用户票链
void save_user_ticket_fun(struct user_ticket *new, struct userinfo *user_name);
//删除节点后保存用户票链
void save_delete_ticket_fun(struct userinfo *user_name);
//保存航班
void save_flight_fun(struct ticket_library *new);
//保存注册信息函数
void save_userinfo_fun(struct userinfo * new);
//用户票链添加购买的票
void add_user_ticket(int insurance, int price,
                     struct userinfo *user_name,
                     struct ticket_library *find_ticket);
//vip购买保险
int vip_buy_insurance(struct userinfo *user_name,
                      struct user_ticket *vip_ticket,
                      struct ticket_library *find_ticket);
//普通用户购买保险
int buy_insurance(struct userinfo *user_name,
                  struct user_ticket *vip_ticket,
                  struct ticket_library *find_ticket);
//vip购票
int vip_buy_ticket(struct userinfo *user_name,
                   struct ticket_library *ticket_library_head);
//普通用户购票
int usr_buy_ticket(struct userinfo *user_name,
                   struct ticket_library *ticket_library_head);
//购票入口
int buy_ticket(
  struct userinfo * user_name,
  struct ticket_library * ticket_library_head,
  struct id_car *id_car_head);
//退票
int return_ticket(struct userinfo *user_name,
                  struct ticket_library *ticket_library_head);
//重新对应购买保险
int change_ticket_insurance(struct user_ticket *p,
                            struct userinfo *user_name,
                            struct ticket_library *find_ticket);
//购买改签后的票
int change_ticket_fun(struct user_ticket *p,
                      char *change_num, struct userinfo *user_name,
                      struct ticket_library *ticket_library_head);
// 改签
int change_ticket(struct userinfo *user_name,
                  struct ticket_library *ticket_library_head);
//退单与改签
int refund_and_change(
  struct userinfo * user_name,
  struct ticket_library * ticket_library_head);



#endif