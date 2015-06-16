//
//  Trajectory.h
//  TrajectoryPolynomial
//
//  Created by Chao Liu on 11/5/14.
//  Copyright (c) 2014 ModLab. All rights reserved.
//

#ifndef __TrajectoryPolynomial__Trajectory__
#define __TrajectoryPolynomial__Trajectory__

#include "Polynomial.h"

class Trajectory {
public:
    Trajectory(float startTime, float endTime, uint16_t sectionNum, Polynomial *positionPolynomial);
    Trajectory(uint16_t positionNum, float min_position, float max_position, Polynomial *positionPolynomial);
    void GeneratePositionTrajectory(float *position_trajectory);
    void GenerateVelocityTrajectory(float *velocity_trajectory);
    void GeneratePositionVelocityTrajectory(float *position_trajectory, float *velocity);
    vector<float> returnTimeArrayPosition(void);
    vector<float> returnTimeArrayVelocity(void);
    void updateTrajectory(float startTime, float endTime, uint16_t sectionNum, Polynomial *positionPolynomial);
    void updateTrajectory(uint16_t positionNum, float min_position, float max_position, Polynomial *positionPolynomial);
	
    
private:
    float _startTime;
    float _endTime;
    uint16_t _sectionNum;
    Polynomial *_positionPolynomial;
    float diff;
    float step;
    vector<float> time_array;
    uint16_t _positionNum;
    float _min_position;
    float _max_position;
    float positionStep;
    float constant_velocity;
};


#endif /* defined(__TrajectoryPolynomial__Trajectory__) */
