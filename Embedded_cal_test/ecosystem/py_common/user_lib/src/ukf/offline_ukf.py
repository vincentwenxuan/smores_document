from numpy import zeros, isnan, append
from numpy.linalg import eig
from ukf_python import Ukf, ExpectedAccelleration
from ctypes import c_float
from matplotlib.pyplot import plot, figure

class OfflineUkf:

  def __init__(self):
    self.estimator = Ukf()
  
  def Process(self, t, z_meas):
    # allocate storage
    self.t = append(t[0], t)
    self.q = zeros((t.size+1, 4), dtype=c_float)
    self.w = zeros((t.size+1, 3), dtype=c_float)
    self.P = zeros((t.size+1, 6, 6), dtype=c_float)
    self.z_meas = z_meas;
    
    # initial estimator state
    self.q[0,:] = self.estimator.q
    self.w[0,:] = self.estimator.w
    self.P[0,:,:] = self.estimator.P
    
    # store estimator results
    for i in xrange(t.size):
      self.estimator.Update(z_meas[i,:], t[i])
      self.q[i+1,:] = self.estimator.q
      self.w[i+1,:] = self.estimator.w
      self.P[i+1,:,:] = self.estimator.P
      
      terminate = False
      if isnan(self.q[i+1,:]).any() or isnan(self.w[i+1,:]).any() or isnan(self.P[i+1,:]).any():
        print "Terminated with nan."
        terminate = True
        
      if terminate:
        self.t = self.t[:i+2]
        self.q = self.q[:i+2,:]
        self.w = self.w[:i+2,:]
        self.P = self.P[:i+2,:]
        self.z_meas = self.z_meas[:i+2,:];
        break

  def ExpectedAccel(self):
    self.exp_accel = zeros((t.size, 3), dtype=c_float)
    for i in xrange(t.size):
      temp = zeros(3, dtype=c_float)
      ExpectedAccelleration(self.q[i,:], temp)
      self.exp_accel[i,:] = temp
    
  def PlotCompareAccel(self):
    self.ExpectedAccel()
    return plot(self.t[1:], self.exp_accel[:,0], 'r', self.t[1:], self.exp_accel[:,1], 'g', self.t[1:], self.exp_accel[:,2], 'b',
                self.t[1:], self.z_meas[:,0], 'r.', self.t[1:], self.z_meas[:,1], 'g.', self.t[1:], self.z_meas[:,2], 'b.')
                
  def PlotCompareVel(self):
    return plot(self.t, self.w[:,0], 'r', self.t, self.w[:,1], 'g', self.t, self.w[:,2], 'b',
                self.t[1:], self.z_meas[:,3], 'r.', self.t[1:], self.z_meas[:,4], 'g.', self.t[1:], self.z_meas[:,5], 'b.')

  def PlotQuat(self):
    return plot(self.t, self.q[:,0], 'k.', self.t, self.q[:,1], 'r.', self.t, self.q[:,2], 'g.', self.t, self.q[:,3], 'b.')
    
  def PlotQuatCov(self):
    return plot(self.t, self.P[:,0,0], 'r.', self.t, self.P[:,1,1], 'g.', self.t, self.P[:,2,2], 'b.')
    
  def PlotVel(self):
    return plot(self.t, self.w[:,0], 'r.', self.t, self.w[:,1], 'g.', self.t, self.w[:,2], 'b.')
    
  def PlotVelCov(self):
    return plot(self.t, self.P[:,3,3], 'r.', self.t, self.P[:,4,4], 'g.', self.t, self.P[:,5,5], 'b.')




if __name__=='__main__':

  from packet_server import PacketServer
  from server_plot_utils import gather
  from matplotlib.pyplot import show
  
  ps = PacketServer()
  
  print "Loading File..."
  # ps.FromFile('clamped_z.json')
  ps.FromFile('motions.json')
  
  print "Preparing Inputs..."
  z_meas = gather(ps, 4, ('ax', 'ay', 'az', 'wx', 'wy', 'wz')).astype(c_float)
  t = gather(ps, 4, 'micro_t').astype(c_float)
  
  print "Running Ukf..."
  u = OfflineUkf()
  u.Process(t, z_meas)
  
  print "Done."
  
  figure()
  u.PlotQuat()
  figure()
  u.PlotVel()
  show()
