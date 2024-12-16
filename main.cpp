extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

void encode_yuv422p_to_dnxhd(const char* input_file, const char* output_file) {
	// 输入参数
	int width = 1920, height = 1080, bitrate = 120000000;
	int fps = 25;

	// 初始化 FFmpeg
	//av_register_all();

	// 打开输入文件
	FILE* input = fopen(input_file, "rb");
	if (!input) {
		perror("Failed to open input file");
		return;
	}

	// 创建输出上下文
	AVFormatContext* fmt_ctx = nullptr;
	avformat_alloc_output_context2(&fmt_ctx, nullptr, "dnxhd", output_file);

	// 创建视频流
	AVStream* stream = avformat_new_stream(fmt_ctx, nullptr);
	const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_DNXHD);
	AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);

	codec_ctx->width = width;
	codec_ctx->height = height;
	codec_ctx->pix_fmt = AV_PIX_FMT_YUV422P;
	//codec_ctx->time_base = (AVRational){ 1, fps };
	codec_ctx->time_base.den = 1;
	codec_ctx->time_base.num = fps;
	codec_ctx->bit_rate = bitrate;
	codec_ctx->flags |= AV_CODEC_FLAG_INTERLACED_DCT | AV_CODEC_FLAG_INTERLACED_ME;

	avcodec_open2(codec_ctx, codec, nullptr);
	stream->codecpar->codec_id = AV_CODEC_ID_DNXHD;
	avcodec_parameters_from_context(stream->codecpar, codec_ctx);

	// 打开输出文件
	if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
		avio_open(&fmt_ctx->pb, output_file, AVIO_FLAG_WRITE);
	}
	avformat_write_header(fmt_ctx, nullptr);

	// 编码过程
	AVFrame* frame = av_frame_alloc();
	frame->format = codec_ctx->pix_fmt;
	frame->width = codec_ctx->width;
	frame->height = codec_ctx->height;
	av_image_alloc(frame->data, frame->linesize, width, height, codec_ctx->pix_fmt, 32);

	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = nullptr;
	pkt.size = 0;

	while (fread(frame->data[0], 1, width * height * 2, input) > 0) {
		frame->pts++;
		avcodec_send_frame(codec_ctx, frame);
		while (avcodec_receive_packet(codec_ctx, &pkt) == 0) {
			av_interleaved_write_frame(fmt_ctx, &pkt);
			av_packet_unref(&pkt);
		}
	}

	// 清理资源
	av_write_trailer(fmt_ctx);
	fclose(input);
	av_frame_free(&frame);
	avcodec_free_context(&codec_ctx);
	avio_close(fmt_ctx->pb);
	avformat_free_context(fmt_ctx);
}

int main() {
	const char* input_file = R"(D:\Dnx\1yuv-1920x1080.yuv)";
	const char* output_file = R"(D:\Dnx\1yuv-1920x1080.dnx)";
	encode_yuv422p_to_dnxhd(input_file, output_file);
	return 0;
}
