/**
 * OpenAL cross platform audio library
 * Copyright (C) 1999-2007 by authors.
 * This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * Or go to http://www.gnu.org/copyleft/lgpl.html
 */

#include "config.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "alMain.h"
#include "alSource.h"
#include "alBuffer.h"
#include "alListener.h"
#include "alAuxEffectSlot.h"
#include "alu.h"
#include "bs2b.h"
#include "hrtf.h"
#include "static_assert.h"

#include "backends/base.h"
#include "midi/base.h"


static_assert((INT_MAX>>FRACTIONBITS)/MAX_PITCH > BUFFERSIZE,
              "MAX_PITCH and/or BUFFERSIZE are too large for FRACTIONBITS!");

struct ChanMap {
    enum Channel channel;
    ALfloat angle;
    ALfloat elevation;
};

/* Cone scalar */
ALfloat ConeScale = 1.0f;

/* Localized Z scalar for mono sources */
ALfloat ZScale = 1.0f;

extern inline ALfloat minf(ALfloat a, ALfloat b);
extern inline ALfloat maxf(ALfloat a, ALfloat b);
extern inline ALfloat clampf(ALfloat val, ALfloat min, ALfloat max);

extern inline ALdouble mind(ALdouble a, ALdouble b);
extern inline ALdouble maxd(ALdouble a, ALdouble b);
extern inline ALdouble clampd(ALdouble val, ALdouble min, ALdouble max);

extern inline ALuint minu(ALuint a, ALuint b);
extern inline ALuint maxu(ALuint a, ALuint b);
extern inline ALuint clampu(ALuint val, ALuint min, ALuint max);

extern inline ALint mini(ALint a, ALint b);
extern inline ALint maxi(ALint a, ALint b);
extern inline ALint clampi(ALint val, ALint min, ALint max);

extern inline ALint64 mini64(ALint64 a, ALint64 b);
extern inline ALint64 maxi64(ALint64 a, ALint64 b);
extern inline ALint64 clampi64(ALint64 val, ALint64 min, ALint64 max);

extern inline ALuint64 minu64(ALuint64 a, ALuint64 b);
extern inline ALuint64 maxu64(ALuint64 a, ALuint64 b);
extern inline ALuint64 clampu64(ALuint64 val, ALuint64 min, ALuint64 max);

extern inline ALfloat lerp(ALfloat val1, ALfloat val2, ALfloat mu);
extern inline ALfloat cubic(ALfloat val0, ALfloat val1, ALfloat val2, ALfloat val3, ALfloat mu);


static inline void aluCrossproduct(const ALfloat *inVector1, const ALfloat *inVector2, ALfloat *outVector)
{
    outVector[0] = inVector1[1]*inVector2[2] - inVector1[2]*inVector2[1];
    outVector[1] = inVector1[2]*inVector2[0] - inVector1[0]*inVector2[2];
    outVector[2] = inVector1[0]*inVector2[1] - inVector1[1]*inVector2[0];
}

static inline ALfloat aluDotproduct(const ALfloat *inVector1, const ALfloat *inVector2)
{
    return inVector1[0]*inVector2[0] + inVector1[1]*inVector2[1] +
           inVector1[2]*inVector2[2];
}

static inline void aluNormalize(ALfloat *inVector)
{
    ALfloat lengthsqr = aluDotproduct(inVector, inVector);
    if(lengthsqr > 0.0f)
    {
        ALfloat inv_length = 1.0f/sqrtf(lengthsqr);
        inVector[0] *= inv_length;
        inVector[1] *= inv_length;
        inVector[2] *= inv_length;
    }
}

static inline ALvoid aluMatrixVector(ALfloat *vector, ALfloat w, ALfloat (*restrict matrix)[4])
{
    ALfloat temp[4] = {
        vector[0], vector[1], vector[2], w
    };

    vector[0] = temp[0]*matrix[0][0] + temp[1]*matrix[1][0] + temp[2]*matrix[2][0] + temp[3]*matrix[3][0];
    vector[1] = temp[0]*matrix[0][1] + temp[1]*matrix[1][1] + temp[2]*matrix[2][1] + temp[3]*matrix[3][1];
    vector[2] = temp[0]*matrix[0][2] + temp[1]*matrix[1][2] + temp[2]*matrix[2][2] + temp[3]*matrix[3][2];
}


static void UpdateDryStepping(DirectParams *params, ALuint num_chans)
{
    ALuint i, j;

    if(!params->Moving)
    {
        for(i = 0;i < num_chans;i++)
        {
            MixGains *gains = params->Mix.Gains[i];
            for(j = 0;j < MAX_OUTPUT_CHANNELS;j++)
            {
                gains[j].Current = gains[j].Target;
                gains[j].Step = 1.0f;
            }
        }
        params->Moving = AL_TRUE;
        params->Counter = 0;
        return;
    }

    for(i = 0;i < num_chans;i++)
    {
        MixGains *gains = params->Mix.Gains[i];
        for(j = 0;j < MAX_OUTPUT_CHANNELS;j++)
        {
            ALfloat cur = maxf(gains[j].Current, FLT_EPSILON);
            ALfloat trg = maxf(gains[j].Target, FLT_EPSILON);
            if(fabs(trg - cur) >= GAIN_SILENCE_THRESHOLD)
                gains[j].Step = powf(trg/cur, 1.0f/64.0f);
            else
                gains[j].Step = 1.0f;
            gains[j].Current = cur;
        }
    }
    params->Counter = 64;
}

static void UpdateWetStepping(SendParams *params)
{
    ALfloat cur, trg;

    if(!params->Moving)
    {
        params->Gain.Current = params->Gain.Target;
        params->Gain.Step = 1.0f;

        params->Moving = AL_TRUE;
        params->Counter = 0;
        return;
    }

    cur = maxf(params->Gain.Current, FLT_EPSILON);
    trg = maxf(params->Gain.Target, FLT_EPSILON);
    if(fabs(trg - cur) >= GAIN_SILENCE_THRESHOLD)
        params->Gain.Step = powf(trg/cur, 1.0f/64.0f);
    else
        params->Gain.Step = 1.0f;
    params->Gain.Current = cur;

    params->Counter = 64;
}


