#pragma once
#include <Vertex/Core/Application.h>


typedef struct DiscordRichPresence {
    const char* state;   /* max 128 bytes */
    const char* details; /* max 128 bytes */
    int64_t startTimestamp;
    int64_t endTimestamp;
    const char* largeImageKey;  /* max 32 bytes */
    const char* largeImageText; /* max 128 bytes */
    const char* smallImageKey;  /* max 32 bytes */
    const char* smallImageText; /* max 128 bytes */
    const char* partyId;        /* max 128 bytes */
    int partySize;
    int partyMax;
    const char* matchSecret;    /* max 128 bytes */
    const char* joinSecret;     /* max 128 bytes */
    const char* spectateSecret; /* max 128 bytes */
    int8_t instance;
} DiscordRichPresence;

typedef struct DiscordUser {
    const char* userId;
    const char* username;
    const char* discriminator;
    const char* avatar;
} DiscordUser;

typedef struct DiscordEventHandlers {
    void (*ready)(const DiscordUser* request);
    void (*disconnected)(int errorCode, const char* message);
    void (*errored)(int errorCode, const char* message);
    void (*joinGame)(const char* joinSecret);
    void (*spectateGame)(const char* spectateSecret);
    void (*joinRequest)(const DiscordUser* request);
} DiscordEventHandlers;

#define DISCORD_REPLY_NO 0
#define DISCORD_REPLY_YES 1
#define DISCORD_REPLY_IGNORE 2

void Discord_Initialize(const char* applicationId,
    DiscordEventHandlers* handlers,
    int autoRegister,
    const char* optionalSteamId) 
{
    Ref<::Vertex::DynamicLibraryInstance> instance = ::Vertex::Application::Get().GetDLLInstance("plugins\\discord-rpc.dll");

    auto func = instance->getFunction<void(*)(const char*, DiscordEventHandlers*, int, const char*)>("Discord_Initialize");

    func(applicationId, handlers, autoRegister, optionalSteamId);
}

void Discord_Shutdown()
{
    Ref<::Vertex::DynamicLibraryInstance> instance = ::Vertex::Application::Get().GetDLLInstance("plugins\\discord-rpc.dll");

    auto func = instance->getFunction<void(*)()>("Discord_Shutdown");

    func();
}

void Discord_RunCallbacks()
{
    Ref<::Vertex::DynamicLibraryInstance> instance = ::Vertex::Application::Get().GetDLLInstance("plugins\\discord-rpc.dll");

    auto func = instance->getFunction<void(*)()>("Discord_RunCallbacks");

    func();
}

void Discord_UpdateConnection()
{
    Ref<::Vertex::DynamicLibraryInstance> instance = ::Vertex::Application::Get().GetDLLInstance("plugins\\discord-rpc.dll");

    auto func = instance->getFunction<void(*)()>("Discord_UpdateConnection");

    func();
}

void Discord_UpdatePresence(const DiscordRichPresence* presence)
{
    Ref<::Vertex::DynamicLibraryInstance> instance = ::Vertex::Application::Get().GetDLLInstance("plugins\\discord-rpc.dll");

    auto func = instance->getFunction<void(*)(const DiscordRichPresence*)>("Discord_UpdatePresence");

    func(presence);
}

void Discord_ClearPresence()
{
    Ref<::Vertex::DynamicLibraryInstance> instance = ::Vertex::Application::Get().GetDLLInstance("plugins\\discord-rpc.dll");

    auto func = instance->getFunction<void(*)()>("Discord_ClearPresence");

    func();
}

void Discord_Respond(const char* userid, /* DISCORD_REPLY_ */ int reply)
{
    Ref<::Vertex::DynamicLibraryInstance> instance = ::Vertex::Application::Get().GetDLLInstance("plugins\\discord-rpc.dll");

    auto func = instance->getFunction<void(*)(const char*, int)>("Discord_Respond");

    func(userid, reply);
}

void Discord_UpdateHandlers(DiscordEventHandlers* handlers)
{
    Ref<::Vertex::DynamicLibraryInstance> instance = ::Vertex::Application::Get().GetDLLInstance("plugins\\discord-rpc.dll");

    auto func = instance->getFunction<void(*)(DiscordEventHandlers*)>("Discord_UpdateHandlers");

    func(handlers);
}