# Core 디렉터리 발표 요약

## 개요

`Core` 디렉터리는 게임의 전체 흐름을 제어하는 중심 계층입니다.
`GameInstance`는 맵 이동 후에도 유지해야 하는 플레이어 정보를 저장하고, `GameMode`는 서버 권한의 게임 규칙과 레벨 이동을 처리하며, `GameState`는 클라이언트와 공유해야 하는 상태와 UI 알림을 담당합니다.

## GameInstance

### PS3GameInstance

- 맵 이동 후에도 유지되어야 하는 플레이어 식별 정보를 캐싱합니다.
- 플레이어의 UniqueId 또는 PlayerName을 Key로 사용해 `Player1`, `Player2` 같은 `PlayerIdentity`를 저장합니다.
- 스테이지가 바뀌어도 같은 플레이어가 같은 시작 위치와 역할 흐름을 유지할 수 있게 합니다.

## GameMode

### PS3GameModeBase

- 모든 스테이지 GameMode의 공통 기반 클래스입니다.
- 플레이어 접속 시 `PlayerState`에 `PlayerIdentity`를 부여하고, 해당 Identity에 맞는 `PlayerStart`를 찾습니다.
- `InteractionSwitchComponent`를 등록하고, 모든 스위치가 활성화되면 탈출문 상태를 열고 스테이지 클리어 타이머를 시작합니다.
- 플레이어 사망 상태를 구독해 스테이지별로 게임오버 또는 재시작 흐름을 처리합니다.
- 현재 스테이지 재시작, 다음 스테이지 이동, 타이틀 이동 같은 공통 레벨 이동 기능을 제공합니다.

### PS3GameModeS1

- Stage 1 전용 GameMode입니다.
- 인터랙션 스위치 타이머 사용을 활성화합니다.
- 특정 오버랩 조건이 만족되면 Stage 1 Blocking Volume을 비활성화하도록 GameState에 요청합니다.
- Stage 1 데이터 에셋에서 다음 스테이지 레벨 경로를 읽어옵니다.

### PS3GameModeS2

- Stage 2 전용 GameMode입니다.
- 랜덤 충돌 함정 배치를 생성하고, 등록된 `RandomCollisionTrapComponent`에 충돌 상태를 적용합니다.
- 플레이어가 생명을 잃고 리스폰해야 할 때 기존 Pawn을 제거하고 새 Pawn을 스폰합니다.
- 플레이어 사망 시 스테이지 재시작/게임오버 흐름을 사용하는 스테이지입니다.

### PS3GameModeS3

- Stage 3 전용 GameMode입니다.
- 시작 시 모든 플레이어의 보이스챗 상태를 비활성화합니다.
- 모든 플레이어가 무전기 오브젝트를 획득하면 보이스챗 상태를 `Conversion`으로 바꾸고 Blocking Volume을 해제합니다.
- Stage 3의 가짜 함정 표시 여부를 플레이어별로 랜덤 생성하고, 각 플레이어 컨트롤러에 개별 결과를 전달합니다.
- 플레이어 사망 시 게임오버 흐름을 사용하는 스테이지입니다.

### PS3GameModeS4

- Stage 4 저울 퍼즐 전용 GameMode입니다.
- 데이터 에셋의 후보 무게 중 하나를 랜덤 선택해 고정 오브젝트 무게를 정합니다.
- 플레이어 두 명의 무게를 제외한 목표 덤벨 합계를 계산해 GameState에 복제합니다.
- 저울 판정 성공 시 Stage 4 첫 번째 문을 열도록 GameState에 요청합니다.

### PS3GameModeS5

- Stage 5 역할 분리 퍼즐 전용 GameMode입니다.
- Field Player와 Screen Player 역할 선택을 처리하고, 선택된 역할에 맞는 Controller와 Pawn을 스폰합니다.
- 월드의 `GimmickBase`들을 수집해 진짜 기믹과 가짜 기믹을 랜덤으로 구성합니다.
- 진짜 기믹 조건이 충족되면 Screen Player를 실제 캐릭터 조작 상태로 전환합니다.
- 게임 시작 타이머, 제한 시간 감소, 시간 차감, 게임오버 처리를 관리합니다.
- Screen Player가 조작하는 문, 탈출 기믹, 스폰 조건을 중앙에서 관리합니다.

### PS3GameModeEnd

