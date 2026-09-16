#include "PS3LobbySubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Online/OnlineSessionNames.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Component/VoicePluginControlComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/PlayerHUD.h"

bool UPS3LobbySubsystem::Prepare(FName Operation, bool bRequireLogin)
{
	// 진행 중인 비동기 요청을 다른 요청으로 덮어쓰지 않습니다.
	if (bBusy)
	{
		OnOperationCompleted.Broadcast(Operation, false, TEXT("Another operation is running."));
		return false;
	}
	IOnlineSubsystem* EOS = Online::GetSubsystem(GetWorld(), FName(TEXT("EOS")));
	if (!EOS)
	{
		Complete(Operation, false, TEXT("EOS subsystem unavailable. Check project configuration."));
		return false;
	}
	Identity = EOS->GetIdentityInterface();
	Sessions = EOS->GetSessionInterface();
	if (!Identity.IsValid() || !Sessions.IsValid())
	{
		Complete(Operation, false, TEXT("EOS identity/session interface unavailable."));
		return false;
	}
	if (bRequireLogin && Identity->GetLoginStatus(0) != ELoginStatus::LoggedIn)
	{
		Complete(Operation, false, TEXT("Login first."));
		return false;
	}
	bBusy = true;
	return true;
}

void UPS3LobbySubsystem::Complete(FName Operation, bool bSuccess, const FString& Message)
{
	bBusy = false;
	OnOperationCompleted.Broadcast(Operation, bSuccess, Message);
}

//민웅 수정 시작
void UPS3LobbySubsystem::BroadcastTextNotify(EPS3TextNotifyType NotifyType) const
{
	if (NotifyType == EPS3TextNotifyType::None)
	{
		return;
	}

	if (const UWorld* World = GetWorld())
	{
		for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PlayerController = It->Get();
			if (!IsValid(PlayerController) || !PlayerController->IsLocalController())
			{
				continue;
			}

			APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PlayerController->GetHUD());
			if (!IsValid(PlayerHUD) || !PlayerHUD->IsUIReady())
			{
				continue;
			}

			PlayerHUD->SetTextNotifyVisible(true);
			PlayerHUD->ShowTextNotify(NotifyType);
		}
	}
}

EPS3TextNotifyType UPS3LobbySubsystem::ConsumePendingTextNotify()
{
	const EPS3TextNotifyType NotifyType = PendingTextNotify;
	PendingTextNotify = EPS3TextNotifyType::None;
	return NotifyType;
}
//민웅 수정 끝

void UPS3LobbySubsystem::Login()
{
	if (!Prepare(TEXT("Login"), false)) return;
	if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		////민웅 수정 시작
		BroadcastTextNotify(EPS3TextNotifyType::LobbyAlreadyLogin);
		//민웅 수정 끝
		Complete(TEXT("Login"), true, TEXT("Already logged in."));
		return;
	}
	LoginHandle = Identity->AddOnLoginCompleteDelegate_Handle(0, FOnLoginCompleteDelegate::CreateUObject(this, &ThisClass::HandleLogin));
	if (!Identity->Login(0, FOnlineAccountCredentials(TEXT("accountportal"), TEXT(""), TEXT(""))))
	{
		Identity->ClearOnLoginCompleteDelegate_Handle(0, LoginHandle);
		Complete(TEXT("Login"), false, TEXT("Login request could not start."));
	}
}

void UPS3LobbySubsystem::HandleLogin(int32 UserNum, bool bSuccess, const FUniqueNetId& UserId, const FString& Error)
{
	Identity->ClearOnLoginCompleteDelegate_Handle(UserNum, LoginHandle);
	//민웅 수정 시작
	if (bSuccess)
	{
		BroadcastTextNotify(EPS3TextNotifyType::LobbyLogin);
	}
	//민웅 수정 끝
	Complete(TEXT("Login"), bSuccess, bSuccess ? TEXT("Logged in.") : Error);
}