static ALvoid CalcListenerParams(ALlistener *Listener)
{
    ALfloat N[3], V[3], U[3], P[3];

    /* AT then UP */
    N[0] = Listener->Forward[0];
    N[1] = Listener->Forward[1];
    N[2] = Listener->Forward[2];
    aluNormalize(N);
    V[0] = Listener->Up[0];
    V[1] = Listener->Up[1];
    V[2] = Listener->Up[2];
    aluNormalize(V);
    /* Build and normalize right-vector */
    aluCrossproduct(N, V, U);
    aluNormalize(U);

    Listener->Params.Matrix[0][0] =  U[0];
    Listener->Params.Matrix[0][1] =  V[0];
    Listener->Params.Matrix[0][2] = -N[0];
    Listener->Params.Matrix[0][3] =  0.0f;
    Listener->Params.Matrix[1][0] =  U[1];
    Listener->Params.Matrix[1][1] =  V[1];
    Listener->Params.Matrix[1][2] = -N[1];
    Listener->Params.Matrix[1][3] =  0.0f;
    Listener->Params.Matrix[2][0] =  U[2];
    Listener->Params.Matrix[2][1] =  V[2];
    Listener->Params.Matrix[2][2] = -N[2];
    Listener->Params.Matrix[2][3] =  0.0f;
    Listener->Params.Matrix[3][0] =  0.0f;
    Listener->Params.Matrix[3][1] =  0.0f;
    Listener->Params.Matrix[3][2] =  0.0f;
    Listener->Params.Matrix[3][3] =  1.0f;

    P[0] = Listener->Position[0];
    P[1] = Listener->Position[1];
    P[2] = Listener->Position[2];
    aluMatrixVector(P, 1.0f, Listener->Params.Matrix);
    Listener->Params.Matrix[3][0] = -P[0];
    Listener->Params.Matrix[3][1] = -P[1];
    Listener->Params.Matrix[3][2] = -P[2];

    Listener->Params.Velocity[0] = Listener->Velocity[0];
    Listener->Params.Velocity[1] = Listener->Velocity[1];
    Listener->Params.Velocity[2] = Listener->Velocity[2];
    aluMatrixVector(Listener->Params.Velocity, 0.0f, Listener->Params.Matrix);
}

