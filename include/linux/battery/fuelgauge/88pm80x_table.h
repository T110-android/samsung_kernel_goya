#ifndef __88PM80X_TABLE_H
#define __88PM80X_TABLE_H


/* temperature for goldenve */
/* -10       89.4050     93.5136     97.7229 */
#define TBAT_M10C	94	/* -10C: 93.5136k ohm */
/* 0        57.5058     59.8810     62.2981 */
#define TBAT_0C		60	/* 0C: 59.8810k ohm */
/* 20       25.7312     26.5772     27.4263 */
#define TBAT_20C	27	/* 20C: 26.5772k ohm */
/* 35       14.7729     15.2854     15.8013 */
#define TBAT_35C	15	/* 35C: 15.2854k ohm */
/* 45       10.4184     10.8172     11.2211 */
#define TBAT_45C	11	/* 45C: 10.8172k ohm */

/* -25C ~ 60C */
static int temperature_table[] = {
/* -25 ~ -21 */
193, 183, 174, 166, 158,
/* -20 ~ -11*/
150, 143, 136, 130, 124, 118, 113, 107, 103, 98,
/* -10 ~ -1 */
94, 89, 85, 82, 78, 75, 71, 68, 65, 63,
/* 0 ~ 9 */
60, 57, 55, 53, 51, 48, 46, 45, 43, 41,
/* 10 ~ 19 */
39, 38, 36, 35, 34, 32, 31, 30, 29, 28,
/* 20 ~ 29 */
27, 26, 25, 24, 22, 22, 21, 20, 20, 19,
/* 30 ~ 39 */
18, 18, 17, 16, 16, 15, 15, 14, 14, 13,
/* 40 ~ 49 */
13, 12, 12, 12, 11, 11, 10, 10, 10, 9,
/* 50 ~ 59 */
9, 9, 9, 8, 8, 8, 8, 7, 7, 7,
/* 60 ~ 70 */
7, 6, 6, 6, 6, 6
};
/*
 * Battery parameters from battery vendor for fuel gauge.
 * For SS aruba battery paramters, the extraction was
 * performed at T=25C and with ICHG=IDIS=0.5A, they are:
 * Rs = 0.11 Ohm + 0.01 Ohm;
 * R1 = 0.04 Ohm;
 * R2 = 0.03 Ohm;
 * C1 = 1000F
 * C2 = 10000F
 * Csoc = 5476F (Qtot = 5476C)
 */

/* we use m-Ohm by *1000 for calculation */
static int r1 = 40;
static int r2 = 30;
static int r_s = 120;
static int rtot;
static int c1 = 1000;
static int c2 = 10000;
//static int c_soc = 6286;
//static int c_soc = 5868; //SSG env batt 3
//static int c_soc = 14000; //Asus battery
static int c_soc = 5400; //GoldenVE 1500 mAh battery
//static int c_soc_Tm10 = 5400; //GoldenVE battery capacity at T=-10
//static int c_soc_T0 = 5400; //GoldenVE battery capacity at T=-20
//static int K_T = 1; // c_soc / c_soc(T)

static int r_off_initial = 60;
//static int r_off = 80; //new value obtained with adaptive method
static int r_off = 60; // Value used for Samsung setup (external battery)
static int deltaR_to_store;
static int short_slp_counter = 0; // to take in account of slp_cnt = 1 sec in SSG environment
/*
 * v1, v2 is voltage, the unit of orignal formula design is "Volt",
 * since we can not do float calculation, we introduce the factor of
 * 1024 * 1024 to enlarge the resolution for LSB, the value of it is
 * "Volt with factor" in the driver.
 *
 * v1 and v2 are signded variable, normally, for charging case they
 * are negative value, for discharging case, they are positive value.
 *
 * v3 is SOC, original formular is 0~1, since we can not do float
 * calculation, for improve the resolution of it, the range value
 * is enlarged  by multiply 10000 in v3_res, but we report
 * v3 = v3_res/1000 to APP/Android level.
 */
static int v1, v2, v3, cap = 100;
static int factor = 1024 * 1024;
static int factor2 = 1000;
//static int count_cap = 0;
static int ib_adaptive = 0;
static int term1,term2,term3 = 0;
static int adaptive_gain_x_I0 = 125000;
static int first_long_sleep_flag_adaptive = 0;
static int deltaR = 0;
static int v1_mv, v2_mv, v3_res, v3_res_t1, v3_res_t2;
static int deltat_t2mt1, delta_soc_t1mt2, delta_soc_t1mt3;
static int vbat_mv;
static int t1_start;

static int G_T0 = 1;
static int Off_T0 = 0;
static int G_Tm10 = 2;
static int Off_Tm10 = -100;

/*
 * LUT of exp(-x) function, we need to calcolate exp(-x) with x=SLEEP_CNT/RC,
 * Since 0<SLEEP_CNT<1000 and RCmin=20 we should calculate exp function in [0-50]
 * Because exp(-5)= 0.0067 we can use following approximation:
 *	f(x)= exp(-x) = 0   (if x>5);
 * 20 points in [0-5] with linear interpolation between points.
 *  [-0.25, 0.7788] [-0.50, 0.6065] [-0.75, 0.4724] [-1.00, 0.3679]
 *  [-1.25, 0.2865] [-1.50, 0.2231] [-1.75, 0.1738] [-2.00, 0.1353]
 *  [-2.25, 0.1054] [-2.50, 0.0821] [-2.75, 0.0639] [-3.00, 0.0498]
 *  [-3.25, 0.0388] [-3.50, 0.0302] [-3.75, 0.0235] [-4.00, 0.0183]
 *  [-4.25, 0.0143] [-4.50, 0.0111] [-4.75, 0.0087] [-5.00, 0.0067]
 */
static int exp_data[] = {
	7788, 6065, 4724, 3679,
	2865, 2231, 1738, 1353,
	1054, 821,  639,  498,
	388,  302,  235,  183,
	143,  111,  87,   67 };

/*
 * State of Charge.
 * The first number is voltage, the second number is soc point.
 */
#if 0
static int ocv_table[] = {
 3217, 3347, 3438, 3505, 3551, 3582, 3610, 3636, 3658, 3675,
 3686, 3693, 3699, 3705, 3713, 3723, 3732, 3741, 3748, 3754,
 3759, 3765, 3769, 3772, 3774, 3776, 3778, 3779, 3780, 3781,
 3782, 3783, 3784, 3785, 3786, 3788, 3790, 3792, 3794, 3797,
 3800, 3803, 3806, 3809, 3813, 3817, 3821, 3825, 3830, 3834,
 3840, 3845, 3851, 3858, 3866, 3875, 3886, 3896, 3906, 3915,
 3925, 3933, 3942, 3950, 3958, 3966, 3974, 3982, 3990, 3999,
 4008, 4017, 4026, 4036, 4046, 4056, 4066, 4076, 4086, 4096,
 4107, 4117, 4128, 4139, 4150, 4161, 4172, 4184, 4195, 4207,
 4218, 4230, 4242, 4254, 4266, 4278, 4290, 4302, 4314, 4326 };
