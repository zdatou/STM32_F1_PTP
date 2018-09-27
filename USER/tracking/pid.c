/*
 * pid.c
 *
 *  Created on: 2018-1-21
 *      Author: admin
 */
#include "pid.h"

void PIDinit(PIDTypedef *sptr)
{
  //memset(sptr, 0, sizeof(PIDTypedef));
	//Kp, Ki, Kd不初始化
//	sptr->SetPoint = 0;
	sptr->SumError = 0;
	sptr->ProcessVal = 0;
	sptr->IntegratedError = 0;
	sptr->ut = 0;
	sptr->LastError = 0;
	sptr->PrevError = 0;
//	sptr->Proportion = sptr->Kp;
//	sptr->Integral = sptr->Ti/sptr->SampleTime;
//	sptr->Derivative = sptr->Td * sptr->SampleTime;
}




double LocPIDCalc(PIDTypedef *sptr, double nextPoint)
{
	double  iError,dError ;
	double output = 0 ;

		iError = sptr->SetPoint-nextPoint;       // 偏差
		sptr->IntegratedError += sptr->Integral * ((iError));//积分
		if(sptr->IntegratedError > sptr->isMax) sptr->IntegratedError = sptr->isMax;
		else if(sptr->IntegratedError < (-sptr->isMax)) sptr->IntegratedError = -sptr->isMax;
		dError = iError - sptr->LastError; 			   // 微分
		sptr->LastError = iError;

		output =  sptr->Proportion * iError           	// 比例项
						 + sptr->IntegratedError 	  // 积分项
						 + sptr->Derivative * dError;

		if(output > sptr->isMax) output = sptr->isMax;
		else if( output < sptr->isMin ) output = sptr->isMin;

	return( output );
}

double LocPIDCalcFunc(PIDTypedef *sptr, double nextPoint,double iError)
{
	double  Error , dError ;
	double output = 0 ;

		Error = sptr->SetPoint-nextPoint;       // 偏差
		sptr->IntegratedError += sptr->Integral * ((iError));//积分
		if(sptr->IntegratedError > sptr->isMax) sptr->IntegratedError = sptr->isMax;
		else if(sptr->IntegratedError < (-sptr->isMax)) sptr->IntegratedError = -sptr->isMax;
		dError = Error - sptr->LastError; 			   // 微分
		sptr->LastError = Error;

		output =  sptr->Proportion * iError           	// 比例项
						 + sptr->IntegratedError 	  // 积分项
						 + sptr->Derivative * dError;

		if(output > sptr->isMax) output = sptr->isMax;
		else if( output < sptr->isMin ) output = sptr->isMin;

	return( output );
}

