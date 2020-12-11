# https://blog.csdn.net/a19990412/article/details/80409266
# https://blog.csdn.net/yang10560/article/details/78567325
# @Time    : 2018/5/22 17:23
# @Author  : Sean
# @Site    :
# @File    : 1.py
# @Software: PyCharm

import numpy as np

A = np.array([[5.0, 2, 1], [-1, 4, 2], [2, -3, 10]])
B = np.array([-12.0, 20, 3])
x0 = np.array([0.0, 0, 0])
x = np.array([0.0, 0, 0])

times = 0

while True:
    for i in range(3):
        temp = 0
        for j in range(3):
            if i != j:
                temp += x0[j] * A[i][j]
        x[i] = (B[i] - temp) / A[i][i]
    calTemp = max(abs(x - x0))
    times += 1
    if calTemp < 1e-4:
        break
    else:
        x0 = x.copy()
    print(times)
    print(x)
    print(calTemp)

print(times)
print(x)
