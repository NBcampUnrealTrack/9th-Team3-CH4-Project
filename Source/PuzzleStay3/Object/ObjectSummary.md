# Object 디렉터리 발표 요약

## 개요

`Object` 디렉터리는 레벨에 배치되는 주요 게임 오브젝트를 관리합니다.
문, 기믹, 저울 퍼즐, 덤벨, 함정, 카메라, PlayerStart처럼 실제 월드 안에서 플레이어와 상호작용하는 액터들이 포함되어 있습니다.

## 문 오브젝트

### Door - 원빈

- 스테이지 전반에서 사용하는 일반 문 액터입니다.
- `DoorID`와 스위치의 `SwitchID`를 기준으로 연결된 스위치를 자동 탐색하고, 모든 조건이 만족되면 문을 엽니다.
- 최종 탈출문이나 Stage 4 첫 번째 문처럼 GameState 이벤트에 직접 반응하는 문도 처리합니다.
- 문 열림 상태는 Replication으로 동기화되며, `DoorBlockingVolumeComponent`와 연결해 문이 완전히 열리기 전까지 이동을 막습니다.

### ControlDoor - 명현

- Stage 5에서 버튼 타입과 대응되는 컨트롤 문입니다.
- `Door_A`, `Door_B`, `Door_C`, `Door_D` 중 하나의 타입을 가지며, 랜덤 타입 지정도 지원합니다.
- 문 타입에 따라 Decal을 표시하고, Screen Player가 조작한 버튼 타입이 문 타입과 맞으면 Timeline으로 문을 여닫습니다.
- Screen Player 스폰 이후 문 상태 변화와 연출을 Multicast RPC로 동기화합니다.

## 퍼즐 오브젝트

### GimmickBase - 원빈

- 기믹 액터들의 공통 기반 클래스입니다.
- 활성화 여부와 완료 여부를 Replication으로 관리합니다.
- 실제 상호작용 결과는 Blueprint 이벤트로 확장할 수 있도록 열어둔 구조입니다.
- Stage 5에서는 GameMode가 월드의 `GimmickBase`를 수집해 진짜/가짜 기믹을 섞는 데 사용합니다.

### Jeoul - 원빈

- Stage 4 저울 퍼즐의 핵심 액터입니다.
- 플레이어와 덤벨의 무게를 계산해 목표 무게와 비교합니다.
- 성공/실패 결과에 따라 기울기 애니메이션, 컷씬, UI 알림을 Multicast로 전달합니다.
- 성공 시 GameMode에 결과를 알리고 Stage 4 첫 번째 문 개방으로 이어집니다.

### Dumbbell - 원빈

- Stage 4 저울 퍼즐에 사용되는 무게 오브젝트입니다.
- `Heavy`, `Medium`, `Light` 타입에 따라 무게가 달라집니다.
- 플레이어가 집거나 내려놓을 수 있으며, 들고 있는 플레이어 정보가 Replication으로 동기화됩니다.
- 상호작용 가능 여부에 따라 UI 안내를 표시합니다.

## 함정 오브젝트

### RealDeathTrap - 경환

- 실제 사망 함정 액터입니다.
- 플레이어가 Collision에 들어오면 한 번만 트리거되고, 델리게이트로 후속 사망 처리를 연결합니다.
- 함정 로직과 외부 처리 로직을 분리하기 위한 이벤트 기반 구조입니다.

## 보조 오브젝트

### PS3PlayerStartBase - 명현

- 플레이어 시작 위치에 타입을 부여하는 PlayerStart 확장 클래스입니다.
- `Player1`, `Player2`, `FieldPlayer`, `ScreenPlayer` 등 역할별 스폰 위치 선택에 사용됩니다.
- GameMode의 `FindPlayerStart` 로직과 연결됩니다.

### PS3CameraActor - 명현

- 프로젝트 전용 카메라 액터입니다.
- 기본 `ACameraActor`를 상속한 래퍼 역할이며, 스테이지나 연출용 카메라를 프로젝트 타입으로 구분할 수 있게 합니다.

## 발표용 핵심 정리

`Object` 계층은 플레이어가 실제로 보고 상호작용하는 월드 액터를 담당합니다.
문과 기믹은 GameMode/GameState와 연결되어 서버 권한으로 상태를 결정하고, Replication과 Multicast를 통해 모든 클라이언트에 같은 결과와 연출을 전달합니다.
