/*
 * Copyright (c) 2020 Vsevolod Kremianskii
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "bikfile.h"

#include <stdexcept>

#include <boost/filesystem.hpp>

#include "../audio/stream.h"
#include "../common/log.h"
#include "../common/streamreader.h"

#include "video.h"

#if REONE_ENABLE_VIDEO

extern "C" {

#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"

}

#endif

namespace fs = boost::filesystem;

using namespace std;

using namespace reone::audio;

namespace reone {

namespace video {

static const char kSignature[] = "BIKi";

#if REONE_ENABLE_VIDEO

class BinkVideoDecoder {
public:
    BinkVideoDecoder(const fs::path &path) : _path(path) {
    }

    ~BinkVideoDecoder() {
        deinit();
    }

    void deinit() {
        if (_videoBuffer) {
            av_free(_videoBuffer);
            _videoBuffer = nullptr;
        }
        if (_frameRgb) {
            av_free(_frameRgb);
            _frameRgb = nullptr;
        }
        if (_frame) {
            av_free(_frame);
            _frame = nullptr;
        }
        if (_swrContext) {
            swr_free(&_swrContext);
        }
        if (_swsContext) {
            sws_freeContext(_swsContext);
            _swsContext = nullptr;
        }
        if (_audioCodecCtx) {
            avcodec_close(_audioCodecCtx);
            _audioCodecCtx = nullptr;
        }
        if (_videoCodecCtx) {
            avcodec_close(_videoCodecCtx);
            _videoCodecCtx = nullptr;
        }
        if (_formatCtx) {
            avformat_close_input(&_formatCtx);
        }
    }

    void load() {
        openInput(_path);
        findStreams();
        openVideoCodec();

        if (hasAudio()) {
            openAudioCodec();
        }
        initConverters();
        initFrames();
        initVideo();
        processStream();
    }

    shared_ptr<Video> video() const {
        return _video;
    }

private:
    fs::path _path;
    AVFormatContext *_formatCtx { nullptr };
    int _videoStreamIdx { -1 };
    int _audioStreamIdx { -1 };
    AVCodecContext *_videoCodecCtx { nullptr };
    AVCodecContext *_audioCodecCtx { nullptr };
    SwsContext *_swsContext { nullptr };
    SwrContext *_swrContext { nullptr };
    AVFrame *_frame { nullptr };
    AVFrame *_frameRgb { nullptr };
    uint8_t *_videoBuffer { nullptr };
    shared_ptr<Video> _video;

    void openInput(const fs::path &path) {
        if (avformat_open_input(&_formatCtx, path.string().c_str(), nullptr, nullptr) != 0) {
            throw runtime_error("BIK: failed to open");
        }
    }

    void findStreams() {
        if (avformat_find_stream_info(_formatCtx, nullptr) != 0) {
            throw runtime_error("BIK: failed to find stream info");
        }
        for (uint32_t i = 0; i < _formatCtx->nb_streams; ++i) {
            AVMediaType codecType = _formatCtx->streams[i]->codec->codec_type;
            switch (codecType) {
                case AVMEDIA_TYPE_VIDEO:
                    _videoStreamIdx = i;
                    break;
                case AVMEDIA_TYPE_AUDIO:
                    _audioStreamIdx = i;
                    break;
                default:
                    break;
            }
        }
        if (_videoStreamIdx == -1) {
            throw runtime_error("BIK: video stream not found");
        }
    }

    void openVideoCodec() {
        AVCodecContext *codecCtx = _formatCtx->streams[_videoStreamIdx]->codec;
        AVCodec *codec = avcodec_find_decoder(codecCtx->codec_id);
        if (!codec) {
            throw runtime_error("BIK: video codec not found");
        }
        _videoCodecCtx = avcodec_alloc_context3(codec);
        if (avcodec_copy_context(_videoCodecCtx, codecCtx) != 0) {
            throw runtime_error("BIK: failed to copy a video codec context");
        }
        if (avcodec_open2(_videoCodecCtx, codec, nullptr) != 0) {
            throw runtime_error("BIK: failed to open a video codec");
        }
    }

    bool hasAudio() const {
        return _audioStreamIdx != -1;
    }

    void openAudioCodec() {
        AVCodecContext *codecCtx = _formatCtx->streams[_audioStreamIdx]->codec;
        AVCodec *codec = avcodec_find_decoder(codecCtx->codec_id);
        if (!codec) {
            throw runtime_error("BIK: audio codec not found");
        }
        _audioCodecCtx = avcodec_alloc_context3(codec);
        if (avcodec_copy_context(_audioCodecCtx, codecCtx) != 0) {
            throw runtime_error("BIK: failed to copy an audio codec context");
        }
        if (avcodec_open2(_audioCodecCtx, codec, nullptr) != 0) {
            throw runtime_error("BIK: failed to open an audio codec");
        }
    }

    void initConverters() {
        _swsContext = sws_getContext(
            _videoCodecCtx->width, _videoCodecCtx->height,
            _videoCodecCtx->pix_fmt,
            _videoCodecCtx->width, _videoCodecCtx->height,
            AV_PIX_FMT_RGB24,
            SWS_BILINEAR,
            nullptr, nullptr, nullptr);

        if (hasAudio()) {
            _swrContext = swr_alloc_set_opts(
                nullptr,
                AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_S16, _audioCodecCtx->sample_rate,
                _audioCodecCtx->channel_layout, _audioCodecCtx->sample_fmt, _audioCodecCtx->sample_rate,
                0, nullptr);

            swr_init(_swrContext);
        }
    }

    void initFrames() {
        _frame = av_frame_alloc();
        _frameRgb = av_frame_alloc();

        int bufSize = avpicture_get_size(AV_PIX_FMT_RGB24, _videoCodecCtx->width, _videoCodecCtx->height);
        _videoBuffer = static_cast<uint8_t *>(av_malloc(bufSize));
        avpicture_fill(reinterpret_cast<AVPicture *>(_frameRgb), _videoBuffer, AV_PIX_FMT_RGB24, _videoCodecCtx->width, _videoCodecCtx->height);
    }

    void initVideo() {
        AVRational &frameRate = _formatCtx->streams[_videoStreamIdx]->r_frame_rate;

        _video = make_shared<Video>();
        _video->_width = _videoCodecCtx->width;
        _video->_height = _videoCodecCtx->height;
        _video->_fps = frameRate.num / static_cast<float>(frameRate.den);

        if (hasAudio()) {
            _video->_audio = make_shared<AudioStream>();
        }
    }

    void processStream() {
        AVPacket packet;

        while (av_read_frame(_formatCtx, &packet) >= 0) {
            int gotFrame = 0;

            if (packet.stream_index == _videoStreamIdx) {
                avcodec_decode_video2(_videoCodecCtx, _frame, &gotFrame, &packet);
                if (gotFrame == 0) continue;

                sws_scale(
                    _swsContext,
                    _frame->data, _frame->linesize, 0, _videoCodecCtx->height,
                    _frameRgb->data, _frameRgb->linesize);

                ByteArray data(3ll * _videoCodecCtx->width * _videoCodecCtx->height);
                for (int y = 0; y < _videoCodecCtx->height; ++y) {
                    int dstIdx = 3 * _videoCodecCtx->width * y;
                    uint8_t *src = _frameRgb->data[0] + static_cast<long long>(y) * _frameRgb->linesize[0];
                    memcpy(&data[dstIdx], src, 3ll * _videoCodecCtx->width);
                }

                Video::Frame frame;
                frame.data = move(data);
                _video->_frames.push_back(move(frame));

            } else if (_audioStreamIdx != -1 && packet.stream_index == _audioStreamIdx) {
                avcodec_send_packet(_audioCodecCtx, &packet);
                avcodec_receive_frame(_audioCodecCtx, _frame);

                int sampleCount = swr_get_out_samples(_swrContext, _frame->nb_samples);
                int bufSize = av_samples_get_buffer_size(nullptr, 1, sampleCount, AV_SAMPLE_FMT_S16, 1);
                ByteArray samples(bufSize);
                uint8_t *samplesPtr = reinterpret_cast<uint8_t *>(&samples[0]);

                swr_convert(
                    _swrContext,
                    &samplesPtr, sampleCount,
                    const_cast<const uint8_t **>(&_frame->extended_data[0]), _frame->nb_samples);

                AudioStream::Frame frame;
                frame.format = AudioFormat::Mono16;
                frame.sampleRate = _audioCodecCtx->sample_rate;
                frame.samples = move(samples);
                _video->_audio->add(move(frame));
            }
        }

        av_free_packet(&packet);
    }
};

#endif

BikFile::BikFile(const fs::path &path) : _path(path) {
}

void BikFile::load() {
#if REONE_ENABLE_VIDEO
    if (!fs::exists(_path)) {
        throw runtime_error("BIK: file not found: " + _path.string());
    }

    BinkVideoDecoder decoder(_path);
    decoder.load();

    _video = decoder.video();
    _video->init();
#endif // REONE_ENABLE_VIDEO
}

shared_ptr<Video> BikFile::video() const {
    return _video;
}

} // namespace video

} // namespace reone
