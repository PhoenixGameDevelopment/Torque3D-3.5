

//Modified based on renderMeshExample.h, by Jack Stone, Phoenix Game Development
//contact@phoenixgamedevelopment.com
//www.phoenixgamedevelopment.com

//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef _METABLOBEXAMPLE_H_
#define _METABLOBEXAMPLE_H_

#ifndef _SCENEOBJECT_H_
#include "scene/sceneObject.h"
#endif
#ifndef _GFXVERTEXBUFFER_H_
#include "gfx/gfxVertexBuffer.h"
#endif
#ifndef _GFXPRIMITIVEBUFFER_H_
#include "gfx/gfxPrimitiveBuffer.h"
#endif

#ifndef _GFXSTATEBLOCK_H_
#include "gfx/gfxStateBlock.h"
#endif

#ifndef _CUBEGRIDH_H_
#include "CUBE_GRID.h"
#endif

#ifndef _METABALL_H_
#include "METABALL.h"
#endif


#ifndef _SCENEOBJECT_H_
#include "scene/sceneObject.h"
#endif
#ifndef _GFXPRIMITIVEBUFFER_H_
#include "gfx/gfxPrimitiveBuffer.h"
#endif
#ifndef _GFXVERTEXBUFFER_H_
#include "gfx/gfxVertexBuffer.h"
#endif
#ifndef _GFXSTATEBLOCK_H_
#include "gfx/gfxStateBlock.h"
#endif
#ifndef _RENDERPASSMANAGER_H_
#include "renderInstance/renderPassManager.h"
#endif
#ifndef _PRIMBUILDER_H_
#include "gfx/primBuilder.h"
#endif
#ifndef _LIGHTINFO_H_
#include "lighting/lightInfo.h"
#endif
#ifndef _LIGHTFLAREDATA_H_
#include "T3D/lightFlareData.h"
#endif
#ifndef _TRESPONSECURVE_H_
#include "math/util/tResponseCurve.h"
#endif

class BaseMatInstance;


//-----------------------------------------------------------------------------
// This class implements a basic SceneObject that can exist in the world at a
// 3D position and render itself. There are several valid ways to render an
// object in Torque. This class implements the preferred rendering method which
// is to submit a MeshRenderInst along with a Material, vertex buffer,
// primitive buffer, and transform and allow the RenderMeshMgr handle the
// actual setup and rendering for you.
//-----------------------------------------------------------------------------

class metaBlobExample : public SceneObject
{
	typedef SceneObject Parent;

	// Networking masks
	// We need to implement a mask specifically to handle
	// updating our transform from the server object to its
	// client-side "ghost". We also need to implement a
	// maks for handling editor updates to our properties
	// (like material).
	enum MaskBits 
	{
		TransformMask = Parent::NextFreeMask << 0,
		UpdateMask    = Parent::NextFreeMask << 1,
		NextFreeMask  = Parent::NextFreeMask << 2
	};
public:
	//--------------------------------------------------------------------------
	// Rendering variables
	//--------------------------------------------------------------------------
	// The name of the Material we will use for rendering
	String            mMaterialName;
	// The actual Material instance
	BaseMatInstance*  mMaterialInst;

	// Define our vertex format here so we don't have to
	// change it in multiple spots later
	typedef GFXVertexPNT VertexType;

	VectorF mLightDir;
	VectorF mEyePos;



	GFXShaderRef mShader; //I hope this works...

	// Shared shader constant blocks
	GFXShaderConstBufferRef mShaderConsts;
	GFXShaderConstHandle *mLightDirSC;

	GFXStateBlockRef mStateBlock;


	// The handles for our StateBlocks
	GFXStateBlockRef mNormalSB;
	GFXStateBlockRef mReflectSB;

	// The GFX vertex and primitive buffers
	GFXVertexBufferHandle< VertexType > mVertexBuffer;
	GFXPrimitiveBufferHandle            mPrimitiveBuffer;

	metaBlobExample();
	virtual ~metaBlobExample();

	void DrawSurface(float threshold);

	void createGeometry();
	void UpdateFrame();
	void initmetablobs();

	void resetall(int i);

	Vector<METABALL> metaballs;
	S32 mGridSize;
	S32 mNumMetaBalls;
	S32 mMinExtent;
	S32 mMaxExtent;
	S32 mMaxTickCount;
	F32 mGravAccel;
	F32 mTimeScale;

	F32 mForce;
	F32 mElevationAngle;

	CUBE_GRID cubeGrid;

	F32 mthreshold;

	bool moveready;

	S32 mboxsize;

	F32 grav_accel;

	S32 mtickcount; // create a new metaball after this many ticks
	S32 mnumcurrentmetaballs;


	S32 id;

	//	bool moveupdate;
	// Declare this object as a ConsoleObject so that we can
	// instantiate it into the world and network it
	DECLARE_CONOBJECT(metaBlobExample);

	//--------------------------------------------------------------------------
	// Object Editing
	// Since there is always a server and a client object in Torque and we
	// actually edit the server object we need to implement some basic
	// networking functions
	//--------------------------------------------------------------------------
	// Set up any fields that we want to be editable (like position)
	static void initPersistFields();

	// Allows the object to update its editable settings
	// from the server object to the client
	virtual void inspectPostApply();

	// Handle when we are added to the scene and removed from the scene
	bool onAdd();
	void onRemove();

	U32  mVertCount;
	U32 mPrimCount;
	// Override this so that we can dirty the network flag when it is called
	void setTransform( const MatrixF &mat );

	// This function handles sending the relevant data from the server
	// object to the client object
	U32 packUpdate( NetConnection *conn, U32 mask, BitStream *stream );
	// This function handles receiving relevant data from the server
	// object and applying it to the client object
	void unpackUpdate( NetConnection *conn, BitStream *stream );
	void WaterFlowMove();
	//--------------------------------------------------------------------------
	// Object Rendering
	// Torque utilizes a "batch" rendering system. This means that it builds a
	// list of objects that need to render (via RenderInst's) and then renders
	// them all in one batch. This allows it to optimized on things like
	// minimizing texture, state, and shader switching by grouping objects that
	// use the same Materials.
	//--------------------------------------------------------------------------
	// Create the geometry for rendering

	// Get the Material instance
	void updateMaterial();

	// This is the function that allows this object to submit itself for rendering
	void prepRenderImage( SceneRenderState *state );
	void render( ObjectRenderInst *ri, SceneRenderState *state, BaseMatInstance *overrideMat );
};

#endif // _metaBlobEXAMPLE_H_