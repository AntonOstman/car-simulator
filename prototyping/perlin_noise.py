from matplotlib import pyplot as plt
import numpy as np
from math import floor
from math import floor

world_size = 100
grid_size = 10

def plot_curve_function(func):
    x = np.linspace(-2,2,100)
    y = func(x)
    plt.plot(x,y)
    plt.show()

def hash(x, y, seed):

    h = x * 374761393 + y * 668265263 + seed * 700001
    h = (h ^ (h >> 13)) * 1274126177
    h = (h ^ (h >> 16)) & 0xFFFFFFFF
    return h / 0xFFFFFFFF


def S(t):
    """Ease curve"""
    # return 6 * t**5 - 15*t**4 + 10*t**3
    return 3.0*t**2 - 2.0*t**3

def interpolate(x,y, ul, ur, ll, lr):
    # interpolate in x direction
    L = ll + S(x - floor(x))*(lr - ll)
    U = ul + S(x - floor(x))*(ur - ul)
    # interpolate y
    noise = L + S(y - floor(y))*(U - L)
    return noise 

def get_ul_ur_ll_lr(gx, gy, grid):

    gxidx, gyidx = floor(gx), floor(gy)

    ll = np.dot(grid[gxidx,gyidx,:], np.array([gx - gxidx, gy - gyidx])) 
    lr = np.dot(grid[gxidx+1,gyidx,:], np.array([gx - (gxidx+1), gy - gyidx])) 
    ul = np.dot(grid[gxidx,gyidx+1,:], np.array([gx - gxidx, gy - (gyidx+1)])) 
    ur = np.dot(grid[gxidx+1,gyidx+1,:], np.array([gx - (gxidx+1), gy - (gyidx+1)])) 

    ul = np.clip(ul, -1,1)
    ur = np.clip(ur, -1,1)
    ll = np.clip(ll, -1,1)
    lr = np.clip(lr, -1,1)

    return ul, ur, ll, lr

def world_to_grid(wx, wy, world_size, grid_size):
    assert world_size > grid_size, 'Smaller world than grid does not make sense'
    return wx / world_size * grid_size - 1, wy / world_size * grid_size - 1

grid = 2 * np.random.rand(grid_size, grid_size, 2) - 1


grid_norm = np.linalg.norm(grid, axis=2, keepdims=True)

grid_normed = grid / grid_norm

world = np.zeros((world_size, world_size, 1))

for wx in range(world_size):
    for wy in range(world_size):

        gx,gy = world_to_grid(wx,wy, world_size, grid_size)

        ul, ur, ll, lr = get_ul_ur_ll_lr(gx,gy,grid_normed)

        world[wx,wy] = interpolate(gx,gy,ul,ur,ll,lr)

# plt.imshow(world)
# plt.show()

# a = lambda t: 6 * t**5 - 15*t**4 + 10*t**3
# b = lambda t: 3.0*t**3 - 2.0*t**2
# c = lambda t: 36*t - 2.0
# d = lambda t: 6 - 12*t

# plot_curve_function(a)
# plot_curve_function(b)
# plot_curve_function(c)
# plot_curve_function(d)

a = np.arange(0,1000)
b = np.arange(0,1000)
aa,bb = np.meshgrid(a,b)

hashes = hash(aa.flatten(),bb.flatten(),123)

np.histogram(hashes.flatten())
plt.hist(hashes, bins=10000)
plt.show()
