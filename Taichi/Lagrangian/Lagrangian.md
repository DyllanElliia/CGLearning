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

![image-20201126200222293](Lagrangian.assets/image-20201126200222293.png)



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

# Elasticity

弹性和有限元教程：

![image-20201126200813020](Lagrangian.assets/image-20201126200813020.png)

描述弹性的方式：

![image-20201126200848228](Lagrangian.assets/image-20201126200848228.png)

>   deformation：形变
>
>   $J=形变前的体积/形变后的体积$

## 超弹性模型

![image-20201126201501913](Lagrangian.assets/image-20201126201501913.png)

>   Stress：材料内部的弹性力；
>
>   Strain：

![image-20201126201813320](Lagrangian.assets/image-20201126201813320.png)

材料微元之间会相互影响；

![image-20201126202154783](Lagrangian.assets/image-20201126202154783.png)

>   上述所有参数都和杨氏模量成正比；
>
>   应用场景各不相同；
>
>   Poisson ratio：材料保持自身体积的能力（Poisson ratio=0的橡皮筋，拉伸时，中间横截面面积不变）常用于可压缩元。

![image-20201126203005075](Lagrangian.assets/image-20201126203005075.png)

>   一般用Noe-Hookean模型；
>
>   当形变程度很小时，可以考虑采用Linear elasticity，因为一般不会旋转这类小元，且方程易解。

# Lagrangian finite elements on linear tetrahedral meshes



![image-20201126203453378](Lagrangian.assets/image-20201126203453378.png)

>   将连续PDE转换为离散微分几何元素
>
>   网站：有限元素周期表

![image-20201126203841948](Lagrangian.assets/image-20201126203841948.png)

公式1：$一个元素形变后的位置=静止的位置*F+b(offset)$

公式2：纠错$dx$不是$x$，

三角元素推导：

![image-20201126204220825](Lagrangian.assets/image-20201126204220825.png)

![image-20201126204651148](Lagrangian.assets/image-20201126204651148.png)

![image-20201126204729308](Lagrangian.assets/image-20201126204729308.png)

## Implicit linear trangular FEM simulation

![image-20201126205347654](Lagrangian.assets/image-20201126205347654.png)

>   M：质量矩阵，每个元素是这个元素的质量；

# TaiChi Features

## Objective data-oriented programming

同时支持面向数据和面向对象编程。

![image-20201126210312721](Lagrangian.assets/image-20201126210312721.png)

>   staticmethod：魔法指令，生成静态函数；

![image-20201126210515714](Lagrangian.assets/image-20201126210515714.png)



## Metaprogramming（元编程）

![image-20201126210720065](Lagrangian.assets/image-20201126210720065.png)

>   lazily instantiated：未实例化就不编译；

![image-20201126210945522](Lagrangian.assets/image-20201126210945522.png)

![image-20201126211136633](Lagrangian.assets/image-20201126211136633.png)

**维度无关编程：**

![image-20201126211243216](Lagrangian.assets/image-20201126211243216.png)

>   ti.grouped(y)：将y变量打包。

Tensor反射（自省）

![image-20201126211505528](Lagrangian.assets/image-20201126211505528.png)

![image-20201126211631216](Lagrangian.assets/image-20201126211631216.png)

>   判断是在编译器实现。

![image-20201126211651928](Lagrangian.assets/image-20201126211651928.png)

>   在编译器循环展开。

![image-20201126211742575](Lagrangian.assets/image-20201126211742575.png)

![image-20201126211925576](Lagrangian.assets/image-20201126211925576.png)

>   变量重命名。

## Differentiable Programming（可微编程）

![image-20201126212107657](Lagrangian.assets/image-20201126212107657.png)

![image-20201126212227883](Lagrangian.assets/image-20201126212227883.png)

-p：命令行查看；

-P：命令行查看，渲染；

-s：导出；

![image-20201126213426246](Lagrangian.assets/image-20201126213426246.png)

![image-20201126213435049](Lagrangian.assets/image-20201126213435049.png)

## Visualization

![image-20201126214034832](Lagrangian.assets/image-20201126214034832.png)