ALvoid CalcNonAttnSourceParams(ALvoice *voice, const ALsource *ALSource, const ALCcontext *ALContext)
{
    static const struct ChanMap MonoMap[1] = { { FrontCenter, 0.0f, 0.0f } };
    static const struct ChanMap StereoMap[2] = {
        { FrontLeft,  DEG2RAD(-30.0f), DEG2RAD(0.0f) },
        { FrontRight, DEG2RAD( 30.0f), DEG2RAD(0.0f) }
    };
    static const struct ChanMap StereoWideMap[2] = {
        { FrontLeft,  DEG2RAD(-90.0f), DEG2RAD(0.0f) },
        { FrontRight, DEG2RAD( 90.0f), DEG2RAD(0.0f) }
    };
    static const struct ChanMap RearMap[2] = {
        { BackLeft,  DEG2RAD(-150.0f), DEG2RAD(0.0f) },
        { BackRight, DEG2RAD( 150.0f), DEG2RAD(0.0f) }
    };
    static const struct ChanMap QuadMap[4] = {
        { FrontLeft,  DEG2RAD( -45.0f), DEG2RAD(0.0f) },
        { FrontRight, DEG2RAD(  45.0f), DEG2RAD(0.0f) },
        { BackLeft,   DEG2RAD(-135.0f), DEG2RAD(0.0f) },
        { BackRight,  DEG2RAD( 135.0f), DEG2RAD(0.0f) }
    };
    static const struct ChanMap X51Map[6] = {
        { FrontLeft,   DEG2RAD( -30.0f), DEG2RAD(0.0f) },
        { FrontRight,  DEG2RAD(  30.0f), DEG2RAD(0.0f) },
        { FrontCenter, DEG2RAD(   0.0f), DEG2RAD(0.0f) },
        { LFE, 0.0f, 0.0f },
        { SideLeft,    DEG2RAD(-110.0f), DEG2RAD(0.0f) },
        { SideRight,   DEG2RAD( 110.0f), DEG2RAD(0.0f) }
    };
    static const struct ChanMap X61Map[7] = {
        { FrontLeft,    DEG2RAD(-30.0f), DEG2RAD(0.0f) },
        { FrontRight,   DEG2RAD( 30.0f), DEG2RAD(0.0f) },
        { FrontCenter,  DEG2RAD(  0.0f), DEG2RAD(0.0f) },
        { LFE, 0.0f, 0.0f },
        { BackCenter,   DEG2RAD(180.0f), DEG2RAD(0.0f) },
        { SideLeft,     DEG2RAD(-90.0f), DEG2RAD(0.0f) },
        { SideRight,    DEG2RAD( 90.0f), DEG2RAD(0.0f) }
    };
    static const struct ChanMap X71Map[8] = {
        { FrontLeft,   DEG2RAD( -30.0f), DEG2RAD(0.0f) },
        { FrontRight,  DEG2RAD(  30.0f), DEG2RAD(0.0f) },
        { FrontCenter, DEG2RAD(   0.0f), DEG2RAD(0.0f) },
        { LFE, 0.0f, 0.0f },
        { BackLeft,    DEG2RAD(-150.0f), DEG2RAD(0.0f) },
        { BackRight,   DEG2RAD( 150.0f), DEG2RAD(0.0f) },
        { SideLeft,    DEG2RAD( -90.0f), DEG2RAD(0.0f) },
        { SideRight,   DEG2RAD(  90.0f), DEG2RAD(0.0f) }
    };

    ALCdevice *Device = ALContext->Device;
    ALfloat SourceVolume,ListenerGain,MinVolume,MaxVolume;
    ALbufferlistitem *BufferListItem;
    enum FmtChannels Channels;
    ALfloat DryGain, DryGainHF, DryGainLF;
    ALfloat WetGain[MAX_SENDS];
    ALfloat WetGainHF[MAX_SENDS];
    ALfloat WetGainLF[MAX_SENDS];
    ALuint NumSends, Frequency;
    ALboolean Relative;
    const struct ChanMap *chans = NULL;
    ALuint num_channels = 0;
    ALboolean DirectChannels;
    ALboolean isbformat = AL_FALSE;
    ALfloat Pitch;
    ALuint i, j, c;

    /* Get device properties */
    NumSends  = Device->NumAuxSends;
    Frequency = Device->Frequency;

    /* Get listener properties */
    ListenerGain = ALContext->Listener->Gain;

    /* Get source properties */
    SourceVolume    = ALSource->Gain;
    MinVolume       = ALSource->MinGain;
    MaxVolume       = ALSource->MaxGain;
    Pitch           = ALSource->Pitch;
    Relative        = ALSource->HeadRelative;
    DirectChannels  = ALSource->DirectChannels;

    voice->Direct.OutBuffer = Device->DryBuffer;
    for(i = 0;i < NumSends;i++)
    {
        ALeffectslot *Slot = ALSource->Send[i].Slot;
        if(!Slot && i == 0)
            Slot = Device->DefaultSlot;
        if(!Slot || Slot->EffectType == AL_EFFECT_NULL)
            voice->Send[i].OutBuffer = NULL;
        else
            voice->Send[i].OutBuffer = Slot->WetBuffer;
    }

    /* Calculate the stepping value */
    Channels = FmtMono;
    BufferListItem = ATOMIC_LOAD(&ALSource->queue);
    while(BufferListItem != NULL)
    {
        ALbuffer *ALBuffer;
        if((ALBuffer=BufferListItem->buffer) != NULL)
        {
            Pitch = Pitch * ALBuffer->Frequency / Frequency;
            if(Pitch > (ALfloat)MAX_PITCH)
                voice->Step = MAX_PITCH<<FRACTIONBITS;
            else
            {
                voice->Step = fastf2i(Pitch*FRACTIONONE);
                if(voice->Step == 0)
                    voice->Step = 1;
            }

            Channels = ALBuffer->FmtChannels;
            break;
        }
        BufferListItem = BufferListItem->next;
    }

    /* Calculate gains */
    DryGain  = clampf(SourceVolume, MinVolume, MaxVolume);
    DryGain  *= ALSource->Direct.Gain * ListenerGain;
    DryGainHF = ALSource->Direct.GainHF;
    DryGainLF = ALSource->Direct.GainLF;
    for(i = 0;i < NumSends;i++)
    {
        WetGain[i] = clampf(SourceVolume, MinVolume, MaxVolume);
        WetGain[i]  *= ALSource->Send[i].Gain * ListenerGain;
        WetGainHF[i] = ALSource->Send[i].GainHF;
        WetGainLF[i] = ALSource->Send[i].GainLF;
    }

    switch(Channels)
    {
    case FmtMono:
        chans = MonoMap;
        num_channels = 1;
        break;

    case FmtStereo:
        /* HACK: Place the stereo channels at +/-90 degrees when using non-
         * HRTF stereo output. This helps reduce the "monoization" caused
         * by them panning towards the center. */
        if(Device->FmtChans == DevFmtStereo && !Device->Hrtf)
            chans = StereoWideMap;
        else
            chans = StereoMap;
        num_channels = 2;
        break;

    case FmtRear:
        chans = RearMap;
        num_channels = 2;
        break;

    case FmtQuad:
        chans = QuadMap;
        num_channels = 4;
        break;

    case FmtX51:
        chans = X51Map;
        num_channels = 6;
        break;

    case FmtX61:
        chans = X61Map;
        num_channels = 7;
        break;

    case FmtX71:
        chans = X71Map;
        num_channels = 8;
        break;

    case FmtBFormat2D:
        num_channels = 3;
        isbformat = AL_TRUE;
        DirectChannels = AL_FALSE;
        break;

    case FmtBFormat3D:
        num_channels = 4;
        isbformat = AL_TRUE;
        DirectChannels = AL_FALSE;
        break;
    }

    if(isbformat)
    {
        ALfloat N[3], V[3], U[3];
        ALfloat matrix[4][4];

        /* AT then UP */
        N[0] = ALSource->Orientation[0][0];
        N[1] = ALSource->Orientation[0][1];
        N[2] = ALSource->Orientation[0][2];
        aluNormalize(N);
        V[0] = ALSource->Orientation[1][0];
        V[1] = ALSource->Orientation[1][1];
        V[2] = ALSource->Orientation[1][2];
        aluNormalize(V);
        if(!Relative)
        {
            ALfloat (*restrict lmatrix)[4] = ALContext->Listener->Params.Matrix;
            aluMatrixVector(N, 0.0f, lmatrix);
            aluMatrixVector(V, 0.0f, lmatrix);
        }
        /* Build and normalize right-vector */
        aluCrossproduct(N, V, U);
        aluNormalize(U);

        matrix[0][0] =  1.0f;
        matrix[0][1] =  0.0f;
        matrix[0][2] =  0.0f;
        matrix[0][3] =  0.0f;
        matrix[1][0] =  0.0f;
        matrix[1][1] = -N[2];
        matrix[1][2] = -N[0];
        matrix[1][3] =  N[1];
        matrix[2][0] =  0.0f;
        matrix[2][1] =  U[2];
        matrix[2][2] =  U[0];
        matrix[2][3] = -U[1];
        matrix[3][0] =  0.0f;
        matrix[3][1] = -V[2];
        matrix[3][2] = -V[0];
        matrix[3][3] =  V[1];

        for(c = 0;c < num_channels;c++)
        {
            MixGains *gains = voice->Direct.Mix.Gains[c];
            ALfloat Target[MAX_OUTPUT_CHANNELS];

            ComputeBFormatGains(Device, matrix[c], DryGain, Target);
            for(i = 0;i < MAX_OUTPUT_CHANNELS;i++)
                gains[i].Target = Target[i];
        }
        /* B-Format cannot handle logarithmic gain stepping, since the gain can
         * switch between positive and negative values. */
        voice->Direct.Moving = AL_FALSE;
        UpdateDryStepping(&voice->Direct, num_channels);

        voice->IsHrtf = AL_FALSE;
        for(i = 0;i < NumSends;i++)
            WetGain[i] *= 1.4142f;
    }
    else if(DirectChannels != AL_FALSE)
    {
        for(c = 0;c < num_channels;c++)
        {
            MixGains *gains = voice->Direct.Mix.Gains[c];
            int idx;

            for(j = 0;j < MAX_OUTPUT_CHANNELS;j++)
                gains[j].Target = 0.0f;
            if((idx=GetChannelIdxByName(Device, chans[c].channel)) != -1)
                gains[idx].Target = DryGain;
        }
        UpdateDryStepping(&voice->Direct, num_channels);

        voice->IsHrtf = AL_FALSE;
    }
    else if(Device->Hrtf)
    {
        for(c = 0;c < num_channels;c++)
        {
            if(chans[c].channel == LFE)
            {
                /* Skip LFE */
                voice->Direct.Mix.Hrtf.Params[c].Delay[0] = 0;
                voice->Direct.Mix.Hrtf.Params[c].Delay[1] = 0;
                for(i = 0;i < HRIR_LENGTH;i++)
                {
                    voice->Direct.Mix.Hrtf.Params[c].Coeffs[i][0] = 0.0f;
                    voice->Direct.Mix.Hrtf.Params[c].Coeffs[i][1] = 0.0f;
                }
            }
            else
            {
                /* Get the static HRIR coefficients and delays for this
                 * channel. */
                GetLerpedHrtfCoeffs(Device->Hrtf,
                                    chans[c].elevation, chans[c].angle, 1.0f, DryGain,
                                    voice->Direct.Mix.Hrtf.Params[c].Coeffs,
                                    voice->Direct.Mix.Hrtf.Params[c].Delay);
            }
        }
        voice->Direct.Counter = 0;
        voice->Direct.Moving  = AL_TRUE;
        voice->Direct.Mix.Hrtf.IrSize = GetHrtfIrSize(Device->Hrtf);

        voice->IsHrtf = AL_TRUE;
    }
    else
    {
        for(c = 0;c < num_channels;c++)
        {
            MixGains *gains = voice->Direct.Mix.Gains[c];
            ALfloat Target[MAX_OUTPUT_CHANNELS];

            /* Special-case LFE */
            if(chans[c].channel == LFE)
            {
                int idx;
                for(i = 0;i < MAX_OUTPUT_CHANNELS;i++)
                    gains[i].Target = 0.0f;
                if((idx=GetChannelIdxByName(Device, chans[c].channel)) != -1)
                    gains[idx].Target = DryGain;
                continue;
            }

            ComputeAngleGains(Device, chans[c].angle, chans[c].elevation, DryGain, Target);
            for(i = 0;i < MAX_OUTPUT_CHANNELS;i++)
                gains[i].Target = Target[i];
        }
        UpdateDryStepping(&voice->Direct, num_channels);

        voice->IsHrtf = AL_FALSE;
    }
    for(i = 0;i < NumSends;i++)
    {
        voice->Send[i].Gain.Target = WetGain[i];
        UpdateWetStepping(&voice->Send[i]);
    }

    {
        ALfloat gainhf = maxf(0.01f, DryGainHF);
        ALfloat gainlf = maxf(0.01f, DryGainLF);
        ALfloat hfscale = ALSource->Direct.HFReference / Frequency;
        ALfloat lfscale = ALSource->Direct.LFReference / Frequency;
        for(c = 0;c < num_channels;c++)
        {
            voice->Direct.Filters[c].ActiveType = AF_None;
            if(gainhf != 1.0f) voice->Direct.Filters[c].ActiveType |= AF_LowPass;
            if(gainlf != 1.0f) voice->Direct.Filters[c].ActiveType |= AF_HighPass;
            ALfilterState_setParams(
                &voice->Direct.Filters[c].LowPass, ALfilterType_HighShelf, gainhf,
                hfscale, 0.0f
            );
            ALfilterState_setParams(
                &voice->Direct.Filters[c].HighPass, ALfilterType_LowShelf, gainlf,
                lfscale, 0.0f
            );
        }
    }
    for(i = 0;i < NumSends;i++)
    {
        ALfloat gainhf = maxf(0.01f, WetGainHF[i]);
        ALfloat gainlf = maxf(0.01f, WetGainLF[i]);
        ALfloat hfscale = ALSource->Send[i].HFReference / Frequency;
        ALfloat lfscale = ALSource->Send[i].LFReference / Frequency;
        for(c = 0;c < num_channels;c++)
        {
            voice->Send[i].Filters[c].ActiveType = AF_None;
            if(gainhf != 1.0f) voice->Send[i].Filters[c].ActiveType |= AF_LowPass;
            if(gainlf != 1.0f) voice->Send[i].Filters[c].ActiveType |= AF_HighPass;
            ALfilterState_setParams(
                &voice->Send[i].Filters[c].LowPass, ALfilterType_HighShelf, gainhf,
                hfscale, 0.0f
            );
            ALfilterState_setParams(
                &voice->Send[i].Filters[c].HighPass, ALfilterType_LowShelf, gainlf,
                lfscale, 0.0f
            );
        }
    }
}