#else
static int ocv_dischg[] = { //GoldenVE battery
3187, 3325, 3463, 3556, 3620, 3666, 3687, 3693, 3695, 3696,
3697, 3698, 3699, 3700, 3703, 3712, 3722, 3730, 3738, 3746,
3751, 3755, 3759, 3763, 3767, 3771, 3774, 3776, 3778, 3780,
3782, 3784, 3786, 3788, 3790, 3793, 3795, 3797, 3800, 3803,
3806, 3809, 3812, 3815, 3819, 3822, 3826, 3830, 3834, 3838,
3843, 3847, 3852, 3857, 3863, 3869, 3877, 3885, 3895, 3906,
3918, 3929, 3938, 3947, 3955, 3962, 3969, 3976, 3980, 3983,
3987, 3993, 4002, 4013, 4027, 4044, 4060, 4072, 4078, 4082,
4089, 4098, 4108, 4118, 4128, 4138, 4148, 4158, 4169, 4179,
4189, 4200, 4211, 4221, 4233, 4244, 4255, 4267, 4279, 4291, 4303,
}; //added by M.C. to fix charger removal issue

static int ocv_chg[] = { //GoldenVE battery
3187, 3325, 3463, 3556, 3620, 3666, 3687, 3693, 3695, 3696,
3697, 3698, 3699, 3700, 3703, 3712, 3722, 3730, 3738, 3746,
3751, 3755, 3759, 3763, 3767, 3771, 3774, 3776, 3778, 3780,
3782, 3784, 3786, 3788, 3790, 3793, 3795, 3797, 3800, 3803,
3806, 3809, 3812, 3815, 3819, 3822, 3826, 3830, 3834, 3838,
3843, 3847, 3852, 3857, 3863, 3869, 3877, 3885, 3895, 3906,
3918, 3929, 3938, 3947, 3955, 3962, 3969, 3976, 3980, 3983,
3987, 3993, 4002, 4013, 4027, 4044, 4060, 4072, 4078, 4082,
4089, 4098, 4108, 4118, 4128, 4138, 4148, 4158, 4169, 4179,
4189, 4200, 4211, 4221, 4233, 4244, 4255, 4267, 4279, 4291, 4303,
};
#endif
/*
 * The rtot(Rtotal) of battery is much different in different temperature,
 * so we introduced data in different temperature, the typical sample point
 * of temperature are -5/10/25/40 C.
 * For charging case, we support the data of charging current of 1200 mA
 * in those temperatures, so we have 4 table for it.
 * For discharging case, we have data 500/1000/1500 mA discharging current
 * case, then we have 12 tables for it.
 */
static int rtot_tm20_i0p1[] = {//GoldenVE battery, modified from 68 to 100
12700, 12700, 12700, 12700, 12700, 12700, 10700, 8700, 7720, 7020,
6470, 6030, 5650, 5320, 5060, 4880, 4740, 4600, 4500, 4390,
4280, 4150, 4060, 3970, 3890, 3810, 3730, 3650, 3580, 3500,
3500, 3480, 3420, 3360, 3310, 3260, 3210, 3200, 3200, 3150,
3100, 3100, 3100, 3000, 3000, 3000, 2970, 2960, 2940, 2920,
2910, 2900, 2900, 2900, 2900, 3000, 3000, 3010, 3010, 3010,
3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010,
3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010,
3010, 3110, 3110, 3210, 3310, 3310, 3410, 3410, 3510, 3510,
3510, 3510, 3610, 3710, 3710, 3710, 3710, 3710, 3710, 3710};

static int rtot_tm20_i0p3[] = {//GoldenVE battery, modified from 68 to 100
12700, 12700, 12700, 12700, 12700, 12700, 10700, 8700, 7720, 7020,
6470, 6030, 5650, 5320, 5060, 4880, 4740, 4600, 4500, 4390,
4280, 4150, 4060, 3970, 3890, 3810, 3730, 3650, 3580, 3500,
3500, 3480, 3420, 3360, 3310, 3260, 3210, 3200, 3200, 3150,
3100, 3100, 3100, 3000, 3000, 3000, 2970, 2960, 2940, 2920,
2910, 2900, 2900, 2900, 2900, 3000, 3000, 3010, 3010, 3010,
3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010,
3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010,
3010, 3110, 3110, 3210, 3310, 3310, 3410, 3410, 3510, 3510,
3510, 3510, 3610, 3710, 3710, 3710, 3710, 3710, 3710, 3710};

static int rtot_tm20_i0p5[] = {//GoldenVE battery
12700, 12700, 12700, 12700, 12700, 12700, 10700, 8700, 7720, 7020,
6470, 6030, 5650, 5320, 5060, 4880, 4740, 4600, 4500, 4390,
4280, 4150, 4060, 3970, 3890, 3810, 3730, 3650, 3580, 3500,
3500, 3480, 3420, 3360, 3310, 3260, 3210, 3200, 3200, 3150,
3100, 3100, 3100, 3000, 3000, 3000, 2970, 2960, 2940, 2920,
2910, 2900, 2900, 2900, 2900, 3000, 3000, 3010, 3010, 3010,
3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010,
3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010,
3010, 3110, 3110, 3210, 3310, 3310, 3410, 3410, 3510, 3510,
3510, 3510, 3610, 3710, 3710, 3710, 3710, 3710, 3710, 3710};
/*
static int rtot_tm20_i0p5[] = {//GoldenVE battery
10500, 10500, 10500, 10500, 10500, 10500, 10500, 10500, 10500, 10500,
10500, 10500, 10500, 10500, 10500, 10500, 9600, 8790, 8060, 7390,
6780, 6230, 5730, 5280, 4870, 4510, 4190, 3900, 3640, 3420,
3220, 3040, 2880, 2790, 2650, 2540, 2460, 2380, 2320, 2270,
2220, 2180, 2140, 2110, 2080, 2050, 2020, 2000, 1970, 1950,
1930, 1920, 1900, 1890, 1870, 1860, 1860, 1850, 1850, 1850,
1850, 1860, 1860, 1850, 1850, 1850, 1840, 1840, 1840, 1830,
1820, 1810, 1800, 1810, 1820, 1840, 1850, 1860, 1860, 1850,
1840, 1850, 1840, 1840, 1840, 1840, 1840, 1840, 1840, 1840,
1840, 1840, 1840, 1840, 1840, 1840, 1840, 1840, 1840, 1840};
*/
static int rtot_tm20_i0p7[] = {//GoldenVE battery
9990, 9990, 9990, 9990, 9990, 9990, 9990, 9990, 9990, 9990,
9990, 9990, 9990, 9990, 9990, 9990, 9990, 9990, 9990, 9990,
9990, 9990, 9990, 9990, 9990, 9990, 9990, 9990, 9990, 9990,
9990, 9990, 9990, 9990, 9990, 9990, 9990, 9990, 9990, 9990,
9990, 9990, 9990, 9990, 9990, 9990, 9140, 8340, 7610, 6940,
6320, 5750, 5240, 4770, 4350, 3980, 3640, 3340, 3080, 2850,
2660, 2490, 2310, 2210, 2090, 2010, 1950, 1910, 1860, 1830,
1800, 1780, 1760, 1760, 1750, 1750, 1760, 1750, 1750, 1740,
1730, 1730, 1730, 1730, 1730, 1730, 1730, 1730, 1730, 1730,
1730, 1730, 1730, 1730, 1730, 1730, 1730, 1730, 1730, 1730};

static int rtot_tm20_i1p0[] = {//GoldenVE battery, equal to 0.7
12700, 12700, 12700, 12700, 12700, 12700, 10700, 8700, 7720, 7020,
6470, 6030, 5650, 5320, 5060, 4880, 4740, 4600, 4500, 4390,
4280, 4150, 4060, 3970, 3890, 3810, 3730, 3650, 3580, 3500,
3500, 3480, 3420, 3360, 3310, 3260, 3210, 3200, 3200, 3150,
3100, 3100, 3100, 3000, 3000, 3000, 2970, 2960, 2940, 2920,
2910, 2900, 2900, 2900, 2900, 3000, 3000, 3010, 3010, 3010,
3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010,
3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010, 3010,
3010, 3110, 3110, 3210, 3310, 3310, 3410, 3410, 3510, 3510,
3510, 3510, 3610, 3710, 3710, 3710, 3710, 3710, 3710, 3710};

static int rtot_tm20_i1p5[] = {// da modificare, ottenuto con interpolazione
2360, 2360, 2360, 2360, 2360, 2360, 2410, 2810, 3060, 3230,
3350, 3440, 3510, 3280, 2900, 2640, 2470, 2340, 2220, 2150,
2060, 1990, 1910, 1850, 1780, 1720, 1670, 1620, 1570, 1520,
1480, 1450, 1410, 1380, 1350, 1330, 1310, 1290, 1270, 1250,
1230, 1220, 1210, 1200, 1190, 1180, 1180, 1170, 1170, 1170,
1170, 1160, 1170, 1170, 1170, 1170, 1180, 1190, 1190, 1200,
1210, 1210, 1210, 1210, 1210, 1210, 1220, 1220, 1220, 1220,
1220, 1230, 1230, 1240, 1240, 1240, 1240, 1250, 1250, 1250,
1250, 1250, 1240, 1240, 1240, 1240, 1230, 1230, 1220, 1220,
1220, 1220, 1220, 1220, 1220, 1220, 1220, 1220, 1220, 1220};

static int rtot_tm5_i0p1[] = {//GoldenVE battery, modified from 68 to 100.
2400, 2400, 2400, 2400, 2400, 2410, 2330, 2200, 2100, 2100,
1960, 1960, 1960, 1860, 1800, 1750, 1700, 1600, 1560, 1490,
1460, 1460, 1460, 1460, 1460, 1460, 1460, 1460, 1460, 1460,
1460, 1460, 1460, 1460, 1460, 1400, 1300, 1200, 1000, 1000,
1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000,
1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000,
1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1100,
1200, 1250, 1250, 1300, 1360, 1400, 1460, 1460, 1460, 1460,
1460, 1460, 1460, 1460, 1460, 1460, 1460, 1460, 1460, 1460,
1500, 1560, 1560, 1560, 1560, 1560, 1560, 1600, 1600, 1660};

static int rtot_tm5_i0p3[] = {//GoldenVE battery, modified from 78 to 100
2440, 2440, 2440, 2440, 2440, 2220, 2130, 2000, 1950, 1900,
1800, 1700, 1600, 1600, 1500, 1450, 1450, 1350, 1300, 1300,
1260, 1260, 1260, 1260, 1260, 1260, 1260, 1260, 1260, 1260,
1260, 1260, 1260, 1260, 1260, 1160, 1100, 1000, 900, 900,
900, 900, 900, 900, 900, 900, 900, 900, 900, 900,
900, 900, 900, 900, 900, 900, 900, 900, 900, 900,
900, 900, 900, 900, 900, 900, 900, 900, 900, 900,
900, 1000, 1100, 1160, 1210, 1240, 1260, 1260, 1260, 1260,
1260, 1260, 1260, 1260, 1260, 1260, 1260, 1260, 1260, 1260,
1260, 1300, 1300, 1300, 1300, 1360, 1360, 1360, 1360, 1360};
/*
966, 950, 935, 920, 907, 893, 881, 869, 858, 847,
837, 828, 819, 812, 805, 799, 794, 789, 785, 781,
780, 778, 776, 777, 779, 783, 789, 802, 818, 835,
851, 865, 873, 878, 881, 882, 880, 877, 866, 850,
838, 834, 835, 841, 858, 880, 896, 895, 895, 895,
895, 895, 895, 895, 895, 895, 895, 895, 895, 895,
895, 895, 895, 895, 895, 895, 895, 895, 895, 895};*/

static int rtot_tm5_i0p5[] = {//GoldenVE battery
2470, 2470, 2470, 2470, 2240, 2040, 1880, 1760, 1670, 1580,
1510, 1450, 1400, 1350, 1320, 1300, 1280, 1260, 1240, 1220,
1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200,
1200, 1200, 1200, 1200, 1200, 1200, 1200, 1100, 1000, 900,
800, 800, 800, 800, 800, 800, 800, 800, 800, 800,
800, 800, 800, 800, 800, 800, 800, 800, 800, 800,
800, 800, 800, 800, 800, 800, 800, 800, 800, 800,
800, 900, 950, 1050, 1100, 1150, 1200, 1200, 1200, 1200,
1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200,
1200, 1250, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300};

static int rtot_tm5_i0p7[] = {//GoldenVE battery
1880, 1880, 1880, 1880, 1880, 1880, 1880, 1880, 1880, 1880,
1880, 1780, 1640, 1540, 1460, 1400, 1350, 1310, 1270, 1230,
1100, 1100, 1100, 1100, 1100, 1100, 1000, 1000, 1000, 1000,
1000, 1000, 900, 900, 900, 900, 900, 850, 850, 800,
800, 800, 800, 800, 800, 800, 800, 800, 800, 800,
800, 800, 800, 800, 800, 800, 800, 800, 800, 800,
800, 800, 800, 800, 800, 800, 800, 800, 800, 800,
800, 900, 900, 900, 1000, 1000, 1000, 1100, 1100, 1100,
1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100,
1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100};

static int rtot_tm5_i1p0[] = {//GoldenVE battery
1430, 1430, 1430, 1430, 1430, 1430, 1430, 1430, 1430, 1430,
1430, 1430, 1430, 1430, 1430, 1430, 1430, 1430, 1430, 1430,
1430, 1430, 1430, 1430, 1430, 1430, 1430, 1430, 1430, 1430,
1430, 1430, 1430, 1430, 1430, 1430, 1430, 1430, 1430, 1430,
1430, 1430, 1430, 1430, 1370, 1210, 1120, 1060, 1010, 978,
950, 927, 909, 894, 884, 878, 871, 865, 861, 859,
861, 862, 861, 860, 858, 856, 855, 853, 849, 846,
842, 841, 842, 846, 854, 863, 871, 873, 871, 869,
868, 869, 869, 869, 869, 869, 869, 869, 869, 869,
869, 869, 869, 869, 869, 869, 869, 869, 869, 869};

static int rtot_tm5_i1p5[] = {
1070, 1070, 1070, 1070, 1070, 1070, 1070, 1070, 1070, 1070,
1070, 1070, 1070, 1070, 1070, 1070, 1070, 1070, 1070, 1070,
1070, 1070, 1070, 1070, 1070, 1070, 1070, 1070, 1070, 1070,
1070, 1070, 1070, 1070, 1070, 1070, 1070, 1070, 1070, 1070,
1070, 1070, 1070, 1070, 1070, 1070, 1070, 1070, 1070, 1070,
1070, 1070, 1070, 1070, 1070, 1070, 1070, 1070, 1070, 1070,
1070, 1070, 1070, 1070, 1070, 983, 849, 758, 697, 653,
621, 597, 580, 566, 556, 549, 543, 543, 543, 543,
543, 543, 543, 543, 543, 543, 543, 543, 543, 543,
543, 543, 543, 543, 543, 543, 543, 543, 543, 543};

static int rtot_tm5_i0p13c[] = {//not chara
306, 306, 306, 306, 306, 306, 306, 306, 306, 404,
441, 452, 461, 471, 474, 434, 395, 385, 377, 378,
394, 421, 447, 460, 476, 504, 535, 548, 551, 547,
542, 535, 529, 523, 517, 513, 510, 506, 503, 499,
497, 496, 495, 494, 494, 495, 495, 497, 500, 504,
509, 515, 521, 530, 538, 546, 553, 555, 548, 529,
500, 470, 447, 428, 414, 399, 382, 362, 357, 371,
387, 396, 404, 415, 435, 461, 480, 496, 512, 521,
513, 501, 495, 491, 488, 487, 488, 487, 489, 489,
491, 493, 496, 500, 506, 513, 521, 527, 531, 546};


static int rtot_tm5_i0p3c[] = {// not charact.
487, 487, 487, 487, 487, 487, 487, 487, 487, 487,
487, 487, 487, 487, 486, 486, 489, 490, 495, 501,
503, 504, 512, 521, 530, 535, 537, 543, 548, 553,
558, 563, 570, 576, 580, 584, 588, 592, 596, 600,
603, 606, 609, 611, 614, 616, 618, 620, 622, 623,
624, 624, 624, 622, 617, 608, 597, 588, 584, 582,
582, 583, 585, 588, 591, 595, 600, 606, 612, 618,
624, 631, 637, 652, 661, 670, 678, 686, 697, 707,
717, 729, 742, 757, 775, 792, 814, 838, 863, 893,
929, 968, 1010, 1070, 1140, 1230, 1250, 1250, 1250, 1250};

static int rtot_tm5_i0p5c[] = {
487, 487, 487, 487, 487, 487, 487, 487, 487, 487,
487, 487, 487, 487, 486, 486, 489, 490, 495, 501,
503, 504, 512, 521, 530, 535, 537, 543, 548, 553,
558, 563, 570, 576, 580, 584, 588, 592, 596, 600,
603, 606, 609, 611, 614, 616, 618, 620, 622, 623,
624, 624, 624, 622, 617, 608, 597, 588, 584, 582,
582, 583, 585, 588, 591, 595, 600, 606, 612, 618,
624, 631, 637, 652, 661, 670, 678, 686, 697, 707,
717, 729, 742, 757, 775, 792, 814, 838, 863, 893,
929, 968, 1010, 1070, 1140, 1230, 1250, 1250, 1250, 1250};

static int rtot_tm5_i0p7c[] = {
374, 374, 374, 374, 374, 374, 374, 374, 374, 374,
374, 374, 374, 374, 374, 374, 374, 376, 380, 381,
381, 385, 390, 396, 401, 406, 410, 416, 423, 428,
435, 441, 445, 452, 458, 462, 468, 473, 478, 483,
488, 493, 498, 503, 508, 514, 520, 525, 527, 531,
535, 539, 541, 541, 537, 531, 528, 527, 528, 536,
541, 546, 550, 554, 558, 562, 565, 568, 571, 575,
578, 581, 584, 588, 593, 598, 605, 615, 625, 637,
651, 667, 687, 710, 737, 771, 815, 868, 936, 1030,
1130, 1230, 1320, 1390, 1430, 1430, 1430, 1430, 1430, 1430};

static int rtot_tm5_i1p0c[] = {
390, 390, 390, 390, 390, 390, 390, 390, 390, 390,
390, 390, 390, 390, 390, 390, 390, 390, 390, 390,
390, 390, 390, 390, 390, 390, 390, 390, 390, 390,
390, 390, 390, 390, 390, 390, 390, 390, 390, 390,
390, 390, 390, 390, 390, 390, 390, 390, 390, 390,
390, 390, 390, 390, 390, 390, 390, 390, 390, 390,
390, 390, 390, 390, 390, 390, 390, 390, 390, 390,
390, 390, 390, 390, 390, 390, 390, 390, 390, 390,
390, 399, 408, 415, 422, 428, 434, 441, 447, 456,
467, 481, 499, 522, 551, 595, 654, 745, 782, 782};

static int rtot_t10_i0p1[] = {//GoldenVE battery, modified from 73 t0 100
4220, 4220, 4220, 4220, 4220, 2640, 1810, 1180, 752, 631,
564, 564, 564, 564, 564, 564, 564, 564, 592, 568,
533, 533, 533, 533, 513, 484, 444, 414, 405, 396,
389, 384, 381, 379, 375, 371, 365, 360, 355, 351,
348, 345, 342, 339, 337, 334, 333, 333, 331, 330,
329, 328, 328, 329, 332, 341, 356, 379, 414, 454,
492, 516, 525, 520, 512, 511, 514, 516, 493, 446,
399, 363, 327, 327, 327, 327, 327, 327, 327, 327,
327, 327, 327, 327, 327, 327, 327, 327, 327, 327,
327, 327, 327, 327, 327, 327, 327, 327, 327, 327};

static int rtot_t10_i0p3[] = {//GoldenVE battery, modified from 78 to 100
1200, 1200, 1200, 1070, 948, 834, 728, 646, 584, 535,
495, 469, 447, 431, 438, 453, 460, 470, 474, 471,
464, 457, 450, 444, 438, 432, 424, 418, 411, 405,
399, 393, 391, 387, 383, 379, 377, 376, 372, 372,
371, 369, 366, 365, 363, 361, 359, 357, 355, 353,
352, 352, 352, 353, 355, 358, 365, 376, 391, 408,
423, 436, 443, 449, 455, 458, 460, 459, 449, 434,
424, 417, 409, 403, 404, 408, 409, 402, 402, 402,
402, 402, 402, 402, 402, 402, 402, 402, 402, 402,
402, 402, 402, 402, 402, 402, 402, 402, 402, 402};

static int rtot_t10_i0p5[] = {//GoldenVE battery, modified from 78 to 100
1690, 986, 884, 837, 807, 756, 697, 646, 604, 567,
536, 510, 486, 469, 467, 468, 466, 467, 468, 466,
462, 457, 454, 450, 446, 440, 434, 427, 421, 414,
408, 402, 397, 392, 387, 382, 378, 374, 371, 369,
366, 363, 361, 359, 357, 355, 354, 352, 350, 349,
349, 349, 350, 350, 350, 351, 354, 362, 370, 380,
390, 398, 404, 408, 410, 412, 411, 408, 402, 391,
384, 381, 380, 380, 386, 399, 407, 404, 404, 404,
404, 404, 404, 404, 404, 404, 404, 404, 404, 404,
404, 404, 404, 404, 404, 404, 404, 404, 404, 404};

static int rtot_t10_i0p7[] = {//GoldenVE battery
1710, 1710, 1710, 1710, 1710, 1420, 1170, 1040, 952, 880,
821, 772, 731, 696, 667, 652, 640, 627, 618, 610,
600, 589, 579, 571, 563, 555, 546, 537, 528, 520,
510, 503, 495, 487, 480, 472, 466, 459, 453, 447,
441, 436, 431, 427, 422, 419, 416, 413, 411, 409,
407, 406, 405, 405, 405, 406, 409, 413, 420, 428,
436, 442, 446, 449, 450, 451, 451, 450, 446, 439,
432, 430, 430, 433, 441, 451, 460, 463, 458, 450,
445, 443, 441, 440, 438, 435, 434, 433, 433, 433,
433, 433, 433, 433, 433, 433, 433, 433, 433, 433};

static int rtot_t10_i1p0[] = {//GoldenVE battery
1280, 1280, 1280, 1280, 1280, 1110, 1000, 926, 865, 817,
776, 742, 713, 688, 672, 660, 648, 638, 629, 618,
606, 595, 585, 576, 567, 557, 547, 538, 529, 520,
513, 505, 498, 492, 486, 480, 475, 470, 466, 461,
457, 453, 449, 445, 441, 438, 436, 433, 430, 428,
426, 424, 423, 423, 423, 424, 425, 428, 433, 438,
443, 448, 452, 455, 457, 459, 460, 460, 457, 455,
455, 458, 463, 470, 478, 488, 497, 501, 500, 498,
499, 502, 503, 503, 503, 503, 503, 503, 503, 503,
503, 503, 503, 503, 503, 503, 503, 503, 503, 503};

static int rtot_t10_i1p5[] = {
950, 950, 950, 950, 950, 950, 950, 950, 950, 950,
950, 950, 950, 950, 950, 950, 950, 950, 950, 950,
950, 950, 950, 950, 950, 950, 950, 950, 950, 950,
950, 950, 834, 713, 624, 559, 509, 471, 439, 414,
393, 374, 359, 345, 334, 323, 314, 306, 300, 294,
290, 287, 284, 282, 281, 281, 282, 283, 284, 286,
288, 290, 291, 292, 292, 293, 293, 292, 292, 292,
291, 290, 290, 289, 288, 287, 287, 287, 287, 287,
287, 287, 287, 287, 287, 287, 287, 287, 287, 287,
287, 287, 287, 287, 287, 287, 287, 287, 287, 287};

static int rtot_t10_i0p13c[] = {//GoldenVE battery
306, 306, 306, 306, 306, 306, 306, 306, 306, 404,
441, 452, 461, 471, 474, 434, 395, 385, 377, 378,
394, 421, 447, 460, 476, 504, 535, 548, 551, 547,
542, 535, 529, 523, 517, 513, 510, 506, 503, 499,
497, 496, 495, 494, 494, 495, 495, 497, 500, 504,
509, 515, 521, 530, 538, 546, 553, 555, 548, 529,
500, 470, 447, 428, 414, 399, 382, 362, 357, 371,
387, 396, 404, 415, 435, 461, 480, 496, 512, 521,
513, 501, 495, 491, 488, 487, 488, 487, 489, 489,
491, 493, 496, 500, 506, 513, 521, 527, 531, 546};

static int rtot_t10_i0p3c[] = {//GoldenVE battery
423, 423, 423, 423, 423, 423, 423, 423, 423, 423,
423, 423, 423, 423, 423, 416, 414, 413, 414, 419,
425, 431, 437, 443, 448, 452, 456, 458, 460, 461,
461, 461, 460, 460, 459, 459, 459, 459, 459, 459,
459, 459, 459, 460, 460, 460, 461, 461, 462, 461,
462, 462, 461, 459, 457, 454, 448, 440, 430, 419,
408, 399, 393, 388, 385, 382, 381, 382, 386, 392,
396, 395, 392, 387, 378, 369, 366, 376, 398, 319,
347, 342, 335, 328, 325, 324, 325, 328, 330, 333,
337, 342, 346, 352, 357, 369, 380, 386, 387, 380};

static int rtot_t10_i0p5c[] = {//GoldenVE battery
423, 423, 423, 423, 423, 423, 423, 423, 423, 423,
423, 423, 423, 423, 423, 416, 414, 413, 414, 419,
425, 431, 437, 443, 448, 452, 456, 458, 460, 461,
461, 461, 460, 460, 459, 459, 459, 459, 459, 459,
459, 459, 459, 460, 460, 460, 461, 461, 462, 461,
462, 462, 461, 459, 457, 454, 448, 440, 430, 419,
408, 399, 393, 388, 385, 382, 381, 382, 386, 392,
396, 395, 392, 387, 378, 369, 366, 376, 398, 319,
330, 332, 331, 330, 330, 331, 334, 337, 340, 348,
357, 363, 367, 372, 376, 380, 386, 390, 389, 380};

static int rtot_t10_i0p7c[] = {//GoldenVE battery
423, 423, 423, 423, 423, 423, 423, 423, 423, 423,
423, 423, 423, 423, 423, 416, 414, 413, 414, 419,
425, 431, 437, 443, 448, 452, 456, 458, 460, 461,
461, 461, 460, 460, 459, 459, 459, 459, 459, 459,
459, 459, 459, 460, 460, 460, 461, 461, 462, 461,
462, 462, 461, 459, 457, 454, 448, 440, 430, 419,
408, 399, 393, 388, 385, 382, 381, 382, 386, 392,
396, 395, 392, 387, 378, 369, 366, 376, 398, 319,
139, 145, 152, 161, 170, 180, 191, 202, 213, 225,
237, 248, 259, 270, 282, 293, 305, 317, 329, 342};

static int rtot_t10_i1p0c[] = {//GoldenVE battery
324, 324, 324, 324, 324, 324, 324, 324, 324, 324,
324, 324, 324, 324, 324, 324, 324, 324, 324, 324,
324, 324, 324, 323, 323, 323, 324, 325, 326, 328,
329, 330, 332, 333, 334, 335, 336, 337, 338, 339,
340, 341, 341, 342, 343, 344, 345, 346, 347, 347,
348, 349, 350, 350, 349, 348, 347, 345, 341, 337,
333, 330, 328, 328, 328, 329, 331, 334, 340, 347,
352, 358, 365, 369, 369, 368, 367, 372, 383, 395,
404, 410, 417, 424, 432, 442, 454, 466, 480, 498,
518, 540, 566, 598, 632, 675, 725, 744, 744, 744};

static int rtot_t25_i0p1[] = {//GoldenVE battery
9830, 3180, 1710, 1220, 929, 592, 390, 331, 303, 285,
274, 268, 266, 280, 351, 374, 338, 342, 334, 308,
292, 289, 291, 293, 288, 283, 279, 275, 273, 272,
272, 271, 271, 270, 269, 267, 266, 264, 262, 260,
258, 257, 256, 254, 253, 253, 253, 252, 250, 249,
249, 247, 248, 251, 256, 266, 280, 302, 330, 354,
369, 370, 361, 347, 333, 321, 311, 304, 295, 286,
274, 269, 265, 263, 260, 255, 258, 268, 267, 254,
246, 240, 237, 232, 229, 226, 223, 221, 220, 218,
214, 212, 212, 211, 210, 209, 205, 203, 202, 202 };

static int rtot_t25_i0p3[] = {//GoldenVE battery
2610, 1590, 853, 661, 553, 451, 379, 339, 313, 294,
279, 268, 261, 260, 279, 295, 293, 291, 290, 284,
277, 272, 270, 269, 267, 265, 263, 261, 258, 257,
256, 255, 254, 253, 252, 251, 249, 248, 246, 244,
243, 241, 240, 238, 237, 235, 234, 232, 231, 230,
229, 229, 229, 229, 231, 235, 241, 251, 264, 278,
291, 300, 304, 304, 303, 299, 296, 292, 283, 273,
265, 261, 257, 255, 255, 257, 261, 261, 252, 242,
238, 237, 235, 233, 231, 230, 228, 226, 225, 223,
221, 220, 219, 218, 217, 217, 217, 217, 217, 217 };

static int rtot_t25_i0p5[] = {//GoldenVE battery
1730, 1200, 706, 581, 514, 452, 401, 365, 339, 319,
302, 288, 278, 272, 280, 289, 292, 294, 294, 290,
284, 280, 277, 275, 272, 269, 266, 263, 260, 258,
256, 254, 252, 251, 249, 248, 247, 245, 244, 243,
242, 240, 239, 238, 237, 236, 234, 233, 232, 231,
230, 229, 229, 229, 230, 231, 235, 241, 250, 259,
268, 274, 278, 280, 281, 281, 280, 278, 272, 265,
259, 256, 255, 255, 257, 262, 266, 263, 254, 245,
241, 240, 240, 238, 237, 235, 233, 232, 231, 229,
228, 227, 227, 227, 227, 227, 227, 227, 227, 227 };

static int rtot_t25_i0p7[] = {//GoldenVE battery
1320, 1040, 668, 574, 523, 476, 433, 401, 375, 354,
336, 320, 307, 298, 300, 304, 305, 306, 307, 304,
300, 296, 292, 289, 286, 282, 278, 274, 271, 267,
265, 262, 259, 257, 255, 253, 251, 249, 248, 247,
245, 244, 243, 242, 241, 240, 240, 239, 238, 237,
236, 235, 234, 234, 235, 236, 239, 243, 249, 256,
262, 268, 271, 272, 273, 274, 274, 272, 267, 261,
256, 254, 254, 256, 261, 267, 272, 269, 261, 252,
248, 247, 246, 245, 245, 243, 242, 241, 241, 241,
241, 241, 241, 241, 241, 241, 241, 241, 241, 241 };

static int rtot_t25_i1p0[] = { //GoldenVE battery
1030, 1030, 784, 683, 633, 586, 543, 507, 477, 452,
430, 411, 394, 381, 375, 372, 367, 365, 362, 357,
352, 347, 343, 338, 334, 329, 323, 318, 313, 307,
303, 298, 294, 290, 286, 282, 279, 276, 273, 270,
268, 266, 264, 263, 261, 260, 259, 258, 258, 257,
257, 256, 256, 256, 257, 258, 261, 264, 268, 274,
278, 282, 285, 286, 287, 287, 286, 284, 281, 276,
272, 271, 272, 275, 280, 287, 293, 293, 288, 282,
278, 276, 275, 275, 275, 275, 275, 275, 275, 275,
275, 275, 275, 275, 275, 275, 275, 275, 275, 275 };


static int rtot_t25_i1p5[] = {
877, 877, 877, 877, 877, 877, 877, 877, 877, 877,
877, 877, 856, 686, 576, 502, 447, 407, 377, 352,
331, 312, 296, 282, 270, 260, 251, 243, 236, 230,
225, 220, 215, 211, 208, 204, 201, 199, 196, 194,
192, 190, 189, 187, 186, 185, 184, 184, 184, 183,
183, 183, 183, 183, 183, 184, 185, 187, 189, 191,
193, 194, 195, 195, 195, 195, 194, 194, 193, 192,
191, 191, 190, 189, 188, 188, 187, 187, 187, 187,
187, 187, 187, 187, 187, 187, 187, 187, 187, 187,
187, 187, 187, 187, 187, 187, 187, 187, 187, 187};

static int rtot_t25_i0p13c[] = { //GoldenVE battery
298, 298, 298, 298, 298, 298, 298, 298, 297, 299,
306, 315, 328, 345, 363, 345, 328, 326, 318, 319,
332, 341, 358, 384, 395, 390, 388, 389, 388, 385,
380, 374, 368, 362, 357, 353, 349, 346, 343, 341,
339, 338, 337, 337, 338, 339, 339, 342, 346, 349,
353, 359, 365, 371, 378, 384, 388, 388, 383, 374,
358, 331, 308, 290, 277, 268, 263, 263, 275, 284,
281, 275, 272, 273, 275, 279, 285, 299, 317, 321,
313, 305, 301, 298, 297, 296, 294, 294, 292, 292,
295, 297, 298, 300, 301, 302, 304, 306, 303, 296};

static int rtot_t25_i0p3c[] = { //GoldenVE battery
215, 215, 215, 215, 215, 215, 215, 215, 219, 225,
232, 240, 250, 258, 247, 237, 235, 233, 233, 237,
245, 253, 261, 264, 264, 263, 262, 260, 258, 256,
254, 252, 251, 249, 248, 248, 248, 249, 249, 250,
251, 252, 254, 257, 259, 261, 264, 267, 271, 275,
279, 284, 290, 295, 300, 304, 303, 297, 285, 269,
254, 241, 233, 227, 224, 223, 222, 223, 227, 232,
234, 236, 240, 245, 252, 255, 254, 252, 255, 260,
262, 262, 261, 262, 264, 265, 266, 269, 270, 272,
276, 279, 282, 286, 290, 293, 297, 318, 339, 358};

static int rtot_t25_i0p5c[] = {//GoldenVE battery
214, 214, 214, 214, 214, 214, 214, 214, 214, 214,
214, 220, 228, 233, 228, 221, 220, 218, 216, 219,
224, 229, 231, 232, 232, 231, 230, 229, 228, 228,
227, 226, 226, 226, 226, 227, 227, 228, 229, 230,
232, 234, 235, 238, 240, 242, 244, 247, 250, 253,
255, 257, 258, 259, 259, 257, 254, 248, 239, 230,
221, 214, 210, 207, 205, 204, 204, 205, 209, 214,
218, 220, 223, 226, 228, 228, 227, 227, 231, 237,
239, 239, 239, 241, 242, 243, 245, 247, 249, 252,
255, 259, 262, 272, 284, 294, 307, 321, 338, 355};

static int rtot_t25_i0p7c[] = {//GoldenVE battery
218, 218, 218, 218, 218, 218, 218, 218, 218, 218,
218, 218, 218, 218, 218, 213, 212, 209, 208, 211,
214, 217, 217, 217, 216, 216, 215, 215, 215, 215,
215, 215, 215, 216, 217, 217, 219, 220, 221, 223,
224, 226, 227, 229, 230, 231, 232, 233, 234, 235,
235, 236, 236, 237, 236, 235, 232, 228, 221, 215,
209, 204, 201, 199, 198, 198, 198, 199, 203, 209,
213, 215, 217, 219, 220, 218, 217, 217, 223, 228,
231, 231, 232, 233, 235, 237, 239, 241, 243, 248,
257, 263, 270, 278, 286, 296, 308, 321, 337, 352};

static int rtot_t25_i1p0c[] = {//GoldenVE battery
204, 204, 204, 204, 204, 204, 204, 204, 204, 204,
204, 204, 204, 204, 204, 204, 204, 204, 204, 204,
205, 206, 205, 205, 205, 205, 205, 206, 207, 207,
208, 209, 210, 212, 213, 214, 215, 216, 217, 217,
218, 219, 219, 220, 220, 221, 221, 221, 221, 222,
222, 222, 222, 222, 222, 221, 219, 216, 212, 207,
202, 199, 197, 196, 195, 195, 196, 198, 202, 207,
211, 214, 216, 217, 216, 213, 211, 212, 217, 223,
226, 228, 229, 231, 236, 240, 243, 246, 250, 254,
259, 264, 270, 276, 284, 293, 303, 314, 327, 337};

static int rtot_t40_i0p1[] = {//GoldenVE battery, I = 0.1A
9020, 9020, 2530, 1720, 1290, 854, 468, 335, 301, 288,
282, 281, 284, 302, 382, 430, 390, 394, 388, 358,
343, 344, 351, 352, 343, 332, 320, 309, 298, 283,
261, 242, 228, 219, 214, 209, 205, 201, 198, 196,
193, 190, 189, 186, 184, 184, 182, 180, 179, 178,
175, 174, 175, 177, 181, 188, 198, 209, 211, 204,
197, 197, 199, 199, 199, 196, 189, 169, 133, 91,
79, 99, 118, 128, 142, 171, 228, 268, 237, 190,
166, 159, 156, 153, 151, 149, 147, 144, 144, 140,
135, 134, 132, 130, 131, 129, 128, 128, 130, 130};

static int rtot_t40_i0p3[] = {//GoldenVE battery
3020, 3020, 931, 666, 525, 386, 282, 241, 224, 215,
208, 204, 201, 203, 228, 245, 238, 236, 235, 228,
222, 218, 219, 220, 219, 216, 213, 209, 206, 203,
201, 199, 197, 195, 193, 191, 189, 187, 185, 183,
181, 178, 176, 174, 172, 170, 168, 166, 165, 163,
163, 162, 162, 162, 163, 166, 169, 176, 184, 192,
197, 199, 199, 196, 194, 191, 186, 179, 166, 152,
146, 149, 153, 156, 161, 174, 193, 200, 190, 172,
164, 161, 159, 158, 156, 155, 154, 152, 151, 150,
148, 147, 146, 145, 145, 145, 145, 145, 145, 145};

static int rtot_t40_i0p5[] = {//GoldenVE battery
2170, 2170, 609, 459, 380, 306, 253, 227, 212, 202,
195, 189, 185, 185, 197, 208, 208, 208, 207, 204,
201, 198, 198, 198, 197, 196, 195, 193, 191, 190,
189, 187, 186, 185, 184, 182, 181, 179, 178, 176,
174, 173, 171, 169, 168, 166, 165, 163, 162, 160,
159, 158, 158, 158, 158, 160, 163, 169, 175, 183,
189, 193, 194, 193, 191, 189, 186, 181, 173, 164,
159, 159, 160, 162, 166, 175, 185, 188, 180, 170,
165, 163, 161, 159, 158, 157, 156, 155, 154, 152,
151, 150, 150, 150, 150, 150, 150, 150, 150, 150};

static int rtot_t40_i0p7[] = {//GoldenVE battery
1340, 1340, 479, 379, 327, 280, 244, 223, 209, 199,
191, 185, 180, 178, 186, 194, 196, 198, 199, 197,
195, 193, 192, 192, 191, 190, 189, 188, 186, 185,
184, 183, 182, 181, 180, 179, 178, 176, 175, 174,
172, 171, 169, 168, 167, 165, 164, 163, 161, 160,
159, 158, 158, 157, 157, 158, 161, 165, 170, 177,
182, 186, 188, 189, 188, 187, 185, 182, 176, 170,
165, 164, 164, 166, 169, 176, 183, 184, 178, 170,
166, 164, 163, 162, 161, 159, 158, 157, 157, 157,
157, 157, 157, 157, 157, 157, 157, 157, 157, 157};

static int rtot_t40_i1p0[] = { //GoldenVE battery
1050, 1050, 428, 349, 310, 275, 246, 227, 213, 202,
194, 187, 182, 179, 183, 188, 191, 194, 196, 195,
194, 193, 192, 191, 191, 189, 188, 187, 185, 184,
183, 181, 180, 179, 178, 177, 176, 175, 174, 173,
172, 171, 170, 169, 168, 167, 166, 165, 164, 163,
162, 161, 160, 160, 160, 160, 162, 165, 169, 173,
177, 181, 182, 183, 184, 183, 183, 181, 177, 172,
169, 167, 167, 169, 173, 178, 183, 183, 178, 172,
169, 167, 167, 167, 167, 167, 167, 167, 167, 167,
167, 167, 167, 167, 167, 167, 167, 167, 167, 167};

