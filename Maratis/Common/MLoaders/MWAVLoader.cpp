#include <MCore.h>
#include <MEngine.h>
#include <cassert>

// Should actually include csdtint but this requires stdc++11 on gcc
#include <stdint.h>
#include "MWAVLoader.h"

// "RIFF" in ASCII (0x52494646 as big endian)
#define CHUNK_ID 0x46464952

// "WAVE" in ASCII (0x57415645 as big endian)
#define FORMAT 0x45564157

// "data" in ASCII (0x64617461 as big endian)
#define SUBCHUNK_ID  0x61746164

#pragma pack(push, 1)
typedef struct
{
    uint32_t chunk_id;
    uint32_t chunk_size;
    uint32_t format;
    uint32_t subchunk_id;
    uint32_t subchunk_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    uint32_t subchunk2_id;
    uint32_t subchunk2_size;
}wave_header_t;
#pragma pack(pop)

// TODO: More error checking!
bool M_loadWAVSound(const char * filename, void * data)
{
    MFile* file;
    char* file_buffer = NULL;

    if((!data) || (!filename))
        return false;

    MSound* sound = (MSound*) data;

    if(!(file = M_fopen(filename, "rb")))
    {
        fprintf(stderr, "ERROR: Could not open WAV sound file!\n");
    }

    M_fseek(file, 0, SEEK_END);
    unsigned long filesize = file->tell();
    file->seek(0, SEEK_SET);

    file_buffer = (char*) malloc(filesize);
    assert(file_buffer);

    file->read(file_buffer, filesize, 1);
    file->close();

    wave_header_t* header = (wave_header_t*) file_buffer;

    // Check for sanity
    if(header->chunk_id != CHUNK_ID || header->format != FORMAT
            || header->subchunk2_id != SUBCHUNK_ID)
    {
        MLOG_ERROR("File is not a valid WAV file!\n");
        return false;
    }

    uint32_t chunk_size = header->chunk_size;

    M_SOUND_FORMAT format = static_cast<M_SOUND_FORMAT>(-1);
    switch(header->num_channels)
    {
        case 1:
            if(header->bits_per_sample == 16)
                format = M_SOUND_FORMAT_MONO16;
            else if(header->bits_per_sample == 8)
                format = M_SOUND_FORMAT_MONO8;
            else
            {
                MLOG_ERROR("This RIFF file is not a mono 8 or 16 bit PCM file!")
            }
        break;

        case 2:
            if(header->bits_per_sample == 16)
                format = M_SOUND_FORMAT_STEREO16;
            else if(header->bits_per_sample == 8)
                format = M_SOUND_FORMAT_STEREO8;
            else
            {
                MLOG_ERROR("This RIFF file is not a stereo 8 or 16 bit PCM file!")
            }

        break;

        default:
            MLOG_ERROR("Could not load WAV file! Unknown format!\n");
            return false;
    }

    sound->create(format, header->subchunk2_size, header->sample_rate);
    memcpy((char*)sound->getData(), (char*) (++header), header->subchunk2_size);

    free(file_buffer);
    return true;
}
