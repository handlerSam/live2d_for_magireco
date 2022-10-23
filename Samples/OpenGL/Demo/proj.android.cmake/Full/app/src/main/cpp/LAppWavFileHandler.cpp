/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */



#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <iostream>
#include <string>
#include <malloc.h>

// ref:https://github.com/lieff/minimp3/blob/master/minimp3.h
#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#include <sys/stat.h>
#include "LAppPal.hpp"
using namespace std;

char* ret_str;

char* wavReturn_int16(int16_t* buffer, int sampleRate, uint32_t totalSampleCount, int channels = 1) {
    LAppPal::PrintLog("[SGY3] sampleRate %d",sampleRate);
    //ret_str = (char*)malloc(5 * 1024 * 1024);
    ret_str = new char[5*1024*1024];
    char* str_p = ret_str;
    //Modify the buffer length written
    totalSampleCount *= sizeof(int16_t)*channels;
    int nbit = 16;
    int FORMAT_PCM = 1;
    int nbyte = nbit / 8;
    char text[4] = { 'R', 'I', 'F', 'F' };
    uint32_t long_number = 36 + totalSampleCount;
    memcpy(str_p, text, 4);
    str_p += 4;
    memcpy(str_p, &long_number, 4);
    str_p += 4;
    text[0] = 'W';
    text[1] = 'A';
    text[2] = 'V';
    text[3] = 'E';
    memcpy(str_p, text, 4);
    str_p += 4;
    text[0] = 'f';
    text[1] = 'm';
    text[2] = 't';
    text[3] = ' ';
    memcpy(str_p, text, 4);
    str_p += 4;

    long_number = 16;
    memcpy(str_p, &long_number, 4);
    str_p += 4;
    int16_t short_number = FORMAT_PCM;//Default audio format
    memcpy(str_p, &short_number, 2);
    str_p += 2;
    short_number = channels; //  Number of audio channels
    memcpy(str_p, &short_number, 2);
    str_p += 2;
    long_number = sampleRate; //  Sampling Rate
    memcpy(str_p, &long_number, 4);
    str_p += 4;
    long_number = sampleRate * nbyte; //  Bit rate
    memcpy(str_p, &long_number, 4);
    str_p += 4;
    short_number = nbyte; //  Block alignment
    memcpy(str_p, &short_number, 2);
    str_p += 2;
    short_number = nbit; //  Sampling accuracy
    memcpy(str_p, &short_number, 2);
    str_p += 2;
    char data[4] = { 'd', 'a', 't', 'a' };
    memcpy(str_p, data, 4);
    str_p += 4;
    long_number = totalSampleCount;
    memcpy(str_p, &long_number, 4);
    str_p += 4;
    memcpy(str_p, buffer, totalSampleCount);
    str_p += totalSampleCount;
    return ret_str;
}

//mp3 decoding
int16_t* DecodeMp3ToBuffer(unsigned char* fileContent, uint32_t *sampleRate, uint32_t *totalSampleCount, unsigned int *channels, int music_size)
{
    LAppPal::PrintLog("[SGY3] -22");
    int alloc_samples = 1024 * 1024, num_samples = 0;
    int16_t *music_buf = (int16_t *)malloc(alloc_samples * 2 * 2);
    //unsigned char *file_buf = (unsigned char *)getFileBuffer(filename, &music_size);
    unsigned char *file_buf = (unsigned char *)fileContent;
    if (file_buf != NULL)
    {
        unsigned char *buf = file_buf;
        mp3dec_frame_info_t info;
        mp3dec_t dec;
        LAppPal::PrintLog("[SGY3] -23");
        mp3dec_init(&dec);
        LAppPal::PrintLog("[SGY3] -24");
        for (;;)
        {
            int16_t frame_buf[2 * 1152];
            int samples = mp3dec_decode_frame(&dec, buf, music_size, frame_buf, &info);
            LAppPal::PrintLog("[SGY3] -25");
            if (alloc_samples < (num_samples + samples))
            {
                alloc_samples *= 2;
                int16_t* tmp = (int16_t *)realloc(music_buf, alloc_samples * 2 * info.channels);
                if (tmp)
                    music_buf = tmp;
            }
            if (music_buf)
                memcpy(music_buf + num_samples*info.channels, frame_buf, samples*info.channels * 2);
            num_samples += samples;
            if (info.frame_bytes <= 0 || music_size <= (info.frame_bytes + 4))
                break;
            buf += info.frame_bytes;
            music_size -= info.frame_bytes;
        }
        if (alloc_samples > num_samples)
        {
            int16_t* tmp = (int16_t *)realloc(music_buf, num_samples * 2 * info.channels);
            if (tmp)
                music_buf = tmp;
        }
        LAppPal::PrintLog("[SGY3] -26, sampleRate:%d, channels:%d,totalSampleCount:%d",info.hz,info.channels,num_samples);
        if (sampleRate)
            *sampleRate = info.hz;
        if (channels)
            *channels = info.channels;
        if (num_samples)
            *totalSampleCount = num_samples;

        free(file_buf);
        return music_buf;
    }
    if (music_buf)
        free(music_buf);
    LAppPal::PrintLog("[SGY3] -27");
    return 0;
}

