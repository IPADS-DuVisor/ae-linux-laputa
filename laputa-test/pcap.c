const int BATCH_SIZE = 32;

// From https://wiki.wireshark.org/Development/LibpcapFileFormat
typedef struct pcap_hdr_s {
	uint32_t magic_number;  /* magic number */
	uint16_t version_major; /* major version number */
	uint16_t version_minor; /* minor version number */
	int32_t  thiszone;      /* GMT to local correction */
	uint32_t sigfigs;       /* accuracy of timestamps */
	uint32_t snaplen;       /* max length of captured packets, in octets */
	uint32_t network;       /* data link type */
} pcap_hdr_t;

typedef struct pcaprec_hdr_s {
	uint32_t ts_sec;        /* timestamp seconds */
	uint32_t ts_usec;       /* timestamp microseconds */
	uint32_t incl_len;      /* number of octets of packet saved in file */
	uint32_t orig_len;      /* actual length of packet */
} pcaprec_hdr_t;

// Public stubs that forward the calls to the driver-specific implementations
static inline uint32_t ixy_rx_batch(struct virtio_device* dev, uint16_t queue_id, struct pkt_buf* bufs[], uint32_t num_bufs) {
 	return dev->rx_batch(dev, queue_id, bufs, num_bufs);
}

int pcap(struct virtio_device* dev) {

	FILE* pcap = fopen("pcap.txt", "wb");
	if (pcap == NULL) {
		error("failed to open file.\n");
	}

	int64_t n_packets = 10;
	printf("Capturing packets...\n");

	pcap_hdr_t header = {
		.magic_number =  0xa1b2c3d4,
		.version_major = 2,
		.version_minor = 4,
		.thiszone = 0,
		.sigfigs = 0,
		.snaplen = 65535,
		.network = 1, // Ethernet
	};
	fwrite(&header, sizeof(header), 1, pcap);

	struct pkt_buf* bufs[BATCH_SIZE];
	while (n_packets != 0) {
		uint32_t num_rx = ixy_rx_batch(dev, 0, bufs, BATCH_SIZE);
		// printf("batch return \n");
		struct timeval tv;
		gettimeofday(&tv, NULL);

		for (uint32_t i = 0; i < num_rx && n_packets != 0; i++) {
			pcaprec_hdr_t rec_header = {
				.ts_sec = tv.tv_sec,
				.ts_usec = tv.tv_usec,
				.incl_len = bufs[i]->size,
				.orig_len = bufs[i]->size
			};
			printf("size is %ld\n", bufs[i]->size);
			fwrite(&rec_header, sizeof(pcaprec_hdr_t), 1, pcap);

			fwrite(bufs[i]->data, bufs[i]->size, 1, pcap);

			pkt_buf_free(bufs[i]);
			// n_packets == -1 indicates unbounded capture
			if (n_packets > 0) {
				n_packets--;
			}
		}
	}

	fclose(pcap);
	return 0;
}
