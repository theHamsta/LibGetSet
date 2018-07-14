import _epipolar_consistency
import numpy as np


def imshow(array, title=''):
    array = np.ascontiguousarray(array.astype(np.float32))
    img = _epipolar_consistency.NrrdImage.fromArray(array)
    img.show('title')
