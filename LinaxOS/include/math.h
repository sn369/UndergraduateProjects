#ifndef __MATH_H__
#define __MATH_H__

#ifdef __cplusplus
extern "C" {
#endif

#define M_PI  (3.1415926535897932384626433832795)
#define M_2PI  (M_PI * 2)
#define M_PI_2  (M_PI / 2)

double sin(double x);
double cos(double x);
double tan(double x);
double asin(double x);
double acos(double x);
double atan(double x);
double sqrt(double x);
double fabs(double x);
double floor(double x);
double ceil(double x);
double round(double x);
long lround(double x);
double pow(double x, double y);
double exp(double x);
double log(double x);

int isinf(double x);  // TODO: Impliement
int isnan(double x);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif
