#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Dec 19 14:42:40 2018

@author: sangna
"""

import numpy as np
import matplotlib.pyplot as plt
import scipy
from scipy import stats

A = np.random.randn(50, 100)
e = (0.1**0.5)*np.random.randn(50, 1)
real_x = scipy.sparse.random(100, 1, 0.05, data_rvs=stats.norm().rvs).toarray()
b = A.dot(real_x) + e

# 软门限算子


def SoftThreshold(x_in, k, arg_p):
    Thres = k*arg_p
    if x_in < -Thres:
        return x_in+Thres
    elif x_in > Thres:
        return x_in-Thres
    else:
        return 0

# 求S(x)梯度


def DS(x_in):
    ans = A.dot(x_in)-b
    ans = A.T.dot(ans)
    return ans


"""
邻近点梯度法 【Proximal Gradient Method ，PG】
n：迭代次数
alpha：步长
p：惩罚项系数
"""


def PG(n, alpha, p):
    x0 = np.zeros((100, 1))
    x = [x0]
    for i in range(n):
        alpha = alpha / (i+1)**0.5
        mid = x[i] - alpha*DS(x[i])
        x_out = np.zeros((100, 1))
        for j in range(100):
            x_out[j] = SoftThreshold(mid[j], alpha, p)
        x.append(x_out)
    return x


"""
交替方向乘子法
迭代次数：n
步长：c
"""


def ADMM(n, c, p):
    x0 = np.zeros((100, 1))
    z0 = np.zeros((100, 1))
    v0 = np.zeros((100, 1))
    x, z, v = [x0], [z0], [v0]
    I_mat = np.eye(100)  # 100维的单位矩阵
    coef = A.T.dot(A) + c*I_mat
    coef = np.asmatrix(coef).I
    bias = A.T.dot(b)
    for i in range(n):
        z_out = np.zeros((100, 1))
        x_out = coef.dot(v[i]+c*z[i]+bias)
        x.append(x_out)
        for j in range(100):
            temp = (x[i+1]-v[i]/c)[j]
            z_out[j] = SoftThreshold(temp, 1/c, p)
        z.append(z_out)
        v_out = v[i] + c*(z[i+1]-x[i+1])
        v.append(v_out)
    return x


"""
次梯度法
n：迭代次数
step：递减步长
"""


def sgn(x_in):
    if x_in > 0:
        return 1
    elif x_in < 0:
        return -1
    else:
        return 0


def SubGradient(n, step, p):
    x0 = np.zeros((100, 1))
    x = [x0]
    for i in range(n):
        x_out = np.zeros((100, 1))
        step = step / (i+1)**0.5
        for j in range(100):
            x_out[j] = x[i][j] - step*(DS(x[i])[j]+p*sgn(x[i][j]))
        x.append(x_out)
    return x

# 绘制每次迭代结果到真值和最优解的距离


def draw_pic(x, method):
    L2_real = []
    for vec_x in x:
        L2_real.append(np.linalg.norm(vec_x-real_x))
    plt.figure()
    plt.title("L2 distance from true value using "+method)
    plt.plot(L2_real, 'blue')
    plt.grid()

    i = np.argmin(L2_real)  # 最优解的下标记为i
    opt_x = x[i]
    plt.scatter(i, L2_real[i], c='purple', s=50, marker='*')
    #print(L2_real)

    L2_opt = []
    for vec_x in x:
        L2_opt.append(np.linalg.norm(vec_x-opt_x))
    plt.figure()
    plt.title("L2 distance from optimal value using "+method)
    plt.plot(L2_opt, 'orange')
    plt.scatter(i, L2_opt[i], c='purple', s=50, marker='*')
    plt.grid()
    #print(L2_opt)
    return

# 绘制不同p的结果


def draw(arg_n, arg_step, value_p, method):
    list_x = []
    list_p = []
    for p in value_p:
        if method == 'PG':
            list_x.append(PG(arg_n, arg_step, p))
        elif method == 'ADMM':
            list_x.append(ADMM(arg_n, arg_step, p))
        else:
            list_x.append(SubGradient(arg_n, arg_step, p))
        list_p.append('p='+str(p))
    list_p = tuple(list_p)
    plt.figure()
    for my_x in list_x:
        error = []
        for vec_x in my_x:
            error.append(np.linalg.norm(vec_x-real_x))
        plt.plot(error)
    plt.grid()
    plt.title('L2 distance from true value using ' + method)
    plt.legend(list_p, loc='upper right')
    return


def PG_Solution():
    n = 15
    alpha = 0.01
    value_p = [0.5, 1, 5, 10, 15]   # PG with small p
    draw(n, alpha, value_p, 'PG')
    value_p = [15, 30, 50, 80, 100]  # PG with large p
    draw(n, alpha, value_p, 'PG')
    draw_pic(PG(n, alpha, 15), 'PG')
    return


def ADMM_Solution():
    n = 80
    c = 3
    value_p = [0.0001, 0.001, 0.005, 0.01, 0.1]  # ADMM with small p
    draw(n, c, value_p, 'ADMM')
    value_p = [0.1, 0.2, 0.5, 0.8, 1.2]  # ADMM with large p
    draw(n, c, value_p, 'ADMM')
    draw_pic(ADMM(n, c, 0.1), 'ADMM')


def SG_Solution():
    n = 15
    alpha = 0.01
    value_p = [0.01, 0.05, 0.1, 0.5, 1]   # SG with small p
    draw(n, alpha, value_p, 'SG')
    value_p = [1, 5, 8, 10, 20]  # SG with large p
    draw(n, alpha, value_p, 'SG')
    draw_pic(PG(n, alpha, 5), 'SG')


PG_Solution()
ADMM_Solution()
SG_Solution()
