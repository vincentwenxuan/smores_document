//
//  Polynomial.h
//  TrajectoryPolynomial
//
//  Created by Chao Liu on 11/5/14.
//  Copyright (c) 2014 ModLab. All rights reserved.
//

#ifndef __TrajectoryPolynomial__Polynomial__
#define __TrajectoryPolynomial__Polynomial__

#include "stm32f30x.h"
#include <math.h>
#include <string.h>
#include <vector>

using namespace std;

class Polynomial {
public:
    Polynomial(float* coeff, uint8_t terms);
    float Eval(float x);
    vector<float> returnCoefficient(void);
    uint8_t returnTerms(void);
    void updateCofficient(float* coeff);
	
private:
    uint8_t _terms;
    vector<float> _coeff;
};


#endif /* defined(__TrajectoryPolynomial__Polynomial__) */
