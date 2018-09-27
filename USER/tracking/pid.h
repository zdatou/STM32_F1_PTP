/*
 * pid.h
 *
 *  Created on: 2018-1-21
 *      Author: admin
 */

#ifndef PID_H_
#define PID_H_

//PID控制周期
#define   PIDCTRLPERIOD   5000  //ms

//extern int SysTime;

//PID类型定义
typedef struct
{
	double  SetPoint; 			  //  设定目标 Desired Value，保证控制精度，选择double
	double  ProcessVal;
	double  SumError;			    //	误差累计

	double  Proportion;       //  比例常数 Proportional Const
	double  Integral;         //  积分常数 Integral Const
	double  Derivative;       //  微分常数 Derivative Const

	double  Kp;       //  比例常数 Proportional Const
	double  Ti;         //  积分常数 Integral Const
	double  Td;       //  微分常数 Derivative Const

	double  IntegratedError;
	double  ut;

	double   LastError;        //  Error[-1]
	double   PrevError;        //  Error[-2]

	double   isMax;
	double 	isMin;

	int 	SampleTime;
}PIDTypedef;

double    LocPIDCalc(PIDTypedef *sptr, double nextPoint);  //位置式
double    LocPIDCalcFunc(PIDTypedef *sptr, double nextPoint,double iError);
void     PIDinit(PIDTypedef *sptr);


#endif /* PID_H_ */