void UPS3LobbySubsystem::CreateLobby()
{
	if (!Prepare(TEXT("CreateLobby"), true)) return;
	if (Sessions->GetNamedSession(NAME_GameSession))
	{
		Complete(TEXT("CreateLobby"), false, TEXT("Leave the current lobby first."));
		return;
	}
	// RTC 자동 참가 전에 송신을 차단하고 채널 콜백을 등록합니다.
	if (!InitializeLocalVoice())
	{
		Complete(TEXT("CreateLobby"), false, TEXT("Local voice initialization failed. Lobby creation cancelled."));
		return;
	}
	FOnlineSessionSettings Settings;
	Settings.NumPublicConnections = 2;
	Settings.bIsLANMatch = false;
	Settings.bShouldAdvertise = true;
	Settings.bAllowJoinInProgress = true;
	Settings.bUsesPresence = true;
	Settings.bAllowJoinViaPresence = true;
	Settings.bUseLobbiesIfAvailable = true;
	Settings.bUseLobbiesVoiceChatIfAvailable = true;
	Settings.Set(FName(TEXT("PS3_LOBBY_TEST")), true, EOnlineDataAdvertisementType::ViaOnlineService);
	CreateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::HandleCreate));
	if (!Sessions->CreateSession(0, NAME_GameSession, Settings))
	{
		Sessions->ClearOnCreateSessionCompleteDelegate_Handle(CreateHandle);
		Complete(TEXT("CreateLobby"), false, TEXT("Create request could not start."));
	}
}

void UPS3LobbySubsystem::HandleCreate(FName SessionName, bool bSuccess)
{
	Sessions->ClearOnCreateSessionCompleteDelegate_Handle(CreateHandle);
	if (!bSuccess)
	{
		Complete(TEXT("CreateLobby"), false, TEXT("Lobby creation failed."));
		return;
	}

	//민웅 수정시작
	PendingTextNotify = EPS3TextNotifyType::LobbyCreated;
	//민웅 수정 끝

	// 생성한 EOS 로비를 유지한 채 호스트의 대기방을 리슨 서버로 엽니다.
	// UI에서는 생성 성공 이벤트를 받아 OpenLevel을 중복 호출하지 않습니다.
	UGameplayStatics::OpenLevel(
		this,
		FName(TEXT("/Game/PuzzleStay3/Dev/Maps/EOS_WaitingRoom")),
		true,
		TEXT("listen"));

	// 맵 로딩 및 서버 준비 완료가 아닌, 로비 생성과 이동 요청 완료입니다.
	Complete(TEXT("CreateLobby"), true, TEXT("Lobby created. Opening waiting room as listen server..."));
}
void UPS3LobbySubsystem::FindLobbies()
{
	if (!Prepare(TEXT("FindLobbies"), true)) return;
	FoundLobbies.Empty();
	Search = MakeShared<FOnlineSessionSearch>();
	Search->bIsLanQuery = false;
	Search->MaxSearchResults = 20;
	Search->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	Search->QuerySettings.Set(FName(TEXT("PS3_LOBBY_TEST")), true, EOnlineComparisonOp::Equals);
	FindHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::HandleFind));
	if (!Sessions->FindSessions(0, Search.ToSharedRef()))
	{
		Sessions->ClearOnFindSessionsCompleteDelegate_Handle(FindHandle);
		Complete(TEXT("FindLobbies"), false, TEXT("Search request could not start."));
	}
}

void UPS3LobbySubsystem::HandleFind(bool bSuccess)
{
	Sessions->ClearOnFindSessionsCompleteDelegate_Handle(FindHandle);
	if (bSuccess && Search.IsValid())
	{
		for (const FOnlineSessionSearchResult& Result : Search->SearchResults)
		{
			FPS3LobbyInfo Info;
			Info.OwnerName = Result.Session.OwningUserName;
			Info.OpenSlots = Result.Session.NumOpenPublicConnections;
			FoundLobbies.Add(Info);
		}
	}
	//민웅 수정 시작
	PendingTextNotify = EPS3TextNotifyType::FindLobby;
	BroadcastTextNotify(EPS3TextNotifyType::FindLobby);
	//민웅 수정 끝
	Complete(TEXT("FindLobbies"), bSuccess, FString::Printf(TEXT("Found %d lobbies."), FoundLobbies.Num()));
}

void UPS3LobbySubsystem::JoinLobby(int32 ResultIndex)
{
	if (!Prepare(TEXT("JoinLobby"), true)) return;
	if (Sessions->GetNamedSession(NAME_GameSession) || !Search.IsValid() || !Search->SearchResults.IsValidIndex(ResultIndex) || !FoundLobbies.IsValidIndex(ResultIndex))
	{
		Complete(TEXT("JoinLobby"), false, TEXT("Invalid result or already in a lobby."));
		return;
	}
	if (!InitializeLocalVoice())
	{
		Complete(TEXT("JoinLobby"), false, TEXT("Local voice initialization failed. Lobby join cancelled."));
		return;
	}
	JoinHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::HandleJoin));
	if (!Sessions->JoinSession(0, NAME_GameSession, Search->SearchResults[ResultIndex]))
	{
		Sessions->ClearOnJoinSessionCompleteDelegate_Handle(JoinHandle);
		Complete(TEXT("JoinLobby"), false, TEXT("Join request could not start."));
	}
}

