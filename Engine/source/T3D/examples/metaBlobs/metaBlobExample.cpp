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


//Modified based on renderMeshExample.h, by Jack Stone, Phoenix Game Development
//contact@phoenixgamedevelopment.com
//www.phoenixgamedevelopment.com


#include "platform/platform.h"
#include "T3D/examples/metaBlobs/metaBlobExample.h"

#include "math/mathIO.h"
#include "scene/sceneRenderState.h"
#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"
#include "materials/materialManager.h"
#include "materials/baseMatInstance.h"
#include "renderInstance/renderPassManager.h"
#include "lighting/lightQuery.h"
#include "console/engineAPI.h"

#include "gfx/sim/gfxStateBlockData.h"

#include "gfx/primBuilder.h"
#include "gfx/gfxTransformSaver.h"
#include "gfx/gfxDrawUtil.h"

#include "math/mRandomDeck.h"
#include "math/mRandomSet.h"
#include "materials/shaderData.h"

#include "T3D/shapeBase.h"
#include "T3D/gameBase/gameConnection.h"

IMPLEMENT_CO_NETOBJECT_V1(metaBlobExample);

ConsoleDocClass( metaBlobExample, 
	"@brief An example scene object which renders a mesh.\n\n"
	"This class implements a basic SceneObject that can exist in the world at a "
	"3D position and render itself. There are several valid ways to render an "
	"object in Torque. This class implements the preferred rendering method which "
	"is to submit a MeshRenderInst along with a Material, vertex buffer, "
	"primitive buffer, and transform and allow the RenderMeshMgr handle the "
	"actual setup and rendering for you.\n\n"
	"See the C++ code for implementation details.\n\n"
	"@ingroup Examples\n" );

int maxmetaballs = 50;
int g = 0;
MRandom 	mRandom( Platform::getRealMilliseconds() + 1 );

//-----------------------------------------------------------------------------
// Object setup and teardown
//-----------------------------------------------------------------------------

metaBlobExample::metaBlobExample()
{
	// Flag this object so that it will always
	// be sent across the network to clients
	mNetFlags.set( Ghostable | ScopeAlways );

	// Set it as a "static" object that casts shadows
	mTypeMask |= StaticObjectType | StaticShapeObjectType;

	// Make sure we the Material instance to NULL
	// so we don't try to access it incorrectly
	mMaterialInst = NULL;
	mShader = NULL;

	mGridSize = 30;
	mNumMetaBalls = 4;
	mMinExtent = 40;
	mMaxExtent = 80;
	mMaxTickCount = 40;

	mTimeScale = 0.01;
	mGravAccel = 0.5*mTimeScale;

	mForce = 50;
	mElevationAngle = 25;

	id = g++;
	
}

metaBlobExample::~metaBlobExample()
{
	if ( mMaterialInst )
		SAFE_DELETE( mMaterialInst );
}


//-----------------------------------------------------------------------------
// Object Editing
//-----------------------------------------------------------------------------
void metaBlobExample::initPersistFields()
{
	addGroup( "Rendering" );
	addField( "material",      TypeMaterialName, Offset( mMaterialName, metaBlobExample ),"The name of the material used to render the mesh." );
	endGroup( "Rendering" );

	addGroup( "Fluid Dynamics" );

	addField( "mGridSize",      TypeS32, Offset( mGridSize, metaBlobExample       ),"The Complexity of the Fluid System, default: 40" );
	addField( "numMetaballs",   TypeS32, Offset( mNumMetaBalls, metaBlobExample   ),"The max number of Metaballs to be created, default: 10" );
	addField( "minExtent",      TypeS32, Offset( mMinExtent, metaBlobExample      ),"The Minimum extent of the Fluid System, default: 40" );
	addField( "maxExtent",      TypeS32, Offset( mMaxExtent, metaBlobExample      ),"The Maximum extent of the Fluid System, default: 80" );
	//addField( "boxSize",      TypeF32, Offset( mBoxSize, metaBlobExample        ),"The Maximum extent of the Fluid System, default: 80" );
	addField( "maxtickcount",   TypeS32, Offset( mMaxTickCount, metaBlobExample   ),"The number of ticks before a new metaball is generated in the stream default: 40" );
	addField( "GravAccel",      TypeF32, Offset( mGravAccel, metaBlobExample      ),"The force of gravity default: 9.81" );
	addField( "timescale",      TypeF32, Offset( mTimeScale, metaBlobExample      ),	"The speed of the simulation default: 0.01" );
	addField( "Force",          TypeF32, Offset( mForce, metaBlobExample          ),	"The force of the water flow default: 0.0" );
	addField( "ElevationAngle", TypeF32, Offset( mElevationAngle, metaBlobExample ),"The elevation from the horizontal that this fluid emitter has. To be replaced by a more elegant solution!" );

	endGroup( "Fluid Dynamics" );
	// SceneObject already handles exposing the transform
	Parent::initPersistFields();
}

