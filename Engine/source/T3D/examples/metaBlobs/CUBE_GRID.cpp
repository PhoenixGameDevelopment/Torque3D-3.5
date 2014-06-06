//////////////////////////////////////////////////////////////////////////////////////////
//	CUBE_GRID.cpp
//	function definitions for grid of cubes for "marching cubes" algorithm
//	Downloaded from: www.paulsprojects.net
//	Created:	20th July 2002
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	

//Modified by Jack Stone, Phoenix Game Development
//contact@phoenixgamedevelopment.com
//www.phoenixgamedevelopment.com

#include <windows.h>
#include <GL/gl.h>

#ifndef CBH
#include "CUBE_GRID.h"
#endif

#include "console/consoleTypes.h"


bool CUBE_GRID::CreateMemory()
{
	vertices=new CUBE_GRID_VERTEX[(maxGridSize+1)*(maxGridSize+1)*(maxGridSize+1)];
	if(!vertices)
	{
		return false;
	}

	cubes=new CUBE_GRID_CUBE[maxGridSize*maxGridSize*maxGridSize];
	if(!cubes)
	{
		return false;
	}

	return true;
}
		

bool CUBE_GRID::Init(int gridSize)
{
	
	//VERTICES
	numVertices=(gridSize+1)*(gridSize+1)*(gridSize+1);
	
	int currentVertex=0;

	for(int i=0; i<gridSize+1; i++)
	{
		for(int j=0; j<gridSize+1; j++)
		{
			for(int k=0; k<gridSize+1; k++)
			{
				
				vertices[currentVertex].position.set
						((i*20.0f)/(gridSize)-10.0f, (j*20.0f)/(gridSize)-10.0f, (k*20.0f)/(gridSize)-10.0f);

				currentVertex++;

			}
		}
	}

	//CUBES
	numCubes=(gridSize)*(gridSize)*(gridSize);

	int currentCube=0;

	for(int i=0; i<gridSize; i++)
	{
		for(int j=0; j<gridSize; j++)
		{
			for(int k=0; k<gridSize; k++)
			{
				cubes[currentCube].vertices[0]=&vertices[(i*(gridSize+1)+j)*(gridSize+1)+k];
				cubes[currentCube].vertices[1]=&vertices[(i*(gridSize+1)+j)*(gridSize+1)+k+1];
				cubes[currentCube].vertices[2]=&vertices[(i*(gridSize+1)+(j+1))*(gridSize+1)+k+1];
				cubes[currentCube].vertices[3]=&vertices[(i*(gridSize+1)+(j+1))*(gridSize+1)+k];
				cubes[currentCube].vertices[4]=&vertices[((i+1)*(gridSize+1)+j)*(gridSize+1)+k];
				cubes[currentCube].vertices[5]=&vertices[((i+1)*(gridSize+1)+j)*(gridSize+1)+k+1];
				cubes[currentCube].vertices[6]=&vertices[((i+1)*(gridSize+1)+(j+1))*(gridSize+1)+k+1];
				cubes[currentCube].vertices[7]=&vertices[((i+1)*(gridSize+1)+(j+1))*(gridSize+1)+k];

				currentCube++;

				
			}
		}
	}

	return true;
}

void CUBE_GRID::FreeMemory()
{
	if(vertices)
		delete [] vertices;
	vertices=NULL;
	numVertices=0;

	if(cubes)
		delete [] cubes;
	cubes=NULL;
	numCubes=0;
}
