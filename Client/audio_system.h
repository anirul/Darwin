#pragma once

#include <string>
#include <map>

#include "Common/client_audio.pb.h"

namespace darwin::audio {

    class AudioSystem {
    public:
        AudioSystem(const proto::ClientAudio& client_audio);
        ~AudioSystem();
        void PlayMusic(proto::AudioMusicEnum audio_music_enum);
        void PlaySound(proto::AudioSoundEnum audio_sound_enum);

    protected:
        unsigned short GetAudioFormatFromEnum(
            proto::AudioFormatEnum audio_format_enum) const;

    private:
        proto::ClientAudio client_audio_;
        std::map<proto::AudioMusicEnum, void*> music_queues_;
        std::map<proto::AudioSoundEnum, void*> sound_queues_;
        proto::AudioMusicEnum current_music_;
    };

}  // namespace darwin::audio.
