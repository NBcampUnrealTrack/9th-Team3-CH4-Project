# CheatManager 디렉터리 발표 요약

## 개요

`CheatManager` 디렉터리는 개발 및 테스트 중 스테이지 진행과 퍼즐 정답 확인을 빠르게 수행하기 위한 치트 명령을 담당합니다.
Unreal의 `UCheatManager`를 상속한 `UPS3CheatManager`가 콘솔 Exec 명령을 제공하고, `PS3PlayerControllerBase`는 같은 기능을 단축키와 서버 RPC로 실행할 수 있게 연결합니다.

## 주요 클래스

### PS3CheatManager

- `UCheatManager` 기반의 프로젝트 전용 치트 매니저입니다.
- `UFUNCTION(Exec)` 명령으로 콘솔에서 치트 기능을 호출할 수 있습니다.
- GameMode 또는 GameState를 조회해 스테이지 이동, 재시작, 퍼즐 디버그 정보를 출력합니다.
- 실행 결과는 `UE_LOG`와 화면 디버그 메시지로 함께 표시합니다.

### PS3PlayerControllerBase 연동

- 생성자에서 `CheatClass = UPS3CheatManager::StaticClass()`로 프로젝트 전용 CheatManager를 등록합니다.
- `InputKey`를 오버라이드해 `Ctrl + Alt + 숫자` 단축키를 처리합니다.
- 클라이언트에서 누른 단축키는 `ServerRPC_RunShippingCheatCommand`를 통해 서버 권한으로 실행됩니다.
- 패키징 환경에서도 테스트 명령을 사용할 수 있도록 CheatManager 기능을 Controller 쪽에 한 번 더 연결해 둔 구조입니다.

## 치트 명령 목록

| 단축키 | Exec 명령 | 기능 |
| --- | --- | --- |
| `Ctrl + Alt + 1` | `PS3NextStage` | 현재 스테이지를 클리어 처리하고 다음 스테이지로 이동 |
| `Ctrl + Alt + 2` | `PS3RestartStage` | 현재 스테이지 재시작 |
| `Ctrl + Alt + 3` | `PS3ShowStage2Layout` | Stage 2 랜덤 충돌 발판 배열 출력 |
| `Ctrl + Alt + 4` | `PS3ShowStage4WeightAnswer` | Stage 4 저울 퍼즐의 고정 무게와 목표 덤벨 합계 출력 |
| `Ctrl + Alt + 5` | `PS3ShowStage5RealSwitches` | Stage 5의 진짜 interaction switch 번호 출력 |

## 스테이지별 활용

### 공통 스테이지 진행

- `PS3NextStage`는 `APS3GameModeBase::GoToNextStageForCheat()`를 호출합니다.
- 내부적으로 기존 스테이지 클리어 로직인 `StageClear()`를 사용하므로 실제 진행 흐름과 같은 방식으로 레벨 이동이 일어납니다.
- `PS3RestartStage`는 `APS3GameModeBase::StageRestart()`를 호출해 현재 레벨을 다시 로드합니다.

### Stage 2

- `APS3GameModeS2::GetRandomCollisionLayoutResults()`에서 랜덤 충돌 배열을 가져옵니다.
- 각 인덱스를 `BlockAll` 또는 `NoCollision`으로 변환해 화면에 표시합니다.
- 랜덤 발판 퍼즐의 현재 정답/오답 배치를 빠르게 확인할 수 있습니다.

### Stage 4

- `APS3GameStateS4`의 `GetFixedObjectWeight()`와 `GetTargetBalancedWeight()`를 읽습니다.
- 저울 퍼즐에서 고정 오브젝트 무게와 맞춰야 하는 덤벨 합계를 바로 확인할 수 있습니다.
- GameState 값을 사용하므로 클라이언트에 공유되는 퍼즐 상태를 기준으로 확인합니다.

### Stage 5

- `APS3GameModeS5::GetRealInteractionGimmickLabelsForCheat()`를 호출합니다.
- 등록된 기믹 중 실제 정답 interaction switch만 골라 태그 또는 이름 기준으로 출력합니다.
- Screen Player와 Field Player가 맞춰야 하는 진짜 스위치를 빠르게 검증할 수 있습니다.

## 발표용 핵심 정리

`CheatManager`는 반복 테스트 시간을 줄이기 위한 개발 보조 시스템입니다.
스테이지 이동과 재시작은 기존 GameMode 흐름을 그대로 호출하고, 퍼즐 정답 확인은 각 스테이지의 GameMode/GameState에서 필요한 상태만 읽어옵니다.
또한 콘솔 Exec 명령과 `Ctrl + Alt + 숫자` 단축키를 모두 지원해 에디터와 실행 빌드 양쪽에서 테스트 편의성을 제공합니다.
