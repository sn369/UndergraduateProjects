# -*- coding: utf-8 -*-
"""
1. f(x) = 1/(1+x^2)插值
姓名：桑娜
学号：16343065
""" 
#==============================================================================
import numpy as np
import matplotlib.pyplot as plt
from scipy import interpolate

def f(x):
    return 1/(1 + x*x)

X_insr = np.linspace(-5, 5, 11)         # 插值节点

X_draw = np.linspace(-5, 5, 101)      # 绘图节点
Y_draw = 1 / (1 + X_draw**2)

#X_comput = np.linspace(-5, 5, 101)      # 计算误差节点
#Y_comput = 1 / (1 + X_comput**2)        

"""
This function is for drawing picture of true function and 
new function we receive via diffrent interpolation method.
Plus, it can compute the average error of each interpolation.
"""
def draw_pic(X_insr, T_draw, title, new_f_name):

    Y_insr = 1 / (1 + X_insr**2) 
    fig = plt.figure(figsize=(8,5))
    ax = fig.subplots()
    ax.set_title(title,fontsize=18)
    
    ax.scatter(X_insr, Y_insr, s=90, c='b')
    ax.plot(X_draw, Y_draw, 'b', linewidth=2, label="f (x)")
    ax.plot(X_draw, T_draw, 'c--', linewidth=2, label = new_f_name)
    ax.grid() 
       
    leg = plt.legend(fontsize=15)
    leg.get_frame().set_alpha(0.5)
    plt.show()
    
    error = np.sum(abs(T_draw - Y_draw))/101
    print("Error of", title, "is", error)
    
#==============================================================================        
#(1) Lagrange interpolation
def get_Li(xi, X_insr):
    def Li(x):
        result = 1
        for each_x in X_insr:
            if each_x != xi:
                result *= (x - each_x)
                result /= (xi - each_x)
        return result
    return Li
        
def Lagrange_interpolation(f, X_insr):
    def Lag(x):
        result = 0
        for xi in X_insr:
            result += get_Li(xi, X_insr)(x)*f(xi)
        return result
    return Lag

Lag_func = Lagrange_interpolation(f,X_insr)
T_draw = [Lag_func(x) for x in X_draw]               
draw_pic(X_insr, T_draw, "Lagrange interpolation", "Lag(x)")

#==============================================================================   
#(2) Hermite interpolation

def get_alpha_i(xi):
    def alpha_i(x):
        result = 0
        for each_x in X_insr:
            if each_x != xi:
                result += 1/(xi - each_x)
        result = (1 + 2*(xi-x)*result)*(get_Li(xi, X_insr)(x))**2
        return result
    return alpha_i
                
def get_beta_i(xi):
    def beta_i(x):
        return (x - xi)*(get_Li(xi, X_insr)(x)**2)
    return beta_i

"""
derivation
"""
def d(x):
    return (-2)*x/((1+x**2)**2)

def Hermite_interpolation(f):
    def H(x):
        result = 0
        for xi in X_insr:
            result += f(xi)*get_alpha_i(xi)(x)
            result += d(xi)*get_beta_i(xi)(x)
        return result
    return H

H = Hermite_interpolation(f)
T_draw = [H(x) for x in X_draw]    
draw_pic(X_insr, T_draw, "Hermite interpolation", "H (x)")

#==============================================================================
#(3) Chabyshev零点lagrange插值
import math
x_zero_points = np.array([5*math.cos((2*k+1)*math.pi/22) for k in range(11)])
Lag = Lagrange_interpolation(f, x_zero_points)
T_draw = [Lag(x) for x in X_draw]  
draw_pic(x_zero_points, T_draw, "Chabyshev-Lagrange interpolation","L(x)")

#==============================================================================
#(4) 分段线性插值
def linear(x1, x2):
    def linear_func(x):
        w1 = (x-x2)/(x1-x2)
        w2 = (x-x1)/(x2-x1)
        return w1*f(x1) + w2*f(x2)
    return linear_func

I = []
for i in range(len(X_insr)-1):
    I.append(linear(X_insr[i], X_insr[i+1]))

T_draw = [] 
for i in range(len(X_insr)-1):
    for x in X_draw[i*10:i*10+10]:
        T_draw.append(I[i](x))
T_draw.append(f(5))
draw_pic(X_insr, T_draw, "Peicewise Linear interpolation", "I (x)")

#==============================================================================
#(5) 分段3次Hermite插值
def I_hermite(x1, x2):
    def I_h(x):
        w1 = (x-x2)/(x1-x2)
        w2 = (x-x1)/(x2-x1)
        result = w1*w1*((1 + 2*w2)*f(x1) + (x-x1)*d(x1))
        result += w2*w2*((1 + 2*w1)*f(x2) + (x-x2)*d(x2))
        return result
    return I_h

I_h = []
for i in range(len(X_insr)-1):
    I_h.append(I_hermite(X_insr[i], X_insr[i+1]))
T_draw = [] 
for i in range(len(X_insr)-1):
    for x in X_draw[i*10:i*10+10]:
        T_draw.append(I_h[i](x))
T_draw.append(f(5))
draw_pic(X_insr, T_draw, "Peicewise Hermite interpolation", "I_h(x)")

#==============================================================================
#(6) 3次样条插值
Y_insr = 1 / (1 + X_insr**2) 
t = interpolate.splrep(X_insr, Y_insr)
T_draw = interpolate.splev(X_draw, t)
draw_pic(X_insr, T_draw, "Cubic spline interpolation", "T(x)")