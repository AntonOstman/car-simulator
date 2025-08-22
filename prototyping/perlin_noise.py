import numpy as np
import matplotlib.pyplot as plt

def wang_hash(x, y, seed):
    hash_val = seed & 0xFFFFFFFF
    hash_val ^= (x * 0x1f351f35) & 0xFFFFFFFF
    hash_val ^= (y * 0x3f893f89) & 0xFFFFFFFF
    hash_val = ((hash_val ^ (hash_val >> 16)) * 0x85ebca6b) & 0xFFFFFFFF
    hash_val = ((hash_val ^ (hash_val >> 13)) * 0xc2b2ae35) & 0xFFFFFFFF
    hash_val = (hash_val ^ (hash_val >> 16)) & 0xFFFFFFFF
    return hash_val

def perlin_hash(x, y, seed):
    hash_val = wang_hash(x, y, seed)

    return (hash_val & 0xFFFF) * (2.0 * np.pi / 65536.0)

def perlin_hash_discrete(x, y, seed):
    hash_val = wang_hash(x, y, seed)
    gradient_index = hash_val & 7

    angles = [i * np.pi / 4 for i in range(8)]
    return angles[gradient_index]

def improved_perlin_hash(x, y, seed):
    hash_val = wang_hash(x, y, seed)
    gradients = [
        (1, 1), (-1, 1), (1, -1), (-1, -1),
        (1, 0), (-1, 0), (0, 1), (0, -1),
        (2, 1), (-2, 1), (1, 2), (-1, 2)
    ]

    gradient = gradients[hash_val % 12]
    return np.arctan2(gradient[1], gradient[0])

def terrain_noise_angle(x, y, seed, hash_func=perlin_hash):
    return hash_func(x, y, seed)


def generate_perlin_grid(grid_size, world_size, hash_func, seed=1234):

    grid_x, grid_y = np.meshgrid(range(grid_size), range(grid_size))
    grid_mesh = np.stack([grid_x, grid_y], axis=-1)
    grid = np.zeros((grid_size, grid_size, 2))

    for i in range(grid_size):
        for j in range(grid_size):
            angle = terrain_noise_angle(grid_mesh[i,j,0], grid_mesh[i,j,1], seed, hash_func)
            grid[i,j,0] = np.cos(angle)
            grid[i,j,1] = np.sin(angle)


    grid_norm = np.linalg.norm(grid, axis=2, keepdims=True)
    grid_normed = np.where(grid_norm > 0, grid / grid_norm, grid)

    return grid_normed

def simple_perlin_interpolation(world_size, grid_size, grid_normed):
    world = np.zeros((world_size, world_size))

    for wx in range(world_size):
        for wy in range(world_size):

            gx = wx * (grid_size - 1) / (world_size - 1)
            gy = wy * (grid_size - 1) / (world_size - 1)

            gx_int = int(gx)
            gy_int = int(gy)

            if gx_int < grid_size - 1 and gy_int < grid_size - 1:

                fx = gx - gx_int
                fy = gy - gy_int


                g00 = grid_normed[gy_int, gx_int]
                g01 = grid_normed[gy_int, gx_int + 1]
                g10 = grid_normed[gy_int + 1, gx_int]
                g11 = grid_normed[gy_int + 1, gx_int + 1]


                d00 = np.array([fx, fy])
                d01 = np.array([fx - 1, fy])
                d10 = np.array([fx, fy - 1])
                d11 = np.array([fx - 1, fy - 1])


                dot00 = np.dot(g00, d00)
                dot01 = np.dot(g01, d01)
                dot10 = np.dot(g10, d10)
                dot11 = np.dot(g11, d11)


                u = 3 * fx**2 - 2 * fx**3
                v = 3 * fy**2 - 2 * fy**3

                top = dot00 + u * (dot01 - dot00)
                bottom = dot10 + u * (dot11 - dot10)
                world[wx, wy] = top + v * (bottom - top)

    return world

def compare_hash_functions():
    grid_size = 2
    world_size = 128
    seed = 5

    hash_functions = [
        (lambda x, y, s: (wang_hash(x, y, s) & 0xFFFFFF) / 0xFFFFFF * 2 * np.pi, "Raw Wang Hash"),
        (perlin_hash, "Perlin Hash (Continuous)"),
        (perlin_hash_discrete, "Perlin Hash (8 Directions)"),
        (improved_perlin_hash, "Improved Perlin (12 Directions)")
    ]

    fig, axes = plt.subplots(2, 2, figsize=(12, 12))
    fig.suptitle('Perlin Noise with Different Hash Functions', fontsize=16)

    for i, (hash_func, title) in enumerate(hash_functions):
        row = i // 2
        col = i % 2

        grid_normed = generate_perlin_grid(grid_size, world_size, hash_func, seed)
        world = simple_perlin_interpolation(world_size, grid_size, grid_normed)
        breakpoint()

        im = axes[row, col].imshow(world, cmap='terrain', interpolation='bilinear')
        axes[row, col].set_title(title)
        plt.colorbar(im, ax=axes[row, col], shrink=0.8)

    plt.tight_layout()
    plt.show()

def plot3d():
    grid_size = 2
    world_size = 128
    seed = 5

    hash_functions = [
        (lambda x, y, s: (wang_hash(x, y, s) & 0xFFFFFF) / 0xFFFFFF * 2 * np.pi, "Raw Wang Hash"),
        (perlin_hash, "Perlin Hash (Continuous)"),
        (perlin_hash_discrete, "Perlin Hash (8 Directions)"),
        (improved_perlin_hash, "Improved Perlin (12 Directions)")
    ]

    hash_func, name = hash_functions[0]

    grid_normed = generate_perlin_grid(grid_size, world_size, hash_func, seed)
    world = simple_perlin_interpolation(world_size, grid_size, grid_normed)

    points = np.zeros([3, world_size * world_size]) 
    for j in range(world_size):
        for k in range(world_size):
            points[:, j*k + k] = np.array([j,k,world[j,k]])
            
    fig = plt.figure()
    ax = fig.add_subplot(projection='3d')
    ax.scatter(points[0,:], points[1,:], points[2,:])

    plt.tight_layout()
    plt.show()

def test_gradient_distribution():
    seed = 1234
    size = 50

    fig, axes = plt.subplots(1, 3, figsize=(15, 5))

    hash_functions = [
        (perlin_hash, "Continuous Angles"),
        (perlin_hash_discrete, "8 Discrete Directions"),
        (improved_perlin_hash, "12 Discrete Directions")
    ]

    for i, (hash_func, title) in enumerate(hash_functions):
        angles = []
        for x in range(size):
            for y in range(size):
                angle = hash_func(x, y, seed)
                angles.append(angle)

        axes[i].hist(angles, bins=50, alpha=0.7)
        axes[i].set_title(f"{title}\n{len(set(angles))} unique values")
        axes[i].set_xlabel("Angle (radians)")
        axes[i].set_ylabel("Frequency")

    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    # test_gradient_distribution()

    # plot3d()

    compare_hash_functions()
