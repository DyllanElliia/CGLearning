import taichi as ti

ti.init(arch=ti.gpu)

n = 500

pixels = ti.var(dt=ti.f32, shape=(2*n, n))


@ti.func
def z_sqrt(z):
    return ti.Vector([z[0]**2-z[1]**2, 2*z[0]*z[1]])


@ti.kernel
def paint(time: ti.f32):
    c = ti.Vector([ti.sin(0.7930547126259008-0.5+time*0.0001)
                   * 0.4, ti.sin(time*0.01)*0.7])
    # c = ti.Vector([0.285, 0])
    for i, j in pixels:
        z = ti.Vector([float(i) / (n) - 1, float(j) / (n) - 0.5])*3
        count = 0
        while z.norm() < 10 and count < 100:
            z = z_sqrt(z)+c
            count += 1
        pixels[i, j] = 1-count/101


gpu = ti.GUI("My Julis", (n*2, n))
result_dir = './JulisI'
video_manager = ti.VideoManager(
    output_dir=result_dir, framerate=60, automatic_build=False)


for t in range(600):
    paint(t)
    pixels_img = pixels.to_numpy()
    video_manager.write_frame(pixels_img)
    gpu.set_image(pixels)
    gpu.show()

video_manager.make_video(gif=True, mp4=True)
print(f'MP4 video is saved to {video_manager.get_output_filename(".mp4")}')
print(f'GIF video is saved to {video_manager.get_output_filename(".gif")}')