ALvoid CalcSourceParams(ALvoice *voice, const ALsource *ALSource, const ALCcontext *ALContext)
{
    ALCdevice *Device = ALContext->Device;
    ALfloat Velocity[3],Direction[3],Position[3],SourceToListener[3];
    ALfloat InnerAngle,OuterAngle,Angle,Distance,ClampedDist;
    ALfloat MinVolume,MaxVolume,MinDist,MaxDist,Rolloff;
    ALfloat ConeVolume,ConeHF,SourceVolume,ListenerGain;
    ALfloat DopplerFactor, SpeedOfSound;
    ALfloat AirAbsorptionFactor;
    ALfloat RoomAirAbsorption[MAX_SENDS];
    ALbufferlistitem *BufferListItem;
    ALfloat Attenuation;
    ALfloat RoomAttenuation[MAX_SENDS];
    ALfloat MetersPerUnit;
    ALfloat RoomRolloffBase;
    ALfloat RoomRolloff[MAX_SENDS];
    ALfloat DecayDistance[MAX_SENDS];
    ALfloat DryGain;
    ALfloat DryGainHF;
    ALfloat DryGainLF;
    ALboolean DryGainHFAuto;
    ALfloat WetGain[MAX_SENDS];
    ALfloat WetGainHF[MAX_SENDS];
    ALfloat WetGainLF[MAX_SENDS];
    ALboolean WetGainAuto;
    ALboolean WetGainHFAuto;
    ALfloat Pitch;
    ALuint Frequency;
    ALint NumSends;
    ALint i, j;

    DryGainHF = 1.0f;
    DryGainLF = 1.0f;
    for(i = 0;i < MAX_SENDS;i++)
    {
        WetGainHF[i] = 1.0f;
        WetGainLF[i] = 1.0f;
    }

    /* Get context/device properties */
    DopplerFactor = ALContext->DopplerFactor * ALSource->DopplerFactor;
    SpeedOfSound  = ALContext->SpeedOfSound * ALContext->DopplerVelocity;
    NumSends      = Device->NumAuxSends;
    Frequency     = Device->Frequency;

    /* Get listener properties */
    ListenerGain  = ALContext->Listener->Gain;
    MetersPerUnit = ALContext->Listener->MetersPerUnit;

    /* Get source properties */
    SourceVolume   = ALSource->Gain;
    MinVolume      = ALSource->MinGain;
    MaxVolume      = ALSource->MaxGain;
    Pitch          = ALSource->Pitch;
    Position[0]    = ALSource->Position[0];
    Position[1]    = ALSource->Position[1];
    Position[2]    = ALSource->Position[2];
    Direction[0]   = ALSource->Direction[0];
    Direction[1]   = ALSource->Direction[1];
    Direction[2]   = ALSource->Direction[2];
    Velocity[0]    = ALSource->Velocity[0];
    Velocity[1]    = ALSource->Velocity[1];
    Velocity[2]    = ALSource->Velocity[2];
    MinDist        = ALSource->RefDistance;
    MaxDist        = ALSource->MaxDistance;
    Rolloff        = ALSource->RollOffFactor;
    InnerAngle     = ALSource->InnerAngle;
    OuterAngle     = ALSource->OuterAngle;
    AirAbsorptionFactor = ALSource->AirAbsorptionFactor;
    DryGainHFAuto   = ALSource->DryGainHFAuto;
    WetGainAuto     = ALSource->WetGainAuto;
    WetGainHFAuto   = ALSource->WetGainHFAuto;
    RoomRolloffBase = ALSource->RoomRolloffFactor;

    voice->Direct.OutBuffer = Device->DryBuffer;
    for(i = 0;i < NumSends;i++)
    {
        ALeffectslot *Slot = ALSource->Send[i].Slot;

        if(!Slot && i == 0)
            Slot = Device->DefaultSlot;
        if(!Slot || Slot->EffectType == AL_EFFECT_NULL)
        {
            Slot = NULL;
            RoomRolloff[i] = 0.0f;
            DecayDistance[i] = 0.0f;
            RoomAirAbsorption[i] = 1.0f;
        }
        else if(Slot->AuxSendAuto)
        {
            RoomRolloff[i] = RoomRolloffBase;
            if(IsReverbEffect(Slot->EffectType))
            {
                RoomRolloff[i] += Slot->EffectProps.Reverb.RoomRolloffFactor;
                DecayDistance[i] = Slot->EffectProps.Reverb.DecayTime *
                                   SPEEDOFSOUNDMETRESPERSEC;
                RoomAirAbsorption[i] = Slot->EffectProps.Reverb.AirAbsorptionGainHF;
            }
            else
            {
                DecayDistance[i] = 0.0f;
                RoomAirAbsorption[i] = 1.0f;
            }
        }
        else
        {
            /* If the slot's auxiliary send auto is off, the data sent to the
             * effect slot is the same as the dry path, sans filter effects */
            RoomRolloff[i] = Rolloff;
            DecayDistance[i] = 0.0f;
            RoomAirAbsorption[i] = AIRABSORBGAINHF;
        }

        if(!Slot || Slot->EffectType == AL_EFFECT_NULL)
            voice->Send[i].OutBuffer = NULL;
        else
            voice->Send[i].OutBuffer = Slot->WetBuffer;
    }

    /* Transform source to listener space (convert to head relative) */
    if(ALSource->HeadRelative == AL_FALSE)
    {
        ALfloat (*restrict Matrix)[4] = ALContext->Listener->Params.Matrix;
        /* Transform source vectors */
        aluMatrixVector(Position, 1.0f, Matrix);
        aluMatrixVector(Direction, 0.0f, Matrix);
        aluMatrixVector(Velocity, 0.0f, Matrix);
    }
    else
    {
        const ALfloat *ListenerVel = ALContext->Listener->Params.Velocity;
        /* Offset the source velocity to be relative of the listener velocity */
        Velocity[0] += ListenerVel[0];
        Velocity[1] += ListenerVel[1];
        Velocity[2] += ListenerVel[2];
    }

    SourceToListener[0] = -Position[0];
    SourceToListener[1] = -Position[1];
    SourceToListener[2] = -Position[2];
    aluNormalize(SourceToListener);
    aluNormalize(Direction);

    /* Calculate distance attenuation */
    Distance = sqrtf(aluDotproduct(Position, Position));
    ClampedDist = Distance;

    Attenuation = 1.0f;
    for(i = 0;i < NumSends;i++)
        RoomAttenuation[i] = 1.0f;
    switch(ALContext->SourceDistanceModel ? ALSource->DistanceModel :
                                            ALContext->DistanceModel)
    {
        case InverseDistanceClamped:
            ClampedDist = clampf(ClampedDist, MinDist, MaxDist);
            if(MaxDist < MinDist)
                break;
            /*fall-through*/
        case InverseDistance:
            if(MinDist > 0.0f)
            {
                if((MinDist + (Rolloff * (ClampedDist - MinDist))) > 0.0f)
                    Attenuation = MinDist / (MinDist + (Rolloff * (ClampedDist - MinDist)));
                for(i = 0;i < NumSends;i++)
                {
                    if((MinDist + (RoomRolloff[i] * (ClampedDist - MinDist))) > 0.0f)
                        RoomAttenuation[i] = MinDist / (MinDist + (RoomRolloff[i] * (ClampedDist - MinDist)));
                }
            }
            break;

        case LinearDistanceClamped:
            ClampedDist = clampf(ClampedDist, MinDist, MaxDist);
            if(MaxDist < MinDist)
                break;
            /*fall-through*/
        case LinearDistance:
            if(MaxDist != MinDist)
            {
                Attenuation = 1.0f - (Rolloff*(ClampedDist-MinDist)/(MaxDist - MinDist));
                Attenuation = maxf(Attenuation, 0.0f);
                for(i = 0;i < NumSends;i++)
                {
                    RoomAttenuation[i] = 1.0f - (RoomRolloff[i]*(ClampedDist-MinDist)/(MaxDist - MinDist));
                    RoomAttenuation[i] = maxf(RoomAttenuation[i], 0.0f);
                }
            }
            break;

        case ExponentDistanceClamped:
            ClampedDist = clampf(ClampedDist, MinDist, MaxDist);
            if(MaxDist < MinDist)
                break;
            /*fall-through*/
        case ExponentDistance:
            if(ClampedDist > 0.0f && MinDist > 0.0f)
            {
                Attenuation = powf(ClampedDist/MinDist, -Rolloff);
                for(i = 0;i < NumSends;i++)
                    RoomAttenuation[i] = powf(ClampedDist/MinDist, -RoomRolloff[i]);
            }
            break;

        case DisableDistance:
            ClampedDist = MinDist;
            break;
    }

    /* Source Gain + Attenuation */
    DryGain = SourceVolume * Attenuation;
    for(i = 0;i < NumSends;i++)
        WetGain[i] = SourceVolume * RoomAttenuation[i];

    /* Distance-based air absorption */
    if(AirAbsorptionFactor > 0.0f && ClampedDist > MinDist)
    {
        ALfloat meters = maxf(ClampedDist-MinDist, 0.0f) * MetersPerUnit;
        DryGainHF *= powf(AIRABSORBGAINHF, AirAbsorptionFactor*meters);
        for(i = 0;i < NumSends;i++)
            WetGainHF[i] *= powf(RoomAirAbsorption[i], AirAbsorptionFactor*meters);
    }

    if(WetGainAuto)
    {
        ALfloat ApparentDist = 1.0f/maxf(Attenuation, 0.00001f) - 1.0f;

        /* Apply a decay-time transformation to the wet path, based on the
         * attenuation of the dry path.
         *
         * Using the apparent distance, based on the distance attenuation, the
         * initial decay of the reverb effect is calculated and applied to the
         * wet path.
         */
        for(i = 0;i < NumSends;i++)
        {
            if(DecayDistance[i] > 0.0f)
                WetGain[i] *= powf(0.001f/*-60dB*/, ApparentDist/DecayDistance[i]);
        }
    }

    /* Calculate directional soundcones */
    Angle = RAD2DEG(acosf(aluDotproduct(Direction,SourceToListener)) * ConeScale) * 2.0f;
    if(Angle > InnerAngle && Angle <= OuterAngle)
    {
        ALfloat scale = (Angle-InnerAngle) / (OuterAngle-InnerAngle);
        ConeVolume = lerp(1.0f, ALSource->OuterGain, scale);
        ConeHF = lerp(1.0f, ALSource->OuterGainHF, scale);
    }
    else if(Angle > OuterAngle)
    {
        ConeVolume = ALSource->OuterGain;
        ConeHF = ALSource->OuterGainHF;
    }
    else
    {
        ConeVolume = 1.0f;
        ConeHF = 1.0f;
    }

    DryGain *= ConeVolume;
    if(WetGainAuto)
    {
        for(i = 0;i < NumSends;i++)
            WetGain[i] *= ConeVolume;
    }
    if(DryGainHFAuto)
        DryGainHF *= ConeHF;
    if(WetGainHFAuto)
    {
        for(i = 0;i < NumSends;i++)
            WetGainHF[i] *= ConeHF;
    }

    /* Clamp to Min/Max Gain */
    DryGain = clampf(DryGain, MinVolume, MaxVolume);
    for(i = 0;i < NumSends;i++)
        WetGain[i] = clampf(WetGain[i], MinVolume, MaxVolume);

    /* Apply gain and frequency filters */
    DryGain   *= ALSource->Direct.Gain * ListenerGain;
    DryGainHF *= ALSource->Direct.GainHF;
    DryGainLF *= ALSource->Direct.GainLF;
    for(i = 0;i < NumSends;i++)
    {
        WetGain[i]   *= ALSource->Send[i].Gain * ListenerGain;
        WetGainHF[i] *= ALSource->Send[i].GainHF;
        WetGainLF[i] *= ALSource->Send[i].GainLF;
    }

    /* Calculate velocity-based doppler effect */
    if(DopplerFactor > 0.0f)
    {
        const ALfloat *ListenerVel = ALContext->Listener->Params.Velocity;
        ALfloat VSS, VLS;

        if(SpeedOfSound < 1.0f)
        {
            DopplerFactor *= 1.0f/SpeedOfSound;
            SpeedOfSound   = 1.0f;
        }

        VSS = aluDotproduct(Velocity, SourceToListener) * DopplerFactor;
        VLS = aluDotproduct(ListenerVel, SourceToListener) * DopplerFactor;

        Pitch *= clampf(SpeedOfSound-VLS, 1.0f, SpeedOfSound*2.0f - 1.0f) /
                 clampf(SpeedOfSound-VSS, 1.0f, SpeedOfSound*2.0f - 1.0f);
    }

    BufferListItem = ATOMIC_LOAD(&ALSource->queue);
    while(BufferListItem != NULL)
    {
        ALbuffer *ALBuffer;
        if((ALBuffer=BufferListItem->buffer) != NULL)
        {
            /* Calculate fixed-point stepping value, based on the pitch, buffer
             * frequency, and output frequency. */
            Pitch = Pitch * ALBuffer->Frequency / Frequency;
            if(Pitch > (ALfloat)MAX_PITCH)
                voice->Step = MAX_PITCH<<FRACTIONBITS;
            else
            {
                voice->Step = fastf2i(Pitch*FRACTIONONE);
                if(voice->Step == 0)
                    voice->Step = 1;
            }

            break;
        }
        BufferListItem = BufferListItem->next;
    }

    if(Device->Hrtf)
    {
        /* Use a binaural HRTF algorithm for stereo headphone playback */
        ALfloat delta, ev = 0.0f, az = 0.0f;
        ALfloat radius = ALSource->Radius;
        ALfloat dirfact = 1.0f;

        if(Distance > FLT_EPSILON)
        {
            ALfloat invlen = 1.0f/Distance;
            Position[0] *= invlen;
            Position[1] *= invlen;
            Position[2] *= invlen;

            /* Calculate elevation and azimuth only when the source is not at
             * the listener. This prevents +0 and -0 Z from producing
             * inconsistent panning. Also, clamp Y in case FP precision errors
             * cause it to land outside of -1..+1. */
            ev = asinf(clampf(Position[1], -1.0f, 1.0f));
            az = atan2f(Position[0], -Position[2]*ZScale);
        }
        if(radius > Distance)
            dirfact *= Distance / radius;

        /* Check to see if the HRIR is already moving. */
        if(voice->Direct.Moving)
        {
            /* Calculate the normalized HRTF transition factor (delta). */
            delta = CalcHrtfDelta(voice->Direct.Mix.Hrtf.Gain, DryGain,
                                  voice->Direct.Mix.Hrtf.Dir, Position);
            /* If the delta is large enough, get the moving HRIR target
             * coefficients, target delays, steppping values, and counter. */
            if(delta > 0.001f)
            {
                ALuint counter = GetMovingHrtfCoeffs(Device->Hrtf,
                    ev, az, dirfact, DryGain, delta, voice->Direct.Counter,
                    voice->Direct.Mix.Hrtf.Params[0].Coeffs, voice->Direct.Mix.Hrtf.Params[0].Delay,
                    voice->Direct.Mix.Hrtf.Params[0].CoeffStep, voice->Direct.Mix.Hrtf.Params[0].DelayStep
                );
                voice->Direct.Counter = counter;
                voice->Direct.Mix.Hrtf.Gain = DryGain;
                voice->Direct.Mix.Hrtf.Dir[0] = Position[0];
                voice->Direct.Mix.Hrtf.Dir[1] = Position[1];
                voice->Direct.Mix.Hrtf.Dir[2] = Position[2];
            }
        }
        else
        {
            /* Get the initial (static) HRIR coefficients and delays. */
            GetLerpedHrtfCoeffs(Device->Hrtf, ev, az, dirfact, DryGain,
                                voice->Direct.Mix.Hrtf.Params[0].Coeffs,
                                voice->Direct.Mix.Hrtf.Params[0].Delay);
            voice->Direct.Counter = 0;
            voice->Direct.Moving  = AL_TRUE;
            voice->Direct.Mix.Hrtf.Gain = DryGain;
            voice->Direct.Mix.Hrtf.Dir[0] = Position[0];
            voice->Direct.Mix.Hrtf.Dir[1] = Position[1];
            voice->Direct.Mix.Hrtf.Dir[2] = Position[2];
        }
        voice->Direct.Mix.Hrtf.IrSize = GetHrtfIrSize(Device->Hrtf);

        voice->IsHrtf = AL_TRUE;
    }
    else
    {
        MixGains *gains = voice->Direct.Mix.Gains[0];
        ALfloat radius = ALSource->Radius;
        ALfloat Target[MAX_OUTPUT_CHANNELS];

        /* Normalize the length, and compute panned gains. */
        if(!(Distance > FLT_EPSILON) && !(radius > FLT_EPSILON))
        {
            const ALfloat front[3] = { 0.0f, 0.0f, -1.0f };
            ComputeDirectionalGains(Device, front, DryGain, Target);
        }
        else
        {
            ALfloat invlen = 1.0f/maxf(Distance, radius);
            Position[0] *= invlen;
            Position[1] *= invlen;
            Position[2] *= invlen;
            ComputeDirectionalGains(Device, Position, DryGain, Target);
        }

        for(j = 0;j < MAX_OUTPUT_CHANNELS;j++)
            gains[j].Target = Target[j];
        UpdateDryStepping(&voice->Direct, 1);

        voice->IsHrtf = AL_FALSE;
    }
    for(i = 0;i < NumSends;i++)
    {
        voice->Send[i].Gain.Target = WetGain[i];
        UpdateWetStepping(&voice->Send[i]);
    }

    {
        ALfloat gainhf = maxf(0.01f, DryGainHF);
        ALfloat gainlf = maxf(0.01f, DryGainLF);
        ALfloat hfscale = ALSource->Direct.HFReference / Frequency;
        ALfloat lfscale = ALSource->Direct.LFReference / Frequency;
        voice->Direct.Filters[0].ActiveType = AF_None;
        if(gainhf != 1.0f) voice->Direct.Filters[0].ActiveType |= AF_LowPass;
        if(gainlf != 1.0f) voice->Direct.Filters[0].ActiveType |= AF_HighPass;
        ALfilterState_setParams(
            &voice->Direct.Filters[0].LowPass, ALfilterType_HighShelf, gainhf,
            hfscale, 0.0f
        );
        ALfilterState_setParams(
            &voice->Direct.Filters[0].HighPass, ALfilterType_LowShelf, gainlf,
            lfscale, 0.0f
        );
    }
    for(i = 0;i < NumSends;i++)
    {
        ALfloat gainhf = maxf(0.01f, WetGainHF[i]);
        ALfloat gainlf = maxf(0.01f, WetGainLF[i]);
        ALfloat hfscale = ALSource->Send[i].HFReference / Frequency;
        ALfloat lfscale = ALSource->Send[i].LFReference / Frequency;
        voice->Send[i].Filters[0].ActiveType = AF_None;
        if(gainhf != 1.0f) voice->Send[i].Filters[0].ActiveType |= AF_LowPass;
        if(gainlf != 1.0f) voice->Send[i].Filters[0].ActiveType |= AF_HighPass;
        ALfilterState_setParams(
            &voice->Send[i].Filters[0].LowPass, ALfilterType_HighShelf, gainhf,
            hfscale, 0.0f
        );
        ALfilterState_setParams(
            &voice->Send[i].Filters[0].HighPass, ALfilterType_LowShelf, gainlf,
            lfscale, 0.0f
        );
    }
}