static int rtot_t40_i1p5[] = {
829, 829, 829, 829, 829, 829, 829, 829, 583, 455,
383, 338, 306, 284, 268, 257, 246, 234, 224, 214,
206, 201, 197, 195, 192, 189, 186, 183, 180, 177,
174, 171, 168, 166, 164, 162, 160, 159, 157, 156,
156, 155, 154, 154, 154, 154, 153, 153, 153, 152,
152, 152, 151, 151, 152, 153, 154, 155, 157, 160,
161, 163, 164, 164, 164, 164, 164, 163, 163, 162,
162, 161, 161, 160, 160, 159, 158, 158, 158, 158,
158, 158, 158, 158, 158, 158, 158, 158, 158, 158,
158, 158, 158, 158, 158, 158, 158, 158, 158, 158};

static int rtot_t40_i0p13c[] = { //GoldenVE battery
362, 362, 362, 350, 311, 279, 223, 192, 187, 189,
195, 206, 224, 253, 285, 277, 259, 260, 259, 248,
161, 165, 167, 167, 167, 167, 167, 167, 167, 167,
167, 167, 167, 167, 168, 169, 170, 171, 173, 175,
176, 178, 180, 182, 185, 187, 190, 192, 195, 199,
202, 205, 208, 211, 213, 213, 211, 206, 198, 200,
488, 459, 438, 426, 421, 418, 422, 428, 436, 468,
525, 584, 628, 626, 564, 480, 417, 397, 426, 472,
597, 607, 612, 615, 617, 620, 623, 627, 630, 633,
637, 640, 641, 641, 642, 642, 700, 700, 700, 700};

static int rtot_t40_i0p3c[] = { //GoldenVE battery
158, 158, 158, 158, 158, 158, 158, 159, 163, 168,
174, 180, 189, 197, 189, 181, 181, 180, 179, 182,
180, 180, 180, 180, 180, 180, 180, 180, 180, 180,
180, 180, 180, 180, 180, 180, 180, 180, 180, 180,
176, 178, 180, 182, 185, 187, 190, 192, 195, 199,
202, 205, 208, 211, 213, 213, 211, 206, 198, 200,
201, 194, 190, 188, 187, 187, 189, 193, 200, 208,
214, 217, 221, 224, 222, 209, 194, 189, 196, 206,
270, 326, 348, 350, 351, 354, 356, 359, 361, 363,
400, 400, 400, 400, 400, 400, 400, 400, 400, 400};

static int rtot_t40_i0p5c[] = { //GoldenVE battery
145, 145, 145, 145, 145, 145, 145, 145, 145, 145,
148, 154, 160, 165, 160, 154, 154, 153, 152, 155,
180, 180, 180, 180, 180, 180, 180, 180, 180, 180,
180, 180, 180, 180, 180, 180, 180, 180, 180, 180,
176, 178, 180, 182, 185, 187, 190, 192, 195, 199,
202, 205, 208, 211, 213, 213, 211, 206, 198, 200,
201, 194, 190, 188, 187, 187, 189, 193, 200, 208,
214, 217, 221, 224, 222, 209, 194, 189, 196, 206,
211, 214, 216, 218, 220, 222, 224, 226, 227, 230,
233, 235, 237, 238, 246, 261, 274, 291, 313, 340};


static int rtot_t40_i0p7c[] = { //GoldenVE battery
160, 160, 160, 160, 160, 160, 160, 160, 160, 160,
160, 160, 160, 160, 156, 151, 151, 149, 148, 151,
155, 157, 157, 158, 158, 158, 157, 157, 157, 157,
158, 158, 159, 159, 160, 161, 163, 164, 165, 167,
169, 170, 172, 174, 176, 178, 180, 182, 184, 186,
187, 189, 190, 190, 190, 189, 187, 183, 177, 191,
185, 181, 178, 177, 177, 177, 179, 182, 187, 194,
199, 202, 204, 205, 203, 194, 185, 182, 188, 195,
199, 201, 203, 205, 206, 208, 210, 212, 214, 216,
218, 225, 234, 242, 251, 261, 274, 290, 311, 338};

static int rtot_t40_i1p0c[] = { //GoldenVE battery
168, 168, 168, 168, 168, 168, 168, 168, 168, 168,
168, 168, 168, 168, 168, 168, 168, 168, 168, 168,
170, 171, 171, 171, 171, 170, 170, 171, 171, 172,
172, 173, 174, 175, 176, 177, 179, 180, 181, 182,
183, 184, 185, 186, 187, 188, 188, 189, 190, 190,
191, 191, 192, 192, 192, 191, 189, 186, 182, 177,
174, 170, 169, 168, 168, 169, 170, 172, 177, 182,
186, 189, 190, 189, 186, 181, 176, 175, 180, 185,
188, 189, 191, 192, 194, 195, 197, 202, 207, 211,
216, 220, 226, 232, 238, 247, 256, 267, 281, 296};

static int* dis_chg_rtot[6][5] = {
	{rtot_tm20_i0p1,  rtot_tm5_i0p1,  rtot_t10_i0p1,  rtot_t25_i0p1,  rtot_t40_i0p1},
	{rtot_tm20_i0p3,  rtot_tm5_i0p3,  rtot_t10_i0p3,  rtot_t25_i0p3,  rtot_t40_i0p3},
	{rtot_tm20_i0p5,  rtot_tm5_i0p5,  rtot_t10_i0p5,  rtot_t25_i0p5,  rtot_t40_i0p5},
	{rtot_tm20_i0p7,  rtot_tm5_i0p7,  rtot_t10_i0p7,  rtot_t25_i0p7,  rtot_t40_i0p7},
	{rtot_tm20_i1p0,  rtot_tm5_i1p0,  rtot_t10_i1p0,  rtot_t25_i1p0,  rtot_t40_i1p0},
	{rtot_tm20_i1p5,  rtot_tm5_i1p5,  rtot_t10_i1p5,  rtot_t25_i1p5,  rtot_t40_i1p5}
};

static int dischg_ib[6] = {100, 300, 500, 700, 1000, 1500};

static int* chg_rtot[5][4] = {
	{rtot_tm5_i0p13c, rtot_t10_i0p13c, rtot_t25_i0p13c, rtot_t40_i0p13c},
	{rtot_tm5_i0p3c,  rtot_t10_i0p3c,  rtot_t25_i0p3c,  rtot_t40_i0p3c},
	{rtot_tm5_i0p5c,  rtot_t10_i0p5c,  rtot_t25_i0p5c,  rtot_t40_i0p5c},
	{rtot_tm5_i0p7c,  rtot_t10_i0p7c,  rtot_t25_i0p7c,  rtot_t40_i0p7c},
	{rtot_tm5_i1p0c,  rtot_t10_i1p0c,  rtot_t25_i1p0c,  rtot_t40_i1p0c}
};

static int chg_ib[5] = {130,300,500,700, 1000};

#endif
