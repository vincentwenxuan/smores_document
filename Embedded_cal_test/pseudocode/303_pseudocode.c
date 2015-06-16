

function main()
{
    setup()
    while(true)
    {
        update_state_from_faces();
        read_messages_from_3000();

        if (STATE_REQUEST)
        {
            sendUPD(303_state)
        }

        if (MOTOR_COMMAND)
        {

        }
    }
}

/*************  VARIABLES  ***************/
typedef struct 303_state {
   int position[4]  // One for each face.  Alternatively we could use different struct elements for each face.
   int velocity[4]
   int acceleration[4]
   int t[4]
   /* declare as many members as desired, but the entire structure size must be known to the compiler. */
};  
/*
This is a global struct in the 303 which describes the state of the 
SMORES module.
*/

/*************  FUNCTION DECLARATIONS ***************/

function setup();
/*
    sets everything up.
*/

function update_state_from_faces();
/*
    Sends four I2c burst read requests to the four face micros.  Updates 303_state with data received.  
*/

function read_messages_from_3000();
/*
    TODO: this might entail a 5ms pause.  We need to look into this.

*/

function sendUPD();
/*
    Uses the 3000 to send some data over UDP.
*/

/*********** END FUNCTION DECLARATIONS  **************/