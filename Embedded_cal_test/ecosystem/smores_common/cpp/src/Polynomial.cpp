//
//  Polynomial.cpp
//  TrajectoryPolynomial
//
//  Created by Chao Liu on 11/5/14.
//  Copyright (c) 2014 ModLab. All rights reserved.
//

#include "Polynomial.h"

Polynomial::Polynomial(float* coeff, uint8_t terms)
{
    _terms = terms;
    for(int i=0; i<_terms; i++)
    {
      _coeff.push_back(*(coeff+i));
    }
}

float Polynomial::Eval(float x)
{
    float res = 0;
    for (int i=0; i<_terms; i++) {
      res += _coeff[i] * (float)pow(x, _terms-1-i);
    }
    return res;
}

vector<float> Polynomial::returnCoefficient(void)
{
    return _coeff;
}

uint8_t Polynomial::returnTerms(void)
{
    return _terms;
}

void Polynomial::updateCofficient(float* coeff)
{
    for(int i=0; i<_terms; i++)
    {
        _coeff[i] = *(coeff+i);
//      _coeff.push_back(*(coeff+i));
    }
}