void metaBlobExample::inspectPostApply()
{
	Parent::inspectPostApply();

	// Flag the network mask to send the updates
	// to the client object
	setMaskBits( UpdateMask );
}

class moveMetablobsLAVALAMP : public SimEvent
{
public:
	metaBlobExample *mb;

	moveMetablobsLAVALAMP(metaBlobExample *mbin){
		mb = mbin;
	}


	void process( SimObject *object )
	{

		//move metablobs:

		/*
		//update balls' position
		float c = 2.0f*(float)cos(timer.GetTime()/600);

		metaballs[0].position.x=-4.0f*(float)cos(timer.GetTime()/700) - c;
		metaballs[0].position.y=4.0f*(float)sin(timer.GetTime()/600) - c;

		metaballs[1].position.x=5.0f*(float)sin(timer.GetTime()/400) + c;
		metaballs[1].position.y=5.0f*(float)cos(timer.GetTime()/400) - c;

		metaballs[2].position.x=-5.0f*(float)cos(timer.GetTime()/400) - 0.2f*(float)sin(timer.GetTime()/600);
		metaballs[2].position.y=5.0f*(float)sin(timer.GetTime()/500) - 0.2f*(float)sin(timer.GetTime()/400);
		*/

		//pick a destination for all metaballs that is outside the box, when they reach the edge, pick a new random destination:


		//	float tgtxpos = mRandom.randF(-50,50);
		//	float tgtypos = mRandom.randF(-50,50);
		//	float tgtzpos = mRandom.randF(-50,50);


		//	float tgtxpos = 230;
		//	float tgtypos = 75;
		//	float tgtzpos = 2;
		//	int boxsize = 20;

		/*
		for(int i = 0; i < mb->mNumMetaBalls;i++){
			if(mb->metaballs[i].targetposition.x <= -999){

				float tgtxpos = mRandom.randF(mb->mboxsize+5,mb->mboxsize+50);
				if(mRandom.randF() >0.5)
					tgtxpos *= -1;
				float tgtypos = mRandom.randF(mb->mboxsize+5,mb->mboxsize+50);
				if(mRandom.randF() >0.5)
					tgtypos *= -1;
				float tgtzpos = mRandom.randF(mb->mboxsize+5,mb->mboxsize+50);
				if(mRandom.randF() >0.5)
					tgtzpos *= -1;

				mb->metaballs[i].targetposition = Point3F(tgtxpos,tgtypos,tgtzpos);
			}

			if(mb->metaballs[i].position.x > (mb->mboxsize/2) || mb->metaballs[i].position.x < -(mb->mboxsize/2)){

				float tgtxpos = mRandom.randF(mb->mboxsize+5,mb->mboxsize+50);
				if(mRandom.randF() >0.5)
					tgtxpos *= -1;
				float tgtypos = mRandom.randF(mb->mboxsize+5,mb->mboxsize+50);
				if(mRandom.randF() >0.5)
					tgtypos *= -1;
				float tgtzpos = mRandom.randF(mb->mboxsize+5,mb->mboxsize+50);
				if(mRandom.randF() >0.5)
					tgtzpos *= -1;
				mb->metaballs[i].targetposition = Point3F(tgtxpos,tgtypos,tgtzpos);
			}
			else if(mb->metaballs[i].position.y > (mb->mboxsize/2) || mb->metaballs[i].position.y < -(mb->mboxsize/2)){

				float tgtxpos = mRandom.randF(mb->mboxsize+5,mb->mboxsize+50);
				if(mRandom.randF() >0.5)
					tgtxpos *= -1;
				float tgtypos = mRandom.randF(mb->mboxsize+5,mb->mboxsize+50);
				if(mRandom.randF() >0.5)
					tgtypos *= -1;
				float tgtzpos = mRandom.randF(mb->mboxsize+5,mb->mboxsize+50);
				if(mRandom.randF() >0.5)
					tgtzpos *= -1;
				mb->metaballs[i].targetposition = Point3F(tgtxpos,tgtypos,tgtzpos);
			}
			else if(mb->metaballs[i].position.z > (mb->mboxsize/2) || mb->metaballs[i].position.z < -(mb->mboxsize/2)){

				float tgtxpos = mRandom.randF(mb->mboxsize+5,mb->mboxsize+50);
				if(mRandom.randF() >0.5)
					tgtxpos *= -1;
				float tgtypos = mRandom.randF(mb->mboxsize+5,mb->mboxsize+50);
				if(mRandom.randF() >0.5)
					tgtypos *= -1;
				float tgtzpos = mRandom.randF(mb->mboxsize+5,mb->mboxsize+50);
				if(mRandom.randF() >0.5)
					tgtzpos *= -1;
				mb->metaballs[i].targetposition = Point3F(tgtxpos,tgtypos,tgtzpos);
			}

			VectorF B = mb->metaballs[i].position;
			VectorF P = mb->metaballs[i].targetposition;

			VectorF BP = P - B;
			VectorF newPos = B + ((F32)0.01 * BP);

			mb->metaballs[i].position = newPos;
			Con::printf("VEC1: %f %f %f" , P.x , P.y , P.z);
			Con::printf("VEC2: %f %f %f" , B.x , B.y , B.z);
			Con::printf("VEC3: %f %f %f" , BP.x , BP.y , BP.z);
			Con::printf("VEC4: %f %f %f" , newPos.x , newPos.y , newPos.z);

		}


		//if(metaballs[0].position.x >= 5 && metaballs[0].position.x <= 30)
		//	metaballs[0].position.x -= 5;

		//	 if(metaballs[0].position.x > 30){
		//	metaballs[0].position.x+= 5;
		//		 metaballs[0].position.x = 0;
		//	}

		//metaballs[0].position.y=0;

		//	metaballs[1].position.x=5;
		//	metaballs[1].position.y=5;

		//	metaballs[2].position.x=-5;
		//	metaballs[2].position.y=-5;


		//clear the field
		for(int i=0; i<cubeGrid.numVertices; i++)
		{
			cubeGrid.vertices[i].value=0.0f;
			cubeGrid.vertices[i].normal.zero();//LoadZero();
		}

		//evaluate the scalar field at each point
		Point3F ballToPoint;
		float squaredRadius;
		Point3F ballPosition;
		float normalScale;

		for(int i=0; i<mb->mNumMetaBalls; i++)
		{
			squaredRadius=mb->metaballs[i].squaredRadius;
			ballPosition=mb->metaballs[i].position;

			//VC++6 standard does not inline functions
			//by inlining these maually, in this performance-critical area,
			//almost a 100% increase in speed is found
			for(int j=0; j<cubeGrid.numVertices; j++)
			{
				//ballToPoint=cubeGrid.vertices[j].position-ballPosition;
				ballToPoint.x=cubeGrid.vertices[j].position.x-ballPosition.x;
				ballToPoint.y=cubeGrid.vertices[j].position.y-ballPosition.y;
				ballToPoint.z=cubeGrid.vertices[j].position.z-ballPosition.z;

				//get squared distance from ball to point
				//float squaredDistance=ballToPoint.GetSquaredLength();
				float squaredDistance=	ballToPoint.x*ballToPoint.x +
					ballToPoint.y*ballToPoint.y +
					ballToPoint.z*ballToPoint.z;
				if(squaredDistance==0.0f)
					squaredDistance=0.0001f;

				//value = r^2/d^2
				cubeGrid.vertices[j].value+=squaredRadius/squaredDistance;

				//normal = (r^2 * v)/d^4
				normalScale=squaredRadius/(squaredDistance*squaredDistance);

				cubeGrid.vertices[j].normal.x+=ballToPoint.x*normalScale;
				cubeGrid.vertices[j].normal.y+=ballToPoint.y*normalScale;
				cubeGrid.vertices[j].normal.z+=ballToPoint.z*normalScale;
			}
		}

		mb->moveready = true;

		Sim::postEvent(Sim::getRootGroup(), // post to the root group   
			new moveMetablobsLAVALAMP(mb),  // a new MyKewlEvent  
			Sim::getCurrentTime() + 25); // 100 ms in the future  
*/
	}
	

};


