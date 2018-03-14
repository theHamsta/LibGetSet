
from os.path import join, dirname
import subprocess

def launch_nrrd_view():
    binary = join(dirname(__file__), 'nrrdView')
    args = []

    subprocess.call([binary] + args)
    