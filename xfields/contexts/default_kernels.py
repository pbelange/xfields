import sysconfig
from pathlib import Path

import numpy as np

thisfolder = Path(__file__).parent.absolute()
pkg_root = thisfolder.parent.absolute()
so_suffix = sysconfig.get_config_var('EXT_SUFFIX')

shared_kernel_descriptions = {
    'p2m_rectmesh3d':{
        'args':(
            (('scalar', np.int32),   'nparticles',),
            (('array',  np.float64), 'x',),
            (('array',  np.float64), 'y',),
            (('array',  np.float64), 'z',),
            (('array',  np.float64), 'part_weights'),
            (('scalar', np.float64), 'x0',),
            (('scalar', np.float64), 'y0',),
            (('scalar', np.float64), 'z0',),
            (('scalar', np.float64), 'dx',),
            (('scalar', np.float64), 'dy',),
            (('scalar', np.float64), 'dz',),
            (('scalar', np.int32),   'nx',),
            (('scalar', np.int32),   'ny',),
            (('scalar', np.int32),   'nz',),
            (('array',  np.float64), 'grid1d'),),
        'num_threads_from_arg': 'nparticles'
        },
    'm2p_rectmesh3d':{
        'args':(
            (('scalar', np.int32),   'nparticles',),
            (('array',  np.float64), 'x',),
            (('array',  np.float64), 'y',),
            (('array',  np.float64), 'z',),
            (('scalar', np.float64), 'x0',),
            (('scalar', np.float64), 'y0',),
            (('scalar', np.float64), 'z0',),
            (('scalar', np.float64), 'dx',),
            (('scalar', np.float64), 'dy',),
            (('scalar', np.float64), 'dz',),
            (('scalar', np.int32),   'nx',),
            (('scalar', np.int32),   'ny',),
            (('scalar', np.int32),   'nz',),
            (('scalar', np.int32),   'n_quantities',),
            (('array',  np.int32), 'offsets_mesh_quantities',),
            (('array',  np.float64), 'mesh_quantity',),
            (('array',  np.float64), 'particles_quantity'),),
        'num_threads_from_arg': 'nparticles'
        },
    }

pyopencl_default_kernels = {
    'kernel_descriptions': shared_kernel_descriptions,
    'src_files': [
        pkg_root.joinpath('src/linear_interpolators.h')
        ]
    }

cupy_default_kernels = {
    'kernel_descriptions': shared_kernel_descriptions,
    'src_files': [
        pkg_root.joinpath('src/linear_interpolators.h')
        ]
    }

cpu_default_kernels = {
    'kernel_descriptions': shared_kernel_descriptions,
    'src_files': [
        pkg_root.joinpath('src/linear_interpolators.h')
        ]
    }
