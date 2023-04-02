#pragma once
struct AVFormatContext;
struct AVPacket;
struct AVCodec;
struct AVCodecContext;
struct AVFrame;
struct AVFrame;
struct SwsContext;
class XFFmpeg
{
public:

	//����ƵԴ
	int Open(const char *url);
	//��ȡһ֡��Ƶ���ڲ��洢
	bool Read();

	//�����������rgb����  ͨ����Ա
	bool Decode(int outweight, int outheight);

	bool Close();
	int Totalms = 0;
	int fps = 0;
	XFFmpeg();
	~XFFmpeg();

	unsigned char* rgb = 0;

protected:
	//���װ������
	AVFormatContext* ic = 0;
	
	//��ȡ����Ƶ֡
	AVPacket* pkt = 0;

	//������������ 
	AVCodec* codec = 0;
	//��������Ա  �����߳�Ҳ������
	AVCodecContext* vcodec = 0;
	

	//������yuv֡
	AVFrame* frame = 0;

	//yuvת��rgb
	SwsContext* sws = 0;

};