void metaBlobExample::resetall(int i){

	metaballs[i].velocity = mForce*mTimeScale; // meters per second
	metaballs[i].position = Point3F(0,0,0); // The shell positions, each unit a meter.
	metaballs[i].velocityvec = Point3F(0,0,0); //xvel, zvel; // The shell X and Y (2D) velocity components.

	metaballs[i].angle = mDegToRad(mElevationAngle);// Angle in radians, approximately 23º.

}

class moveMetablobsWATERFLOW : public SimEvent
{
public:
	metaBlobExample *mb;

	moveMetablobsWATERFLOW(metaBlobExample *mbin){
		mb = mbin;
	}

	void process( SimObject *object )
	{
		mb->moveready = true;
		mb->WaterFlowMove();

		Sim::postEvent(Sim::getRootGroup(), // post to the root group   
			new moveMetablobsWATERFLOW(mb),  // a new MyKewlEvent  
			Sim::getCurrentTime() + 50); // 100 ms in the future  

	}
};

void metaBlobExample::WaterFlowMove(){

		//Real-Time updating:
		//Sync vars with gui

		//move metablobs according to realistic physics:
		//should have gravity, and should have a "flow" vector influencing the strength of the liquid flow

		Point3F direction(0,0,1);

		mtickcount--;

		if(mtickcount <= 0){
			mtickcount = mMaxTickCount;
			if(mnumcurrentmetaballs < mNumMetaBalls)
				mnumcurrentmetaballs++;
		}

		for(int i = 0; i < mnumcurrentmetaballs;i++){ //numcurrentMetaballs

			if(metaballs[i].position.x > (mboxsize/2) || metaballs[i].position.x < -(mboxsize/2)){
				resetall(i);
			}
			else if(metaballs[i].position.y > (mboxsize/2) || metaballs[i].position.y < -(mboxsize/2)){
				resetall(i);
			}
			else if(metaballs[i].position.z > (mboxsize/2) || metaballs[i].position.z < -(mboxsize/2)){
				resetall(i);
			}

			//choose a direction for the fluid flow:
			VectorF oldpos = metaballs[i].position;

			metaballs[i].velocityvec.x = cos(metaballs[i].angle) * (metaballs[i].velocity);
			metaballs[i].velocityvec.z = sin(metaballs[i].angle) * (metaballs[i].velocity);

			metaballs[i].position.x += metaballs[i].velocityvec.x;
			metaballs[i].position.z += (metaballs[i].velocityvec.z - (grav_accel));

			metaballs[i].velocity = sqrt(pow(metaballs[i].velocityvec.x, 2) + pow((metaballs[i].velocityvec.z - grav_accel), 2));
			metaballs[i].angle = atan2f((metaballs[i].velocityvec.z - grav_accel), metaballs[i].velocityvec.x);

		}

		//clear the field
		for(int i=0; i<cubeGrid.numVertices; i++)
		{
			cubeGrid.vertices[i].value=0.0f;
			cubeGrid.vertices[i].normal.zero();//LoadZero();
		}

		//evaluate the scalar field at each point
		Point3F ballToPoint;
		float squaredRadius;
		Point3F ballPosition;
		float normalScale;

		for(int i=0; i<mNumMetaBalls; i++)
		{
			squaredRadius=metaballs[i].squaredRadius;
			ballPosition=metaballs[i].position;

			//VC++6 standard does not inline functions
			//by inlining these maually, in this performance-critical area,
			//almost a 100% increase in speed is found
			for(int j=0; j<cubeGrid.numVertices; j++)
			{
				ballToPoint.x=cubeGrid.vertices[j].position.x-ballPosition.x;
				ballToPoint.y=cubeGrid.vertices[j].position.y-ballPosition.y;
				ballToPoint.z=cubeGrid.vertices[j].position.z-ballPosition.z;

				//get squared distance from ball to point
				//float squaredDistance=ballToPoint.GetSquaredLength();
				float squaredDistance=	ballToPoint.x*ballToPoint.x +
					ballToPoint.y*ballToPoint.y +
					ballToPoint.z*ballToPoint.z;
				if(squaredDistance==0.0f)
					squaredDistance=0.0001f;

				//value = r^2/d^2
				cubeGrid.vertices[j].value+=squaredRadius/squaredDistance;

				//normal = (r^2 * v)/d^4
				normalScale=squaredRadius/(squaredDistance*squaredDistance);

				cubeGrid.vertices[j].normal.x+=ballToPoint.x*normalScale;
				cubeGrid.vertices[j].normal.y+=ballToPoint.y*normalScale;
				cubeGrid.vertices[j].normal.z+=ballToPoint.z*normalScale;
			}
		}
		moveready = true;

		inspectPostApply();

	}

	static const char* uitoa(U32 i)
{
   static char buf[32];
   dSprintf(buf, sizeof(buf), "%d", i);
   return buf;
}

