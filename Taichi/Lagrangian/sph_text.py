import taichi as ti
from taichi.lang.kernel import kernel

ti.init(arch=ti.gpu)
n = 600
box_min = ti.Vector(2, dt=ti.f32, shape=1)
box_max = ti.Vector(2, dt=ti.f32, shape=1)
p_num = 900.0
pos = ti.Vector(2, dt=ti.f32, shape=int(p_num))
rho = ti.Vector(2, dt=ti.f32, shape=int(p_num))
f = ti.Vector(2, dt=ti.f32, shape=int(p_num))
v = ti.Vector(2, dt=ti.f32, shape=int(p_num))
rhoA = 1
kernelR = 0.05
gravity = 9.8
m = 1
dt = 0.001


@ti.kernel
def countRho():
    for i in range(p_num):
        rho[i] = 0
        for j in range(p_num):
            if (pos[i]-pos[j]).norm() < kernelR and i != j:
                rho[i] += 1
        rho[i] /= 1
    return


@ti.kernel
def countF():
    for i in range(p_num):
        f[i] = [0, -m*gravity]
        f[i] += rho[i]/rhoA
    return


@ti.kernel
def run():
    countRho()
    countF()
    for p in pos:
        if p.x < 0:
            p.x = 0
        if p.x > 1:
            p.x = 1
        if p.y < 0:
            p.y = 0
        if p.y > 1:
            p.y = 1
    for i in range(p_num):
        pos[i] += f[i]*dt/m


@ti.kernel
def init():
    box_min = [0, 0]
    box_max = [300, 300]
    for i in range(p_num):
        pos[i] = [ti.random()*0.3+0.2, ti.random()*0.3+0.2]
        print(pos[i])


gui = ti.GUI("mySPH", (n, n), background_color=0x25A6D9)
init()
for i in range(1000000):
    gui.circles(pos.to_numpy(), radius=3, color=0xFF00FF)
    gui.show()
