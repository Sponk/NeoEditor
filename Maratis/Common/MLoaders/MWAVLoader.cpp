#include <MCore.h>
#include <cassert>
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
    u_int32_t chunk_id;
    u_int32_t chunk_size;
    u_int32_t format;
    u_int32_t subchunk_id;
    u_int32_t subchunk_size;
    u_int16_t audio_format;
    u_int16_t num_channels;
    u_int32_t sample_rate;
    u_int32_t byte_rate;
    u_int16_t block_align;
    u_int16_t bits_per_sample;
    u_int32_t subchunk2_id;
    u_int32_t subchunk2_size;
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
        fprintf(stderr, "ERROR: File is not a valid WAV file!\n");
        return false;
    }

    u_int32_t chunk_size = header->chunk_size;

    M_SOUND_FORMAT format;
    switch(header->num_channels)
    {
        case 1:
            if(header->bits_per_sample == 16)
                format = M_SOUND_FORMAT_MONO16;
            else
                format = M_SOUND_FORMAT_MONO8;
        break;

        case 2:
            if(header->bits_per_sample == 16)
                format = M_SOUND_FORMAT_STEREO16;
            else
                format = M_SOUND_FORMAT_STEREO8;
        break;

        default:
            fprintf(stderr, "Could not load WAV file! Unknown format!\n");
            return false;
    }

    sound->create(format, header->subchunk2_size, header->sample_rate);
    memcpy((char*)sound->getData(), (char*) (++header), header->subchunk2_size);

    free(file_buffer);
    return true;
}