int onadd = 1;
	bool metaBlobExample::onAdd()
	{
		if ( !Parent::onAdd() ){
			Con::printf("Error!");
			return false;
		}

		// Set up a 1x1x1 bounding box
		mObjBox.set( Point3F( -0.25f, -0.25f, -0.25f ),
			Point3F(  0.25f,  0.25f,  0.25f ) );

		resetWorldBox();
		
		 if ( isClientObject() ){
			 		Con::printf("print once... %d" , onadd);
		// Add this object to the scene
		addToScene();

		GameConnection* connection = GameConnection::getConnectionToServer(); 
		ShapeBase* control = dynamic_cast<ShapeBase*>(connection->getControlObject());  

		U32 tir =	(U32)this->getServerObject()->getId();

		initmetablobs();
		
		  Con::executef( this, "onAdd",Con::getIntArg(this->getServerObject()->getId()) );  
		moveready = false;
		onadd = 1;

		}
		
		return true;
	}

void  metaBlobExample::initmetablobs(){

	//set up grid
	cubeGrid.CreateMemory();

	mForce = 0;
	mElevationAngle = 0;

	mGridSize = 30;
	mNumMetaBalls = 4;
	mMinExtent = 40;
	mMaxExtent = 80;
	mMaxTickCount = 40;
	mTimeScale = 0.01;
	mGravAccel = 0.5*mTimeScale;

	mthreshold = 0.5f;
	mboxsize = mMinExtent+mMaxExtent;
	moveready = false;

	mtickcount = mMaxTickCount; // create a new metaball after this many ticks
	mnumcurrentmetaballs = 0;
	grav_accel = 0.5*mTimeScale;

	Con::printf("Init...");
	//VERTICES
	cubeGrid.numVertices=(mGridSize+1)*(mGridSize+1)*(mGridSize+1);

	int currentVertex=0;

	for(int i=0; i<mGridSize+1; i++)
	{
		for(int j=0; j<mGridSize+1; j++)
		{
			for(int k=0; k<mGridSize+1; k++)
			{

				cubeGrid.vertices[currentVertex].position.set
					((i*mMaxExtent)/(mGridSize)-mMinExtent, (j*mMaxExtent)/(mGridSize)-mMinExtent, (k*mMaxExtent)/(mGridSize)-mMinExtent);

				currentVertex++;

			}
		}
	}

	//CUBES
	cubeGrid.numCubes=(mGridSize)*(mGridSize)*(mGridSize);

	int currentCube=0;

	for(int i=0; i<mGridSize; i++)
	{
		for(int j=0; j<mGridSize; j++)
		{
			for(int k=0; k<mGridSize; k++)
			{
				cubeGrid.cubes[currentCube].vertices[0]=&cubeGrid.vertices[(i*(mGridSize+1)+j)*(mGridSize+1)+k];
				cubeGrid.cubes[currentCube].vertices[1]=&cubeGrid.vertices[(i*(mGridSize+1)+j)*(mGridSize+1)+k+1];
				cubeGrid.cubes[currentCube].vertices[2]=&cubeGrid.vertices[(i*(mGridSize+1)+(j+1))*(mGridSize+1)+k+1];
				cubeGrid.cubes[currentCube].vertices[3]=&cubeGrid.vertices[(i*(mGridSize+1)+(j+1))*(mGridSize+1)+k];
				cubeGrid.cubes[currentCube].vertices[4]=&cubeGrid.vertices[((i+1)*(mGridSize+1)+j)*(mGridSize+1)+k];
				cubeGrid.cubes[currentCube].vertices[5]=&cubeGrid.vertices[((i+1)*(mGridSize+1)+j)*(mGridSize+1)+k+1];
				cubeGrid.cubes[currentCube].vertices[6]=&cubeGrid.vertices[((i+1)*(mGridSize+1)+(j+1))*(mGridSize+1)+k+1];
				cubeGrid.cubes[currentCube].vertices[7]=&cubeGrid.vertices[((i+1)*(mGridSize+1)+(j+1))*(mGridSize+1)+k];

				currentCube++;


			}
		}
	}

	//	//set up metaballs
	for(int i=0; i<maxmetaballs; i++){
		METABALL b;
		b.Init(mForce,mDegToRad(mElevationAngle));

		metaballs.push_back(b);
	}

	//END OF INITIALISATION

	Sim::postEvent(Sim::getRootGroup(), // post to the root group   
		new moveMetablobsWATERFLOW(this),  // a new Event  
		Sim::getCurrentTime() + 1000); // 100 ms in the future  

	
}


