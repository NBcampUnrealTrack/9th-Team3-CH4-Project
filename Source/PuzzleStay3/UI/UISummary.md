# UI 디렉터리 발표 요약

## 개요

`UI` 디렉터리는 플레이어 HUD, MVVM ViewModel, 각종 UserWidget을 관리합니다.
게임 로직에서 발생한 이벤트는 `UIDelegatesSubsystem`과 `PS3ViewModel`을 거쳐 `PlayerHUD`로 전달되고, `PlayerHUD`가 실제 Widget을 호출해 화면을 갱신하는 구조입니다.

## HUD

### PlayerHUD

- 전체 UI의 중심 허브 역할을 하는 HUD 클래스입니다.
- `PS3RootHUDWidget`을 생성해 Viewport에 추가하고, 내부 Widget 참조를 수집합니다.
- `PS3ViewModel`과 연결되어 게임 이벤트를 실제 UI 표시 함수로 변환합니다.
- 텍스트 알림, 생명 UI, 상호작용 알림, 타이머, 튜토리얼, 문 버튼, 보이스 아이콘, 옵션, 타이틀, 게임오버, Stage 5 역할 선택 UI를 통합 관리합니다.

## ViewModel

### PS3ViewModel

- MVVM 기반 UI 중간 계층입니다.
- GameState, GameMode, Component에서 브로드캐스트한 UI 델리게이트를 구독합니다.
- 받은 이벤트를 `PlayerHUD` 호출로 변환해 실제 Widget을 갱신합니다.
- 생명 수, 상호작용 표시 여부, 문 잠금 상태 등 Blueprint 바인딩용 FieldNotify 값을 관리합니다.
- UI 버튼 입력도 다시 Controller/GameMode 쪽으로 전달하는 요청 델리게이트를 제공합니다.

## Root Widget

### PS3RootHUDWidget

- HUD에 포함되는 하위 Widget들을 한 곳에서 보관하는 루트 위젯입니다.
- TextNotify, LifeCount, InteractionNotify, TimerNotify, TutorialNotify, DoorOpenButton, VoiceChatIcon, OptionPopup, Title, GameOver, Stage5RoleSelect 위젯 접근자를 제공합니다.

## 알림 UI

### TextNotifyWidget

- 스테이지 안내, 로비 안내, 문 열림/닫힘, 무전기, 저울 결과 같은 텍스트 알림을 표시합니다.
- `EPS3TextNotifyType`에 따라 표시 문구, 폰트 크기, 표시 시간을 결정합니다.
- 알림 표시 시 사운드 재생도 함께 처리합니다.

### InteractionNotifyWidget

- 플레이어가 상호작용 가능한 상황에서 키 안내 이미지를 표시합니다.
- Interact, Drop, Stage 5 상호작용 상태를 카운트 기반으로 관리해 여러 요청이 겹쳐도 UI가 안정적으로 유지됩니다.

### TutorialNotifyWidget

- 튜토리얼 안내 UI를 일정 시간 동안 표시합니다.
- 표시 후 타이머가 끝나면 자동으로 숨깁니다.

## 상태 표시 UI

### LifeCountWidget

- 플레이어의 남은 생명 수를 하트 이미지로 표시합니다.
- 최대 생명 수에 맞춰 하트 이미지를 재구성하고, 현재 생명 수에 따라 채워진 하트와 빈 하트를 갱신합니다.

### TimerNotifyWidget

- 여러 개의 타이머 알림을 동시에 관리하는 컨테이너 위젯입니다.
- 같은 TimerId가 들어오면 기존 타이머를 갱신하고, 새 TimerId는 Entry Widget을 생성합니다.
- 모든 타이머가 끝나면 자동으로 숨겨집니다.

### TimerNotifyEntryWidget

- 개별 타이머 하나를 표시하는 위젯입니다.
- 남은 시간 비율을 Dynamic Material로 표현하고, 시간이 끝나면 종료 이벤트를 발생시킵니다.
- Stage 5 제한 시간, 기믹 타이머, 시간 차감 알림 등에 사용됩니다.

### VoiceChatIconWidget

- 플레이어가 말하고 있는지 여부를 표시하는 보이스챗 아이콘 위젯입니다.
- speaking 상태에 따라 표시/숨김을 전환합니다.

## 조작 UI

### DoorOpenButtonWidget

- Stage 5 Screen Player가 문 A/B/C/D를 조작하는 버튼 UI입니다.
- 버튼 Press/Release를 DoorIndex와 bool 값으로 변환해 HUD/ViewModel로 전달합니다.
- 문 잠금 상태와 누름 상태 필터 이미지를 갱신합니다.

### Stage5RoleSelectWidget

- Stage 5 시작 전 Field Player와 Screen Player 역할을 선택하는 UI입니다.
- 버튼 클릭 결과를 `EPS3PlayerRole`로 변환해 HUD/ViewModel에 전달합니다.

## 메뉴 UI

### TitleWidget

- 타이틀 화면의 게임 시작, 옵션, 게임 종료 버튼을 관리합니다.
- 버튼 입력을 HUD를 통해 ViewModel 요청으로 전달합니다.

### OptionPopupWidget

- 옵션 팝업 UI입니다.
- BGM/SFX 볼륨, 보이스챗 활성화, 해상도 설정, 닫기 버튼을 처리합니다.
- 변경 값은 HUD/ViewModel을 통해 PlayerController 쪽 설정 처리로 전달됩니다.

### GameOverWidget

- 게임오버 화면 UI입니다.
- 재시작과 타이틀로 나가기 버튼을 제공하며, 요청을 HUD/ViewModel로 전달합니다.

### EndingVideoWidget

- 엔딩 영상 재생을 담당하는 위젯입니다.
- MediaPlayer와 MediaSource를 사용하고, 영상 종료 이벤트를 처리합니다.

## 유틸리티

### PS3UIBlueprintFunctionLibrary

- UI Blueprint에서 사용할 수 있는 보조 함수 모음입니다.
- 현재는 bool 값을 `ESlateVisibility`로 변환하는 함수를 제공합니다.

## 발표용 핵심 정리

`UI` 계층은 게임 로직과 화면 표시를 분리하기 위해 HUD와 ViewModel을 중심으로 구성되어 있습니다.
게임 상태 변화는 ViewModel로 모이고, ViewModel은 PlayerHUD를 통해 각 Widget을 갱신합니다.
이 구조 덕분에 게임 로직은 UI 세부 구현을 직접 알 필요 없이 델리게이트와 요청 함수만으로 화면을 제어할 수 있습니다.
