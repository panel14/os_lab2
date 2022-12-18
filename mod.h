struct net_device_info {
	unsigned int mem_start;
	unsigned int mem_end;
	unsigned long base_address;
	unsigned long state;
};

struct inode_info {
	unsigned long i_no;
	int i_count;
	unsigned long long i_mode;
	unsigned long long i_size;
	long m_time;
};
