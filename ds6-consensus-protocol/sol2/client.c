#include "util.h"

const char* dst_ip = "127.0.0.1";
#define NUM_SRV 4

// 임의의 key를 생성해서 반환해줌
void generate_key(char* key) {
    uint64_t number = rand() % DATASET_SIZE;
    for (int i = 0; i < 5; ++i) number = ((number << 3) - number + 7) & 0xFFFFFFFFFFFFFFFF;
    key[KEY_SIZE - 1] = '\0';
    for (int i = KEY_SIZE - 2; i >= 0; i--) {
        int index = number % SET_SIZE;
        key[i] = SET[index];
        number /= SET_SIZE;
    }
}

int main(int argc, char *argv[]) {

  srand((unsigned int)time(NULL));  // 난수 발생기 초기화
  /* 서버 구조체 설정 */
	int SERVER_PORT = 5001;
	struct sockaddr_in srv_addr; // 패킷을 수신할 서버의 정보를 담을 소켓 구조체를 생성한다.
	memset(&srv_addr, 0, sizeof(srv_addr)); // 구조체를 모두 '0'으로 초기화해준다.
	srv_addr.sin_family = AF_INET; // IPv4를 사용할 것이므로 AF_INET으로 family를 지정한다.
	srv_addr.sin_port = htons(SERVER_PORT); // 서버의 포트번호를 넣어준다. 이 때 htons()를 통해 byte order를 network order로 변환한다.
	inet_pton(AF_INET, dst_ip, &srv_addr.sin_addr);  // 문자열인 IP주소를 바이너리로 변환한 후 소켓 구조체에 저장해준다.

  /* 소켓 생성 */
	int sock; // 소켓 디스크립터(socket descriptor)를 생성한다.
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { // socket()으로 IPv4(AF_INET), UDP(SOC_DGRAM)를 사용하는 소켓을 생성 시도한다.
		printf("Could not create socket\n"); // sock으로 return되는 값이 -1이라면 소켓 생성에 실패한 것이다.
		exit(1);
	}

	int n = 0;

  struct KVS SendMsg={0,}; // 송신용으로 쓸 메시지 구조체 생성 및 초기화

  struct sockaddr_in src_addr; // 패킷을 수신하였을 때, 해당 패킷을 보낸 송신자(Source)의 정보를 저장하기 위한 소켓 구조체
  socklen_t src_addr_len = sizeof(src_addr); // 수신한 패킷의 소켓 구조체 크기를 저장함. IPv4를 사용하므로 sockaddr_in 크기인 16바이트가 저장됨.
  int cnt = 0; // 패킷 5개를 전송한다.
  size_t pkt_size = 0;
	while(cnt < 5){
    printf("Request ID: %d\n",cnt++);
    // Your codes should be here
    /* 서버 3개 소켓 생성 */
    const char* dst_ip = "127.0.0.1"; // 하나의 host안에서 통신할 것이므로 서버주소는 localhost(i.e., 127.0.0.1)임
    struct sockaddr_in server_addr[3]; // 패킷을 수신할 서버의 정보를 담을 소켓 구조체를 생성한다.
    int server_ports[3] = {5001, 5002, 5003};
    for(int i = 0; i < 3; i++){
      memset(&server_addr[i], 0, sizeof(server_addr[i])); // 구조체를 모두 '0'으로 초기화해준다.
      server_addr[i].sin_family = AF_INET; // IPv4를 사용할 것이므로 AF_INET으로 family를 지정한다.
      server_addr[i].sin_port = htons(server_ports[i]); // 서버의 포트번호를 넣어준다. 이 때 htons()를 통해 byte order를 network order로 변환한다.
      inet_pton(AF_INET, dst_ip, &server_addr[i].sin_addr); // 문자열인 IP주소를 바이너리로 변환한 후 소켓 구조체에 저장해준다.
    }
    int server_sock[3]; // server의 정보를 담을 소켓 구조체 생성
    for (int i = 0; i < 3; i++){ // server의 정보를 담을 소켓 구조체 초기화
      if ((server_sock[i] = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Could not create socket\n");
        exit(1);
      }
    }
    struct sockaddr_in src_addr[3]; // 각 서버로부터 패킷을 수신하였을 때, 해당 패킷을 보낸 서버의 정보를 저장하기 위한 소켓 구조체 생성
    socklen_t src_addr_len[3];
    for(int i = 0; i < 3; i++){
      src_addr_len[i] = sizeof(src_addr[i]);
    }

    /* 송신 파트 */
    SendMsg.type = READ_REQ; // 요청 타입을 읽기로 선언
    strcpy(SendMsg.value, "");
    pkt_size = sizeof(struct KVS) - VALUE_SIZE; 
    generate_key(SendMsg.key); // key를 새로 생성
    printf("Sent bytes: %ld\n", pkt_size);
    printf("Type: %s Key: %s Value: %s\n", get_type(SendMsg), SendMsg.key, SendMsg.value);
    for(int i = 0; i < 3; i++){ // 각 서버로 메시지 전송
      sendto(server_sock[i], &SendMsg, pkt_size, 0, (struct sockaddr *)&server_addr[i], sizeof(server_addr[i])); // 생성한 메시지를 서버1, 2, 3으로 송신
    }

    /* 수신 파트 */
    struct KVS RecvMsg[3] = {0}; // 수신용으로 쓸 메시지 구조체 생성 및 초기화
    int n_recv[3] = {0};
    for(int i = 0; i < 3; i++){
      n_recv[i] = recvfrom(server_sock[i], &RecvMsg[i], sizeof(RecvMsg[i]), 0, (struct sockaddr *)&src_addr[i], &src_addr_len[i]); // 각 서버로부터 답장 수신
    }
    if(n_recv[0] > 0 && n_recv[1] > 0 && n_recv[2] > 0){ // 각 서버로부터 수신에 성공하면
      for(int i = 0; i < 3; i++){ // 각 서버로부터 온 메시지 내용 출력
        printf("Received bytes: %ld\n", sizeof(RecvMsg[i]));
        printf("Type: %s Key: %s Value: %s\n", get_type(RecvMsg[i]), RecvMsg[i].key, RecvMsg[i].value);
      }
      // 모든 서버로부터 도착한 메시지를 수합해서 2개의 이상의 서버가 동일한 값을 받으면
      // 즉, 모든 서버로부터 도착한 메시지가 모두 다르면 False로 출력하고 나머지 경우들에는 True로 출력
      if (strcmp(RecvMsg[0].value, RecvMsg[1].value) != 0 
          && strcmp(RecvMsg[1].value, RecvMsg[2].value) != 0 
          && strcmp(RecvMsg[0].value, RecvMsg[2].value) != 0) {
        printf("False\n\n");
      }
      else {
        printf("True\n\n");
      }
    }
	}

	close(sock); // 소켓을 닫아준다.
	return 0;
}
