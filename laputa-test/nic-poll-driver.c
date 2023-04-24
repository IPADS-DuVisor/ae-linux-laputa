/* Can't add new CONFIG parameters in an external module, so define them here */
#define CONFIG_ICENET_MTU 1500
#define CONFIG_ICENET_RING_SIZE 64
#define CONFIG_ICENET_CHECKSUM
#define CONFIG_ICENET_TX_THRESHOLD 16

#define ICENET_NAME "icenet"
#define ICENET_SEND_REQ 0
#define ICENET_RECV_REQ 8
#define ICENET_SEND_COMP 16
#define ICENET_RECV_COMP 18
#define ICENET_COUNTS 20
#define ICENET_MACADDR 24
#define ICENET_INTMASK 32
#define ICENET_TXCSUM_REQ 40
#define ICENET_RXCSUM_RES 48
#define ICENET_CSUM_ENABLE 49

#define ICENET_INTMASK_TX 1
#define ICENET_INTMASK_RX 2
#define ICENET_INTMASK_BOTH 3

#define ETH_HEADER_BYTES 14
#define ALIGN_BYTES 8
#define ALIGN_MASK 0x7
#define ALIGN_SHIFT 3
#define MAX_FRAME_SIZE (CONFIG_ICENET_MTU + ETH_HEADER_BYTES + NET_IP_ALIGN)
#define DMA_PTR_ALIGN(p) ((typeof(p)) (__ALIGN_KERNEL((uintptr_t) (p), ALIGN_BYTES)))
#define DMA_LEN_ALIGN(n) (((((n) - 1) >> ALIGN_SHIFT) + 1) << ALIGN_SHIFT)
#define MACADDR_BYTES 6


unsigned long icenet_io_base;
unsigned long icenet_mem_rx_base;
unsigned long icenet_mem_tx_base;
unsigned long rx_packets = 0, rx_bytes = 0;
unsigned long tx_packets = 0, tx_bytes = 0;

struct list rx_recving_list;



static inline int recv_req_avail(void ) {
	return (ioread32(icenet_base + ICENET_COUNTS) >> 8) & 0xff;
}


static inline void post_recv(unsigned long *page_addr) {
	unsigned long addr = virt_to_phys(page_addr);

	iowrite64(addr, icenet_io_base + ICENET_RECV_REQ);
	push_page(&rx_recving_list, page_addr);
}

static void alloc_recv(void) {
	int hw_recv_cnt = recv_req_avail();
	int sw_recv_cnt = get_free_pages(icenet_mem_rx_base);
	int recv_cnt = (hw_recv_cnt < sw_recv_cnt) ? hw_recv_cnt : sw_recv_cnt;
	unsigned long* page_addr;

	for ( ; recv_cnt > 0; recv_cnt--) {
		page_addr = alloc_page(icenet_mem_base, MAX_FRAME_SIZE);
		post_recv(page_addr);
	}
}


static inline int recv_comp_avail() {
	return (ioread32(icenet_io_base + ICENET_COUNTS) >> 24) & 0xff;
}

static inline int recv_comp_len() {
	return ioread16(nic->iomem + ICENET_RECV_COMP);
}

static void polling_recv() {
	int n = recv_comp_avail();	
	int i, len;
	unsigned long* page_addr;
	for(int i = 0; i < n; i++) {
		len = recv_comp_len();
		page_addr = pop_page(&rx_recving_list);
		rx_packets++;
	}
}


static inline int send_req_avail(struct icenet_device *nic) {
    return ioread32(icenet_io_base + ICENET_COUNTS) & 0xff;
}

static inline int send_space(int nfrags) {
    return send_req_avail() >= nfrags;
}

uint32_t icenet_tx_batch(struct virtio_device_userspace* dev,
        uint16_t queue_id, struct pkt_buf* bufs[], uint32_t num_bufs) {
    uintptr_t addr;
    uint64_t packet;
    uint64_t partial = 0;
    uint64_t len;
    // when there is tx queue in nic
    while (!send_space(num_bufs));

    for (int i = 0; i < num_bufs; i++) {
        addr = virt_to_phys(bufs[i].data);
        packet = (partial << 63) | (len << 48) | (addr & 0xffffffffffffL);
        iowrite(packet, icenet_io_base + ICENET_SEND_REQ);
    }

    // make sure num_bufs has been sent
    for (int i = 0; i < num_bufs; i++) {
        ioread16(icenet_io_base + ICENET_SEND_COMP);
    }
    return 0;
}


static void icenet_init_mac_address(struct net_device *ndev)
{
	uint64_t macaddr = ioread64(nic->iomem + ICENET_MACADDR);
	printf("mac addr is 0x%lx\n". macaddr);
}



struct void ice_init_userspace(const char* name,
        uint16_t rx_queues, uint16_t tx_queues) {
    // not build icenet driver, so no need to remove virtio driver
    // remove_virtio_driver(name);

    struct icenet_device_userspace* dev = calloc(1, sizeof(*dev));

    icenet_base = 0x3000008000UL;
    return;
}
