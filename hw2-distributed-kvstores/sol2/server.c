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
  struct KVS RecvMsg; // 수신용으로 쓸 메시지 구조체 생성

	struct sockaddr_in src_addr; // 패킷을 수신하였을 때, 해당 패킷을 보낸 송신자(Source)의 정보를 저장하기 위한 소켓 구조체
  socklen_t src_addr_len = sizeof(src_addr);
	size_t pkt_size = 0;
	while (!quit) {
    // Your codes should be here
	memset(&RecvMsg, 0, sizeof(RecvMsg)); // 메시지 초기화
	n = recvfrom(sock, &RecvMsg, sizeof(RecvMsg), 0, (struct sockaddr *)&src_addr, &src_addr_len); // 클라이언트로부터 요청 메시지 수신
		if (n > 0) { // 수신에 성공했다면 
			printf("Type: %s Key: %s Value: %s\n", get_type(RecvMsg), RecvMsg.key, RecvMsg.value); // 요청 메시지의 type, key, value 필드 출력
			int index = hash64(RecvMsg.key) % DATASET_SIZE; // key에 대한 index 계산
			if(RecvMsg.type == READ_REQ){ // 읽기 요청인 경우
				strncpy(RecvMsg.value, kv[index], VALUE_SIZE - 1); // index에 해당하는 값을 메시지의 value 필드에 복사
				RecvMsg.type = READ_REP; // 메시지 type을 읽기 답장으로 설정
				sendto(sock, &RecvMsg, sizeof(RecvMsg), 0, (struct sockaddr *)&src_addr, sizeof(src_addr)); // 클라이언트에게 응답 메시지 송신
			}
			else { // 쓰기 요청인 경우
				strncpy(kv[index], RecvMsg.value, VALUE_SIZE - 1); // 메시지의 value 필드에 index에 해당하는 값 복사
				strncpy(RecvMsg.value, "", VALUE_SIZE); // 메시지의 value 필드를 비움
				RecvMsg.type = WRITE_REP; // 메시지 type을 쓰기 답장으로 설정
				sendto(sock, &RecvMsg, sizeof(RecvMsg) - VALUE_SIZE, 0, (struct sockaddr *)&src_addr, sizeof(src_addr)); // 클라이언트에게 응답 메시지 송신
			}
		}
	}

	printf("\nCtrl+C pressed. Exit the program after closing the socket\n");
	close(sock);

	return 0;
}
