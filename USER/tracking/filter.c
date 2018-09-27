/*
 * filter.c
 *
 *  Created on: 2018-1-24
 *      Author: admin
 */

/*-------------------------------------------------------------------------------------------------------------*/
/*
        Q:过程噪声，Q增大，动态响应变快，收敛稳定性变坏
        R:测量噪声，R增大，动态响应变慢，收敛稳定性变好
*/

#include "filter.h"

double KalmanFilter(const double ResrcData,double ProcessNiose_Q,double MeasureNoise_R,double InitialPrediction)
{
		double R = MeasureNoise_R;
		double Q = ProcessNiose_Q;

        static double x_last = 0;

        double x_mid = x_last;
        double x_now;

        static double p_last = 20;

        double p_mid ;
        double p_now;
        double kg;

        x_mid=x_last; //x_last=x(k-1|k-1),x_mid=x(k|k-1)
        p_mid=p_last+Q; //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=噪声
        kg=p_mid/(p_mid+R); //kg为kalman filter，R为噪声
        x_now=x_mid+kg*(ResrcData-x_mid);//估计出的最优值

        p_now=(1-kg)*p_mid;//最优值对应的covariance

        p_last = p_now; //更新covariance值
        x_last = x_now; //更新系统状态值

        return x_now;
}


int AvrValueFiter(int buf[] , int bufsize)//去除野值 平均滤波
{
	unsigned int i = 0;
	int tmp_value = 0;
	int isMax = 0;
	int isMin = 0;

	for( i = 0 ; i < bufsize ; i++)
	{

		tmp_value += buf[i];

		if ( buf[i] > isMax)
		{
			isMax = buf[i];
		}
		if (buf[i] < isMin)
		{
			isMin = buf[i];
		}
	}
	tmp_value = (tmp_value - isMax - isMin)/(bufsize-2);

	return tmp_value;
}

// 中位值平均滤波法（又称防脉冲干扰平均滤波法）（算法1）

int Filter(int filter_buf[] , int bufsize)
{
  int i, j;
  int filter_temp, filter_sum = 0;
 // int filter_buf[bufsize] = {0};
//  for(i = 0; i < bufsize; i++) {
//    filter_buf[i] = buf[i];

//  }
  // 采样值从小到大排列（冒泡法）
  for(j = 0; j < bufsize - 1; j++) {
    for(i = 0; i < bufsize - 1 - j; i++) {
      if(filter_buf[i] > filter_buf[i + 1]) {
        filter_temp = filter_buf[i];
        filter_buf[i] = filter_buf[i + 1];
        filter_buf[i + 1] = filter_temp;
      }
    }
  }
  // 去除最大最小极值后求平均
  for(i = 1; i < bufsize - 1; i++) filter_sum += filter_buf[i];
  return filter_sum / (bufsize - 2);
}




