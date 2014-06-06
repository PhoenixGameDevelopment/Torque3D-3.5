//////////////////////////////////////////////////////////////////////////////////////////
//	METABALL.h
//	Class declaration for metaball
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

class METABALL
{
public:
	//double xpos = 0, zpos = 0; // The shell positions, each unit a meter.
	Point3F position;

	double angle; // Angle in radians, approximately 23º.
	double velocity; // meters per second

	Point3F velocityvec;
	
	//double xvel, zvel; // The shell X and Y (2D) velocity components.


	float squaredRadius;
	Point3F targetposition;

	void Init(float vel,float angle)
	{

	position = Point3F(-199.0f,-199.0f,-199.0f);
	angle = angle;

	double timescale = 0.01;

	velocity = vel*timescale;
	velocityvec = Point3F(0,0,0);

	squaredRadius = 2.0f;
	Point3F targetposition = Point3F(-999.9f,-999.9f,-999.9f);

	}
};