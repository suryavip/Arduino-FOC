#ifndef MAGNETICSENSORSPI_LIB_H
#define MAGNETICSENSORSPI_LIB_H

#include "Arduino.h"
#include <SPI.h>
#include "../common/base_classes/Sensor.h"
#include "../common/foc_utils.h"
#include "../common/time_utils.h"

#define DEF_ANGLE_REGISTAR 0x3FFF

struct MagneticSensorSPIConfig_s  {
  int spi_mode;
  long clock_speed;
  int bit_resolution;
  int angle_register;
  int data_start_bit;
  int command_rw_bit;
  int command_parity_bit;
};
// typical configuration structures
extern MagneticSensorSPIConfig_s AS5147_SPI, MA730_SPI;

class MagneticSensorSPI: public Sensor{
 public:
    /**
     *  MagneticSensorSPI class constructor
     * @param cs  SPI chip select pin 
     * @param bit_resolution   sensor resolution bit number
     * @param angle_register  (optional) angle read register - default 0x3FFF
     */
    MagneticSensorSPI(int cs, float bit_resolution, int angle_register = 0);
    /**
     *  MagneticSensorSPI class constructor
     * @param config   SPI config
     * @param cs  SPI chip select pin
     */
    MagneticSensorSPI(MagneticSensorSPIConfig_s config, int cs);

    /** sensor initialise pins */
    void init(SPIClass* _spi = &SPI);

    // implementation of abstract functions of the Sensor class
    /** get current angle (rad) */
    float getAngle() override;
    /** get current angular velocity (rad/s) **/
    float getVelocity() override;
    /**
     *  set current angle as zero angle 
     * return the angle [rad] difference
     */
    float initRelativeZero() override;
    /**
     *  set absolute zero angle as zero angle
     * return the angle [rad] difference
     */
    float initAbsoluteZero() override;
    /** returns 1 because it is the absolute sensor */
    int hasAbsoluteZero() override;
    /** returns 0  maning it doesn't need search for absolute zero */

    int needsAbsoluteZeroSearch() override;

    // returns the spi mode (phase/polarity of read/writes) i.e one of SPI_MODE0 | SPI_MODE1 | SPI_MODE2 | SPI_MODE3
    int spi_mode;
    
    /* returns the speed of the SPI clock signal */
    long clock_speed;
    

  private:
    float cpr; //!< Maximum range of the magnetic sensor
    // spi variables
    int angle_register; //!< SPI angle register to read
    int chip_select_pin; //!< SPI chip select pin
	  SPISettings settings; //!< SPI settings variable
    // spi functions
    /** Stop SPI communication */
    void close(); 
    /** Read one SPI register value */
    word read(word angle_register);
    /** Calculate parity value  */
    byte spiCalcEvenParity(word value);


    word zero_offset; //!< user defined zero offset
    /**
     * Function getting current angle register value
     * it uses angle_register variable
     */
    int getRawCount();

    // total angle tracking variables
    float full_rotation_offset; //!<number of full rotations made
    float angle_data_prev; //!< angle in previous position calculation step

    // velocity calculation variables
    float angle_prev; //!< angle in previous velocity calculation step
    long velocity_calc_timestamp; //!< last velocity calculation timestamp
    
    int bit_resolution; //!< the number of bites of angle data
    int command_parity_bit; //!< the bit where parity flag is stored in command
    int command_rw_bit; //!< the bit where read/write flag is stored in command
    int data_start_bit; //!< the the position of first bit

    SPIClass* spi;
};


#endif