
#include "PS3GameModeS3.h"

#include "Core/GameState/PS3GameStateS3.h"
#include "GameFramework/GameStateBase.h"
#include "Player/PlayerState/PS3PlayerState.h"


void APS3GameModeS3::BeginPlay()
{
	Super::BeginPlay();

	MakeRandomVisibleResults();
	
	//스테이지 시작 시 모든 플레이어의 기본 보이스챗 비활성화
	SetAllPlayersVoiceChatState(EVoiceChatState::Inactive);
	
	CheckVoiceObjectHeldPlayerCount();
}

void APS3GameModeS3::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (!IsValid(NewPlayer)) return;
	
	APS3PlayerState* PS = NewPlayer->GetPlayerState<APS3PlayerState>();
		
	ApplyVoiceChatStateToPlayer(PS);
	
}


//CustomVoiceComponent에서 델리게이트 방송해줘야함 OnVoiceObjectHeld() -> 플레이어와 상호작용 시
//CustomVoiceComponent에서 비긴플레이에 자기 자신을 등록해줘야함 -> InteractionSiwtchCompo 참고
//필요없음
//무전기 상호작용 성공 시 델리게이트 대신 이 함수호출로 바꿔주세요

void APS3GameModeS3::NotifyVoiceObjectHeldStateChanged()
{
	if (!HasAuthority()) return;
	
	CheckVoiceObjectHeldPlayerCount();
}

//플레이어스테이트에 bool IsVoiceObjectHeld = false; 변수 선언 후 setter로 변수 값 제어
//플레이어스테이트에 IsVoiceObjectHeld() getter 함수 필요
//bool IsVoiceObjectHeld = true; 일 경우 CustomVoiceComponent와 상호작용 불가능하도록 설정해야함
void APS3GameModeS3::CheckVoiceObjectHeldPlayerCount()
{
	int32 VoiceObjectHeldPlayerCount = 0;
	
	if (!IsValid(GameState)) return;
	
	//GameState = Unreal의 AGameModeBase가 기본으로 가지고 있는 현재 게임 상태 객체
	//PlayerArray = AGameStateBase 안에 이미 있는 플레이어 목록
	//반드시 프로젝트에서 APS3PlayerState를 실제 PlayerState 클래스로 쓰도록 설정 되어야 함
	for (APlayerState* PlayerState : GameState->PlayerArray) 
	{
		APS3PlayerState* PS3PlayerState = Cast<APS3PlayerState>(PlayerState);
		if (!IsValid(PS3PlayerState)) continue;
	
		if (PS3PlayerState->IsVoiceObjectHeld())
		{
			VoiceObjectHeldPlayerCount++;
		}
	}
	
	ChangeVoiceChatStateAndBlockingVolumeCollision(VoiceObjectHeldPlayerCount);
}

void APS3GameModeS3::ChangeVoiceChatStateAndBlockingVolumeCollision(int32 Count)
{
	if (Count < 2) return;
	
	APS3GameStateS3* GS = GetGameState<APS3GameStateS3>();
	if (!IsValid(GS)) return;
	
	if (GS->IsStage3VoiceChatActivated()) return;
	
	GS->SetStage3VoiceChatActivated(true);
	GS->SetStage3BlockingVolumeDisabled(true);
	
	//모든 플레이어의 보이스챗 상태를 Conversion으로 변경
	SetAllPlayersVoiceChatState(EVoiceChatState::Conversion);
}

//플레이어는 상호작용 성공 시 Enum Class VoiceChatstate의 상태 적용 필요
//플레이어스테이트에 SetVoiceChatState() 함수 필요
void APS3GameModeS3::SetAllPlayersVoiceChatState(EVoiceChatState NewState)
{
	if (!IsValid(GameState)) return;
	
	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		APS3PlayerState* PS3PlayerState = Cast<APS3PlayerState>(PlayerState);
		if (!IsValid(PS3PlayerState)) continue;

		
		PS3PlayerState->SetVoiceChatState(NewState);
	}
}

void APS3GameModeS3::ApplyVoiceChatStateToPlayer(APS3PlayerState* PlayerState)
{
	if (!IsValid(PlayerState)) return;
	
	const APS3GameStateS3* GS = GetGameState<APS3GameStateS3>();
	const bool bActivated = IsValid(GS) && GS->IsStage3VoiceChatActivated();
	
	PlayerState->SetVoiceChatState(
		bActivated ? EVoiceChatState::Conversion : EVoiceChatState::Inactive);
}

void APS3GameModeS3::MakeRandomVisibleResults()
{
	RandomVisibleResults = FMath::RandBool();
}

bool APS3GameModeS3::GetRandomVisibleResults()
{
	return RandomVisibleResults;
}



	




