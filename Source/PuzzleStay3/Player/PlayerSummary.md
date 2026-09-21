# Player 디렉터리 발표 요약

## 개요

`Player` 디렉터리는 플레이어 입력, 캐릭터 조작, 역할 선택, 온라인 로비, 생명/상태 동기화를 담당합니다.
플레이어가 실제로 조작하는 `Character`, 입력을 처리하는 `Controller`, 네트워크 상태를 저장하는 `PlayerState`, EOS 로비를 관리하는 `LobbySubsystem`으로 구성됩니다.

## Character

### PS3PlayerCharacter

- 플레이어가 조작하는 기본 캐릭터입니다.
- 이동, 시점 회전, 점프, 상호작용, 들고 있는 오브젝트 드롭을 처리합니다.
- 상호작용은 서버 RPC로 실행되며, 캐릭터 앞쪽의 Sphere Trace/Overlap을 통해 `PS3InteractableInterface` 대상 액터나 컴포넌트를 찾습니다.
- `PlayerIdentity`가 변경되면 Blueprint 이벤트로 외형을 갱신합니다.
- Stage 5에서는 Field Player 조작 가능 여부를 `bCanUseFieldControls`로 제어합니다.

## Controller

### PS3PlayerControllerBase

- 모든 플레이어 컨트롤러의 공통 기반 클래스입니다.
- HUD ViewModel 바인딩, 옵션 메뉴 입력, 게임 재시작/타이틀 이동 요청, 사운드 설정 변경을 처리합니다.
- 개발용 치트 명령도 포함되어 있어 다음 스테이지 이동, 현재 스테이지 재시작, 퍼즐 정답 확인 등에 사용됩니다.

### PS3PlayerController

- 일반 Field Player용 컨트롤러입니다.
- 이동, 시점, 점프, 상호작용, 드롭, Push-To-Talk 입력을 Enhanced Input으로 처리합니다.
- `CustomVoiceComponent`와 `VoicePluginControlComponent`를 보유해 보이스챗 입력과 EOS 음성 시스템을 연결합니다.
- Stage 3의 플레이어별 가짜 함정 표시 결과를 클라이언트에 적용합니다.
- Stage 4 저울 컷씬에서 카메라 전환, 캐릭터 회전, 컷씬 종료 복구를 담당합니다.
- 리스폰 시 입력 상태와 Push-To-Talk 상태를 정리합니다.

### PS3ChoiceController

- Stage 5 시작 전 역할 선택 화면에서 사용하는 컨트롤러입니다.
- 고정 카메라를 사용하고 UI 전용 입력 모드로 전환합니다.
- Field Player 또는 Screen Player 역할 선택을 서버에 전달합니다.
- 모든 플레이어가 역할을 선택하면 Stage 5 게임 시작 타이머가 진행됩니다.

### PS3ScreenPlayerController

- Stage 5의 Screen Player용 컨트롤러입니다.
- A/B/C/D 버튼 입력으로 `ControlDoor`를 열고 닫습니다.
- 문 조작 요청은 서버 RPC로 전달되어 모든 클라이언트에 문 상태가 동기화됩니다.
- Screen Player 전용 카메라 시점과 UI 표시를 관리합니다.

## PlayerState

### PS3PlayerState

- 플레이어의 네트워크 동기화 상태를 저장합니다.
- 생명 수, 사망 여부, 역할, Player1/Player2 식별자, 보이스챗 상태, 무전기 보유 여부, 상호작용 상태를 Replication으로 관리합니다.
- 생명 차감 시 남은 생명이 있으면 리스폰 요청 델리게이트를 발생시키고, 0이 되면 사망 상태로 전환합니다.
- 각 상태 변경은 OnRep와 델리게이트를 통해 UI, GameMode, Character에 전달됩니다.

## Interaction

### PS3InteractableInterface

- 플레이어가 상호작용할 수 있는 대상의 공통 인터페이스입니다.
- `CanInteract`로 상호작용 가능 여부를 확인하고, `Interact`로 실제 상호작용을 실행합니다.
- Character, 기믹, 덤벨, 보물 등 서로 다른 오브젝트를 동일한 방식으로 처리하게 해줍니다.

## Online

### PS3LobbySubsystem

- EOS 기반 로비 기능을 담당하는 GameInstanceSubsystem입니다.
- 로그인, 로비 생성, 로비 검색, 로비 참가, 로비 나가기를 처리합니다.
- 로비 생성 성공 시 WaitingRoom을 Listen Server로 열고, 참가자는 JoinSession 결과로 받은 주소로 ClientTravel합니다.
- 로비 생성/참가 전 로컬 보이스를 초기화해 EOS 로비 음성과 연결합니다.
- 로그인, 방 생성, 방 참가 상황에 맞는 UI 안내 문구도 표시합니다.

## 발표용 핵심 정리

`Player` 계층은 플레이어 입력부터 네트워크 상태, 역할 선택, 보이스챗, 로비 접속까지 담당하는 플레이어 중심 시스템입니다.
`Controller`는 입력과 UI 요청을 처리하고, `Character`는 실제 월드 조작을 수행하며, `PlayerState`는 서버와 클라이언트가 공유해야 하는 플레이어 상태를 복제합니다.
