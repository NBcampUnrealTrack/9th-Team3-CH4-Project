
#include "PS3GameModeS3.h"

#include "Core/GameState/PS3GameStateS3.h"
#include "PuzzleStay3/Component/CustomVoiceComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Player/PlayerState/PS3PlayerState.h"


void APS3GameModeS3::BeginPlay()
{
	Super::BeginPlay();

	//스테이지 시작 시 모든 플레이어의 기본 보이스챗 비활성화
	SetAllPlayersVoiceChatState(EVoiceChatState::Inactive);
}

//CustomVoiceComponent에서 델리게이트 방송해줘야함 OnVoiceObjectHeld() -> 플레이어와 상호작용 시
//CustomVoiceComponent에서 비긴플레이에 자기 자신을 등록해줘야함 -> InteractionSiwtchCompo 참고

void APS3GameModeS3::RegisterCustomVoiceComponent(UCustomVoiceComponent* CustomVoiceComponent)
{
	if (!IsValid(CustomVoiceComponent)) return;

	// CustomVoiceComponent->OnVoiceObjectHeld.AddUObject(
	// 	this,
	// 	&APS3GameModeS3::HandleVoiceObjectHeld
	// );
}

void APS3GameModeS3::HandleVoiceObjectHeld()
{
	CheckVoiceObjectHeldPlayerCount();
}

//플레이어스테이트에 bool IsVoiceObjectHeld = false; 변수 선언 후 setter로 변수 값 제어
//플레이어스테이트에 IsVoiceObjectHeld() getter 함수 필요
//bool IsVoiceObjectHeld = true; 일 경우 CustomVoiceComponent와 상호작용 불가능하도록 설정해야함
void APS3GameModeS3::CheckVoiceObjectHeldPlayerCount()
{
	int32 VoiceObjectHeldPlayerCount = 0;
	
	//GameState = Unreal의 AGameModeBase가 기본으로 가지고 있는 현재 게임 상태 객체
	//PlayerArray = AGameStateBase 안에 이미 있는 플레이어 목록
	//반드시 프로젝트에서 APS3PlayerState를 실제 PlayerState 클래스로 쓰도록 설정 되어야 함
	for (APlayerState* PlayerState : GameState->PlayerArray) 
	{
		APS3PlayerState* PS3PlayerState = Cast<APS3PlayerState>(PlayerState);
		if (!IsValid(PS3PlayerState)) continue;
	
		// if (PS3PlayerState->IsVoiceObjectHeld())
		// {
		// 	VoiceObjectHeldPlayerCount++;
		// }
	}
	
	ChangeVoiceChatStateAndBlockingVolumeCollision(VoiceObjectHeldPlayerCount);
}

void APS3GameModeS3::ChangeVoiceChatStateAndBlockingVolumeCollision(int32 Count)
{
	if (Count < 2) return;
	
	//모든 플레이어의 보이스챗 상태를 Conversion으로 변경
	SetAllPlayersVoiceChatState(EVoiceChatState::Conversion);

	APS3GameStateS3* GS = GetGameState<APS3GameStateS3>();
	if (!IsValid(GS)) return;

	GS->SetStage3BlockingVolumeDisabled(true);
}

//플레이어는 상호작용 성공 시 Enum Class VoiceChatstate의 상태 적용 필요
//플레이어스테이트에 SetVoiceChatState() 함수 필요
void APS3GameModeS3::SetAllPlayersVoiceChatState(EVoiceChatState NewState)
{
	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		APS3PlayerState* PS3PlayerState = Cast<APS3PlayerState>(PlayerState);
		if (!IsValid(PS3PlayerState)) continue;

		
		//PS3PlayerState->SetVoiceChatState(NewState);
	}
}
