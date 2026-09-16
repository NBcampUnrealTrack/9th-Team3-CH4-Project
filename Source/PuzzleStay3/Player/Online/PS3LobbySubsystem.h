#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/Enum/PS3TextNotifyType.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "PS3LobbySubsystem.generated.h"


USTRUCT(BlueprintType)
struct FPS3LobbyInfo
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) FString OwnerName;
	UPROPERTY(BlueprintReadOnly) int32 OpenSlots = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPS3LobbyOperationCompleted, FName, Operation, bool, bSuccess, const FString&, Message);

// EOS 로비/음성 연결 및 생성 성공 후 호스트 대기방 이동을 처리합니다.
UCLASS()
class PUZZLESTAY3_API UPS3LobbySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Deinitialize() override;
	UFUNCTION(BlueprintCallable, Category="PS3|EOS") void Login();
	UFUNCTION(BlueprintCallable, Category="PS3|EOS") void CreateLobby();
	UFUNCTION(BlueprintCallable, Category="PS3|EOS") void FindLobbies();
	UFUNCTION(BlueprintCallable, Category="PS3|EOS") void JoinLobby(int32 ResultIndex);
	UFUNCTION(BlueprintCallable, Category="PS3|EOS") void LeaveLobby();
	// 로그인 후 현재 로컬 Controller의 Voice를 연결합니다. 로비 생성/참가 전에 자동 호출됩니다.
	UFUNCTION(BlueprintCallable, Category="PS3|EOS") bool InitializeLocalVoice();
	//민웅 수정 시작
	EPS3TextNotifyType ConsumePendingTextNotify();
	//민웅 수정 끝
	UPROPERTY(BlueprintReadOnly, Category="PS3|EOS") TArray<FPS3LobbyInfo> FoundLobbies;
	UPROPERTY(BlueprintReadOnly, Category="PS3|EOS") bool bBusy = false;
	UPROPERTY(BlueprintAssignable, Category="PS3|EOS") FPS3LobbyOperationCompleted OnOperationCompleted;
private:
	bool Prepare(FName Operation, bool bRequireLogin);
	void Complete(FName Operation, bool bSuccess, const FString& Message);
	//민웅 수정 시작
	void BroadcastTextNotify(EPS3TextNotifyType NotifyType) const;
	//민웅 수정 끝
	void HandleLogin(int32 UserNum, bool bSuccess, const FUniqueNetId& UserId, const FString& Error);
	void HandleCreate(FName SessionName, bool bSuccess);
	void HandleFind(bool bSuccess);
	void HandleJoin(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void HandleDestroy(FName SessionName, bool bSuccess);
	void StopLocalVoice();
	IOnlineIdentityPtr Identity;
	IOnlineSessionPtr Sessions;
	TSharedPtr<FOnlineSessionSearch> Search;
	FDelegateHandle LoginHandle, CreateHandle, FindHandle, JoinHandle, DestroyHandle;
	//민웅 수정 시작
	EPS3TextNotifyType PendingTextNotify = EPS3TextNotifyType::None;
	//민웅 수정 끝
};