static inline ALint aluF2I25(ALfloat val)
{
    /* Clamp the value between -1 and +1. This handles that with only a single branch. */
    if(fabsf(val) > 1.0f)
        val = (ALfloat)((0.0f < val) - (val < 0.0f));
    /* Convert to a signed integer, between -16777215 and +16777215. */
    return fastf2i(val*16777215.0f);
}

static inline ALfloat aluF2F(ALfloat val)
{ return val; }
static inline ALint aluF2I(ALfloat val)
{ return aluF2I25(val)<<7; }
static inline ALuint aluF2UI(ALfloat val)
{ return aluF2I(val)+2147483648u; }
static inline ALshort aluF2S(ALfloat val)
{ return aluF2I25(val)>>9; }
static inline ALushort aluF2US(ALfloat val)
{ return aluF2S(val)+32768; }
static inline ALbyte aluF2B(ALfloat val)
{ return aluF2I25(val)>>17; }
static inline ALubyte aluF2UB(ALfloat val)
{ return aluF2B(val)+128; }

#define DECL_TEMPLATE(T, func)                                                \
static void Write_##T(const ALfloatBUFFERSIZE *InBuffer, ALvoid *OutBuffer,   \
                      ALuint SamplesToDo, ALuint numchans)                    \
{                                                                             \
    ALuint i, j;                                                              \
    for(j = 0;j < numchans;j++)                                               \
    {                                                                         \
        const ALfloat *in = InBuffer[j];                                      \
        T *restrict out = (T*)OutBuffer + j;                                  \
        for(i = 0;i < SamplesToDo;i++)                                        \
            out[i*numchans] = func(in[i]);                                    \
    }                                                                         \
}

