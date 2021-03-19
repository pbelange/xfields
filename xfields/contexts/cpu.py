import numpy as np

from .default_kernels import cpu_default_kernels

from xobjects.context import ContextCpu


class XfCpuContext(ContextCpu):

    """

       Creates a CPU Context object, that allows performing the computations
       on conventionla CPUs.

    Args:
        default_kernels (bool): If ``True``, the Xfields defult kernels are
            automatically imported.
    Returns:
        XfCpuContext: context object.

    """

    def __init__(self, default_kernels=True):

        super().__init__()

        if default_kernels:
            self.add_kernels(src_files=cpu_default_kernels['src_files'],
                    kernel_descriptions=cpu_default_kernels['kernel_descriptions'])

