基于事件驱动模型写的socket网络编程库。
QQ:809205580
博客：http://blog.csdn.net/zjhsucceed_329/

如有问题，欢迎指正。

版本信息
1.0 创建
1.1 优化，可识别非法IP地址。

使用说明
1、解压
# tar jxvf libsocket-1.1.tar.bz2
# cd libsocket-1.1

2、编译
# make

3、安装
# make install

4、移植到arm平台
# make clean

修改Makefile
PREFIX = $(PWD)/install

# make CC=arm-linux-gcc
# make install
拷贝install/include/libsocket.h到交叉编译器
拷贝install/lib/libsocket.so*到交叉编译器和嵌入式平台(注意拷贝时加上参数-a)

库函数使用说明详见test.c，包含udp、tcp客户、tcp服务测试。


