#ifndef _INC_BME280_H
#define _INC_BME280_H
#include <stdbool.h>
#include "hardware/i2c.h"
#include "debug.h"

#define BME280_CONCAT_BYTES(byte_one, byte_two) ((byte_one << 8) | byte_two)

#define BME280_ADDRESS 0x76
#define BME280_READ 0x1U
#define BME280_WRITE 0x0U
#define BME280_PRESSURE_TEMP_CALIB_DATA_LEN 26U
#define BME280_HUM_CALIB_DATA_LEN 0x7U

/*Here Goes all the necessary basic Register address of BME280*/
#define BME280_WHO_AM_I_REG 0xD0 // Also Known as Chip ID reg
#define BME280_RESET_REG 0xE0

/**
 * @brief Here goes all the Configuration Registers of BME280
 */
#define BMB280_CONFIG_REG 0xF5
#define BME280_CTRL_MEAS_REG 0xF4
#define BME280_STATUS_REG 0xF3
#define BME280_CTRL_HUM_REG 0xF2

/**
 * @brief Here goes all the Register Holds the calibration values
 */
#define BME280_CALIB_DATA_REG_SEC_ONE 0x88
#define BME280_CALIB_DATA_REG_SEC_TWO 0xE1

/**
 * @brief Here goes all the bit location of inside configuration registers
 */
#define BME280_SPI3W_EN (0x1U << 0U)

/**
 * @brief Here Goes All the things related to the mode in ctrl_meas(control measurement) register
 */
#define BME280_CTRL_MEAS_MODE_Pos 0x0U
#define BME280_CTRL_MEAS_MODE_Msk 0x3U
#define BME280_CTRL_MEAS_MODE_SLEEP 0x0U
#define BME280_CTRL_MEAS_MODE_FORCED 0x1U
#define BME280_CTRL_MEAS_MODE_NORMAL 0x3U

/**
 * @brief Here Goes All the things related to the OSRS_T(Temperature Oversampling) in ctrl_meas(control measurement) register
 */
#define BME280_CTRL_MEAS_OSRST_Pos 0x05U
#define BME280_CTRL_MEAS_OSRST_MSK (0x7U << BME280_CTRL_MEAS_OSRST_Pos)
#define BME280_CTRL_MEAS_OSRST1 (0x01U << BME280_CTRL_MEAS_OSRST_Pos)
#define BME280_CTRL_MEAS_OSRST2 (0x02U << BME280_CTRL_MEAS_OSRST_Pos)
#define BME280_CTRL_MEAS_OSRST4 (0x03U << BME280_CTRL_MEAS_OSRST_Pos)
#define BME280_CTRL_MEAS_OSRST8 (0x04U << BME280_CTRL_MEAS_OSRST_Pos)
#define BME280_CTRL_MEAS_OSRST16 (0x05U << BME280_CTRL_MEAS_OSRST_Pos)

/**
 * @brief Here Goes All the things related to the OSRS_P(Pressure Oversampling) in ctrl_meas(control measurement) register
 */
#define BME280_CTRL_MEAS_OSRSP_Pos 0x2U
#define BME280_CTRL_MEAS_OSRSP_Msk (0x7U << BME280_CTRL_MEAS_OSRSP_Pos)
#define BME280_CTRL_MEAS_OSRSP1 (0x01U << BME280_CTRL_MEAS_OSRSP_Pos)
#define BME280_CTRL_MEAS_OSRSP2 (0x02U << BME280_CTRL_MEAS_OSRSP_Pos)
#define BME280_CTRL_MEAS_OSRSP4 (0x03U << BME280_CTRL_MEAS_OSRSP_Pos)
#define BME280_CTRL_MEAS_OSRSP8 (0x04U << BME280_CTRL_MEAS_OSRSP_Pos)
#define BME280_CTRL_MEAS_OSRSP16 (0x05U << BME280_CTRL_MEAS_OSRSP_Pos)

/**
 * @brief Here Goes All the things related to the OSRS_H(Humidity Oversampling) in ctrl_hum(control humidity) register
 */
#define BME280_CTRL_HUM_OSRSH_Pos 0x0U
#define BME280_CTRL_HUM_OSRSH_Msk (0x7U << BME280_CTRL_HUM_OSRSH_Pos)
#define BME280_CTRL_HUM_OSRSH1 (0x01U << BME280_CTRL_HUM_OSRSH_Pos)
#define BME280_CTRL_HUM_OSRSH2 (0x02U << BME280_CTRL_HUM_OSRSH_Pos)
#define BME280_CTRL_HUM_OSRSH4 (0x03U << BME280_CTRL_HUM_OSRSH_Pos)
#define BME280_CTRL_HUM_OSRSH8 (0x04U << BME280_CTRL_HUM_OSRSH_Pos)
#define BME280_CTRL_HUM_OSRSH16 (0x05U << BME280_CTRL_HUM_OSRSH_Pos)