unsigned char* convertMP3ToWAV(unsigned char* fileContent, int music_size)
{
    LAppPal::PrintLog("[SGY3] -20");
    // if (argc < 2) return -1;
    // char* in_file = argv[1];
    //Total audio samples
    uint32_t totalSampleCount = 0;
    //Audio sampling rate
    uint32_t sampleRate = 0;
    //Number of channels
    unsigned int channels = 0;
    int16_t* wavBuffer = NULL;
    wavBuffer = DecodeMp3ToBuffer(fileContent, &sampleRate, &totalSampleCount, &channels,music_size);
    LAppPal::PrintLog("[SGY3] -21");
    //Save result
    wavReturn_int16(wavBuffer, sampleRate, totalSampleCount, channels);
    //std::cout << "  Save time: " << int(nSaveTime * 1000) << "  millisecond" << std::endl;
    LAppPal::PrintLog("[SGY3] -31");
//    if (wavBuffer)
//    {
//        free(wavBuffer);
//    }
    //getchar();
    //std::cout << "Press any key to exit the program \n" << std::endl;
    LAppPal::PrintLog("[SGY3] -30");
    return (unsigned char*)ret_str;
}



















#include "LAppWavFileHandler.hpp"
#include <cmath>
#include <cstdint>
#include "LAppPal.hpp"

LAppWavFileHandler::LAppWavFileHandler()
    : _pcmData(NULL), _userTimeSeconds(0.0f), _lastRms(0.0f), _sampleOffset(0)
{
}

LAppWavFileHandler::~LAppWavFileHandler()
{
    if (_pcmData != NULL)
    {
        ReleasePcmData();
    }
}

Csm::csmBool LAppWavFileHandler::Update(Csm::csmFloat32 deltaTimeSeconds)
{
    Csm::csmUint32 goalOffset;
    Csm::csmFloat32 rms;
    // データロード前/ファイル末尾に達した場合は更新しない
    // if ((_pcmData == NULL) || (_sampleOffset >= _wavFileInfo._samplesPerChannel))
    // {
    //     _lastRms = 0.0f;
    //     return false;
    // }

    if ((_pcmData == NULL))
    {
        //LAppPal::PrintLog("[SGY3] 20");
        _lastRms = 0.0f;
        return false;
    }

    if ((_sampleOffset >= _wavFileInfo._samplesPerChannel))
    {
        //LAppPal::PrintLog("[SGY3] 21: %d, %d",_sampleOffset,_wavFileInfo._samplesPerChannel);
        _lastRms = 0.0f;
        return false;
    }
    //LAppPal::PrintLog("[SGY3] 22");
    // 経過時間後の状態を保持
    _userTimeSeconds += deltaTimeSeconds;
    goalOffset = static_cast<Csm::csmUint32>(_userTimeSeconds * _wavFileInfo._samplingRate);
    if (goalOffset > _wavFileInfo._samplesPerChannel)
    {
        goalOffset = _wavFileInfo._samplesPerChannel;
    }
    //LAppPal::PrintLog("[SGY3] 23");
    // RMS計測
    rms = 0.0f;
    for (Csm::csmUint32 channelCount = 0; channelCount < _wavFileInfo._numberOfChannels; channelCount++)
    {
        for (Csm::csmUint32 sampleCount = _sampleOffset; sampleCount < goalOffset; sampleCount++)
        {
            Csm::csmFloat32 pcm = _pcmData[channelCount][sampleCount];
            rms += pcm * pcm;
        }
    }
    rms = sqrt(rms / (_wavFileInfo._numberOfChannels * (goalOffset - _sampleOffset)));
    //LAppPal::PrintLog("[SGY3] 24");
    _lastRms = rms;
    _sampleOffset = goalOffset;
    return true;
}

