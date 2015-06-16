"""
Rotating cube graphical orientation display.
 
Based on Leonel Machava's pygame demo "Rotating 3D Cube".
"""
import sys, math, pygame
from operator import itemgetter

import argparse
import os
from datetime import datetime
import curses
from time import sleep
from packet_server import PacketServer
from serial_packet import SerialPacket
from numpy import array, dot, float64, identity, outer




def quaternion_matrix(quaternion):
  q = array(quaternion, dtype=float64, copy=True)
  q *= math.sqrt(2.0 / dot(q, q))
  q = outer(q, q)
  return array([
      [1.0-q[2, 2]-q[3, 3],     q[1, 2]-q[3, 0],     q[1, 3]+q[2, 0]],
      [    q[1, 2]+q[3, 0], 1.0-q[1, 1]-q[3, 3],     q[2, 3]-q[1, 0]],
      [    q[1, 3]-q[2, 0],     q[2, 3]+q[1, 0], 1.0-q[1, 1]-q[2, 2]]])




class Point3D:
  def __init__(self, x=0, y=0, z=0):
    self.x, self.y, self.z = float(x), float(y), float(z)
      
  def rotate(self, q):
    p = array([self.x, self.y, self.z])
    p = quaternion_matrix(q).dot(p)
    return Point3D(p[0], p[1], p[2])

  def project(self, win_width, win_height, fov=2000, viewer_distance=20):
    """ Transforms this 3D point to 2D using a perspective projection. """
    factor = fov / (viewer_distance + self.y)
    x = self.x * factor + win_width / 2
    y = -self.z * factor + win_height / 2
    z = self.y
    return Point3D(x, y, z)




class Simulation:
  def __init__(self, ps, sp, win_width = 640, win_height = 480):
    self.ps = ps
    self.sp = sp

    pygame.init()
    
    self.screen = pygame.display.set_mode((win_width, win_height))
    pygame.display.set_caption("Orientation Display")
    
    self.clock = pygame.time.Clock()

    self.vertices = [
        Point3D(-1,1,-1),
        Point3D(1,1,-1),
        Point3D(1,-1,-1),
        Point3D(-1,-1,-1),
        Point3D(-1,1,1),
        Point3D(1,1,1),
        Point3D(1,-1,1),
        Point3D(-1,-1,1)
    ]

    # Define the vertices that compose each of the 6 faces. These numbers are
    # indices to the vertices list defined above.
    self.faces  = [(0,1,2,3),(1,5,6,2),(5,4,7,6),(4,0,3,7),(0,4,5,1),(3,2,6,7)]

    # Define colors for each face
    self.colors = [(255,0,255),(255,0,0),(0,255,0),(0,0,255),(0,255,255),(255,255,0)]

    self.angle = 0
        
  def run(self):
    """ Main Loop """
    
    while True:
    
      # catch messages
      types = []
      while True:
        (cpu_t, bytes) = self.sp.GetPacketBytes()
        type_byte = self.ps.AppendMessage(cpu_t, bytes);
        if type_byte is None:
          break
  
      # get latest quaternion
      if self.ps.msg_byte.has_key(5):
        msg = self.ps.msg_byte[5][-1]
        q = (msg.qw, msg.qx, msg.qy, msg.qz)
      else:
        q = (1,0,0,0)
      
      for event in pygame.event.get():
        if event.type == pygame.QUIT:
          pygame.quit()
          sys.exit()

      self.clock.tick(50)
      self.screen.fill((0,32,0))

      # It will hold transformed vertices.
      t = []
      
      for v in self.vertices:
        # rotate 3D point
        r = v.rotate(q) 
        # Transform the point from 3D to 2D
        p = r.project(self.screen.get_width(), self.screen.get_height())
        # Put the point in the list of transformed vertices
        t.append(p)

      # Calculate the average Z values of each face.
      avg_z = []
      i = 0
      for f in self.faces:
        z = (t[f[0]].z + t[f[1]].z + t[f[2]].z + t[f[3]].z) / 4.0
        avg_z.append([i,z])
        i = i + 1

      # Draw the faces using the Painter's algorithm:
      # Distant faces are drawn before the closer ones.
      for tmp in sorted(avg_z,key=itemgetter(1),reverse=True):
        face_index = tmp[0]
        f = self.faces[face_index]
        pointlist = [(t[f[0]].x, t[f[0]].y), (t[f[1]].x, t[f[1]].y),
                     (t[f[1]].x, t[f[1]].y), (t[f[2]].x, t[f[2]].y),
                     (t[f[2]].x, t[f[2]].y), (t[f[3]].x, t[f[3]].y),
                     (t[f[3]].x, t[f[3]].y), (t[f[0]].x, t[f[0]].y)]
        pygame.draw.polygon(self.screen,self.colors[face_index],pointlist)

          
      self.angle += 1
      
      pygame.display.flip()

if __name__ == "__main__":

  parser = argparse.ArgumentParser()
  parser.add_argument("--port", default="/dev/m4_com")
  parser.add_argument("--portal", action='store_true')
  parser.add_argument("--msginfo", default="types.json")
  args = parser.parse_args()
  
  if args.portal: conn = "portal"
  else: conn = "serial"
  sp = SerialPacket(portstr=args.port, conn_type=conn)
  ps = PacketServer(args.msginfo)

  Simulation(ps, sp).run()
