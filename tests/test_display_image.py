import numpy as np
import time
import pyconrad.autoinit
import epipolar_consistency


def test_display_noise():
    noise = np.random.rand(100, 100, 20)
    noise = np.ascontiguousarray(noise.astype(np.float32))
    img = epipolar_consistency.NrrdImage.fromArray(noise)
    img.show()
    img.show("hallo")


def test_display_phantom():
    phantom = pyconrad.stanfordrsl().conrad.phantom.NumericalSheppLogan3D(
        50, 50, 50).getNumericalSheppLoganPhantom().as_numpy()
    epipolar_consistency.imshow(phantom, "phantom")


def test_non_blocking():
    for i in range(100):
        noise = np.random.rand(100, 100, 20)
        noise = np.ascontiguousarray(noise.astype(np.float32))
        img = epipolar_consistency.NrrdImage.fromArray(noise)
        img.show("hallo", blocking_call=False)

    img.show("hallo", blocking_call=True)


if __name__ == "__main__":
    # test_display_noise()
    # test_display_phantom()
    test_non_blocking()
