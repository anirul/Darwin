#include "audio_system.h"

#include <format>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "frame/logger.h"
#include "frame/file/file_system.h"

namespace darwin::audio {

    AudioSystem::AudioSystem(
        const proto::ClientAudio& client_audio)
        : client_audio_(client_audio),
          current_music_(proto::AUDIO_MUSIC_NONE)
    {
        // Initialize SDL audio subsystem.
        if (!SDL_WasInit(SDL_INIT_AUDIO)) {
            if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
                throw std::runtime_error(
                    std::format(
                        "Failed to initialize SDL audio subsystem ({}).",
                        SDL_GetError()));
            }
        }
        // Initialize SDL mixer.
        if (Mix_OpenAudio(
            client_audio_.sample_rate(),
            GetAudioFormatFromEnum(client_audio_.audio_format_enum()),
            client_audio_.channels(),
            client_audio_.chunk_size())) 
        {
            throw std::runtime_error(
                std::format(
                    "Failed to initialize SDL mixer ({}).", 
                    Mix_GetError()));
        }
        for (const auto& music_queue : client_audio_.music_queues()) {
            auto path = frame::file::FindFile(music_queue.audio_file());
            Mix_Music* music = Mix_LoadMUS(path.string().c_str());
            if (!music) {
                throw std::runtime_error(
                    std::format(
                        "Failed to load music file ({}).", 
                        Mix_GetError()));
            }
            if (music_queues_.contains(music_queue.audio_music_enum())) {
                throw std::runtime_error(
                    std::format(
                        "Duplicate audio music enum ({}).",
                        proto::AudioMusicEnum_Name(
                            music_queue.audio_music_enum())));
            }
            music_queues_.insert(
                std::make_pair(
                    music_queue.audio_music_enum(), 
                    music));
            music_name_.insert(
                std::make_pair(
                    music_queue.audio_music_enum(),
                    music_queue.title()));
            authors_name_.insert(
                std::make_pair(
                    music_queue.audio_music_enum(),
                    music_queue.author()));
        }
        for (const auto& sound_queue : client_audio_.sound_queues()) {
            auto path = frame::file::FindFile(sound_queue.audio_file());
            Mix_Chunk* sound = Mix_LoadWAV(path.string().c_str());
            if (!sound) {
                throw std::runtime_error(
                    std::format(
                        "Failed to load sound file ({}).", 
                        Mix_GetError()));
            }
            if (sound_queues_.contains(sound_queue.audio_sound_enum())) {
                throw std::runtime_error(
                    std::format(
                        "Duplicate audio state enum ({}).",
                        proto::AudioSoundEnum_Name(
                            sound_queue.audio_sound_enum())));
            }
            sound_queues_.insert(
                std::make_pair(
                    sound_queue.audio_sound_enum(), 
                    sound));
        }
    }

    unsigned short AudioSystem::GetAudioFormatFromEnum(
        proto::AudioFormatEnum audio_format_enum) const
    {
        switch (audio_format_enum) {
            case proto::AUDIO_FORMAT_DEFAULT:
                return MIX_DEFAULT_FORMAT;
            case proto::AUDIO_FORMAT_S16_LSB:
                return AUDIO_S16LSB;
            case proto::AUDIO_FORMAT_S16_MSB:
                return AUDIO_S16MSB;
            case proto::AUDIO_FORMAT_S16_SYS:
                return AUDIO_S16SYS;
            default:
                throw std::runtime_error(
                    "Invalid audio format enum.");
        }
    }

    void AudioSystem::PlayMusic(proto::AudioMusicEnum audio_music_enum) {
        // Doens't restart play the same audio state.
        if (current_music_ == audio_music_enum) {
            return;
        }
        int fade_music = 
            static_cast<int>(client_audio_.fade_music() * 1000.0);
        Mix_FadeOutMusic(fade_music);
        if (!music_queues_.contains(audio_music_enum)) {
            frame::Logger::GetInstance()->error(
                std::format(
                    "Audio state enum not found ({}).",
                    proto::AudioMusicEnum_Name(audio_music_enum)));
            // Not found so put the default audio state.
            current_music_ = proto::AUDIO_MUSIC_NONE;
            return;
        }
        Mix_Music* music = 
            reinterpret_cast<Mix_Music*>(music_queues_[audio_music_enum]);
        if (Mix_FadeInMusic(music, -1, fade_music) < 0) {
            throw std::runtime_error(
                std::format(
                    "Failed to play music {} ({}).",
                    proto::AudioMusicEnum_Name(audio_music_enum),
                    Mix_GetError()));
        }
        current_music_ = audio_music_enum;
    }

    void AudioSystem::PlaySound(proto::AudioSoundEnum audio_sound_enum) {
        if (!sound_queues_.contains(audio_sound_enum)) {
            frame::Logger::GetInstance()->error(
                std::format(
                    "Audio sound enum not found ({}).",
                    proto::AudioSoundEnum_Name(audio_sound_enum)));
            return;
        }
        Mix_Chunk* sound = 
            reinterpret_cast<Mix_Chunk*>(sound_queues_[audio_sound_enum]);
        if (Mix_PlayChannel(-1, sound, 0) < 0) {
            frame::Logger::GetInstance()->warn(
                std::format(
                    "Failed to play sound {} ({}).",
                    proto::AudioSoundEnum_Name(audio_sound_enum),
                    Mix_GetError()));
        }
    }

    void AudioSystem::MuteMusic() {
        Mix_VolumeMusic(0);
    }

    float AudioSystem::GetMusicVolume() const {
        constexpr float max_volume = static_cast<float>(MIX_MAX_VOLUME);
        return static_cast<float>(Mix_VolumeMusic(-1)) / max_volume;
    }

    void AudioSystem::SetMusicVolume(float volume) {
        if (volume < 0.0f || volume > 1.0f) {
            throw std::runtime_error("Invalid volume value.");
        }
        constexpr float max_volume = static_cast<float>(MIX_MAX_VOLUME);
        Mix_VolumeMusic(static_cast<int>(volume * max_volume));
    }

    AudioSystem::~AudioSystem()
    {
        Mix_HaltMusic();
        for (const auto& [_, music] : music_queues_) {
            Mix_FreeMusic(reinterpret_cast<Mix_Music*>(music));
        }
        for (const auto& [_, sound] : sound_queues_) {
            Mix_FreeChunk(reinterpret_cast<Mix_Chunk*>(sound));
        }
        Mix_CloseAudio();
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }

} // namespace darwin::audio.
