/* UKF Implementation Over Quaternions and Angular Rates
 * Jimmy Paulos, June 2013
 * Closely follows the algorithm of Kraft 2003 with resolved errata.
 */

#ifndef UKF_PYTHON_H
#define	UKF_PYTHON_H

#define N_SIGMA_PTS 12

//#ifdef __cplusplus
extern "C" {
//#endif

void Update_ctypes( float z_meas[6],
                    float dt,
                    float Q[6][6],
                    float R[6][6],
                    float P[6][6],
                    float q[4],
                    float w[3]);

void UpdateRaw_ctypes( float, float, float, float, float, float,
                    float dt,
                    float Q[6][6],
                    float R[6][6],
                    float P[6][6],
                    float q[4],
                    float w[3]);
                    
void StateMeanCov_ctypes(float q[4][N_SIGMA_PTS],
                         float w[3][N_SIGMA_PTS],
                         float q_est[4],
                         float q_res[4],
                         float w_res[3],
                         float P[6][6],
                         float e[6][N_SIGMA_PTS]);

void SigmaPoints_ctypes(float P[6][6],
                        float Wq[4][N_SIGMA_PTS],
                        float Ww[3][N_SIGMA_PTS]);
                        
void ExpectedAccelleration_ctypes(float q[4], float z_meas_exp[3]);
                    
//#ifdef __cplusplus
}
//#endif // __cplusplus

#endif	// UKF_PYTHON_H