void LAppWavFileHandler::Start(const Csm::csmString &filePath)
{
    // WAVファイルのロード
    if (!LoadWavFile(filePath))
    {
        return;
    }

    // サンプル参照位置を初期化
    _sampleOffset = 0;
    _userTimeSeconds = 0.0f;

    // RMS値をリセット
    _lastRms = 0.0f;
}

Csm::csmFloat32 LAppWavFileHandler::GetRms() const
{
    return _lastRms;
}

Csm::csmBool LAppWavFileHandler::LoadWavFile(const Csm::csmString &filePath)
{
    Csm::csmBool ret;
    LAppPal::PrintLog("[SGY3] -4");
    // 既にwavファイルロード済みならば領域開放
    if (_pcmData != NULL)
    {
        ReleasePcmData();
    }

    LAppPal::PrintLog("[SGY3] -3");
    // ファイルロード

    _byteReader._fileByte = convertMP3ToWAV(LAppPal::LoadFileAsBytes(filePath.GetRawString(), &(_byteReader._fileSize), true),(_byteReader._fileSize));
    //_byteReader._fileByte = LAppPal::LoadFileAsBytes(filePath.GetRawString(), &(_byteReader._fileSize), true),(_byteReader._fileSize);

    _byteReader._readOffset = 0;
    LAppPal::PrintLog("[SGY3] -2");
    // ファイルロードに失敗しているか、先頭のシグネチャ"RIFF"を入れるサイズもない場合は失敗
    if ((_byteReader._fileByte == NULL) || (_byteReader._fileSize < 4))
    {
        LAppPal::PrintLog("[SGY3] -1");
        return false;
    }

    // ファイル名
    _wavFileInfo._fileName = filePath;

    do
    {
        // シグネチャ "RIFF"
        if (!_byteReader.GetCheckSignature("RIFF"))
        {   
            LAppPal::PrintLog("[SGY3] 1");
            ret = false;
            break;
        }
        // ファイルサイズ-8（読み飛ばし）
        _byteReader.Get32LittleEndian();
        // シグネチャ "WAVE"
        if (!_byteReader.GetCheckSignature("WAVE"))
        {
            LAppPal::PrintLog("[SGY3] 2");
            ret = false;
            break;
        }
        // シグネチャ "fmt "
        if (!_byteReader.GetCheckSignature("fmt "))
        {
            LAppPal::PrintLog("[SGY3] 3");
            ret = false;
            break;
        }
        // fmtチャンクサイズ
        const Csm::csmUint32 fmtChunkSize = _byteReader.Get32LittleEndian();
        // フォーマットIDは1（リニアPCM）以外受け付けない
        LAppPal::PrintLog("[SGY3] 4");
        if (_byteReader.Get16LittleEndian() != 1)
        {

            ret = false;
            break;
        }
        // チャンネル数
        _wavFileInfo._numberOfChannels = _byteReader.Get16LittleEndian();
        LAppPal::PrintLog("[SGY3] 5,numberOfChannels:%d",_wavFileInfo._numberOfChannels);
        // サンプリングレート
        _wavFileInfo._samplingRate = _byteReader.Get32LittleEndian();
        LAppPal::PrintLog("[SGY3] 6");
        // データ速度[byte/sec]（読み飛ばし）
        _byteReader.Get32LittleEndian();
        LAppPal::PrintLog("[SGY3] 7");
        // ブロックサイズ（読み飛ばし）
        _byteReader.Get16LittleEndian();
        LAppPal::PrintLog("[SGY3] 8");
        // 量子化ビット数
        _wavFileInfo._bitsPerSample = _byteReader.Get16LittleEndian();
        // fmtチャンクの拡張部分の読み飛ばし
        if (fmtChunkSize > 16)
        {
            _byteReader._readOffset += (fmtChunkSize - 16);
        }
        // "data"チャンクが出現するまで読み飛ばし
        while (!(_byteReader.GetCheckSignature("data")) && (_byteReader._readOffset < _byteReader._fileSize))
        {
            _byteReader._readOffset += _byteReader.Get32LittleEndian();
        }
        // ファイル内に"data"チャンクが出現しなかった
        if (_byteReader._readOffset >= _byteReader._fileSize)
        {
            LAppPal::PrintLog("[SGY3] 9");
            ret = false;
            break;
        }
        // サンプル数
        {
            const Csm::csmUint32 dataChunkSize = _byteReader.Get32LittleEndian();
            LAppPal::PrintLog("[SGY3] dataChunkSize:%d",dataChunkSize);
            _wavFileInfo._samplesPerChannel = (dataChunkSize * 8) / (_wavFileInfo._bitsPerSample * _wavFileInfo._numberOfChannels);
        }
        // 領域確保
        _pcmData = static_cast<Csm::csmFloat32 **>(CSM_MALLOC(sizeof(Csm::csmFloat32 *) * _wavFileInfo._numberOfChannels));
        for (Csm::csmUint32 channelCount = 0; channelCount < _wavFileInfo._numberOfChannels; channelCount++)
        {
            _pcmData[channelCount] = static_cast<Csm::csmFloat32 *>(CSM_MALLOC(sizeof(Csm::csmFloat32) * _wavFileInfo._samplesPerChannel));
        }
        // 波形データ取得
        for (Csm::csmUint32 sampleCount = 0; sampleCount < _wavFileInfo._samplesPerChannel; sampleCount++)
        {
            for (Csm::csmUint32 channelCount = 0; channelCount < _wavFileInfo._numberOfChannels; channelCount++)
            {
                _pcmData[channelCount][sampleCount] = GetPcmSample();
            }
        }
        LAppPal::PrintLog("[SGY3] 10");
        ret = true;

    } while (false);

    // ファイル開放
    LAppPal::ReleaseBytes(_byteReader._fileByte);
    _byteReader._fileByte = NULL;
    _byteReader._fileSize = 0;
    LAppPal::PrintLog("[SGY3] 11");
    return ret;
}

Csm::csmFloat32 LAppWavFileHandler::GetPcmSample()
{
    Csm::csmInt32 pcm32;

    // 32ビット幅に拡張してから-1～1の範囲に丸める
    switch (_wavFileInfo._bitsPerSample)
    {
    case 8:
        pcm32 = static_cast<Csm::csmInt32>(_byteReader.Get8()) - 128;
        pcm32 <<= 24;
        break;
    case 16:
        pcm32 = _byteReader.Get16LittleEndian() << 16;
        break;
    case 24:
        pcm32 = _byteReader.Get24LittleEndian() << 8;
        break;
    default:
        // 対応していないビット幅
        pcm32 = 0;
        break;
    }

    return static_cast<Csm::csmFloat32>(pcm32) / INT32_MAX;
}

void LAppWavFileHandler::ReleasePcmData()
{
    for (Csm::csmUint32 channelCount = 0; channelCount < _wavFileInfo._numberOfChannels; channelCount++)
    {
        CSM_FREE(_pcmData[channelCount]);
    }
    CSM_FREE(_pcmData);
    _pcmData = NULL;
}




