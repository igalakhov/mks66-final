import math
import os

num_frames = 540

template = """
light l1 -0.5 -0.25 0.5 0 100 100
light l0 0.5 -0.25 0.5 255 255 0

shading phong
    
move 250 100 0
    
scale 4.5 4.5 4.5
    
rotate x %f
rotate y %f
    
mesh :meshes/dragon/dragon_%06d.obj
    
save renders/dragon/%06d.png
"""

# x varies from 5 to 20 once
# y varies from -20 to 20 twice

def rx(t):
    t = t-1
    return 12.5 + 7.5*math.sin(math.pi*2*t/num_frames)

def ry(t):
    t = t-1
    return 20*math.sin(math.pi*4*t/num_frames)


if __name__ == '__main__':
    for frame in range(1, num_frames + 1):
        obj_name = "meshes/dragon/dragon_%06d.obj" % frame
        c = open(obj_name).read()
        c = c.replace('mtllib d', 'mtllib meshes/dragon/d')
        n = open(obj_name, 'w+')
        n.write(c)

        s = "scripts/dragon/dragon_%06d.mdl" % frame

        f = open(s, 'w+')
        print(frame, rx(frame), ry(frame))
        f.write(template % (rx(frame), ry(frame), frame, frame))

        #print("rendering....")
        #os.system('./mdl.out %s' % s)


        print(s)