- 엔딩 스테이지 전용 GameMode입니다.
- 최종 보물과 상호작용한 플레이어를 기록합니다.
- 필요한 인원 수만큼 상호작용하면 엔딩 영상 레벨로 이동합니다.
- 중복 상호작용과 이미 엔딩이 트리거된 상태를 방지합니다.

## GameState

### PS3GameStateBase

- 모든 스테이지 GameState의 공통 기반 클래스입니다.
- 탈출문 개방 여부를 Replication으로 동기화합니다.
- 탈출문 상태가 바뀌면 `OnEscapeDoorOpened` 델리게이트를 방송해 Door가 반응하게 합니다.
- UI 표시 딜레이 같은 공통 GameRule 데이터를 초기화합니다.

### PS3GameStateLobby

- 로비 화면 안내 텍스트를 표시하는 GameState입니다.
- 최초 진입, 로그인 성공, 이미 로그인 상태, 방 생성, 방 참가 상황에 맞는 텍스트 알림을 처리합니다.
- PIE 환경에서 UI 생성 타이밍 문제가 생기지 않도록 로컬 `PlayerHUD`를 직접 찾아 텍스트 알림을 표시합니다.
- HUD가 아직 준비되지 않았으면 짧은 간격으로 재시도합니다.

### PS3GameStateS1

- Stage 1 전용 GameState입니다.
- Stage 1 UI 타입, 시작 안내 텍스트, 튜토리얼 UI 표시를 담당합니다.
- Stage 1 Blocking Volume 비활성화 상태를 Replication으로 동기화합니다.
- 상태가 바뀌면 Blocking Volume 컴포넌트가 충돌을 해제할 수 있도록 델리게이트를 방송합니다.

### PS3GameStateS2

- Stage 2 전용 GameState입니다.
- Stage 2 UI 타입과 시작 안내 텍스트를 표시합니다.
- 플레이어 사망으로 인한 게임오버 상태를 Replication으로 동기화합니다.
- 게임오버 상태가 복제되면 UI에 GameOver 화면 표시를 요청합니다.

### PS3GameStateS3

- Stage 3 전용 GameState입니다.
- Stage 3 UI 타입, 시작 안내 텍스트, 무전기 관련 안내 텍스트를 표시합니다.
- Stage 3 Blocking Volume 해제 상태와 보이스챗 활성화 상태를 Replication으로 동기화합니다.
- 플레이어 사망 게임오버 상태를 복제하고, 클라이언트 UI에 GameOver 표시를 요청합니다.

### PS3GameStateS4

- Stage 4 전용 GameState입니다.
- Stage 4 UI 타입, 시작 안내 텍스트, 무전기 관련 안내 텍스트를 표시합니다.
- 저울 퍼즐에 필요한 고정 무게와 목표 무게를 Replication으로 동기화합니다.
- Stage 4 첫 번째 문 개방 상태를 복제하고, Door가 열릴 수 있도록 델리게이트를 방송합니다.

### PS3GameStateS5

- Stage 5 전용 GameState입니다.
- 제한 시간, 역할 선택 상태, 게임 시작 여부, 게임오버 여부를 Replication으로 동기화합니다.
- 게임 시작 시 Stage 5 UI 알림과 타이머 UI를 표시합니다.
- 제한 시간이 감소하거나 페널티 시간이 차감되면 UI 타이머를 갱신합니다.
- 게임오버 시 입력 모드를 UI 전용으로 바꾸고 GameOver UI를 표시합니다.
- Screen Player 스폰 이후 Field Player UI 재할당, 화살표 표시/숨김 같은 클라이언트 연출을 Multicast RPC로 처리합니다.

### PS3GameStateEnd

- 엔딩 스테이지 전용 GameState입니다.
- 엔딩 스테이지 진입 시 텍스트 알림 UI를 표시합니다.
- 엔딩 전용 데이터 에셋을 검증하고, UI에 필요한 안내 타입을 사용합니다.

## 발표용 핵심 정리

`Core`는 게임 진행의 서버 권한 로직과 클라이언트 공유 상태를 분리한 구조입니다.
`GameMode`는 규칙 판정, 플레이어 스폰, 스테이지 이동을 담당하고, `GameState`는 복제 상태와 UI 이벤트를 담당합니다.
`GameInstance`는 맵 이동 후에도 유지해야 하는 플레이어 식별 정보를 저장해 멀티플레이 흐름을 안정적으로 이어줍니다.
