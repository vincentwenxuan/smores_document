# UKF Implementation Over Quaternions and Angular Rates
# Jimmy Paulos, June 2013
# Closely follows the algorithm of Kraft 2003 with resolved errata.

from ctypes import *
from numpy import array, zeros, diag
from numpy.ctypeslib import ndpointer
import numpy
import struct

################################################################################
# Must have already complied to static_ukf.so; try from src:

# g++ -shared -fPIC ../../../cpp_common/user_lib/src/ukf_helper.cpp ../../../cpp_common/user_lib/src/ukf_python.cpp -I ../../../cpp_common/user_lib/inc/ -I ../../../cpp_common/ -o ../obj/_ukf_python.so

################################################################################
# Configure Low Level Access to C++ Library

f = '/home/jpaulos/Workspace/ppr_code/trunk/ecosystem/py_common/user_lib/obj/_ukf_python.so'
clib = cdll.LoadLibrary(f);

# Functions
_Update = clib.Update_ctypes
_Update.argtypes = [ndpointer(c_float),
                    c_float,
                    ndpointer(c_float),
                    ndpointer(c_float),
                    ndpointer(c_float),
                    ndpointer(c_float),
                    ndpointer(c_float)]

_UpdateRaw = clib.UpdateRaw_ctypes
_UpdateRaw.argtypes = [c_float, c_float, c_float, c_float, c_float, c_float,
                    c_float,
                    ndpointer(c_float),
                    ndpointer(c_float),
                    ndpointer(c_float),
                    ndpointer(c_float),
                    ndpointer(c_float)]
                    
StateMeanCov = clib.StateMeanCov_ctypes
StateMeanCov.argtypes = [ ndpointer(c_float),
                          ndpointer(c_float),
                          ndpointer(c_float),
                          ndpointer(c_float),
                          ndpointer(c_float),
                          ndpointer(c_float),
                          ndpointer(c_float)]

SigmaPoints = clib.SigmaPoints_ctypes
SigmaPoints.argtypes = [  ndpointer(c_float),
                          ndpointer(c_float),
                          ndpointer(c_float)]
                          
ExpectedAccelleration = clib.ExpectedAccelleration_ctypes
ExpectedAccelleration.argtypes = [ndpointer(c_float),
                                  ndpointer(c_float)]

################################################################################
# Define Safe Access to C++ Library

class Ukf:
  def __init__(self):
    self.last_t = 0
    self.P = diag(array((1e-2, 1e-2, 1e-2, 1e-2, 1e-2, 1e-2), dtype=c_float))
    self.Q = diag(array((1e-5, 1e-5, 1e-5, 1e-2, 1e-2, 1e-2), dtype=c_float))
    self.R = diag(array((1e-2, 1e-2, 1e-2, 1e-3, 1e-3, 1e-3), dtype=c_float))
    self.q = array((1,0,0,0), dtype=c_float)
    self.w = array((0,0,0), dtype=c_float)

  # z_meas is [acc; w]
  def Update(self, z_meas, t):

    dt = t - self.last_t
    
    if z_meas.dtype == c_float and z_meas.shape == (6,):
      """
      _Update(z_meas,
              dt,
              self.Q,
              self.R,
              self.P,
              self.q,
              self.w)
      """
      # print z_meas
      # print self.w
      # print self.q

      _UpdateRaw(z_meas[0], z_meas[1], z_meas[2], z_meas[3], z_meas[4], z_meas[5],
              dt,
              self.Q,
              self.R,
              self.P,
              self.q,
              self.w)
      self.last_t = t

    else:
      print "Wrong argument type"
      print z_meas.dtype
      print z_meas.shape

    
################################################################################
# Test C Library

if __name__=='__main__':

  numpy.set_printoptions(precision=3)
  numpy.set_printoptions(linewidth=200)
  
  print
  print "**********************************************************************"
  print "Construction and Update Example"
  print
  
  u = Ukf()
  
  print "Original P:"
  print u.P
  print
  
  z_meas = array([0,0,0,0,0,0], dtype=c_float)
  t = 0.001
  u.Update(z_meas, t)
  
  print "After Update:"
  print u.P
  print
  
  print "**********************************************************************"
  print "Covariance Math Verification"
  print
  
  P1 = diag(1e-3 * array([1,2,3,4,5,6], dtype=c_float))
  print "Original P1:"
  print P1
  print

  print "Making Sigma Points..."
  Wq2 = zeros((4,12), dtype=c_float)
  Ww2 = zeros((3,12), dtype=c_float)
  SigmaPoints(P1, Wq2, Ww2)

  q_est = array([1, 0, 0, 0], dtype=c_float)
  q3 = zeros(4, dtype=c_float)
  w3 = zeros(3, dtype=c_float)
  P3 = zeros((6,6), dtype=c_float)
  e3 = zeros((6,12), dtype=c_float)
  StateMeanCov(Wq2, Ww2, q_est, q3, w3, P3, e3)
  print "Remade P3:"
  print P3
  print
  
  print "Making Sigma Points..."
  Wq4 = zeros((4,12), dtype=c_float)
  Ww4 = zeros((3,12), dtype=c_float)
  SigmaPoints(P3, Wq4, Ww4)

  q_est = array([1, 0, 0, 0], dtype=c_float)
  q5 = zeros(4, dtype=c_float)
  w5 = zeros(3, dtype=c_float)
  P5 = zeros((6,6), dtype=c_float)
  e5 = zeros((6,12), dtype=c_float)
  StateMeanCov(Wq4, Ww4, q_est, q5, w5, P5, e5)
  print "Again Recycled P5:"
  print P5
  print
