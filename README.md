# C_flight_ticket_v2.0
基于Linux多用户航班购票系统

-----------------------------服务端--------------------------------

1.将TCP_flight_project.zip解压到共享文件夹下

2.在ubuntu使用 "cd /mnt/hgfs/share/TCP_flight_project" 切换到项目文件夹

3.使用 "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib"命令临时设置环境变量

4.使用 "ifconfig" 命令查看IP地址

5.使用"./bin/project 5000" 命令（端口号：1024-65535都可使用）打开服务端


--------------------------ubuntu版客户端---------------------------

1.1.将TCP_flight_project.zip解压到共享文件夹下

2.在ubuntu使用 "cd /mnt/hgfs/share/TCP_flight_project" 命令切换到项目文件夹

3.使用 "./client 192.168.14.167 5000" 命令打开ubuntu版客户端(192.168.14.167为服务端IP地址)

4.待连接上服务端后即可进行各种操作


--------------------------GEC6818版客户端---------------------------

1.在开发板终端SecureCRT上，使用 cd 命令进入家目录。

2.使用"rz -y"将项目文档中的client_arm传到GEC6818开发板

3.等待传输结束。

4.输入以下命令
[root@GEC6818 ~]#./client_arm 192.168.14.167 5000  然后回车
程序执行，可点击开发板画面提示选项来进行操作。
