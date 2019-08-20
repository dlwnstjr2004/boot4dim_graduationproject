# 8/19일 현 진행상황
- esp32 ble hid 가 컴퓨터로 직접 연결하는 것은 포기
  - 수신부를 만들기 위해 HM-10 BLUETOOTH BLE 모듈과 aruduino leonardo을 구매
  - arduino leonardo 보드를 이용하여 컴퓨터의 마우스를 이동
  - HM-10 모듈을 BAUD = 115200 , NAME = BOOT4DIM으로 변경
  - HM-10 모듈과 esp32 보드가 auto connect 를 할 수 있도록 esp32 client의   UUID를 HM-10 모듈의 UUID와 맞춰주면 auto connect가능하도록 바뀜.
- 자이로 센서를 통한 미세값 조정중
- Reset 버튼을 제작(송신부, 수신부)

# 8/26일 예정상황
- Restandard 함수 조정
- timer 함수 제작
- 수신부 코딩 완료
- 하드웨어 설계 마무리