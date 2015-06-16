//
//  Trajectory.cpp
//  TrajectoryPolynomial
//
//  Created by Chao Liu on 11/5/14.
//  Copyright (c) 2014 ModLab. All rights reserved.
//

#include "Trajectory.h"

Trajectory::Trajectory(float startTime, float endTime, uint16_t sectionNum, Polynomial *positionPolynomial)
{
    _startTime = startTime;
    _endTime = endTime;
    _sectionNum = sectionNum;
    _positionPolynomial = positionPolynomial;
    diff = _endTime - _startTime;
    step = diff/_sectionNum;
    for (int i=0; i<=sectionNum; i++) {
      time_array.push_back(_startTime + i * step);
    }

}

Trajectory::Trajectory(uint16_t positionNum, float min_position, float max_position, Polynomial *positionPolynomial)
{
	_positionPolynomial = positionPolynomial;
	_positionNum = positionNum;
        _min_position = min_position;
        _max_position = max_position;
}

void Trajectory::GeneratePositionTrajectory(float *position_trajectory)
{
    for (int i=0; i<=_sectionNum; i++) {
        position_trajectory[i] = _positionPolynomial->Eval(time_array[i]);
    }
}

void Trajectory::GenerateVelocityTrajectory(float *velocity_trajectory)
{
    vector<float> positionPolynomialCoefficient;
    positionPolynomialCoefficient = _positionPolynomial->returnCoefficient();
    uint8_t positionTerms = 0;
    positionTerms = _positionPolynomial->returnTerms();
    uint8_t velocityTerm = positionTerms-1;
    float velocityPolynomialCoefficient[positionTerms-1];
    for (uint8_t i=0; i<velocityTerm; i++)
    {
      velocityPolynomialCoefficient[i] = positionPolynomialCoefficient[i] * (velocityTerm-i);
    }
    
    Polynomial _velocityPolynomial(velocityPolynomialCoefficient, velocityTerm);
    
    for (int i=0; i<=_sectionNum; i++) {
        velocity_trajectory[i] = _velocityPolynomial.Eval(time_array[i]);
    }
}

void Trajectory::GeneratePositionVelocityTrajectory(float *position_trajectory, float *velocity)
{
    vector<float> positionPolynomialCoefficient;
    positionPolynomialCoefficient = _positionPolynomial->returnCoefficient();
    constant_velocity = positionPolynomialCoefficient[0];
    *velocity = positionPolynomialCoefficient[0];
    float range = _max_position - _min_position;
    positionStep = (range) / (float)_positionNum;
    if (constant_velocity > 0)
    {
        for (int i=0; i<=_positionNum; i++)
        {
            position_trajectory[i] = i * positionStep + positionPolynomialCoefficient[1];
            if (position_trajectory[i] > _max_position)
            {
                position_trajectory[i] = position_trajectory[i] - range;
            }
        }
    }

    if (constant_velocity < 0)
    {
        for (int i=0; i<=_positionNum; i++)
        {
            position_trajectory[i] = -i * positionStep + positionPolynomialCoefficient[1];
            if (position_trajectory[i] < _min_position)
            {
                position_trajectory[i] = position_trajectory[i] + range;
            }
        }
    }
}

vector<float> Trajectory::returnTimeArrayPosition(void)
{
	return time_array;
}

vector<float> Trajectory::returnTimeArrayVelocity(void)
{
	float dt = positionStep / constant_velocity;
	vector<float> time_array_velocity;
	for (int i=0; i<=_positionNum; i++)
	{
		time_array_velocity.push_back(i * dt);
    }
	return time_array_velocity;
}

void Trajectory::updateTrajectory(float startTime, float endTime, uint16_t sectionNum, Polynomial *positionPolynomial)
{
    _startTime = startTime;
    _endTime = endTime;
    _sectionNum = sectionNum;
    _positionPolynomial = positionPolynomial;
    diff = _endTime - _startTime;
    step = diff/_sectionNum;
    for (int i=0; i<=sectionNum; i++) {
//      time_array.push_back(_startTime + i * step);
        time_array[i] = _startTime + i * step;
    }
}

void Trajectory::updateTrajectory(uint16_t positionNum, float min_position, float max_position, Polynomial *positionPolynomial)
{
    _positionPolynomial = positionPolynomial;
    _positionNum = positionNum;
    _min_position = min_position;
    _max_position = max_position;
}
