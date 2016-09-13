/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Created by: Hang Zhang
 * ECE Department, Rutgers University
 * Email: zhang.hang@rutgers.edu
 * Copyright (c) 2016
 *
 * Feel free to reuse and distribute this software for research or 
 * non-profit purpose, subject to the following conditions:
 *  1. The code must retain the above copyright notice, this list of
 *     conditions.
 *  2. Original authors' names are not deleted.
 *  3. The authors' names are not used to endorse or promote products
 *      derived from this software 
 *+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
static int hzproc_(Main_mapping)(lua_State *L)
/*
 * mapping the image pixels based on the lookuptable
 * inputs and output are cuda tensors
 */
{
	/* Check number of inputs */
  if(lua_gettop(L) != 2)
    luaL_error(L,  "HZPROC: Remap: Incorrect number of arguments.\n");
	THCudaTensor* in_  = *(THCudaTensor**)luaL_checkudata(L, 1, 
												"torch.CudaTensor");
	THCudaTensor* map_ = *(THCudaTensor**)luaL_checkudata(L, 2, 
												"torch.CudaTensor");
	/* Check input dims */
	THCState *state = cutorch_getstate(L);
	if (THCudaTensor_nDimension(state, in_) != 3 ||
			THCudaTensor_nDimension(state, map_) != 3)
		luaL_error(L, "HZPROC: Mapping: incorrect input dims. \n");
	/* Init output tensor */
	long i;
	long outsz[3];
	outsz[0] = 3;
	for (i=1; i<3; i++) {
		outsz[i] = THCudaTensor_size(state, map_, i);
	}
	THCudaTensor* out_ =  THCudaTensor_newWithSize3d(state, outsz[0], 
																								outsz[1], outsz[2]);
	HZMapping(state, in_, out_, map_);
	/* return the tensor */
	lua_pop(L, lua_gettop(L));
  luaT_pushudata(L, (void*)out_, "torch.CudaTensor");
	/* C function return number of the outputs */
	return 1;
}

static int hzproc_(Main_affinemapping)(lua_State *L)
/*
 * mapping the image pixels based on the affine transformation matrix
 * inputs and output are cuda tensors
 */
{
	/* Check number of inputs */
  if(lua_gettop(L) != 2)
    luaL_error(L,  "HZPROC: AffineMap: Incorrect number of arguments.\n");
	THCudaTensor* in_  = *(THCudaTensor**)luaL_checkudata(L, 1, 
												"torch.CudaTensor");
	THCudaTensor* matrix_ = *(THCudaTensor**)luaL_checkudata(L, 2, 
												"torch.CudaTensor");
	/* Check input */
	THCState *state = cutorch_getstate(L);
	if (THCudaTensor_nDimension(state, in_) != 3 ||
			THCudaTensor_nDimension(state, matrix_) != 2)
		luaL_error(L, "HZPROC: AffineMap: incorrect input dims. \n");
	if (THCudaTensor_size(state, matrix_, 0) != 3 ||
			THCudaTensor_size(state, matrix_, 1) != 3 )
		luaL_error(L, "HZPROC: AffineMap: incorrect affine matrix size. \n");
	/* Init output tensor */
	THCudaTensor* out_ =  THCudaTensor_new(state);
	THCudaTensor_resizeAs(state, out_, in_);
	HZAffineMap(state, in_, out_, matrix_);
	/* return the tensor */
	lua_pop(L, lua_gettop(L));
  luaT_pushudata(L, (void*)out_, "torch.CudaTensor");
	/* C function return number of the outputs */
	return 1;
}


