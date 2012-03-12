#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<errno.h>
#include<string.h>
#include<pcap.h>
#include "pkt_header.h"
#include "msg_info.h"

#define MAXLINE         300

void got_packet(u_char *args, const struct pcap_pkthdr *header,const u_char *packet){
	MSG_INFO *msg;
	ethernet = (struct sniff_ethernet*)(packet);
	ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
	
	
	size_ip = IP_HL(ip)*4;
	if (size_ip < 20) {
		printf("   * Invalid IP header length: %u bytes\n", size_ip);
		return;
	}

	udp = (struct udp_header *)(packet + SIZE_ETHERNET + sizeof(struct sniff_ip));

	payload = (const char*)(packet + SIZE_ETHERNET + sizeof(struct sniff_ip) + sizeof(struct udp_header));
	
	long usec = header->ts.tv_usec;
	long sec = header->ts.tv_sec;
	double recv_ts = sec+(usec/1000000.0);
	msg = (MSG_INFO*) payload;
	unsigned long long seqnum = msg->seqnumber;
	printf("%lf\t%llu\n", recv_ts, seqnum);
}
int main(int argc, char **argv){
  char *dev, errbuf[PCAP_ERRBUF_SIZE]; /* Error string */
  pcap_t *handle; /* Session handle */
  struct bpf_program fp;		/* The compiled filter expression */
  char filter_exp[] = "ip and udp and port 9877";	/* The filter expression */
  bpf_u_int32 mask;		/* The netmask of our sniffing device */
  bpf_u_int32 net;		/* The IP of our sniffing device */
  const u_char *packet;		/* The actual packet */
  char *ifilename=NULL;
  char errbuffer[300];
  if(argc>1){
       ifilename = argv[1];
  }else{
  	printf("Usage: ./pcapoffline filename");
	exit(1);
  }
  
  handle = pcap_open_offline(ifilename,errbuffer);
  if (handle == NULL) {
  	fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
	return(2);
  }
  if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
    fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
    return(2);
  }
  if (pcap_setfilter(handle, &fp) == -1) {
    fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
    return(2);
  }
  freopen(strcat(ifilename,".txt"),"w",stdout);
  printf("timestamp\tseqnum\n");
  if( pcap_loop(handle, -1, got_packet , NULL)==-1){
    fprintf(stderr, "Meet Error during Pcap_Loop: %s\n",pcap_geterr(handle));
  }
  
  return 0;
}