/**
 * @brief Here goes all the things related to Filter options in Config Register
 */
#define BME280_CONFIG_FILER_Pos 0x2U
#define BME280_CONFIG_FILER_Msk (0x7U << BME280_CONFIG_FILER_Pos)
#define BME280_CONFIG_FILTER_OFF 0x0U
#define BME280_CONFIG_FILTER_COE2 (0x1U << BME280_CONFIG_FILER_Pos)
#define BME280_CONFIG_FILTER_COE4 (0x2U << BME280_CONFIG_FILER_Pos)
#define BME280_CONFIG_FILTER_COE8 (0x3U << BME280_CONFIG_FILER_Pos)
#define BME280_CONFIG_FILTER_COE16 (0x4U << BME280_CONFIG_FILER_Pos)

/**
 * @brief Here Goes all the Things related to T_SB (standby Time) of config register
 */
#define BME280_CONFIG_TSB_Pos 0x5U
#define BME280_CONFIG_TSB_Msk (0x7U << BME280_CONFIG_TSB_Pos)
#define BME280_CONFIG_TSB0 (0x0U << BME280_CONFIG_TSB_Pos) // Standby Time 0.5 MS
#define BME280_CONFIG_TSB1 (0x1U << BME280_CONFIG_TSB_Pos) // Standby Time 62.5 MS
#define BME280_CONFIG_TSB2 (0x2U << BME280_CONFIG_TSB_Pos) // Standby Time 125 MS
#define BME280_CONFIG_TSB3 (0x3U << BME280_CONFIG_TSB_Pos) // Standby Time 250 MS
#define BME280_CONFIG_TSB4 (0x4U << BME280_CONFIG_TSB_Pos) // Standby Time 500 MS
#define BME280_CONFIG_TSB5 (0x5U << BME280_CONFIG_TSB_Pos) // Standby Time 1000 MS
#define BME280_CONFIG_TSB6 (0x6U << BME280_CONFIG_TSB_Pos) // Standby Time 10 MS
#define BME280_CONFIG_TSB7 (0x7U << BME280_CONFIG_TSB_Pos) // Standby Time 20 MS

/**
 * @brief Here Goes all the Status of Status Registers
 */
#define BME280_STATUS_MEASURING (0x1U << 3U)
#define BME280_STATUS_IM_UPDATE (0x1U << 0)

/**
 * @brief Here goes all the necessary Struct for BME280
 */
typedef struct
{
    double temperature;
    double humidity;
    double pressure;

} BME280_Data_t;

typedef struct
{
    uint16_t dig_t1;

    /*! Calibration coefficient for the temperature sensor */
    int16_t dig_t2;

    /*! Calibration coefficient for the temperature sensor */
    int16_t dig_t3;

    /*! Calibration coefficient for the pressure sensor */
    uint16_t dig_p1;

    /*! Calibration coefficient for the pressure sensor */
    int16_t dig_p2;

    /*! Calibration coefficient for the pressure sensor */
    int16_t dig_p3;

    /*! Calibration coefficient for the pressure sensor */
    int16_t dig_p4;

    /*! Calibration coefficient for the pressure sensor */
    int16_t dig_p5;

    /*! Calibration coefficient for the pressure sensor */
    int16_t dig_p6;

    /*! Calibration coefficient for the pressure sensor */
    int16_t dig_p7;

    /*! Calibration coefficient for the pressure sensor */
    int16_t dig_p8;

    /*! Calibration coefficient for the pressure sensor */
    int16_t dig_p9;

    /*! Calibration coefficient for the humidity sensor */
    uint8_t dig_h1;

    /*! Calibration coefficient for the humidity sensor */
    int16_t dig_h2;

    /*! Calibration coefficient for the humidity sensor */
    uint8_t dig_h3;

    /*! Calibration coefficient for the humidity sensor */
    int16_t dig_h4;

    /*! Calibration coefficient for the humidity sensor */
    int16_t dig_h5;

    /*! Calibration coefficient for the humidity sensor */
    int8_t dig_h6;

    /*! Variable to store the intermediate temperature coefficient */
    int32_t t_fine;
} BME280_Calib_Data_t;

typedef struct
{
    uint8_t calib_data_fetched;
    /* data */
} BME280_Status_t;

/*Here Goes all the function for BME280*/
void BME280_Init();
void BME280_Get_Calib_Data();
void BME280_Write_Reg(uint8_t i2c_addr, uint8_t reg, uint8_t conf);
uint8_t BME280_Read_Reg(uint8_t i2c_addr, uint8_t reg);
void BME280_Read_Buffer(uint8_t i2c_addr, uint8_t reg, uint8_t *buffer, uint8_t len);
double BME280_ReadTemperature();
bool BME280_isMeasuring();

#endif