DECL_TEMPLATE(ALfloat, aluF2F)
DECL_TEMPLATE(ALuint, aluF2UI)
DECL_TEMPLATE(ALint, aluF2I)
DECL_TEMPLATE(ALushort, aluF2US)
DECL_TEMPLATE(ALshort, aluF2S)
DECL_TEMPLATE(ALubyte, aluF2UB)
DECL_TEMPLATE(ALbyte, aluF2B)

#undef DECL_TEMPLATE


ALvoid aluMixData(ALCdevice *device, ALvoid *buffer, ALsizei size)
{
    ALuint SamplesToDo;
    ALeffectslot **slot, **slot_end;
    ALvoice *voice, *voice_end;
    ALCcontext *ctx;
    FPUCtl oldMode;
    ALuint i, c;

    SetMixerFPUMode(&oldMode);

    while(size > 0)
    {
        IncrementRef(&device->MixCount);

        SamplesToDo = minu(size, BUFFERSIZE);
        for(c = 0;c < MAX_OUTPUT_CHANNELS;c++)
            memset(device->DryBuffer[c], 0, SamplesToDo*sizeof(ALfloat));

        V0(device->Backend,lock)();
        V(device->Synth,process)(SamplesToDo, device->DryBuffer);

        ctx = ATOMIC_LOAD(&device->ContextList);
        while(ctx)
        {
            ALenum DeferUpdates = ctx->DeferUpdates;
            ALenum UpdateSources = AL_FALSE;

            if(!DeferUpdates)
                UpdateSources = ATOMIC_EXCHANGE(ALenum, &ctx->UpdateSources, AL_FALSE);

            if(UpdateSources)
                CalcListenerParams(ctx->Listener);

            /* source processing */
            voice = ctx->Voices;
            voice_end = voice + ctx->VoiceCount;
            while(voice != voice_end)
            {
                ALsource *source = voice->Source;
                if(!source) goto next;

                if(source->state != AL_PLAYING && source->state != AL_PAUSED)
                {
                    voice->Source = NULL;
                    goto next;
                }

                if(!DeferUpdates && (ATOMIC_EXCHANGE(ALenum, &source->NeedsUpdate, AL_FALSE) ||
                                     UpdateSources))
                    voice->Update(voice, source, ctx);

                if(source->state != AL_PAUSED)
                    MixSource(voice, source, device, SamplesToDo);
            next:
                voice++;
            }

            /* effect slot processing */
            slot = VECTOR_ITER_BEGIN(ctx->ActiveAuxSlots);
            slot_end = VECTOR_ITER_END(ctx->ActiveAuxSlots);
            while(slot != slot_end)
            {
                if(!DeferUpdates && ATOMIC_EXCHANGE(ALenum, &(*slot)->NeedsUpdate, AL_FALSE))
                    V((*slot)->EffectState,update)(device, *slot);

                V((*slot)->EffectState,process)(SamplesToDo, (*slot)->WetBuffer[0],
                                                device->DryBuffer, device->NumChannels);

                for(i = 0;i < SamplesToDo;i++)
                    (*slot)->WetBuffer[0][i] = 0.0f;

                slot++;
            }

            ctx = ctx->next;
        }

        slot = &device->DefaultSlot;
        if(*slot != NULL)
        {
            if(ATOMIC_EXCHANGE(ALenum, &(*slot)->NeedsUpdate, AL_FALSE))
                V((*slot)->EffectState,update)(device, *slot);

            V((*slot)->EffectState,process)(SamplesToDo, (*slot)->WetBuffer[0],
                                            device->DryBuffer, device->NumChannels);

            for(i = 0;i < SamplesToDo;i++)
                (*slot)->WetBuffer[0][i] = 0.0f;
        }

        /* Increment the clock time. Every second's worth of samples is
         * converted and added to clock base so that large sample counts don't
         * overflow during conversion. This also guarantees an exact, stable
         * conversion. */
        device->SamplesDone += SamplesToDo;
        device->ClockBase += (device->SamplesDone/device->Frequency) * DEVICE_CLOCK_RES;
        device->SamplesDone %= device->Frequency;
        V0(device->Backend,unlock)();

        if(device->Bs2b)
        {
            /* Apply binaural/crossfeed filter */
            for(i = 0;i < SamplesToDo;i++)
            {
                float samples[2];
                samples[0] = device->DryBuffer[0][i];
                samples[1] = device->DryBuffer[1][i];
                bs2b_cross_feed(device->Bs2b, samples);
                device->DryBuffer[0][i] = samples[0];
                device->DryBuffer[1][i] = samples[1];
            }
        }

        if(buffer)
        {
            switch(device->FmtType)
            {
                case DevFmtByte:
                    Write_ALbyte(device->DryBuffer, buffer, SamplesToDo, device->NumChannels);
                    buffer = (char*)buffer + SamplesToDo*device->NumChannels*sizeof(ALbyte);
                    break;
                case DevFmtUByte:
                    Write_ALubyte(device->DryBuffer, buffer, SamplesToDo, device->NumChannels);
                    buffer = (char*)buffer + SamplesToDo*device->NumChannels*sizeof(ALubyte);
                    break;
                case DevFmtShort:
                    Write_ALshort(device->DryBuffer, buffer, SamplesToDo, device->NumChannels);
                    buffer = (char*)buffer + SamplesToDo*device->NumChannels*sizeof(ALshort);
                    break;
                case DevFmtUShort:
                    Write_ALushort(device->DryBuffer, buffer, SamplesToDo, device->NumChannels);
                    buffer = (char*)buffer + SamplesToDo*device->NumChannels*sizeof(ALushort);
                    break;
                case DevFmtInt:
                    Write_ALint(device->DryBuffer, buffer, SamplesToDo, device->NumChannels);
                    buffer = (char*)buffer + SamplesToDo*device->NumChannels*sizeof(ALint);
                    break;
                case DevFmtUInt:
                    Write_ALuint(device->DryBuffer, buffer, SamplesToDo, device->NumChannels);
                    buffer = (char*)buffer + SamplesToDo*device->NumChannels*sizeof(ALuint);
                    break;
                case DevFmtFloat:
                    Write_ALfloat(device->DryBuffer, buffer, SamplesToDo, device->NumChannels);
                    buffer = (char*)buffer + SamplesToDo*device->NumChannels*sizeof(ALfloat);
                    break;
            }
        }

        size -= SamplesToDo;
        IncrementRef(&device->MixCount);
    }

    RestoreFPUMode(&oldMode);
}


ALvoid aluHandleDisconnect(ALCdevice *device)
{
    ALCcontext *Context;

    device->Connected = ALC_FALSE;

    Context = ATOMIC_LOAD(&device->ContextList);
    while(Context)
    {
        ALvoice *voice, *voice_end;

        voice = Context->Voices;
        voice_end = voice + Context->VoiceCount;
        while(voice != voice_end)
        {
            ALsource *source = voice->Source;
            voice->Source = NULL;

            if(source && source->state == AL_PLAYING)
            {
                source->state = AL_STOPPED;
                ATOMIC_STORE(&source->current_buffer, NULL);
                source->position = 0;
                source->position_fraction = 0;
            }

            voice++;
        }
        Context->VoiceCount = 0;

        Context = Context->next;
    }
}