void metaBlobExample::onRemove()
{
	// Remove this object from the scene
	removeFromScene();

	Parent::onRemove();
}

void metaBlobExample::setTransform(const MatrixF & mat)
{
	// Let SceneObject handle all of the matrix manipulation
	Parent::setTransform( mat );

	// Dirty our network mask so that the new transform gets
	// transmitted to the client object
	setMaskBits( TransformMask );
}

U32 metaBlobExample::packUpdate( NetConnection *conn, U32 mask, BitStream *stream )
{
	// Allow the Parent to get a crack at writing its info
	U32 retMask = Parent::packUpdate( conn, mask, stream );

	// Write our transform information
	if ( stream->writeFlag( mask & TransformMask ) )
	{
		mathWrite(*stream, getTransform());
		mathWrite(*stream, getScale());
	}

	// Write out any of the updated editable properties
	if ( stream->writeFlag( mask & UpdateMask ) )
		stream->write( mMaterialName );


	if ( stream->writeFlag( mask & UpdateMask ) ){
		stream->write( mGridSize );
		stream->write( mNumMetaBalls );
		stream->write( mMinExtent );
		stream->write( mMaxExtent );

		stream->write( mMaxTickCount );
		stream->write( mGravAccel );
		stream->write( mTimeScale );
		stream->write( mForce );
		stream->write( mElevationAngle );
	}

	return retMask;
}

