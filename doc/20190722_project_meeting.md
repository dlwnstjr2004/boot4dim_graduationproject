# 7/22일 현 진행상황
- 자이로 센서 기초 코드를 성공
  - 값이 들어는 오지만 0으로 모든 기초코드가 실행될경우 SDA,SCL납땜 상태를 확인하는 것이 좋음
  - 납땜을 다시 함으로써 확인
- 외형 관련 회의를 시작
  - 1안. 3D프린트로 프린트하기
  - 2안. 해외 직구로 현재 있는 건틀렛을 구매
- 근전도 센서 값 확인
  - 근전도 센서도 값도 결국 ADC 값 이므로 똑같이 확인 가능
  - SIG => ADC핀
  - VCC => 3.3V
  - GND => GND
- Flex 센서 3개를 장갑에 넣어서 아두이노 코드를 만듬

# 7/29일 예정상황
- 꾸준히 하드웨어 설계 진행중
  - 외형이 정해지지 않아서 다른 것만 합치는 중
- esp32 BLE HID연결
- 자이로 센서 적분 계산을 코드로 구현


# 프로젝트 오류해결
- 보드 esp32 Dev module 컴파일 에러 <- 이런 에러가 뜬다면 설정이 안료가 안된 것이므로 처음부터 다시 설정할 것!
  - http://www.hardcopyworld.com/ngine/aduino/index.php/archives/3008