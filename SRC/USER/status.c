#include "status.h"
#include <stdio.h>
#include <math.h>

/** @note tot(dotX[tot]): can access dotX[0] - dotX[tot - 1]
  *
  */ 
double* STATUS_CirclePoints(u16 polarD[][4], double planeD[][2], u16 tot)
{
    //定义变量
    double u = 235.5, v, sigma = 0.001, sigma0, best_x = 0, best_y = 0;
    static double x_1, x_2, y_1, y_2, a, x_0, y_0, temp1, tempcos, tempsin, tempdx, tempdy, tempx1, tempx2, tempy1, tempy2;
    s32 time = 0, pretotal = 0, num = 2, test = 10;
    static double final_points[5];
    while (time < tot - num)
    {
        x_1 = planeD[time][0];
        x_2 = planeD[time + num][0];
        y_1 = planeD[time][1];
        y_2 = planeD[time + num][1];
        a = (y_2 - y_1) / (x_2 - x_1);
        x_0 = (x_1 + x_2) / 2;
        y_0 = (y_1 + y_2) / 2;
        temp1 = sqrt(v - 0.25 * pow((y_2 - y_1), 2) - 0.25 * pow((x_2 - x_1), 2));
        tempcos = 1 / sqrt(1 / pow(a, 2) + 1);
        tempsin = (-1 / a) * tempcos;
        tempdx = temp1 * tempcos;
        tempdy = temp1 * tempsin;
        tempx1 = x_0 - tempdx;
        tempx2 = x_0 + tempdx;
        tempy1 = y_0 - tempdy;
        tempy2 = y_0 + tempdy;
        //printf("circle1:(%.4f,%.4f)  circle2:(%.4f,%.4f)",tempx1,tempy1,tempx2 ,tempy2);

        int total_inlier1 = 0, total_inlier2 = 0;

        if (time >= test && time < tot - test)
        {
            for (int index = time - test; index <= time + test; index++)
            {
                if (fabs((pow((planeD[index][0] - tempx1), 2) + pow((planeD[index][1] - tempy1), 2)) - v) < sigma)
                    total_inlier1++;
                if (fabs((pow((planeD[index][0] - tempx2), 2) + pow((planeD[index][1] - tempy2), 2)) - v) < sigma)
                    total_inlier2++;
            }
            if (total_inlier1 > pretotal)
            {
                pretotal = total_inlier1;
                best_x = tempx1;
                best_y = tempy1;
                //printf("dotnum: %d , x: %.4f , y: %.4f", pretotal,best_x,best_y);
            }
            if (total_inlier2 > pretotal)
            {
                pretotal = total_inlier2;
                best_x = tempx2;
                best_y = tempy2;
                //printf("dotnum: %d , x: %.4f , y: %.4f", pretotal,best_x,best_y);
            }
        }
        time += num;
    }
    printf("%%CIRCLE_ANS%%\n[%.4f, %.4f]\n",best_x,best_y);
    final_points[0] = best_x;
    final_points[1] = best_y;
    return final_points; /* static s32[5] */
}