// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IXWebSocket.h"

#include "ALWebsocketBPObj.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWebSocketConnected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWebSocketClosed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWebSocketMessage, const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWebSocketError, const FString&, ErrorMessage);

UCLASS(BlueprintType)
class ALWEBSOCKETLIB_API UALWebsocketBPObj : public UObject{
	GENERATED_BODY()

public:
	UALWebsocketBPObj();
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "ALWebsocket", meta = (DisplayName = "Create Custom WebSocket"))
	static UALWebsocketBPObj* CreateWebSocket(const FString& URL);
 
	UFUNCTION(BlueprintCallable, Category = "ALWebsocket")
	void Connect();
 
	UFUNCTION(BlueprintCallable, Category = "ALWebsocket")
	void Disconnect();
    
	UFUNCTION(BlueprintCallable, Category = "ALWebsocket")
	void SendData(const FString& Message);

	UFUNCTION(BlueprintCallable, Category = "ALWebsocket")
	void SendDataAsBinary(const FString& Message);

	UFUNCTION(BlueprintCallable, Category = "ALWebsocket")
	void SetURL(const FString& newURL);

	UFUNCTION(BlueprintCallable, Category = "ALWebsocket")
	FString GetCurrentURL();

	UFUNCTION(BlueprintCallable, Category = "ALWebsocket", meta=(ToolTip="0:Connecting, 1:Open, 2:Closing, 3:Closed, -1:undef"))
	int GetWebsocketStatus();
  
	UPROPERTY(BlueprintAssignable, Category = "ALWebsocket|Events")
	FOnWebSocketConnected OnConnected;

	UPROPERTY(BlueprintAssignable, Category = "ALWebsocket|Events")
	FOnWebSocketClosed OnClosed;

	UPROPERTY(BlueprintAssignable, Category = "ALWebsocket|Events")
	FOnWebSocketMessage OnMessageReceived;

	UPROPERTY(BlueprintAssignable, Category = "ALWebsocket|Events")
	FOnWebSocketError OnErrored;

private:
    
	TUniquePtr<ix::WebSocket> DynamicSocket;
    
	FString TargetURL;
	bool bIsInitialized = false;

	void InitializeSocket();
};
