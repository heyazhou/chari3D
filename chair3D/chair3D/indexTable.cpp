//��ģ�������Ѿ��ȵ������궨��������ϳ��м�����ֵ������һ����table.yml�ļ�
#include "stdafx.h"
#include "indexTable.h"
#include <vector>
//���뼤���ߣ�������������
void indexTable(Mat& imgMap, int N, string output)
{

	for (int j = 0; j< imgMap.cols; j++)  //��ͼ���м俪ʼ����
	{
		int num = 0;  //���ҵ���ƽ�������������Ӧ�еĵ�����������
		vector<int> PointOfEveryCalplane;
		for (int i = 0; i < imgMap.rows; i++)
		{
			if (imgMap.at<float>(i, j)>1E-8)
			{
				if (imgMap.at<float>(i, j)>200)
					imgMap.at<float>(i, j) = 0.0;
				num++;
				PointOfEveryCalplane.push_back(i);
			}
		}
		if (num == N)//���һ��ͼ
		{
			for (int i = 0; i < N - 1; i++)
			{
				int up = PointOfEveryCalplane[i], next = PointOfEveryCalplane[i + 1];
				float div = (imgMap.at<float>(up, j) - imgMap.at<float>(next, j)) / (up - next);//������֮���б��
				for (int k = up + 1; k < next; k++)
				{
					imgMap.at<float>(k, j) = imgMap.at<float>(k - 1, j) + div;
				}
			}
		}
	}
	FileStorage fs(output, FileStorage::WRITE);
	fs << "TABLE" << imgMap;
	fs.release();

}