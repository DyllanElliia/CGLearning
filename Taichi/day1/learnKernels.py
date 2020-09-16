import taichi as ti

ti.init(arch=ti.gpu)


@ti.kernel
def hello(i: ti.i32):
    a = 40
    print("hello world!", a + i)


@ti.kernel
def calc() -> ti.i32:
    s = 0
    for i in range(10):
        s += i
    return s


hello(2)
print("calc: ", calc())
