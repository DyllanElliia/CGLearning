import taichi as ti
import numpy as np
import math

ti.init(arch=ti.gpu)

eps = 0.01
dt = 0.1

n_vortex = 4
n_tracer = 200000

pos = ti.Vector(2, ti.f32, shape=n_vortex)
new_pos = ti.Vector(2, ti.f32, shape=n_vortex)
vort = ti.var(ti.f32, shape=n_vortex)

tracer = ti.Vector(2, ti.f32, shape=n_tracer)

# FIXME 速度计算
@ti.func
def compute_u_single(p, i):
    r2 = (p - pos[i]).norm()**2
    uv = ti.Vector([pos[i].y - p.y, p.x - pos[i].x])
    return vort[i] * uv / (r2 * math.pi) * 0.5 * (1.0 - ti.exp(-r2 / eps**2))

# FIXME 求解所有涡的速度值（相互影响）
@ti.func
def compute_u_full(p):
    u = ti.Vector([0.0, 0.0])
    for i in range(n_vortex):
        u += compute_u_single(p, i)
    return u

# FIXME 更新涡位移
@ti.kernel
def integrate_vortex():
    for i in range(n_vortex):
        v = ti.Vector([0.0, 0.0])
        for j in range(n_vortex):
            if i != j:
                v += compute_u_single(pos[i], j)
        new_pos[i] = pos[i] + dt * v

    for i in range(n_vortex):
        pos[i] = new_pos[i]

# FIXME 记录涡的路径信息
@ti.kernel
def advect():
    for i in range(n_tracer):
        # Ralston's third-order method
        p = tracer[i]
        v1 = compute_u_full(p)
        v2 = compute_u_full(p + v1 * dt * 0.5)
        v3 = compute_u_full(p + v2 * dt * 0.75)
        tracer[i] += (2 / 9 * v1 + 1 / 3 * v2 + 4 / 9 * v3) * dt

# 初始位置
pos[0] = [0, 1]
pos[1] = [0, -1]
pos[2] = [0, 0.3]
pos[3] = [0, -0.3]
# 涡旋角速度
vort[0] = 2
vort[1] = -2
vort[2] = 2
vort[3] = -2


@ti.kernel
def init_tracers():
    for i in range(n_tracer):
        tracer[i] = [ti.random() - 0.5, ti.random() * 3 - 1.5]


init_tracers()

gui = ti.GUI("Vortex", (2048, 512), background_color=0xFFFFFF)

for T in range(1000):
    for i in range(4):  # substeps
        advect()
        integrate_vortex()

    gui.circles(
        tracer.to_numpy() * np.array([[0.025, 0.1]]) + np.array([[0.0, 0.5]]),
        radius=1,
        color=0x0)

    gui.show()
