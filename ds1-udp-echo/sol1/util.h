#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <time.h>

// Constants
#define KEY_SIZE 16 // 사용할 KEY 크기이다. 16바이트.
#define VALUE_SIZE 16 // 사용할 VALUE 크기이다. 32바이트.
#define DATASET_SIZE 100000 // 데이터셋 크기
#define SET_SIZE 62     // 가능한 문자들의 수 (예: 영문 대소문자 + 숫자)
const char SET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

// 가독성을 위해 메시지 타입별로 매크로를 만듬
#define READ_REQ 0
#define READ_REP 1
#define WRITE_REQ 2
#define WRITE_REP 3

struct KVS { // key value store 구조체
    uint8_t type;  // message type
    char key[KEY_SIZE]; // key
    char value[VALUE_SIZE]; // value
} __attribute__((packed));
