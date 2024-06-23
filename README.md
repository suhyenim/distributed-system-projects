# Distributed system Implementation Projects

UDP, Distributed key-value stores, Look-aside cache, Power of two choices(Load balancing), Primary-backup, Consensus protocol 등 분산시스템 설계 및 구현
```
.
├── hw1-udp-echo
├── hw2-distributed-kvstores
├── hw3-look-aside-cache
├── hw4-power-of-two-choices
├── hw5-primary-backup
├── hw6-consensus-protocol

```
<br>


## 📚 About Projects
### 1️. UDP echo program 구현 
<div>
- 분산 시스템의 실제 동작 원리 이해 <br>  
- POSIX Socket API를 사용해 client-server 프로그램을 실제 구현
</div>
<br> 
<details>
<summary>결과 화면</summary>
<img width="697" alt="스크린샷 2024-05-01 오후 8 51 37" src="https://github.com/s-h-im/distributed-system/assets/100345983/81704d05-9920-4e63-aa49-92085140f2b8">
</details>


### 2️. Distributed key-value stores 구현 
<div>
- 파티셔닝과 분산 데이터 저장소에 대한 이해 <br> 
- 여러 server에 데이터가 나뉘어 저장되어 있는 분산 key-value store server 구현 <br>    
- 해시 함수 기반으로 파티션을 고려해서 요청을 전송하는 client 구현
</div>
<br> 
<details>
<summary>결과 화면</summary>
<img width="697" alt="client" src="https://github.com/s-h-im/distributed-system/assets/100345983/06ca26d6-4ab8-4f40-9e50-6e5525e88c18">
<img width="697" alt="server1" src="https://github.com/s-h-im/distributed-system/assets/100345983/d1edcea6-ad88-475b-9db1-174801d06495">
<img width="697" alt="server2" src="https://github.com/s-h-im/distributed-system/assets/100345983/5e5eb986-8d12-4335-8274-895f80e64abf">
</details>


### 3️. Look-aside cache 구현
<div>
- 캐시 작동 원리에 대해 이해 <br>   
- look-aside cache 구현 <br>   
- client는 읽기 요청을 보낸 후, cache 혹은 server로부터 답장을 받으면 이에 대한 적절한 처리 수행
</div>
<br> 
<details>
<summary>결과 화면</summary>
<img width="697" alt="client" src="https://github.com/s-h-im/distributed-system/assets/100345983/220a6d9e-0ed5-44c9-9886-9e8181e6e7c7">
<img width="697" alt="cache" src="https://github.com/s-h-im/distributed-system/assets/100345983/93dd7828-42d4-4d0a-8d3b-1b8ee098beea">
<img width="697" alt="server" src="https://github.com/s-h-im/distributed-system/assets/100345983/7271f892-a223-4f6c-8cb9-068d90f239b8">
</details>


### 4️. Power of two choices 로드 밸런싱 구현
<div>
- random + least connection <br>
- 임의로 두 개의 server를 선택을 한 후, 선택된 두 개의 server 중에서 현재 전송중인 요청의 수 혹은 연결되어 있는 커넥션의 수가 적은 쪽을 선택하는 알고리즘 구현  
</div>
<br>
<details>
<summary>결과 화면</summary>
<img width="697" alt="client" src="https://github.com/s-h-im/distributed-system/assets/100345983/cb72829a-f3b6-4313-80c6-f9d4e666ba9a">
<img width="578" alt="server1" src="https://github.com/s-h-im/distributed-system/assets/100345983/0e4c8b07-d4c3-4d50-abcd-37899cd956f2">
<img width="585" alt="server2" src="https://github.com/s-h-im/distributed-system/assets/100345983/cf236b9e-5952-4eb1-881c-1369d7396d8b">
<img width="592" alt="server3" src="https://github.com/s-h-im/distributed-system/assets/100345983/3537f684-f545-4152-9503-a72b8100de56">
<img width="599" alt="server4" src="https://github.com/s-h-im/distributed-system/assets/100345983/8ca7952d-dc80-4a45-8136-5c196ddb0f5f">
</details>


### 5️. Primary-backup 구현
<div>
- 복제 프로토콜에 대한 이해 <br>
- Primary-backup 프로토콜 구현 <br>
- client는 모든 요청을 primary server에게 전송 <br>
- primary server는 follower server 두 대로부터 쓰기가 완료되었다는 답장을 수신하면, client에게 쓰기 답장을 전송
</div>
<br>
<details>
<summary>결과 화면</summary>
<img width="697" alt="client" src="https://github.com/s-h-im/distributed-system/assets/100345983/4f7d5871-919a-4c8b-9c9c-e3c3668c8f71">
<img width="697" alt="primary" src="https://github.com/s-h-im/distributed-system/assets/100345983/09f9212c-708a-4527-9019-b136bf7d50f0">
<img width="697" alt="backup1" src="https://github.com/s-h-im/distributed-system/assets/100345983/a031ddcf-b614-42ce-8a7b-6396e641cc7e">
<img width="697" alt="backup2" src="https://github.com/s-h-im/distributed-system/assets/100345983/0dd85e7a-edbb-4a0b-be1f-b1194b011290">
</details>


### 6️. Consensus protocol 구현
<div>
- Consensus, Byzantine faults에 대한 이해 <br>
- Consensus 기반의 시스템 프로토콜을 구현 <br>
- client가 모든 serevr로부터 도착한 메시지를 수합해서 2개의 이상의 server가 동일한 값을 return 했는지 여부를 출력 
</div>
<br>
<details>
<summary>결과 화면</summary>
<img width="697" alt="client1" src="https://github.com/s-h-im/distributed-system/assets/100345983/fac47481-7224-4d3e-8e08-c7d9cf2fc1f3">
<img width="697" alt="client2" src="https://github.com/s-h-im/distributed-system/assets/100345983/6997eacb-b826-403c-81f7-c410c1aa61db">
<img width="697" alt="server1" src="https://github.com/s-h-im/distributed-system/assets/100345983/d8648bf2-8efb-4eb8-a453-1fd1df9403b7">
<img width="697" alt="server2" src="https://github.com/s-h-im/distributed-system/assets/100345983/0fbd195e-4e16-473a-bb6d-887a9cc5ece1">
<img width="697" alt="server3" src="https://github.com/s-h-im/distributed-system/assets/100345983/9bd15531-6ca1-4be1-ad54-0cc2c829720d">
</details>
<br>



## ⌨️ How to start

1. 프로젝트를 적절한 작업폴더 (예를 들어 wsl에서는 /mnt/c/hw1)에 위치시킨다.
2. vim과 같은 에디터를 통해 코드를 수정한다.
3. wsl 터미널을 두 번 실행시킨다. (클라이언트와 서버용 터미널이 각각 하나씩 필요하므로)  
  a. vmware나 virtual box환경에서는 그냥 terminal을 두개 띄우면 된다.
4. `./make`를 입력하여 수정사항 반영하여 컴파일함 컴파일이 안되면 apt-get `install build-essential`로 기본패키지 설치 후 재시도한다.
5. 서버 터미널에서 `./server 5001`로 서버 실행한다. (5001은 포트번호 예시임)
6. 클라이언트 터미널에서 `./client 5001`로 클라이언트를 실행한다.
7. 결과가 제대로 나오지 않는다면 다시 코드 수정 후 `make`로 재컴파일하고 다시 반복한다. 
