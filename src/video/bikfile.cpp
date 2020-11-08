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

#include "../system/log.h"
#include "../system/streamreader.h"

#include "video.h"

#if REONE_ENABLE_VIDEO

extern "C" {

#include "libavformat/avformat.h"
#include "libswscale/swscale.h"

}

#endif

namespace fs = boost::filesystem;

using namespace std;

namespace reone {

namespace video {

static const char kSignature[] = "BIKi";

BikFile::BikFile(const fs::path &path) : _path(path) {
}

void BikFile::load() {
#if REONE_ENABLE_VIDEO
    if (!fs::exists(_path)) {
        throw runtime_error("BIK: file not found: " + _path.string());
    }
    AVFormatContext *formatCtx = nullptr;
    if (avformat_open_input(&formatCtx, _path.string().c_str(), nullptr, nullptr) != 0) {
        throw runtime_error("BIK: failed to open");
    }
    if (avformat_find_stream_info(formatCtx, nullptr) != 0) {
        throw runtime_error("BIK: failed to find stream info");
    }
    int videoStream = -1;
    for (uint32_t i = 0; i < formatCtx->nb_streams; ++i) {
        if (formatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }
    if (videoStream == -1) {
        throw runtime_error("BIK: video stream not found");
    }
    AVCodecContext *codecCtx = formatCtx->streams[videoStream]->codec;
    AVCodec *codec = avcodec_find_decoder(codecCtx->codec_id);
    if (!codec) {
        throw runtime_error("BIK: codec not found");
    }
    AVCodecContext *codecCtxOrig = codecCtx;
    codecCtx = avcodec_alloc_context3(codec);
    if (avcodec_copy_context(codecCtx, codecCtxOrig) != 0) {
        throw runtime_error("BIK: failed to copy codec context");
    }
    if (avcodec_open2(codecCtx, codec, nullptr) != 0) {
        throw runtime_error("BIK: failed to open codec");
    }
    AVFrame *frame = av_frame_alloc();
    AVFrame *frameRGB = av_frame_alloc();
    int numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, codecCtx->width, codecCtx->height);
    uint8_t *buffer = static_cast<uint8_t *>(av_malloc(numBytes));
    avpicture_fill(reinterpret_cast<AVPicture *>(frameRGB), buffer, AV_PIX_FMT_RGB24, codecCtx->width, codecCtx->height);
    AVPacket packet;
    SwsContext *swsCtx = sws_getContext(
        codecCtx->width, codecCtx->height,
        codecCtx->pix_fmt,
        codecCtx->width, codecCtx->height,
        AV_PIX_FMT_RGB24,
        SWS_BILINEAR,
        nullptr, nullptr, nullptr);

    AVRational &fps = formatCtx->streams[videoStream]->r_frame_rate;

    _video = make_shared<Video>();
    _video->_width = codecCtx->width;
    _video->_height = codecCtx->height;
    _video->_fps = fps.num / static_cast<float>(fps.den);

    while (av_read_frame(formatCtx, &packet) >= 0) {
        if (packet.stream_index != videoStream) continue;

        int frameFinished = 0;
        avcodec_decode_video2(codecCtx, frame, &frameFinished, &packet);

        if (frameFinished == 0) continue;

        sws_scale(
            swsCtx,
            frame->data, frame->linesize, 0, codecCtx->height,
            frameRGB->data, frameRGB->linesize);

        ByteArray data(3ll * codecCtx->width * codecCtx->height);
        for (int y = 0; y < codecCtx->height; ++y) {
            int dstIdx = 3 * codecCtx->width * y;
            uint8_t *src = frameRGB->data[0] + static_cast<long long>(y) * frameRGB->linesize[0];
            memcpy(&data[dstIdx], src, 3ll * codecCtx->width);
        }

        Video::Frame frame;
        frame.data = move(data);
        _video->_frames.push_back(move(frame));
    }

    _video->init();

    av_free_packet(&packet);
    av_free(buffer);
    av_free(frameRGB);
    av_free(frame);
    avcodec_close(codecCtx);
    avcodec_close(codecCtxOrig);
    avformat_close_input(&formatCtx);
#endif
}

shared_ptr<Video> BikFile::video() const {
    return _video;
}

} // namespace video

} // namespace reone
