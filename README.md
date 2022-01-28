	ostype.h：主要定义头文件，回调函数类型，enum结构体，以及相关日志定义

	util.h：定义了CRefObject类，主要用于多线程对于类对象的保护作用。
		对外暴漏接口：
			void AddRef()：增加对象的调用次数，初始化为1.
			void RemoveRef()：减少对象的调用次数，减少为0，则删除此对象
			void SetMulFlag()：设置多线程安全标识，设置此标识，则多线程安全
	
	BaseSocket.h：定义了CBaseSocket类，继承与uti.h中的CRefObject类，主要负责socket的相关操作
		1、对外暴漏接口（分为对EPOLL暴漏，和对NETLIB暴漏，NETLIB主要用户来使用）：
			1、重写的SOCKET函数，主要用于socket的直接操作，暴漏给NETLIB使用
				int Listen(const char* server_ip, const uint16_t server_port, callback_t callback, void* callback_data)：用来绑定监听socket的IP，端口，回调函数以及回调函数的参数。
				SOCKET Connect(const char* remote_ip, const uint16_t remote_port, callback_t callback, void * callback_data)：当作客户端向服务端发起连接请求
				int Send(void* buf, int len)：用来向客户端发送消息
				int Recv(void* buf, int len)：用来接收客户端发送消息
				int Close()：关闭socket
			2、主要是CEventDispatch，也就是epoll事件调用的函数
				void OnRead()：对外提供，如果socket有read事件之后，需要调用此函数，来确定是处理新连接还是读事件，主要是epoll用来调用
				void OnWrite()：如果socket有write事件之后，需要调用此函数，来向对手socket发送消息，主要是epoll用来调用
				void OnClose()：如果socket有close事件之后，需要调用此函数，来改变状态，主要是epoll用来调用，并且，需要回调函数，将业务处理结束，然后才能关闭
			3、static 静态函数，主要负责对静态数据unorder_map结构体的处理，暴漏给EPOLL和NETLIB使用
				void AddBaseSocket(CBaseSocket* pSocket)：static函数，对静态数据成员g_socket_map进行添加，保存g_socket_map的socket和CBaseSocket的对应关系
				void RemoveBaseSocket(CBaseSocket* pSocket)：对静态数据成员g_socket_map进行移除，保存g_socket_map的socket和CBaseSocket的对应关系
				CBaseSocket* FindBaseSocket(SOCKET fd)：对静态数据成员g_socket_map进行查找，查找socket文件描述符对应的CBaseSocket对象
		2、内部私有化函数：
			void _SetAddr(const char * ip, const uint16_t port, struct sockaddr_in & addr)：内部函数，用来绑定IP，端口和const sockaddr_in结构体
			void _SetReuseAddr(SOCKET socketfd)：内部函数，设置此socket可重用
			void _SetNonBlock(SOCKET socketfd)：内部函数，设置此socket非阻塞
			void _AcceptNewSocket()：内部函数，接收一个新的客户端连接

	EventDispatch.h：定义了CEventDispatch类，主要负责epoll循环等相关消息。
		1、对外暴漏接口（主要是对NETLIB暴漏）：
			CEventDispatch()：CEventDispatch构造函数，主要用来create一个epoll
			~CEventDispatch()：CEventDispatch析构函数，主要用来删除一个epoll描述符
			static CEventDispatch& GetInstance()：单例模式场景，返回一个单例模式的实例，该方法使用C++11新特性，线程安全
			void AddEvent(SOCKET fd)：使用EPOLL_CTL_ADD，将一个socket添加到epoll中
			void RemoveEvent(SOCKET fd)：使用EPOLL_CTL_DEL，将一个socket从epoll中移除
			void StartDispatch(int wait_time)：开始循环，使用epoll_wati开启循环
			void StopDispatch()：终止循环等待socket事件

	netlib.h：定义了一系列函数，供业务层进行使用
		1、对外暴漏接口
			int netlib_init()：netlib_init初始化
			int netlib_bind(SOCKET fd, NETLIB_OPT opt, void* data)：绑定参数，主要用于绑定CBaseSocket的内部信息，比如回调函数，回调函数参数等
			int netlib_listen(const char* server_ip, const uint16_t server_port, callback_t callback, void* callback_data)：监听socket创建，业务层调用
			SOCKET netlib_connect(const char * ip, uint16_t port, callback_t callback, void * callback_data)：业务调用，主要用来当作客户端去申请连接服务端
			int netlib_recv(SOCKET fd, void * recvBuf, int len)：业务调用，当需要接受消息的时候使用
			int netlib_send(SOCKET fd, void * sendbuf, int len)：业务调用，当需要发送消息的时候使用
			int netlib_close(SOCKET fd)：业务调用，当需要关闭socket时候使用 
			void netlib_eventloop(int wait_time)：开启事件监听循环，调用CEventDispatch的实例，并开启循环
			void netlib_stop_eventloop()：停止事件监听循环，调用CEventDispatch的实例，并停止循环



当前为第一版本：只是实现了3层的调用关系。
第二版本：实现ConnObject，用于系统业务实现