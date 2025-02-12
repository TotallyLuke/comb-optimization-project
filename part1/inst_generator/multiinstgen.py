import numpy as np
import random
from random import uniform, choice
from math import sqrt
import matplotlib.pyplot as plt
import sys

def generate_clusters(n_clusters, board_width, board_height):
    """Generate random cluster centers within the board."""
    return [(random.randint(0, board_width), random.randint(0, board_height)) for _ in range(n_clusters)]

def distance(p1, p2):
    return sqrt((p1[0] - p2[0])**2 + (p1[1] - p2[1])**2)

def save_to_tsplib(hole_positions, filename="output.tsp"):
    """Save generated points to a TSPLIB-style file."""
    with open(filename, 'w') as file:
        file.write("NODE_COORD_SECTION\n")
        
        # Write holes as numbered coordinates
        for i, (x, y) in enumerate(hole_positions, start=1):
            file.write(f"{i} {x:.2f} {y:.2f}\n")

        file.write("EOF\n")


if __name__ == "__main__":
    # Read parameters from command-line arguments
    if len(sys.argv) < 3:
        print("Usage: python3 clustergen.py <num_clusters> <num_holes>")
        sys.exit(1)

    n_clusters = int(sys.argv[1])  # Number of clusters
    num_holes = int(sys.argv[2])   # Number of holes

    board_width = 100  
    board_height = 45  
    min_spacing = 1    
    clusters = generate_clusters(n_clusters, board_width, board_height)

    cluster_stddev = 3 

    holes = []
    for i in range(num_holes):
        if random.random() < 0.5:  # 50% chance to belong to a cluster
            center = choice(clusters)
            x, y = np.random.normal(center[0], cluster_stddev), np.random.normal(center[1], cluster_stddev)
        else:  # 50% chance for uniform distribution
            x, y = uniform(20, board_width-20), uniform(10, board_height-10)

        # Ensure holes are within bounds and maintain spacing
        if all(distance((x, y), hole) > min_spacing for hole in holes):
            holes.append((x, y))

    print(holes)

    x_coords = [hole[0] for hole in holes]
    y_coords = [hole[1] for hole in holes]

    plt.figure(figsize=(10, 5))  # Set the figure size for better visualization
    plt.scatter(x_coords, y_coords, c='blue', marker='o', label='Holes')
    plt.title('PCB Hole Distribution')
    plt.xlabel('Width (mm)')
    plt.ylabel('Height (mm)')
    plt.grid(True)
    plt.show()
    save_to_tsplib(holes, 'random'+str(num_holes)+'.tsp')
    
