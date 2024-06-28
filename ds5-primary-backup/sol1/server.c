#include "util.h"

int SERVER_PORT; // 서버 포트번호
char* kv[DATASET_SIZE]; // 정적 key value stores
void init_kvs(){
  for(int i =0;i<DATASET_SIZE;i++){
		kv[i] = malloc(VALUE_SIZE);
		strcpy(kv[i], "DDDCCCCBBBBAAAA");
		//printf("%s\n",kv[i]);
	}
}

static volatile int quit = 0; // Trigger conditions for SIGINT
void signal_handler(int signum) {
	if(signum == SIGINT){  // Functions for Ctrl+C (SIGINT)
		quit = 1;
	}
}

int main(int argc, char *argv[]) {
	// 프로그램 시작시 입력받은 매개변수를 parsing한다.
	if ( argc < 2 ){
	 printf("Input : %s port number\n", argv[0]);
	 return 1;
	}

	signal(SIGINT, signal_handler); // SIGINT에 대한 핸들러 등록

	SERVER_PORT = atoi(argv[1]); // 입력받은 argument를 포트번호 변수에 넣어준다.

	// 서버의 정보를 담을 소켓 구조체 생성 및 초기화
	struct sockaddr_in srv_addr;
	memset(&srv_addr, 0, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(SERVER_PORT);
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0 i.e., 자기 자신의 IP

	// 소켓을 생성한다.
	int sock;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("Could not create listen socket\n");
		exit(1);
	}

	// 생성한 소켓에 소켓구조체를 bind시킨다.
	if ((bind(sock, (struct sockaddr *)&srv_addr, sizeof(srv_addr))) < 0) {
		printf("Could not bind socket\n");
		exit(1);
	}

	const char* dst_ip = "127.0.0.1"; // 하나의 host안에서 통신할 것이므로 서버주소는 localhost(i.e., 127.0.0.1)임
	struct sockaddr_in follower_addr[2]; // 패킷을 수신할 서버의 정보를 담을 소켓 구조체를 생성한다.
	int follower_ports[2] = {5002, 5003};
	for(int i = 0; i < 2; i++){
		memset(&follower_addr[i], 0, sizeof(follower_addr[i])); // 구조체를 모두 '0'으로 초기화해준다.
		follower_addr[i].sin_family = AF_INET; // IPv4를 사용할 것이므로 AF_INET으로 family를 지정한다.
		follower_addr[i].sin_port = htons(follower_ports[i]); // 서버의 포트번호를 넣어준다. 이 때 htons()를 통해 byte order를 network order로 변환한다.
		inet_pton(AF_INET, dst_ip, &follower_addr[i].sin_addr); // 문자열인 IP주소를 바이너리로 변환한 후 소켓 구조체에 저장해준다.
	}

	// follower의 정보를 담을 소켓 구조체 생성 및 초기화
	int follower_sock[2];
	for (int i = 0; i < 2; i++){
		if ((follower_sock[i] = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("Could not create socket\n");
		exit(1);
		}
	}

	init_kvs(); // key-value store 초기화

	int n = 0;
  	struct KVS RecvMsg={0,}; // 수신용으로 쓸 메시지 구조체 생성 및 초기화

	struct sockaddr_in src_addr; // 패킷을 수신하였을 때, 해당 패킷을 보낸 송신자(Source)의 정보를 저장하기 위한 소켓 구조체
  	socklen_t src_addr_len = sizeof(src_addr);

	//follower들에게 보내기 위해 생성
	struct sockaddr_in src_addr2;
  	socklen_t src_addr_len2 = sizeof(src_addr2);

	struct sockaddr_in src_addr3;
  	socklen_t src_addr_len3 = sizeof(src_addr3);

	size_t pkt_size = 0;
	int cnt = 0;
	while (!quit) {
		if( SERVER_PORT == 5001 ){
			strcpy(RecvMsg.value,""); // 버퍼 초기화
			n = recvfrom(sock, &RecvMsg, sizeof(RecvMsg), MSG_DONTWAIT, (struct sockaddr *)&src_addr, &src_addr_len);
			if (n > 0) {
				printf("Type: %s Key: %s Value: %s\n",get_type(RecvMsg),RecvMsg.key, RecvMsg.value); // 수신한 내용을 출력한다.
				if (RecvMsg.type == READ_REQ){
					RecvMsg.type = READ_REP;
					uint64_t index = hash64(RecvMsg.key) % DATASET_SIZE;
					strcpy(RecvMsg.value,kv[index]);
					pkt_size = n + VALUE_SIZE;
					sendto(sock, &RecvMsg, pkt_size, 0, (struct sockaddr *)&src_addr, sizeof(src_addr));
				}
				else if(RecvMsg.type == WRITE_REQ){
					uint64_t index = hash64(RecvMsg.key) % DATASET_SIZE;
					strcpy(kv[index],RecvMsg.value);
					pkt_size = sizeof(RecvMsg);

					for(int i = 0; i < 2; i++)
						sendto(follower_sock[i], &RecvMsg, pkt_size, 0, (struct sockaddr *)&follower_addr[i], sizeof(follower_addr[i]));
				}
			}

			struct KVS RecvMsg1={0,}; // 수신용으로 쓸 메시지 구조체 생성 및 초기화
			struct KVS RecvMsg2={0,}; // 수신용으로 쓸 메시지 구조체 생성 및 초기화

			int n2 = recvfrom(follower_sock[0], &RecvMsg1, sizeof(RecvMsg1), MSG_DONTWAIT, (struct sockaddr *)&src_addr2, &src_addr_len2);
			int n3 = recvfrom(follower_sock[1], &RecvMsg2, sizeof(RecvMsg2), MSG_DONTWAIT, (struct sockaddr *)&src_addr3, &src_addr_len3);

			if (n2 > 0) {
				if(RecvMsg1.type == WRITE_REP) cnt++;
			}

			if (n3 > 0) {
				if(RecvMsg2.type == WRITE_REP) cnt++;
			}

			if(cnt == 2){
				cnt = 0;
				RecvMsg.type = WRITE_REP;
				pkt_size = sizeof(RecvMsg) - VALUE_SIZE;
				printf("Write is Done\n\n");
				sendto(sock, &RecvMsg, pkt_size, 0, (struct sockaddr *)&src_addr, sizeof(src_addr));
			}
		}
		else if ( SERVER_PORT == 5002 || SERVER_PORT == 5003 ){
			strcpy(RecvMsg.value,""); // 버퍼 초기화
			n = recvfrom(sock, &RecvMsg, sizeof(RecvMsg), MSG_DONTWAIT, (struct sockaddr *)&src_addr, &src_addr_len);
			if (n > 0) {
				printf("Type: %s Key: %s Value: %s\n",get_type(RecvMsg),RecvMsg.key, RecvMsg.value); // 수신한 내용을 출력한다.
				if(RecvMsg.type == WRITE_REQ){
					RecvMsg.type = WRITE_REP;
					uint64_t index = hash64(RecvMsg.key) % DATASET_SIZE;
					strcpy(kv[index],RecvMsg.value);
					pkt_size = n - VALUE_SIZE;
				}
          		printf("Write is Done\n\n");
				sendto(sock, &RecvMsg, pkt_size, 0, (struct sockaddr *)&src_addr, sizeof(src_addr));
			}
		}
	}

	printf("\nCtrl+C pressed. Exit the program after closing the socket\n");
	close(sock);

	return 0;
}
