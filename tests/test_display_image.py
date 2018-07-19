import numpy as np
import time
import pyconrad.autoinit
import epipolar_consistency


def test_display_noise_3d():
    noise = np.random.rand(100, 100, 20)
    noise = np.ascontiguousarray(noise, np.float32)
    img = epipolar_consistency.NrrdImage.fromArray(noise)
    img.show()
    img.show("hallo")


def test_display_noise_2d():
    noise = np.random.rand(100, 100)
    noise = np.ascontiguousarray(noise, np.float32)
    img = epipolar_consistency.NrrdImage.fromArray(noise)
    img.show()


def test_display_noise_1d():
    noise = np.random.rand(100)
    noise = np.ascontiguousarray(noise, np.float32)
    img = epipolar_consistency.NrrdImage.fromArray(noise)
    img.show()


def test_display_phantom():
    phantom = pyconrad.stanfordrsl().conrad.phantom.NumericalSheppLogan3D(
        100, 100, 100).getNumericalSheppLoganPhantom().as_numpy()
    epipolar_consistency.imshow(phantom, "phantom")


def test_non_blocking():
    for i in range(100):
        noise = np.random.rand(100, 100, 20)
        noise = np.ascontiguousarray(noise.astype(np.float32))
        img = epipolar_consistency.NrrdImage.fromArray(noise)
        img.show("hallo", blocking_call=False)

    img.show("hallo", blocking_call=True)


def test_imshow_non_float32():
    for i in range(100):
        noise = np.random.rand(100, 100)
        epipolar_consistency.imshow(noise, "hallo", blocking_call=False)
        epipolar_consistency.imshow(noise+1, "huh", blocking_call=False)

    epipolar_consistency.imshow(noise, "hallo", blocking_call=True)


if __name__ == "__main__":
    # test_display_noise_1d()
    # test_display_noise_2d()
    # test_display_noise_3d()
    test_display_phantom()
    # test_non_blocking()
    # test_imshow_non_float32()