void UPS3LobbySubsystem::HandleJoin(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	Sessions->ClearOnJoinSessionCompleteDelegate_Handle(JoinHandle);

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		Complete(
			TEXT("JoinLobby"),
			false,
			TEXT("Lobby join failed."));
		return;
	}

	FString ConnectString;
	if (!Sessions->GetResolvedConnectString(SessionName, ConnectString)
		|| ConnectString.IsEmpty())
	{
		Complete(
			TEXT("JoinLobby"),
			false,
			TEXT("Server address unavailable. Leave the lobby and retry."));
		return;
	}

	APlayerController* LocalController =
		GetGameInstance()->GetFirstLocalPlayerController();

	if (!IsValid(LocalController))
	{
		Complete(
			TEXT("JoinLobby"),
			false,
			TEXT("Local controller unavailable. Leave the lobby and retry."));
		return;
	}

	LocalController->ClientTravel(ConnectString, TRAVEL_Absolute);

	

	Complete(
		TEXT("JoinLobby"),
		true,
		TEXT("Joined lobby. Connecting to host..."));
}
bool UPS3LobbySubsystem::InitializeLocalVoice()
{
	IOnlineSubsystem* EOS =
		Online::GetSubsystem(GetWorld(), FName(TEXT("EOS")));

	if (!EOS)
	{
		return false;
	}

	const IOnlineIdentityPtr CurrentIdentity =
		EOS->GetIdentityInterface();

	if (!CurrentIdentity.IsValid()
		|| CurrentIdentity->GetLoginStatus(0) != ELoginStatus::LoggedIn)
	{
		return false;
	}

	APlayerController* LocalController =
		GetGameInstance()->GetFirstLocalPlayerController();

	if (!IsValid(LocalController))
	{
		return false;
	}

	UVoicePluginControlComponent* Voice =
		LocalController->FindComponentByClass<
			UVoicePluginControlComponent>();

	return IsValid(Voice) && Voice->InitializeEOSVoice(0);
}

void UPS3LobbySubsystem::StopLocalVoice()
{
	APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController();
	if (UVoicePluginControlComponent* Voice = IsValid(PC) ? PC->FindComponentByClass<UVoicePluginControlComponent>() : nullptr)
	{
		Voice->ShutdownEOSVoice();
	}
}

void UPS3LobbySubsystem::LeaveLobby()
{
	if (!Prepare(TEXT("LeaveLobby"), true)) return;
	if (!Sessions->GetNamedSession(NAME_GameSession))
	{
		Complete(TEXT("LeaveLobby"), true, TEXT("No active lobby."));
		return;
	}
	StopLocalVoice();
	DestroyHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::HandleDestroy));
	if (!Sessions->DestroySession(NAME_GameSession))
	{
		Sessions->ClearOnDestroySessionCompleteDelegate_Handle(DestroyHandle);
		Complete(TEXT("LeaveLobby"), false, TEXT("Leave request could not start. Reinitialize voice to resume."));
	}
}

void UPS3LobbySubsystem::HandleDestroy(FName SessionName, bool bSuccess)
{
	Sessions->ClearOnDestroySessionCompleteDelegate_Handle(DestroyHandle);
	Complete(TEXT("LeaveLobby"), bSuccess, bSuccess ? TEXT("Left lobby.") : TEXT("Leave failed. Reinitialize voice to resume."));
}

void UPS3LobbySubsystem::Deinitialize()
{
	StopLocalVoice();
	if (Identity.IsValid()) Identity->ClearOnLoginCompleteDelegate_Handle(0, LoginHandle);
	if (Sessions.IsValid())
	{
		Sessions->ClearOnCreateSessionCompleteDelegate_Handle(CreateHandle);
		Sessions->ClearOnFindSessionsCompleteDelegate_Handle(FindHandle);
		Sessions->ClearOnJoinSessionCompleteDelegate_Handle(JoinHandle);
		Sessions->ClearOnDestroySessionCompleteDelegate_Handle(DestroyHandle);
	}
	Search.Reset();
	Sessions.Reset();
	Identity.Reset();
	Super::Deinitialize();
}
