#include "bme280.h"
#include "timer.h"

// static BME280_Data_t BME280_Data;
static BME280_Calib_Data_t calib_data;
static BME280_Status_t bme280_status;

void BME280_Write_Reg(uint8_t i2c_addr, uint8_t reg, uint8_t conf)
{
    I2C_Start();
    I2C_SendAddress(i2c_addr, BME280_WRITE);
    I2C_WriteByte(reg);
    I2C_WriteByte(conf);
    I2C_Stop();
}

uint8_t BME280_Read_Reg(uint8_t address, uint8_t reg)
{
    uint8_t data;
    I2C_Start();
    I2C_SendAddress(address, 0);
    I2C_WriteByte(reg);
    I2C_Start();
    I2C_SendAddress(address, 1);
    while (!(I2C1->SR1 & I2C_SR1_RXNE))
        ;
    data = I2C1->DR;

    I2C_Stop();
    return data;
}

void BME280_Read_Buffer(uint8_t i2c_addr, uint8_t reg, uint8_t *buffer, uint8_t len)
{
    I2C_ACK_Enable();
    I2C_Start();
    I2C_SendAddress(i2c_addr, BME280_WRITE);
    I2C_WriteByte(reg);
    I2C_Start();
    I2C_SendAddress(i2c_addr, BME280_READ);
    while (len--)
    {
        *buffer++ = I2C_ReadByte();
        if (len == 1)
        {
            I2C_ACK_Disable();
            I2C_Stop();
        }
    }
}

void BME280_Temp_Pressure_Data_Perser(uint8_t *buffer)
{
    /**
     * @brief Formatting and storing Temperature Calibration Data
     */
    calib_data.dig_t1 = (uint16_t)BME280_CONCAT_BYTES(buffer[1], buffer[0]);
    calib_data.dig_t2 = (int16_t)BME280_CONCAT_BYTES(buffer[3], buffer[2]);
    calib_data.dig_t3 = (int16_t)BME280_CONCAT_BYTES(buffer[5], buffer[4]);

    /**
     * @brief Formatting and storing Pressure Calibration Data
     */
    calib_data.dig_p1 = (uint16_t)BME280_CONCAT_BYTES(buffer[7], buffer[6]);
    calib_data.dig_p2 = (int16_t)BME280_CONCAT_BYTES(buffer[9], buffer[8]);
    calib_data.dig_p3 = (int16_t)BME280_CONCAT_BYTES(buffer[11], buffer[10]);
    calib_data.dig_p4 = (int16_t)BME280_CONCAT_BYTES(buffer[13], buffer[12]);
    calib_data.dig_p5 = (int16_t)BME280_CONCAT_BYTES(buffer[15], buffer[14]);
    calib_data.dig_p6 = (int16_t)BME280_CONCAT_BYTES(buffer[17], buffer[16]);
    calib_data.dig_p7 = (int16_t)BME280_CONCAT_BYTES(buffer[19], buffer[18]);
    calib_data.dig_p8 = (int16_t)BME280_CONCAT_BYTES(buffer[21], buffer[20]);
    calib_data.dig_p9 = (int16_t)BME280_CONCAT_BYTES(buffer[23], buffer[22]);
    calib_data.dig_h1 = buffer[25];
}

static void BME280_Hum_Data_Perser(uint8_t *buffer)
{
    calib_data.dig_h2 = (int16_t)BME280_CONCAT_BYTES(buffer[1], buffer[0]);
    calib_data.dig_h3 = buffer[2];
    calib_data.dig_h4 = (int16_t)((buffer[3] << 4) | (0x0F & buffer[4]));
    calib_data.dig_h5 = (int16_t)((buffer[5] << 4) | (buffer[4] >> 4));
    calib_data.dig_h6 = (int8_t)buffer[6];
}

void BME280_Get_Calib_Data()
{
    uint8_t buffer[28] = {0};
    BME280_Read_Buffer(BME280_ADDRESS, BME280_CALIB_DATA_REG_SEC_ONE, buffer, BME280_PRESSURE_TEMP_CALIB_DATA_LEN);
    BME280_Temp_Pressure_Data_Perser(buffer);
    BME280_Read_Buffer(BME280_ADDRESS, BME280_CALIB_DATA_REG_SEC_TWO, buffer, BME280_HUM_CALIB_DATA_LEN);
    BME280_Hum_Data_Perser(buffer);
    bme280_status.calib_data_fetched = 1;
}

static int32_t compensate_temperature(uint32_t uncomp_data)
{
    int32_t var1;
    int32_t var2;
    int32_t temperature;
    int32_t temperature_min = -4000;
    int32_t temperature_max = 8500;

    var1 = (int32_t)((uncomp_data / 8) - ((int32_t)calib_data.dig_t1 * 2));
    var1 = (var1 * ((int32_t)calib_data.dig_t2)) / 2048;
    var2 = (int32_t)((uncomp_data / 16) - ((int32_t)calib_data.dig_t1));
    var2 = (((var2 * var2) / 4096) * ((int32_t)calib_data.dig_t3)) / 16384;
    calib_data.t_fine = var1 + var2;
    temperature = (calib_data.t_fine * 5 + 128) / 256;

    if (temperature < temperature_min)
    {
        temperature = temperature_min;
    }
    else if (temperature > temperature_max)
    {
        temperature = temperature_max;
    }

    return temperature;
}

double BME280_ReadTemperature()
{
    DelayMS(1000);
    uint8_t buffer[6] = {0};
    BME280_Read_Buffer(BME280_ADDRESS, 0xF7, buffer, sizeof(buffer));
    uint32_t uncomp_temp = (uint32_t)((buffer[3] << 12) | (buffer[4] << 4) | (buffer[5] >> 4));
    int32_t comp_temp = compensate_temperature(uncomp_temp);
    return comp_temp / 100.f;
}

void BME280_Init()
{
    BME280_Write_Reg(BME280_ADDRESS, BME280_RESET_REG, 0xB6);
    DelayMS(10);
    BME280_Write_Reg(BME280_ADDRESS, BME280_CTRL_HUM_REG, BME280_CTRL_HUM_OSRSH8);
    uint8_t reg = BME280_Read_Reg(BME280_ADDRESS, BME280_CTRL_HUM_REG);
    kprintf("Value of HUM CTRL Reg: 0x%x\n", reg);
    BME280_Write_Reg(BME280_ADDRESS, BME280_CTRL_MEAS_REG, BME280_CTRL_MEAS_MODE_NORMAL | BME280_CTRL_MEAS_OSRST8);
    reg = BME280_Read_Reg(BME280_ADDRESS, BME280_CTRL_MEAS_REG);
    kprintf("Value of MEAS CTRL Reg: 0x%x\n", reg);
    BME280_Write_Reg(BME280_ADDRESS, BMB280_CONFIG_REG, BME280_CONFIG_FILTER_COE16);
    reg = BME280_Read_Reg(BME280_ADDRESS, BMB280_CONFIG_REG);
    kprintf("Value of Config Reg: 0x%x\n", reg);

    if (!bme280_status.calib_data_fetched)
    {
        BME280_Get_Calib_Data();
    }
}

bool BME280_isMeasuring()
{
    return (BME280_Read_Reg(BME280_ADDRESS, BME280_STATUS_REG) & (1 << 3));
}

// t_fine carries fine temperature as global value
