/*
 * filter.h
 *
 *  Created on: 2018-1-24
 *      Author: admin
 */

#ifndef FILTER_H_
#define FILTER_H_

typedef struct kalman
	{
		float ProcessNiose_Q;
		float MeasureNoise_R;
		float InitialPrediction;
	}KalManTypedef;

double KalmanFilter(const double ResrcData,double ProcessNiose_Q,double MeasureNoise_R,double InitialPrediction);
//float AvrValueFiter(float buf[] , int bufsize);//去除野值 平均滤波
int AvrValueFiter(int buf[] , int bufsize);
#endif /* FILTER_H_ */
