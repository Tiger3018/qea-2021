#include "status.h"
#include <stdio.h>
#include <math.h>
#include <system_stm32f10x.h>

/** @note tot(dotX[tot]): can access dotX[0] - dotX[tot - 1]
  *
  */ 
	
	
double* STATUS_CirclePoints(u16 polarD[][4], double planeD[][2], u16 tot)
{
    //定义变量
    double r = 237, v=r*r, sigma , sigma0= 0.001, best_x = 0, best_y = 0;
    static double x_1, x_2, y_1, y_2, k, x_0, y_0, temp1, tempcos, tempsin, tempdx, tempdy, tempx1, tempx2, tempy1, tempy2;
    static double final_points[5];
	  int total_inlier1 = 0, total_inlier2 = 0;
	
	  s32 time = 0, pretotal = 0, num = 2, test = 16;   
	
    while (time < tot - num)
    {
        x_1 = planeD[time][0];
        x_2 = planeD[time + num][0];
        y_1 = planeD[time][1];
			  y_2 = planeD[time + num][1];				//取两个间隔为2的点
			
        k = (y_2 - y_1) / (x_2 - x_1);      //斜率
        x_0 = (x_1 + x_2) / 2;
        y_0 = (y_1 + y_2) / 2;              //两个点的中点
			
        temp1 = sqrt(v - 0.25 * pow((y_2 - y_1), 2) - 0.25 * pow((x_2 - x_1), 2));      //圆心到中点的距离
			
        tempsin = 1 / sqrt(1 / pow(k, 2) + 1);
        tempcos = (-1 / k) * tempsin;				//中垂线的倾斜角的sin和cos值   //!!!!!!!//
			
        tempdx = temp1 * tempcos;
        tempdy = temp1 * tempsin;						//中点距离圆心的x与y方向的距离
			
        tempx1 = x_0 - tempdx;
			  tempy1 = y_0 - tempdy;              //坐标较小的圆心
			
        tempx2 = x_0 + tempdx;
        tempy2 = y_0 + tempdy;							//坐标较大的圆心
			

			if (time >= test && time < tot - test)               //初始值: time=0,test=16,tot总数,time每次+2
        {
            for (int index = time - test; index <= time + test; index++)
            {
                sigma = sigma0 * (pow(planeD[time][0], 2) + pow(planeD[time][1], 2));				//小车到点的距离*sigma0
							
                if (fabs((pow((planeD[index][0] - tempx1), 2) + pow((planeD[index][1] - tempy1), 2)) - v) < 1.21*v)    //点到圆心的距离
                    total_inlier1++;
								
                if (fabs((pow((planeD[index][0] - tempx2), 2) + pow((planeD[index][1] - tempy2), 2)) - v) < 1.21*v)
                    total_inlier2++;
								
            }
						
						
            if (total_inlier1 > pretotal)
            {
                pretotal = total_inlier1;
                best_x = tempx1;
                best_y = tempy1;
            }
						
						
            if (total_inlier2 > pretotal)
            {
                pretotal = total_inlier2;
                best_x = tempx2;
                best_y = tempy2;
            }
        }
				
				
        time += num;
    }
		
		
    final_points[0] = best_x;
    final_points[1] = best_y;
		printf("%f|%f\n", final_points[0],final_points[1]);
    return final_points; /* static s32[5] */
}