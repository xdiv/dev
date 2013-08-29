#include "positionclass.h"

PositionClass::PositionClass()
{
	m_frameTime = 0.0f;
	m_rotationY = 0.0f;
	m_leftTurnSpeed  = 0.0f;
	m_rightTurnSpeed = 0.0f;
}


PositionClass::PositionClass(const PositionClass& other)
{
}


PositionClass::~PositionClass()
{
}
//The SetFrameTime function is used to set the frame speed in this class. PositionClass will use that frame time speed to calculate how fast the viewer should be moving and rotating. This function should always be called at the beginning of each frame before using this class to move the viewing position.

void PositionClass::SetFrameTime(float time)
{
	m_frameTime = time;
	return;
}
//GetRotation returns the Y-axis rotation of the viewer. This is the only helper function we need for this tutorial but could be expanded to get more information about the location of the viewer.

void PositionClass::GetRotation(float& y)
{
	y = m_rotationY;
	return;
}
//The movement functions both work the same. Both functions are called each frame. The keydown input variable to each function indicates if the user is pressing the left key or the right key. If they are pressing the key then each frame the speed will accelerate until it hits a maximum. This way the camera speeds up similar to the acceleration in a vehicle creating the effect of smooth movement and high responsiveness. Likewise if the user releases the key and the keydown variable is false it will then smoothly slow down each frame until the speed hits zero. The speed is calculated against the frame time to ensure the movement speed remains the same regardless of the frame rate. Each function then uses some basic math to calculate the new position of the camera.

void PositionClass::TurnLeft(bool keydown)
{
	// If the key is pressed increase the speed at which the camera turns left.  If not slow down the turn speed.
	if(keydown)
	{
		m_leftTurnSpeed += m_frameTime * 0.01f;

		if(m_leftTurnSpeed > (m_frameTime * 0.15f))
		{
			m_leftTurnSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_leftTurnSpeed -= m_frameTime* 0.005f;

		if(m_leftTurnSpeed < 0.0f)
		{
			m_leftTurnSpeed = 0.0f;
		}
	}

	// Update the rotation using the turning speed.
	m_rotationY -= m_leftTurnSpeed;
	if(m_rotationY < 0.0f)
	{
		m_rotationY += 360.0f;
	}

	return;
}


void PositionClass::TurnRight(bool keydown)
{
	// If the key is pressed increase the speed at which the camera turns right.  If not slow down the turn speed.
	if(keydown)
	{
		m_rightTurnSpeed += m_frameTime * 0.01f;

		if(m_rightTurnSpeed > (m_frameTime * 0.15f))
		{
			m_rightTurnSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_rightTurnSpeed -= m_frameTime* 0.005f;

		if(m_rightTurnSpeed < 0.0f)
		{
			m_rightTurnSpeed = 0.0f;
		}
	}

	// Update the rotation using the turning speed.
	m_rotationY += m_rightTurnSpeed;
	if(m_rotationY > 360.0f)
	{
		m_rotationY -= 360.0f;
	}

	return;
}
