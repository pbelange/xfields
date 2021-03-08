import pyopencl as cl
import pyopencl.array as cla
import numpy as np
import numpy.linalg as la

from xfields.multiplatform.pocl import XfPoclKernel

ctx = cl.create_some_context()
queue = cl.CommandQueue(ctx)

# Here he makes the sum of the two arrays 
# with an explicit kernel                 
src_files = [
    '../../xfields/src_c/atomicadd.clh',
    '../../xfields/src_autogenerated/linear_interpolators_pocl.clh']

src_content = ''
for ff in src_files:
    with open(ff, 'r') as fid:
        src_content += ('\n\n' + fid.read())

prg = cl.Program(ctx, src_content).build()
knl_p2m_rectmesh3d = prg.p2m_rectmesh3d
knl_m2p_rectmesh3d = prg.m2p_rectmesh3d

import pickle
with open('../000_sphere/picsphere.pkl', 'rb') as fid:
    ddd = pickle.load(fid)

fmap = ddd['fmap']
x0 = fmap.x_grid[0]
y0 = fmap.y_grid[0]
z0 = fmap.z_grid[0]

dx = fmap.dx
dy = fmap.dy
dz = fmap.dz

nx = fmap.nx
ny = fmap.ny
nz = fmap.nz


args = (
    (np.int32, 'nparticles',),
    (cla.Array, 'x',),
    (cla.Array, 'y',),
    (cla.Array, 'z',),
    (cla.Array, 'part_weights'),
    (np.float64, 'x0',),
    (np.float64, 'y0',),
    (np.float64, 'z0',),
    (np.float64, 'dx',),
    (np.float64, 'dy',),
    (np.float64, 'dz',),
    (np.int32, 'nx',),
    (np.int32, 'ny',),
    (np.int32, 'nz',),
    (cla.Array, 'grid1d'),
    )
arg_types, arg_names = zip(*args)

p2mk = XfPoclKernel(pocl_kernel=knl_p2m_rectmesh3d,
    arg_names=arg_names, arg_types=arg_types,
    num_threads_from_arg='nparticles', command_queue=queue)


# Test p2m
n_gen = 1000000
x_gen_dev = cla.to_device(queue,
        np.zeros([n_gen], dtype=np.float64)+fmap.x_grid[10]
        + 20* dx* np.linspace(0, 1., n_gen))
y_gen_dev = cla.to_device(queue,
        np.zeros([n_gen], dtype=np.float64)+fmap.y_grid[10]
        + 20*dy* np.linspace(0, 1., n_gen))
z_gen_dev = cla.to_device(queue,
        np.zeros([n_gen], dtype=np.float64)+fmap.z_grid[10]
        + 20*dz* np.linspace(0, 1., n_gen))
part_weights_dev = cla.to_device(queue,
        np.arange(0, n_gen, 1,  dtype=np.float64))
dev_buff = cla.to_device(queue, 0*fmap._maps_buffer)
dev_rho = dev_buff[:,:,:,1] # This does not support .data
#dev_rho = dev_buff[:,:,:,0]

import time
t1 = time.time()
event = p2mk(nparticles=n_gen,
    x=x_gen_dev,
    y=y_gen_dev,
    z=z_gen_dev,
    part_weights=part_weights_dev,
    x0=x0, y0=y0, z0=z0, dx=dx, dy=dy, dz=dz,
    nx=nx, ny=ny, nz=nz,
    grid1d=dev_rho)
event.wait()
t2 = time.time()
print(f't = {t2-t1:.2e}')

assert(np.isclose(np.sum(dev_rho.get())*dx*dy*dz,
    np.sum(part_weights_dev.get())))