void metaBlobExample::unpackUpdate(NetConnection *conn, BitStream *stream)
{
	// Let the Parent read any info it sent
	Parent::unpackUpdate(conn, stream);

	if ( stream->readFlag() )  // TransformMask
	{
		mathRead(*stream, &mObjToWorld);
		mathRead(*stream, &mObjScale);

		setTransform( mObjToWorld );
	}

	if ( stream->readFlag() )  // UpdateMask
	{
		stream->read( &mMaterialName );

		if ( isProperlyAdded() )
			updateMaterial();
	}


		if ( stream->readFlag() )  // UpdateMask
	{

		stream->read( &mGridSize );
		stream->read( &mNumMetaBalls );
		stream->read( &mMinExtent );
		stream->read( &mMaxExtent );

		stream->read( &mMaxTickCount );
		stream->read( &mGravAccel );
		stream->read( &mTimeScale );
		stream->read( &mForce );
		stream->read( &mElevationAngle );
	}
}




//-----------------------------------------------------------------------------
// Object Rendering
//-----------------------------------------------------------------------------
void metaBlobExample::createGeometry()
{
if(!moveready){
		return;
	}
	else
		moveready = false;

		Parent::inspectPostApply();

	// Flag the network mask to send the updates
	// to the client object
	setMaskBits( UpdateMask );

	mVertexBuffer = NULL;
	mPrimitiveBuffer = NULL;
	mVertCount = 0;
	mPrimCount = 0;

	for(int i=0; i<cubeGrid.numCubes; i++)
	{
		unsigned char cubeIndex=0;

		if(cubeGrid.cubes[i].vertices[0]->value < mthreshold)
			cubeIndex |= 1;
		if(cubeGrid.cubes[i].vertices[1]->value < mthreshold)
			cubeIndex |= 2;
		if(cubeGrid.cubes[i].vertices[2]->value < mthreshold)
			cubeIndex |= 4;
		if(cubeGrid.cubes[i].vertices[3]->value < mthreshold)
			cubeIndex |= 8;
		if(cubeGrid.cubes[i].vertices[4]->value < mthreshold)
			cubeIndex |= 16;
		if(cubeGrid.cubes[i].vertices[5]->value < mthreshold)
			cubeIndex |= 32;
		if(cubeGrid.cubes[i].vertices[6]->value < mthreshold)
			cubeIndex |= 64;
		if(cubeGrid.cubes[i].vertices[7]->value < mthreshold)
			cubeIndex |= 128;

		for(int k=0; triTable[cubeIndex][k]!=-1; k+=3)
		{
			//noo
			mPrimCount ++;
			mVertCount +=3;	

		}

	}

	static SURFACE_VERTEX edgeVertices[12];

	// Set up our normal and reflection StateBlocks
	GFXStateBlockDesc desc;

	// The normal StateBlock only needs a default StateBlock
	mNormalSB = GFX->createStateBlock( desc );

	// The reflection needs its culling reversed
	desc.cullDefined = true;
	desc.cullMode = GFXCullCW;
	mReflectSB = GFX->createStateBlock( desc );

	// Fill the vertex buffer
	VertexType *pVert = NULL;

	if(mVertCount <= 0){
		return;
	}
	mVertexBuffer.set( GFX,    mVertCount, GFXBufferTypeStatic );

	pVert = mVertexBuffer.lock();

	Point3F halfSize = getObjBox().getExtents() * 0.5f;

	//loop through cubes
	for(int i=0; i<cubeGrid.numCubes; i++)
	{
		//calculate which vertices are inside the surface
		unsigned char cubeIndex=0;

		if(cubeGrid.cubes[i].vertices[0]->value < mthreshold)
			cubeIndex |= 1;
		if(cubeGrid.cubes[i].vertices[1]->value < mthreshold)
			cubeIndex |= 2;
		if(cubeGrid.cubes[i].vertices[2]->value < mthreshold)
			cubeIndex |= 4;
		if(cubeGrid.cubes[i].vertices[3]->value < mthreshold)
			cubeIndex |= 8;
		if(cubeGrid.cubes[i].vertices[4]->value < mthreshold)
			cubeIndex |= 16;
		if(cubeGrid.cubes[i].vertices[5]->value < mthreshold)
			cubeIndex |= 32;
		if(cubeGrid.cubes[i].vertices[6]->value < mthreshold)
			cubeIndex |= 64;
		if(cubeGrid.cubes[i].vertices[7]->value < mthreshold)
			cubeIndex |= 128;
		//look this value up in the edge table to see which edges to interpolate along
		int usedEdges=edgeTable[cubeIndex];

		//if the cube is entirely within/outside surface, no faces			
		if(usedEdges==0 || usedEdges==255)
			continue;

		//update these edges
		for(int currentEdge=0; currentEdge<12; currentEdge++)
		{
			if(usedEdges & 1<<currentEdge)
			{
				CUBE_GRID_VERTEX * v1=cubeGrid.cubes[i].vertices[verticesAtEndsOfEdges[currentEdge*2  ]];
				CUBE_GRID_VERTEX * v2=cubeGrid.cubes[i].vertices[verticesAtEndsOfEdges[currentEdge*2+1]];

				float delta=(mthreshold - v1->value)/(v2->value - v1->value);
				
				edgeVertices[currentEdge].position.x=v1->position.x + delta*(v2->position.x - v1->position.x);
				edgeVertices[currentEdge].position.y=v1->position.y + delta*(v2->position.y - v1->position.y);
				edgeVertices[currentEdge].position.z=v1->position.z + delta*(v2->position.z - v1->position.z);
			
				edgeVertices[currentEdge].normal.x=v1->normal.x + delta*(v2->normal.x - v1->normal.x);
				edgeVertices[currentEdge].normal.y=v1->normal.y + delta*(v2->normal.y - v1->normal.y);
				edgeVertices[currentEdge].normal.z=v1->normal.z + delta*(v2->normal.z - v1->normal.z);
			}
		}


		for(int k=0; triTable[cubeIndex][k]!=-1; k+=3)
		{

			F32 tu = 0;
			F32 tv = 0;

			pVert->normal = (edgeVertices[triTable[cubeIndex][k+1]].normal);	
			pVert->point =(edgeVertices[triTable[cubeIndex][k+1]].position);


			pVert->texCoord = Point2F(pVert->point.x+pVert->point.z,pVert->point.y+pVert->point.z);
			tu = asin(pVert->point.x)/M_PI+0.5;
			tv = asin(pVert->point.y)/M_PI+0.5; 
		
			pVert++;

			pVert->normal  = (edgeVertices[triTable[cubeIndex][k+2]].normal);
			pVert->point =(edgeVertices[triTable[cubeIndex][k+2]].position);

			pVert->texCoord = Point2F(pVert->point.x+pVert->point.z,pVert->point.y+pVert->point.z);
			tu = asin(pVert->point.x)/M_PI+0.5;
			tv = asin(pVert->point.y)/M_PI+0.5; 

			pVert++;

			pVert->normal  = (edgeVertices[triTable[cubeIndex][k+0]].normal);
			pVert->point =(edgeVertices[triTable[cubeIndex][k+0]].position);

			pVert->texCoord = Point2F(pVert->point.x+pVert->point.z,pVert->point.y+pVert->point.z);
			tu = asin(pVert->point.x)/M_PI+0.5;
			tv = asin(pVert->point.y)/M_PI+0.5; 


			pVert++;

		}
	}

	mVertexBuffer.unlock();


	//now, for the primitive buffer:

	U16 *pIdx = NULL;

	mPrimitiveBuffer.set( GFX, mVertCount, mPrimCount, GFXBufferTypeStatic );

	mPrimitiveBuffer.lock(&pIdx);     

	for (U16 i = 0; i < mVertCount; i+=3){
		pIdx[i] = i+1;
		pIdx[i+1] = i+2;
		pIdx[i+2] = i+0;
	}

	mPrimitiveBuffer.unlock();

}


