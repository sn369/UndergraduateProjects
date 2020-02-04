# -*- coding: utf-8 -*-

"""
2. f(x) = 1/(1+x^2)数值积分
姓名：桑娜
学号：16343065 
"""
import numpy as np
import matplotlib.pyplot as plt
import math
##=============================================================================
"""
数值积分
"""
def f(x):
    return 1/(1 + x*x)

true_val = math.pi/2
def compute_error(I):
    print("Accurate value: ", true_val)
    print("I ≈ ", I)
    print("error: ", true_val - I)
    
#==============================================================================
# (1) 复合梯形公式
def linear(x1, x2):
    def linear_func(x):
        w1 = (x-x2)/(x1-x2)
        w2 = (x-x1)/(x2-x1)
        return w1*f(x1) + w2*f(x2)
    return linear_func
M = 10
h = 2/M
X = np.linspace(-1, 1, M+1)
Tn = 0
for xi in X:
    Tn = Tn + 2*f(xi)
Tn = Tn - f(-1) - f(1)
Tn = Tn * h / 2

print("复合梯形公式")
compute_error(Tn)
print("\n")

X_draw = np.linspace(-1, 1, 1001)      # 绘图节点
Y_draw = 1 / (1 + X_draw**2)

new_y = []
step = 100 # 100 / M 
for i in range(len(X)-1):
    for x in X_draw[i*step: i*step+step]:
        new_y.append(linear(X[i], X[i+1])(x))
new_y.append(f(1))
fig = plt.figure(figsize=(8,4))
ax = fig.subplots()
ax.set_title("Compound trapezoid formula",fontsize=20)
ax.scatter(X, [f(xk) for xk in X], s=90, c='b')
ax.plot(X_draw, Y_draw, 'b', linewidth=2.1, label="f (x)")
ax.plot(X_draw, new_y, 'c', linewidth=2.1)
plt.fill(X_draw, new_y, 'c', alpha = 0.5, label="Tn")
leg = plt.legend(fontsize=15)
leg.get_frame().set_alpha(0.5)
plt.show()
##=============================================================================
#(2) 复合辛普森公式
M = 10
h = 2/M
X = np.linspace(-1, 1, M+1)
Tn = f(-1) + f(1)
for i in range(10):
    Tn = Tn + 4*f((X[i] + X[i+1])/2)
for xi in X[1:10]:
    Tn = Tn + 2*f(xi)
Tn = Tn * h /6

print("复合辛普森公式")
compute_error(Tn)
print("\n")

##=============================================================================
#(3) 复合两点高斯公式
x = [-1/math.sqrt(3),1/math.sqrt(3)]

M = 10
mid = [-1 + (2*i+1)/M for i in range(M)]
I = 0
for x_mid in mid:
    I = I + f(x_mid+x[0]/M) + f(x_mid + x[1]/M)
I = I/M
print("复合两点高斯公式")
compute_error(I)
print("\n")

##=============================================================================
#(3) 复合三点高斯公式
x = [math.sqrt(15)/5,0]
A = [ 5/9, 8/9]

M = 10
mid = [-1 + (2*i+1)/M for i in range(M)]

I = 0
for x_mid in mid:
    I = I + A[0]*(f(x_mid+x[0]/M) + f(x_mid-x[0]/M) )+ A[1]*f(x_mid + x[1]/M)
I = I/M
print("复合三点高斯公式")
compute_error(I)
print("\n")

##=============================================================================
#(4) 复合五点高斯公式
x = [math.sqrt(245-14*math.sqrt(70))/21,math.sqrt(245+14*math.sqrt(70))/21, 0]
A = [ (322 + 13*math.sqrt(70))/900, (322 - 13*math.sqrt(70))/900, 128/225]
M = 4
mid = [-1 + (2*i+1)/M for i in range(M)]
                                                                                                                                                                                                                  
I = 0
for x_mid in mid:
    I = I + A[0]*(f(x_mid+x[0]/M) + f(x_mid-x[0]/M))+ A[1]*(f(x_mid + x[1]/M)+f(x_mid - x[1]/M))
    I = I + A[2]*f(x_mid+x[2]/M)
I = I/M
print("复合五点高斯公式")
compute_error(I)
print("\n")