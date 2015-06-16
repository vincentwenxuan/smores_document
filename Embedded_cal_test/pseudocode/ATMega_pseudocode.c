

function main()
{
    setup()
    while(true)
    {
        update_state_from_encoders();
        read_messages_from_303();

        // 303 will make an I2c request for state whenever it wants it.
    }
}

/*************  VARIABLES  ***************/
typedef struct face_state {
   int16 position
   int16 velocity
   int16 acceleration
   int16 t // timestamp.  Needed for estimator. 
   /* declare as many members as desired, but the entire structure size must be known to the compiler. */
};  
/*
This is a global struct in the 303 which describes the state of the 
SMORES module.
*/

/*************  FUNCTIONS DECLARATIONS ***************/

function read_messages_from_303();
/* Reads I2C for messages from 303. */


function update_state();
/*  Does everything necessary to update the state of this face */

function read_raw_data_from_ADCs();
/* Reads raw data from both of the ADC's attached to the wiper */

face_state* run_estimator(new_raw_data, motor_commands, old_state, estimator_params);
/* Runs some kind of estimator (maybe kalman filter) to get a new state estimate. 
 TODO: Figure out whether we need motor information for estimation.  If so, this will be more traffic over i2c. 
*/

/*********** END FUNCTION DECLARATIONS  **************/

/********** FUNCTION DEFINITIONS *********/
function update_state();
/*  Does everything necessary to update the state of this face */
{
  raw_data = read_raw_data_from_ADCs();
  [new_state] = run_estimator(raw_data, old_state, motor_commands);
  face_state = new_state; // store in global struct
}

 /******** END FUNCTION DEFINITIONS ******/