# Component 디렉터리 발표 요약

## 개요

`Component` 디렉터리는 퍼즐 오브젝트, 함정, 보이스챗, UI 알림, 코스메틱 연출처럼 액터에 부착되어 동작하는 기능 단위를 관리합니다.
대부분의 컴포넌트는 GameMode, GameState, PlayerState와 연결되어 서버 권한 판정과 클라이언트 UI/연출 동기화를 분리합니다.

## 상호작용 컴포넌트

### InteractionSwitchComponent - 원빈

- 플레이어가 직접 상호작용하는 기본 스위치 컴포넌트입니다.
- 활성화 상태를 Replication으로 동기화하고, GameMode에 등록되어 모든 스위치가 켜졌는지 확인하는 데 사용됩니다.
- 자동 비활성화 타이머와 UI 타이머 알림도 처리합니다.

### OverlapSwitchComponent - 원빈

- 플레이어가 영역 안에 들어오면 활성화되는 오버랩 기반 스위치입니다.
- 서버에서 오버랩 수를 관리하고, `bIsOverlapped` 상태를 복제해 다른 시스템이 반응할 수 있게 합니다.

### S5_InteractionGimmickComponent - 명현

- Stage 5 전용 상호작용 기믹입니다.
- GameMode가 진짜/가짜 기믹 여부를 지정하며, 실제 기믹과 상호작용하면 Screen Player 스폰 조건에 반영됩니다.
- 플레이어가 바라보는 방향과 오버랩 상태를 기준으로 UI 표시 가능 여부를 판단합니다.

### FinalTreasureComponent - 민웅

- 엔딩 스테이지의 최종 보물 상호작용 컴포넌트입니다.
- 플레이어가 상호작용하면 `PS3GameModeEnd`에 등록하고, 필요한 인원이 모두 상호작용하면 엔딩 영상 레벨로 이동합니다.

## 함정 및 페널티 컴포넌트

### RandomCollisionTrapComponent - 경환

- Stage 2의 랜덤 충돌 함정입니다.
- GameMode가 생성한 랜덤 배치 결과에 따라 충돌 여부를 적용하고, 상태를 복제합니다.
- 가짜 플랫폼과 겹쳤을 때 이벤트를 발생시켜 연출이나 사망 처리와 연결됩니다.

### FakeDeathTrapComponent - 경환

- Stage 3의 플레이어별 가짜 사망 함정 표시를 담당합니다.
- GameMode가 플레이어별로 계산한 표시 결과를 받아 로컬 클라이언트에서만 보이거나 숨기게 합니다.
- 오버랩 이벤트를 통해 코스메틱 연출과 연결됩니다.

### OverlapVolumeDeathComponent - 경환

- 플레이어가 들어오면 생명을 차감하는 데스 볼륨입니다.
- 같은 Pawn이 중복 처리되지 않도록 가드 목록을 관리합니다.
- 생명 차감 성공 시 델리게이트를 발생시켜 리스폰이나 후속 처리를 연결합니다.

### OverlapVolumeTimeDeductionComponent - 명현

- Stage 5의 가짜 기믹 페널티 영역입니다.
- 가짜 기믹에 닿으면 제한 시간을 차감하고, 플레이어를 리스폰시킵니다.
- 차감된 시간은 UI 타이머 알림으로 표시됩니다.

## Blocking Volume 컴포넌트

### DisableBlockingVolumeComponent - 민웅

- Stage 1에서 특정 플레이어가 오버랩하면 GameMode에 Blocking Volume 해제를 요청합니다.
- 서버 권한에서 한 번만 처리되도록 오버랩 여부를 기록합니다.

### Stage1BlockingVolumeComponent - 민웅

- Stage 1의 진행 차단 영역입니다.
- GameState의 Blocking Volume 비활성화 상태를 구독해 충돌을 켜고 끕니다.
- 아직 조건을 만족하지 못한 상태에서 부딪히면 안내 텍스트 UI를 표시합니다.

### Stage3BlockingVolumeComponent - 민웅

- Stage 3의 진행 차단 영역입니다.
- 모든 플레이어가 무전기 조건을 만족해 보이스챗이 활성화되면 충돌을 해제합니다.
- 차단 중 충돌 시 안내 텍스트 UI를 표시합니다.

### DoorBlockingVolumeComponent - 민웅

- 문이 완전히 열리기 전까지 플레이어 이동을 막는 보조 콜리전입니다.
- 문 열림 상태에 따라 Blocking Collision을 켜거나 끕니다.

## 보이스챗 컴포넌트

### CustomVoiceComponent - 준현

- 플레이어 입력과 게임 상태를 연결하는 보이스챗 제어 컴포넌트입니다.
- Push-To-Talk, 마이크 음소거, 보이스 오브젝트 획득/해제 상태를 관리합니다.
- PlayerState의 보이스 상태와 `VoicePluginControlComponent`를 연결해 실제 송신 여부를 결정합니다.

### VoicePluginControlComponent - 준현

- EOS Voice Chat 플러그인과 직접 통신하는 컴포넌트입니다.
- EOS 로그인 사용자 기준으로 VoiceChatUser를 초기화하고, 채널 참가/이탈/수신 상태를 관리합니다.
- 송신 활성화, 마이크 음소거, 보이스 변조 상태를 실제 플러그인에 적용합니다.

## 연출 컴포넌트

### CosmeticComponent - 현준

- 스위치, 함정, 문, 저울 판정 등 여러 게임 이벤트에 반응해 시각/청각 연출을 실행합니다.
- 라이트, 연기, 사운드, 문 투명도 변화 같은 효과를 관리합니다.
- 여러 컴포넌트의 델리게이트에 바인딩되어 게임 로직과 연출을 분리합니다.

## 발표용 핵심 정리

`Component` 계층은 액터에 붙는 실제 기능 단위입니다.
상호작용, 함정, 차단 영역, 보이스챗, 코스메틱 연출을 컴포넌트로 분리해 재사용성을 높였고, GameMode/GameState와 연결해 서버 판정과 클라이언트 표현을 나누는 구조로 설계되어 있습니다.