//Perform per frame updates
void metaBlobExample::UpdateFrame()
{
	
}

void metaBlobExample::updateMaterial()
{
	if ( mMaterialName.isEmpty() )
		return;

	// If the material name matches then don't bother updating it.
	if ( mMaterialInst && mMaterialName.equal( mMaterialInst->getMaterial()->getName(), String::NoCase ) )
		return;

	SAFE_DELETE( mMaterialInst );

	mMaterialInst = MATMGR->createMatInstance( mMaterialName, getGFXVertexFormat< VertexType >() );
	if ( !mMaterialInst )
		Con::errorf( "RenderMeshExample::updateMaterial - no Material called '%s'", mMaterialName.c_str() );
}


void metaBlobExample::prepRenderImage( SceneRenderState *state )
{


	// Do a little prep work if needed
	createGeometry();

	// If we have no material then skip out.
	if ( !mMaterialInst )
		return;

	// If we don't have a material instance after the override then 
	// we can skip rendering all together.
	BaseMatInstance *matInst = state->getOverrideMaterial( mMaterialInst );
	if ( !matInst )
		return;

	// Get a handy pointer to our RenderPassmanager
	RenderPassManager *renderPass = state->getRenderPass();

	// Allocate an MeshRenderInst so that we can submit it to the RenderPassManager
	MeshRenderInst *ri = renderPass->allocInst<MeshRenderInst>();

	// Set our RenderInst as a standard mesh render
	ri->type = RenderPassManager::RIT_Mesh;

	// Calculate our sorting point
	if ( state )
	{
		// Calculate our sort point manually.
		const Box3F& rBox = getRenderWorldBox();
		ri->sortDistSq = rBox.getSqDistanceToPoint( state->getCameraPosition() );      
	} 
	else 
		ri->sortDistSq = 0.0f;

	// Set up our transforms
	MatrixF objectToWorld = getRenderTransform();
	objectToWorld.scale( getScale() );

	ri->objectToWorld = renderPass->allocUniqueXform( objectToWorld );
	ri->worldToCamera = renderPass->allocSharedXform(RenderPassManager::View);
	ri->projection    = renderPass->allocSharedXform(RenderPassManager::Projection);

	// If our material needs lights then fill the RIs 
	// light vector with the best lights.
	if ( matInst->isForwardLit() )
	{
		LightQuery query;
		query.init( getWorldSphere() );
		query.getLights( ri->lights, 8 );
	}

	// Make sure we have an up-to-date backbuffer in case
	// our Material would like to make use of it
	// NOTICE: SFXBB is removed and refraction is disabled!
	//ri->backBuffTex = GFX->getSfxBackBuffer();

	// Set our Material
	ri->matInst = matInst;

	// Set up our vertex buffer and primitive buffer
	ri->vertBuff = &mVertexBuffer;
	ri->primBuff = &mPrimitiveBuffer;

	ri->prim = renderPass->allocPrim();
	ri->prim->type =  GFXTriangleList;
	ri->prim->minIndex = 0;
	ri->prim->startIndex = 0;
	ri->prim->numPrimitives = mPrimCount;//12;
	ri->prim->startVertex = 0;
	ri->prim->numVertices = mVertCount;//36;

	// We sort by the material then vertex buffer
	ri->defaultKey = matInst->getStateHint();
	ri->defaultKey2 = (U32)ri->vertBuff; // Not 64bit safe!

	// Submit our RenderInst to the RenderPassManager
	state->getRenderPass()->addInst( ri );

	//UpdateFrame();
}

void metaBlobExample::render( ObjectRenderInst *ri, SceneRenderState *state, BaseMatInstance *overrideMat )
{

}

DefineEngineMethod( metaBlobExample, postApply, void, (),,
	"A utility method for forcing a network update.\n")
{
	object->inspectPostApply();
}
