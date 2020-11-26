# TimeStep&Mess-spring

![image-20201124211937956](Largrangian.assets/image-20201124211937956.png)

-   Hook's Law



显示积分器：

![image-20201124212254100](Largrangian.assets/image-20201124212254100.png)

-   Time integration
-   symplectic Euler 是常用的欧拉方法

![image-20201124212623396](Largrangian.assets/image-20201124212623396.png)



![image-20201124213011955](Largrangian.assets/image-20201124213011955.png)

-   积分器相关



隐式积分器（Implicit Solver）：

-   缺点：复杂，需要更多优化与衡量，每一个time step会更加珍贵；
-   优点：允许更多容差；



Largrangian Implicit Solver：

![image-20201124213828960](Largrangian.assets/image-20201124213828960.png)

![image-20201124214107119](Largrangian.assets/image-20201124214107119.png)

![image-20201124214210390](Largrangian.assets/image-20201124214210390.png)

-   雅克比迭代；
-   共轭梯度；

![image-20201124214243392](Largrangian.assets/image-20201124214243392.png)

>   求逆是一个十分消耗资源的方法；
>
>   e.g. 稀疏矩阵的逆可能是稠密矩阵，存在爆显存的情况。

![image-20201124214311388](Largrangian.assets/image-20201124214311388.png)

![image-20201124214547287](Largrangian.assets/image-20201124214547287.png)

>   纠错：3的$\beta=1$；





# SPH

![image-20201124214924434](Largrangian.assets/image-20201124214924434.png)

![image-20201124215043611](Largrangian.assets/image-20201124215043611.png)

-   适合自由表面模拟；
    -   烟雾不包含自由表面，因为包含空气；

![image-20201124215207059](Largrangian.assets/image-20201124215207059.png)

-   MCSPH是一个最简单SPH模拟；
-   $D$一般指定材料导数（跟着粒子动的一种导数）

![image-20201124215714393](Largrangian.assets/image-20201124215714393.png)

-   公式2可以保证公式的稳定性，特别是在粘度等模拟上。

![image-20201124215959164](Largrangian.assets/image-20201124215959164.png)

-   PCI-SPH：用了预测修正粒子速度的方法，降低了流体的散度对模拟的干扰。
-   DFSPH：让速度Divergence-free

更多模型：

![image-20201124220403733](Largrangian.assets/image-20201124220403733.png)

显式时间积分：

![image-20201124220430291](Largrangian.assets/image-20201124220430291.png)

定义时间步长，不然会导致系统不稳定。

邻接表优化：

![image-20201124220712668](Largrangian.assets/image-20201124220712668.png)

-   Conpact hashing是一个最新的空间优化方法。

![image-20201124221013346](Largrangian.assets/image-20201124221013346.png)

导出图片ORgifORmp4

![image-20201124221136081](Largrangian.assets/image-20201124221136081.png)

![image-20201124221328849](Largrangian.assets/image-20201124221328849.png)