#include "util.h"

int SERVER_PORT; // 서버 포트번호
int BACKUP_PORT1 = 5002; // Backup 서버 1 포트번호
int BACKUP_PORT2 = 5003; // Backup 서버 2 포트번호

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

    /* Backup 서버 소켓 추가 */
    // Backup 서버1, Backup 서버2 주소 구조체 초기화
    struct sockaddr_in backup_addr1, backup_addr2;
    memset(&backup_addr1, 0, sizeof(backup_addr1));
    backup_addr1.sin_family = AF_INET;
    backup_addr1.sin_port = htons(BACKUP_PORT1);
    backup_addr1.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t backup_addr1_len = sizeof(backup_addr1);
    memset(&backup_addr2, 0, sizeof(backup_addr2));
    backup_addr2.sin_family = AF_INET;
    backup_addr2.sin_port = htons(BACKUP_PORT2);
    backup_addr2.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t backup_addr2_len = sizeof(backup_addr2);
    // Backup 서버1, Backup 서버2 소켓 생성
    int primary_sock, backup_sock1, backup_sock2; 
    if ((backup_sock1 = socket(AF_INET, SOCK_DGRAM, 0)) < 0 || (backup_sock2 = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Could not create backup socket\n");
        exit(1);
    }

	init_kvs(); // key-value store 초기화

	int n = 0;
  	struct KVS RecvMsg={0,}; // 수신용으로 쓸 메시지 구조체 생성 및 초기화

	struct sockaddr_in src_addr; // 패킷을 수신하였을 때, 해당 패킷을 보낸 송신자(Source)의 정보를 저장하기 위한 소켓 구조체
  	socklen_t src_addr_len = sizeof(src_addr);
	size_t pkt_size = 0;
	while (!quit) {
        n = recvfrom(sock, &RecvMsg, sizeof(RecvMsg), MSG_DONTWAIT, (struct sockaddr *)&src_addr, &src_addr_len); // 메시지를 수신한다.
		if (n > 0) {
            printf("Type: %s Key: %s Value: %s\n", get_type(RecvMsg), RecvMsg.key, RecvMsg.value); // 수신한 내용을 출력한다.
            if (SERVER_PORT == 5001) { // Primary 서버인 경우의 동작 (5001번 포트)
                if (RecvMsg.type == READ_REQ) { // 수신한 메시지가 읽기 요청이라면
                    uint64_t index = hash64(RecvMsg.key) % DATASET_SIZE;
					strcpy(RecvMsg.value, kv[index]); // Primary 서버의 kvstore의 해당 index에 대한 value값을 수정한다. 				
					RecvMsg.type = READ_REP; // 읽기 응답으로 메시지 타입을 바꿔준다. 
					pkt_size = n + VALUE_SIZE; // value 값을 포함해서 client로 보내기 때문에 VALUE_SIZE만큼 추가한다.
                    sendto(sock, &RecvMsg, pkt_size, 0, (struct sockaddr *)&src_addr, sizeof(src_addr));
				}
                else if (RecvMsg.type == WRITE_REQ) { // 수신한 메시지가 쓰기 요청이라면                    
                    uint64_t index = hash64(RecvMsg.key) % DATASET_SIZE;
                    strcpy(kv[index], RecvMsg.value);    
                    // Backup 서버들에도 쓰기 요청 보내기
                    sendto(backup_sock1, &RecvMsg, sizeof(RecvMsg), 0, (struct sockaddr *)&backup_addr1, sizeof(backup_addr1));
                    sendto(backup_sock2, &RecvMsg, sizeof(RecvMsg), 0, (struct sockaddr *)&backup_addr2, sizeof(backup_addr2));
                    // Backup 서버들로부터 응답 대기
                    struct KVS RecvMsg1={0,}, RecvMsg2={0,}; // Backup 서버의 수신용으로 쓸 메시지 구조체 생성 및 초기화
                    int n1 = recvfrom(backup_sock1, &RecvMsg1, sizeof(RecvMsg1), 0, (struct sockaddr *)&backup_addr1, &backup_addr1_len);
                    int n2 = recvfrom(backup_sock2, &RecvMsg2, sizeof(RecvMsg2), 0, (struct sockaddr *)&backup_addr2, &backup_addr2_len);
                    if (n1 > 0 && n2 > 0) { // Backup 서버들로부터 응답을 받으면 
                        RecvMsg.type = WRITE_REP; // 쓰기 응답으로 메시지 타입을 바꿔준다. 
                        pkt_size = n - VALUE_SIZE; // value 값 없이 client로 보내기 때문에 VALUE_SIZE만큼 빼준다.
                        printf("Write is Done\n\n");
                        sendto(sock, &RecvMsg, pkt_size, 0, (struct sockaddr *)&src_addr, sizeof(src_addr)); // 클라이언트로 답장을 보낸다.
                    }                 
                }
            }
            else { // Backup 서버인 경우의 동작 (5002, 5003번 포트)
                uint64_t index = hash64(RecvMsg.key) % DATASET_SIZE;
                strcpy(kv[index], RecvMsg.value); // Backup 서버의 kvstore의 해당 index에 대한 value값 수정
                printf("Write is Done\n\n");
                sendto(sock, &RecvMsg, sizeof(RecvMsg), 0, (struct sockaddr *)&src_addr, sizeof(src_addr)); // Primary 서버로 답장을 송신한다. 
            }
        }
        strcpy(RecvMsg.value, ""); // 버퍼 초기화
	}
	printf("\nCtrl+C pressed. Exit the program after closing the socket\n");
	close(sock);
    close(backup_sock1);
    close(backup_sock2);

	return 0;
}
