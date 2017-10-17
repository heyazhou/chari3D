#include "stdafx.h"
#include "skeleton.h"
//ͨ���������±߽磬�Ա궨�����߹Ǽܻ�������ǿ��ֵ��Ϊ��Ӧ�߶�
Mat skeleton(Mat& img, float h, int line)
{

	Mat imgRoi = img;
	//imgRoi = imread(filename, 0);
	Mat imgblur;//��ֵ���ͼ��

	Mat img_sk = Mat::zeros(imgRoi.rows, imgRoi.cols, CV_32FC1);
	threshold(imgRoi, imgblur, 30, 255, CV_THRESH_BINARY);//��ֵ��ͼ����,���߱Ƚϰ���ֵ����--------��ֵ�޸�--------
	//blur(imgblur,imgblur,Size(5,5),Point(-1,-1));

	medianBlur(imgblur, imgblur, 3);//ͼ��ƽ������
	//dilate(imgblur,imgblur,(5,5));//��ʴ���ʹ���
	//	imshow("imgblur", imgblur);//----------------kai----------------------
	//	waitKey();

	Point2i L, R; //��ʾ���������ֵʱint���Ϳ���

	int w = imgRoi.cols, l = imgRoi.rows;//col ����

	for (int j = 0; j < w; j++)
	{
		int isLaser = 0;//f
		int up = 0;
		int buttom = 0;
		int line_panduan = 0;//����ƥ��line
		for (int i = 0; i < l; i++)
		{

			if (buttom)
				break;

			if (imgblur.at<uchar>(i, j) >30 && !isLaser) //��ֵͼ�񣬻Ҷ���ֵ��0�Ǻ�
			{
				isLaser = 1;
				++line_panduan;
				if (line_panduan == line){ up = i; }
				//up = i;
			}
			else if (imgblur.at<uchar>(i, j) <30 && isLaser)
			{

				if (line_panduan == line){ buttom = i - 1; img_sk.at<float>((up + buttom) / 2, j) = h; break; }
				isLaser = 0;
				//img_sk.at<float>((up + buttom) / 2, j) = h;//����߶�ֵ������Ӧ���ߴ���-------------------
				//break;
			}

			//else{
			//	img_sk.at<float>((up + buttom) / 2, j) = 0;
			//}
		}
		if (isLaser)
			R = Point2i((up + buttom) / 2, j);              //ȷ�����ұ߽�
		if (L == Point2i(0, 0) && isLaser)
		{
			L = Point2i((up + buttom) / 2, j);
		}
	}

	//����궨������
	for (int i = 0; i < L.y; i++)
	{
		img_sk.at<float>(L.x, i) = h;
	}
	for (int i = R.y; i < w; i++)
	{
		img_sk.at<float>(R.x, i) = h;
	}
	//imshow("img_sk",img_sk);
	//waitKey(5);
	return img_sk;
}