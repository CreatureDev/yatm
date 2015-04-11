#include "../include/p_loader.h"
#include "../include/m_hashlib.h"
#include <stdio.h>

/* This file is responsible for loading physics assets
 * from the bin into OpenCL buffer space and unloading
 * them when they are  no longer needed. It is also in
 * charge of managing OpenCL data usage to make sure no
 * dupicate data is loaded and ensuring a maximum data
 * capacity is not exceeded.
 *
 * These assets include: meshes, materials, kernels
*/

/* static table_t mesht = {0, '\0'};
 * static table_t mtrlt = {0, '\0'};
 * static table_t krnlt = {0, '\0'};
*/

