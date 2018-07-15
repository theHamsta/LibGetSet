import _epipolar_consistency
import numpy as np


def imshow(array, title='', blocking_call=True):
    array = np.ascontiguousarray(array, np.float32)
    img = _epipolar_consistency.NrrdImage.fromArray(array)
    img.show(title, blocking_call)
