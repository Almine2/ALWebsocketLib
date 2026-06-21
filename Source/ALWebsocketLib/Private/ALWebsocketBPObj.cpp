// Copyright Epic Games, Inc. All Rights Reserved.

#include "ALWebsocketBPObj.h"
#include "Async/Async.h"

UALWebsocketBPObj::UALWebsocketBPObj()
{
}

void UALWebsocketBPObj::BeginDestroy()
{
    Disconnect();
    Super::BeginDestroy();
}

UALWebsocketBPObj* UALWebsocketBPObj::CreateWebSocket(const FString& URL){
    //
    UALWebsocketBPObj* SocketObject = NewObject<UALWebsocketBPObj>();
    SocketObject->TargetURL = URL;
    SocketObject->InitializeSocket();
    return SocketObject;
}

void UALWebsocketBPObj::SetURL(const FString& newURL) {
  if (newURL.IsEmpty()) {
    return;
  }
  if (DynamicSocket && DynamicSocket->getReadyState() == ix::ReadyState::Closed) {
    TargetURL = newURL;
    DynamicSocket->setUrl(TCHAR_TO_UTF8(*newURL));
  }
}

FString UALWebsocketBPObj::GetCurrentURL() {
  return TargetURL;
}

int UALWebsocketBPObj::GetWebsocketStatus() {
  if (DynamicSocket) {
    switch (DynamicSocket->getReadyState()){
    case ix::ReadyState::Connecting:
      return 0;
      break;
    case ix::ReadyState::Open:
      return 1;
      break;
    case ix::ReadyState::Closing:
      return 2;
      break;
    case ix::ReadyState::Closed:
      return 3;
      break;
    default:
      return -1;
      break;
    }
  } else {
    return -1;
  }
}

void UALWebsocketBPObj::InitializeSocket(){
    if (bIsInitialized) return;

    //DynamicSocket = std::make_unique<ix::WebSocket>();
    DynamicSocket = MakeUnique<ix::WebSocket>();
    DynamicSocket->setUrl(TCHAR_TO_UTF8(*TargetURL));
    
    DynamicSocket->setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg){
        if (msg->type == ix::WebSocketMessageType::Message){
            FString ReceivedStr = UTF8_TO_TCHAR(msg->str.c_str());
            AsyncTask(ENamedThreads::GameThread, [this, ReceivedStr](){
                if (IsValid(this)){
                  OnMessageReceived.Broadcast(ReceivedStr);
                   //UE_LOG(LogTemp, Log, TEXT("ALWebsocket got message: %s"), *FString(ReceivedStr));
                }
            });
        }
        else if (msg->type == ix::WebSocketMessageType::Open){
            AsyncTask(ENamedThreads::GameThread, [this](){
                if (IsValid(this)){
                    OnConnected.Broadcast();
                }
            });
        }
        else if (msg->type == ix::WebSocketMessageType::Close) {
          AsyncTask(ENamedThreads::GameThread, [this]() {
            if (IsValid(this)) {
              OnClosed.Broadcast();
            }
          });
        }
        else if (msg->type == ix::WebSocketMessageType::Error) {
          FString errorMessage = UTF8_TO_TCHAR(msg->errorInfo.reason.c_str());
          AsyncTask(ENamedThreads::GameThread, [this, errorMessage]() {
            if (IsValid(this)) {
              OnErrored.Broadcast(errorMessage);
            }
          });
        }
    });

    bIsInitialized = true;
}

void UALWebsocketBPObj::Connect(){
    if (DynamicSocket && !TargetURL.IsEmpty()){
        DynamicSocket->start();
    }
}

void UALWebsocketBPObj::Disconnect(){
    if (DynamicSocket){
        DynamicSocket->stop();
    }
}

void UALWebsocketBPObj::SendData(const FString& Message){
    if (DynamicSocket && DynamicSocket->getReadyState() == ix::ReadyState::Open){
        std::string SendStr(TCHAR_TO_UTF8(*Message));
        DynamicSocket->send(SendStr);
    }
}

void UALWebsocketBPObj::SendDataAsBinary(const FString& Message) {
  if (DynamicSocket && DynamicSocket->getReadyState() == ix::ReadyState::Open) {
    std::string Sendstr(TCHAR_TO_UTF8(*Message));
    DynamicSocket->sendBinary(Sendstr);
  }
}