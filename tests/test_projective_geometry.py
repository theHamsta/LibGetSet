# import epipolar_consistency
import numpy as np
import sys
import epipolar_consistency.projective_geometry as proj


def test_make_projection_matrix():
    matrix = 2 * proj.make_projection_matrix(
        np.ones((3, 3)), np.ones((3, 3)), np.ones((3, 1)))
    print(matrix)
    proj.normalize_projection_matrix(matrix)
    print(matrix)
    print(proj.pseudo_inverse(matrix))

    print(proj.pseudoinverse_and_nullspace(matrix))


def test_module_dir():
    print(dir(epipolar_consistency))


if __name__ == "__main__":
    sys.path.insert(0, '/home/stephan/projects/epipolar_consistency/release')

    # test_module_dir()
    test_make_projection_matrix()
