import taichi as ti

ti.init()
x = ti.var(ti.i32, shape=10)


@ti.kernel
def fill() -> ti.i32:

    for i in range(10):
        x[i] = i
    return x[7]


print("data = ", fill())
