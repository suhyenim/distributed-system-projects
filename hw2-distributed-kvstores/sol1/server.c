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


	init_kvs(); // key-value store 초기화

	int n = 0;

  struct KVS RecvMsg; // 수신용으로 쓸 메시지 구조체 생성 및 초기화
	struct sockaddr_in src_addr; // 패킷을 수신하였을 때, 해당 패킷을 보낸 송신자(Source)의 정보를 저장하기 위한 소켓 구조체
  socklen_t src_addr_len = sizeof(src_addr);
	size_t pkt_size = 0;
	while (!quit) {
    strcpy(RecvMsg.value,""); // RecvMsg 구조체 변수의 value필드를 비워줌. 비워주지 않으면 읽기 요청을 받았을 때, 이전 쓰기 요청의 value필드 값이 남아있게 된다.
		n = recvfrom(sock, &RecvMsg, sizeof(RecvMsg), MSG_DONTWAIT, (struct sockaddr *)&src_addr, &src_addr_len);
		if (n > 0) {
			printf("Type: %s Key: %s Value: %s\n",get_type(RecvMsg),RecvMsg.key, RecvMsg.value); // 수신한 내용을 출력한다.
			if (RecvMsg.type == READ_REQ){
				RecvMsg.type = READ_REP;
  				strcpy(RecvMsg.value,"DDDCCCCBBBBAAAA");
				pkt_size = n + VALUE_SIZE; // 읽기 답장은 VALUE필드를 포함해서 가므로, 기존 수신한 바이트 수(즉 읽기 요청의 크기)에 value_size를 더한다.
			}
			else if(RecvMsg.type == WRITE_REQ){
				RecvMsg.type = WRITE_REP;
				uint64_t index = hash64(RecvMsg.key) % DATASET_SIZE; // 인덱스는 key에 해시함수를 적용하고 DASET_SIZE로 modulo 연산을 수행해서 구한다.
				//printf("%lu\n",index);
			 	strcpy(kv[index],RecvMsg.value);
				pkt_size = n - VALUE_SIZE; // 쓰기 답장은 VALUE필드를 보낼 필요 없으므로 보내고자 하는 패킷 크기에서 제외한다.
				//printf("%d\n",pkt_size);
			}

			sendto(sock, &RecvMsg, pkt_size, 0, (struct sockaddr *)&src_addr, sizeof(src_addr));
		}
	}

	printf("\nCtrl+C pressed. Exit the program after cloing the socket\n");
	close(sock);

	return 0;
}
