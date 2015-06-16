/* UKF Implementation Over Quaternions and Angular Rates
 * Jimmy Paulos, June 2013
 * Closely follows the algorithm of Kraft 2003 with resolved errata.
 */

#include "ukf_python.h"
#include "ukf_helper.h"
#include <stdint.h>
//#include <stdio.h>

/// Function wrapper provides ctypes-friendly non-object oriented interaction.
void Update_ctypes( float z_meas[6],
                    float dt,
                    float Q[6][6],
                    float R[6][6],
                    float P[6][6],
                    float q[4],
                    float w[3]) {
                    
  // printf("%f\t%f\t%f\t%f\t%f\t%f\n", z_meas[0], z_meas[1], z_meas[2], z_meas[3], z_meas[4], z_meas[5]); // looks bad
  // printf("%f\t%f\t%f\t\n", w[0], w[1], w[2]);
  // printf("%f\t%f\t%f\t%f\t\n", q[0], q[1], q[2], q[3]);

  // convert arrays to Eigen types
  Vector6f new_z_meas;
  for(uint8_t i=0; i<6; i++)
    new_z_meas(i) = z_meas[i];

  Matrix6f new_Q;
  Matrix6f new_R;
  Matrix6f new_P;
  for(uint8_t i=0; i<6; i++) {
    for(uint8_t j=0; j<6; j++) {
      new_Q(i,j) = Q[i][j];
      new_R(i,j) = R[i][j];
      new_P(i,j) = P[i][j];
    }
  }

  Quaternionf new_q = Quaternionf(q[0], q[1], q[2], q[3]);

  Vector3f new_w;
  new_w(0) = w[0];
  new_w(1) = w[1];
  new_w(2) = w[2];

  // call update
  //printf("%f\t%f\t%f\n", new_z_meas(3), new_z_meas(4), new_z_meas(5)); // looks bad
  UpdateHelper(new_z_meas, dt, new_Q, new_R, new_P, new_q, new_w);

  // update arrays with Eigen results
  for(uint8_t i=0; i<6; i++)
    for(uint8_t j=0; j<6; j++)
      P[i][j] = new_P(i,j);
  
  q[0] = new_q.w();
  q[1] = new_q.x();
  q[2] = new_q.y();
  q[3] = new_q.z();
  
  w[0] = new_w(0);
  w[1] = new_w(1);
  w[2] = new_w(2);
}

/// Function wrapper provides ctypes-friendly non-object oriented interaction.
void UpdateRaw_ctypes(  float z_meas_ax, float z_meas_ay, float z_meas_az, float z_meas_wx, float z_meas_wy, float z_meas_wz,
                        float dt,
                        float Q[6][6],
                        float R[6][6],
                        float P[6][6],
                        float q[4],
                        float w[3]) {
  float z_meas[6];
  z_meas[0] = z_meas_ax;
  z_meas[1] = z_meas_ay;
  z_meas[2] = z_meas_az;
  z_meas[3] = z_meas_wx;
  z_meas[4] = z_meas_wy;
  z_meas[5] = z_meas_wz;
  Update_ctypes(z_meas, dt, Q, R, P, q, w);
}


void StateMeanCov_ctypes(float q[4][N_SIGMA_PTS],
                         float w[3][N_SIGMA_PTS],
                         float q_est[4],
                         float q_res[4],
                         float w_res[3],
                         float P[6][6],
                         float e[6][N_SIGMA_PTS]) {
                         
  // prepare inputs
  Quaternionf new_q[N_SIGMA_PTS];
  for(uint8_t i=0; i<N_SIGMA_PTS; i++)
    new_q[i] = Quaternionf(q[0][i], q[1][i], q[2][i], q[3][i]);

  Matrix<float,3,N_SIGMA_PTS> new_w;
  for(uint8_t i=0; i<3; i++)
    for(uint8_t j=0; j<N_SIGMA_PTS; j++)
      new_w(i,j) = w[i][j];

  Quaternionf new_q_est = Quaternionf(q_est[0], q_est[1], q_est[2], q_est[3]);
  
  // run function
  Quaternionf q1;
  Vector3f w1;
  Matrix6f P1;
  Matrix<float,6,N_SIGMA_PTS> W1;
  StateMeanCov( new_q, new_w, new_q_est,  // inputs
                q1, w1, P1, W1);          // results
                     
  // format outputs
  q_res[0] = q1.w();
  q_res[1] = q1.x();
  q_res[2] = q1.y();
  q_res[3] = q1.z();
  
  w_res[0] = w1(0);
  w_res[1] = w1(1);
  w_res[2] = w1(2);
  
  for(uint8_t i=0; i<6; i++)
    for(uint8_t j=0; j<6; j++)
      P[i][j] = P1(i,j);
  
  for(uint8_t i=0; i<6; i++)
    for(uint8_t j=0; j<N_SIGMA_PTS; j++)
      e[i][j] = W1(i,j);
  
}

void SigmaPoints_ctypes(float P[6][6],
                        float Wq[4][N_SIGMA_PTS],
                        float Ww[3][N_SIGMA_PTS]) {
  // prepare inputs             
  Matrix6f eig_P;
  for(uint8_t i=0; i<6; i++)
    for(uint8_t j=0; j<6; j++)
      eig_P(i,j) = P[i][j];

  // run function
  Quaternionf eig_Wq[N_SIGMA_PTS];
  Matrix<float,3,N_SIGMA_PTS> eig_Ww;
  SigmaPoints(eig_P, eig_Wq, eig_Ww);
  
  // format outputs
  for(uint8_t i=0; i<3; i++)
    for(uint8_t j=0; j<N_SIGMA_PTS; j++)
      Ww[i][j] = eig_Ww(i,j);
  
  for(uint8_t i=0; i<N_SIGMA_PTS; i++) {
    Wq[0][i] = eig_Wq[i].w();
    Wq[1][i] = eig_Wq[i].x();
    Wq[2][i] = eig_Wq[i].y();
    Wq[3][i] = eig_Wq[i].z();
  }

}

void ExpectedAccelleration_ctypes(float q[4], float z_meas_exp[3]) {
  Quaternionf new_q = Quaternionf(q[0], q[1], q[2], q[3]);
  Vector3f new_z_meas_exp;
  ExpectedAccelleration(new_q, new_z_meas_exp);
  z_meas_exp[0] = new_z_meas_exp(0);
  z_meas_exp[1] = new_z_meas_exp(1);
  z_meas_exp[2] = new_z_meas_exp(2);
}
