/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "bikreader.h"

#include "../audio/stream.h"
#include "../common/logutil.h"
#include "../common/guardutil.h"
#include "../common/streamreader.h"
#include "../graphics/services.h"

#include "video.h"

extern "C" {

#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"

}

namespace fs = boost::filesystem;

using namespace std;

using namespace reone::audio;
using namespace reone::graphics;

namespace reone {

namespace video {

static const char kSignature[] = "BIKi";

class BinkVideoDecoder : public MediaStream<Video::Frame> {
public:
    BinkVideoDecoder(fs::path path, GraphicsServices &graphics) :
        _path(move(path)),
        _graphics(graphics) {
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

    void ignoreFrames(int count) override {
        readVideoFrames(count, true);
    }

    void fetchFrames(int count) override {
        readVideoFrames(count);
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
        readAudioFrames();
    }

    shared_ptr<Video> video() const {
        return _video;
    }

private:
    fs::path _path;
    graphics::GraphicsServices &_graphics;

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
            AVCodecParameters *codecParams = _formatCtx->streams[i]->codecpar;
            switch (codecParams->codec_type) {
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
        AVCodecParameters *codecParams = _formatCtx->streams[_videoStreamIdx]->codecpar;
        AVCodec *codec = avcodec_find_decoder(codecParams->codec_id);
        if (!codec) {
            throw runtime_error("BIK: video codec not found");
        }
        _videoCodecCtx = avcodec_alloc_context3(codec);
        if (avcodec_parameters_to_context(_videoCodecCtx, codecParams) != 0) {
            throw runtime_error("BIK: failed to copy video codec parameters");
        }
        if (avcodec_open2(_videoCodecCtx, codec, nullptr) != 0) {
            throw runtime_error("BIK: failed to open a video codec");
        }
    }

    bool hasAudio() const {
        return _audioStreamIdx != -1;
    }

    void openAudioCodec() {
        AVCodecParameters *codecParams = _formatCtx->streams[_audioStreamIdx]->codecpar;
        AVCodec *codec = avcodec_find_decoder(codecParams->codec_id);
        if (!codec) {
            throw runtime_error("BIK: audio codec not found");
        }
        _audioCodecCtx = avcodec_alloc_context3(codec);
        if (avcodec_parameters_to_context(_audioCodecCtx, codecParams) != 0) {
            throw runtime_error("BIK: failed to copy audio codec parameters");
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

        int bufSize = av_image_get_buffer_size(AV_PIX_FMT_RGB24, _videoCodecCtx->width, _videoCodecCtx->height, 32);
        _videoBuffer = static_cast<uint8_t *>(av_malloc(bufSize));
        av_image_fill_arrays(_frameRgb->data, _frameRgb->linesize, _videoBuffer, AV_PIX_FMT_RGB24, _videoCodecCtx->width, _videoCodecCtx->height, 32);
    }

    void initVideo() {
        AVRational &frameRate = _formatCtx->streams[_videoStreamIdx]->r_frame_rate;

        _video = make_shared<Video>(_graphics);
        _video->_width = _videoCodecCtx->width;
        _video->_height = _videoCodecCtx->height;
        _video->_fps = frameRate.num / static_cast<float>(frameRate.den);

        if (hasAudio()) {
            _video->_audio = make_shared<AudioStream>();
        }
    }

    void readVideoFrames(int count, bool ignore = false) {
        if (count == 0 || _ended) return;

        AVPacket packet;

        while (count > 0) {
            if (av_read_frame(_formatCtx, &packet) < 0) {
                _ended = true;
                break;
            }
            if (packet.stream_index != _videoStreamIdx) continue;

            int retVal = avcodec_receive_frame(_videoCodecCtx, _frame);
            if (retVal == 0 || retVal == AVERROR(EAGAIN)) {
                avcodec_send_packet(_videoCodecCtx, &packet);
                if (retVal == AVERROR(EAGAIN)) continue;
            }

            --count;

            if (ignore) continue;

            sws_scale(
                _swsContext,
                _frame->data, _frame->linesize, 0, _videoCodecCtx->height,
                _frameRgb->data, _frameRgb->linesize);

            auto pixels = make_shared<ByteArray>(3ll * _videoCodecCtx->width * _videoCodecCtx->height);
            for (int y = 0; y < _videoCodecCtx->height; ++y) {
                int dstIdx = 3 * _videoCodecCtx->width * y;
                uint8_t *src = _frameRgb->data[0] + static_cast<long long>(y) * _frameRgb->linesize[0];
                memcpy(pixels->data() + dstIdx, src, 3ll * _videoCodecCtx->width);
            }

            auto frame = make_shared<Video::Frame>();
            frame->pixels = move(pixels);
            _frames.push_back(move(frame));
        }

        av_packet_unref(&packet);
    }

    void readAudioFrames() {
        if (!hasAudio()) return;

        AVPacket packet;

        while (av_read_frame(_formatCtx, &packet) >= 0) {
            if (packet.stream_index != _audioStreamIdx) continue;

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

        av_packet_unref(&packet);

        seekBeginning();
    }

    void seekBeginning() {
        int64_t pos = av_rescale_q(0ll, { 1, AV_TIME_BASE }, _formatCtx->streams[_audioStreamIdx]->time_base);
        av_seek_frame(_formatCtx, _audioStreamIdx, pos, AVSEEK_FLAG_ANY);
    }
};

BikReader::BikReader(fs::path path, GraphicsServices &graphics) :
    _path(move(path)),
    _graphics(graphics) {
}

void BikReader::load() {
    if (!fs::exists(_path)) {
        throw runtime_error("BIK: file not found: " + _path.string());
    }

    auto decoder = make_shared<BinkVideoDecoder>(_path, _graphics);
    decoder->load();

    _video = decoder->video();
    _video->setMediaStream(decoder);
    _video->init();
}

} // namespace video

} // namespace reone
