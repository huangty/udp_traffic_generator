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
#include<sys/time.h>
#include"msg_info.h"
#define MAXLINE 2000
#define MAXPAYLOAD  1458
unsigned long long last_seq = 0;
/*
 *  When Sever Receive a Packet, then use pkt_process to process the packet
 */
void pkt_process(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen)
{
	  int n;
	  socklen_t	len;
	  char buffer[MAXLINE];	  
	  struct timeval begin;
	  gettimeofday(&begin, NULL);
	  unsigned long long lost_packet = 0;
	  unsigned long long num = 1;
	  MSG_INFO *msg;

	  for (;;num++) {
		  len = clilen;
		  memset(&buffer, '\0' , sizeof(buffer));
		  /*Packet Content are put in Buffer*/
	      n = recvfrom(sockfd, buffer, MAXPAYLOAD, 0, pcliaddr, &len);
		  msg = (MSG_INFO*) buffer;
		  unsigned long long seqnum = msg->seqnumber;
		  printf("Receive pkt %llu with size%d\n", seqnum, n);
		  /*if(last_seq == 0){
		  	last_seq = seqnum;
		  }else{
		  	if( (seqnum - last_seq) > 1){
				printf("Received:%llu, last seen:%llu, gap:%llu\n", seqnum, last_seq, seqnum-last_seq);
				lost_packet += (seqnum-last_seq)-1;
				printf("Loss rate so far: %f\n", lost_packet / (float) num);
			}
			last_seq = seqnum;
		  }*/
		  /*echo back to sender*/
		  /*MSG_INFO ack;
		  printf("Receive pkt #%d: echo back packet with seqnumber %d\n",num,seqnum);
		  struct timeval tim;
		  gettimeofday(&tim, NULL);
		  double time_stamp=tim.tv_sec+(tim.tv_usec/1000000.0);
		  ack.seqnumber = seqnum;
		  ack.time_stamp = time_stamp;
		  sendto(sockfd, (char *)&ack , sizeof(MSG_INFO) , 0, pcliaddr, len);*/
	  }
}

int main(int argc, char **argv){
  int sockfd;
  struct sockaddr_in servaddr, cliaddr;
  sockfd = socket(AF_INET, SOCK_DGRAM,0);
  bzero(&servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(9877);
  bind(sockfd, (struct sockaddr *) &servaddr,sizeof(servaddr));

  pkt_process(sockfd, (struct sockaddr *)&cliaddr,sizeof(cliaddr));
  return 0;
}

