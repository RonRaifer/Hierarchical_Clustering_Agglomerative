import time
import numpy as np


def HAC_ours_optimized(A):
    rows = A.shape[0]
    clusters = [[i] for i in range(rows)]  # clusters
    min_idx_val = np.zeros(shape=(rows, 2), dtype=np.float64)  # min vals and their indexes
    # set matrix diagonal to np.nan
    A[range(rows), range(rows)] = np.nan

    # find the minimal values and their indexes
    for i in range(rows):
        first_min_index = np.where(A[:, i] == np.nanmin(A[:, i]))[0][0]  # get indexes of min values
        min_idx_val[i] = first_min_index, A[first_min_index, i]

    cnt = rows

    while cnt > 2:
        # Get index of first minimum value
        first_min_index = np.where(min_idx_val[:, 1] == np.nanmin(min_idx_val[:, 1]))[0][0]
        i_min = first_min_index  # the row of the minimum value
        j_min = int(min_idx_val[i_min][0])  # the column of the minimum value

        # replace row and col to the minimum
        A[:, i_min] = np.minimum(A[:, i_min], A[:, j_min])
        A[i_min, :] = A[:, i_min]

        # merge clusters
        clusters[i_min].extend(clusters[j_min])
        clusters[j_min].clear()

        # delete col and row of j, and update indexes in min_idx_val
        A[:, j_min] = np.nan
        A[j_min, :] = np.nan
        min_idx_val[j_min] = np.nan
        min_idx_val[min_idx_val == j_min] = i_min

        # update index of min value, and it's value.
        first_min_index = np.where(A[:, i_min] == np.nanmin(A[:, i_min]))[0][0]  # new vector after merge
        min_idx_val[i_min] = first_min_index, A[first_min_index, i_min]

        cnt -= 1
    return clusters


start_time = time.time_ns()
A = np.loadtxt("data_2_4.txt", delimiter=',', dtype=np.float64)

print("-----%.2f nano seconds to read matrix-----" % (time.time_ns() - start_time))
start_time = time.time_ns()
cls = HAC_ours_optimized(A)
print("-----%.2f nano seconds-----\n" % (time.time_ns() - start_time))
for cluster in cls:
    if len(cluster) > 0:
        print(f"\n{cluster}")
