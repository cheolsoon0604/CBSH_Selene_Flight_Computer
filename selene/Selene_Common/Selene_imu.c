// --------------------------------------------------------
// MPU6050_read
//
// This is a common function to read multiple bytes
// from an I2C device.
//
// It uses the boolean parameter for Wire.endTransMission()
// to be able to hold or release the I2C-bus.
// This is implemented in Arduino 1.0.1.
//
// Only this function is used to read.
// There is no function for a single byte.
//

int MPU6050_read(int start, uint8_t *buffer, int size)
{
    int i, n, error;

    Wire.beginTransmission(MPU6050_I2C_ADDRESS);
    n = Wire.write(start);
    if (n != 1)
        return (-10);

    n = Wire.endTransmission(false);    // hold the I2C-bus
    if (n != 0)
        return (n);

    // Third parameter is true: relase I2C-bus after data is read.
    Wire.requestFrom(MPU6050_I2C_ADDRESS, size, true);
    i = 0;
    while(Wire.available() && i<size)
    {
        buffer[i++]=Wire.read();
    }
    if ( i != size)
        return (-11);

    return (0);  // return : no error
}


// --------------------------------------------------------
// MPU6050_write
//
// This is a common function to write multiple bytes to an I2C device.
//
// If only a single register is written,
// use the function MPU_6050_write_reg().
//
// Parameters:
//   start : Start address, use a define for the register
//   pData : A pointer to the data to write.
//   size  : The number of bytes to write.
//
// If only a single register is written, a pointer
// to the data has to be used, and the size is
// a single byte:
//   int data = 0;        // the data to write
//   MPU6050_write (MPU6050_PWR_MGMT_1, &c, 1);
//
int MPU6050_write(int start, const uint8_t *pData, int size)
{
    int n, error;

    Wire.beginTransmission(MPU6050_I2C_ADDRESS);
    n = Wire.write(start);        // write the start address
    if (n != 1)
        return (-20);

    n = Wire.write(pData, size);  // write data bytes
    if (n != size)
        return (-21);

    error = Wire.endTransmission(true); // release the I2C-bus
    if (error != 0)
        return (error);

    return (0);         // return : no error
}

// --------------------------------------------------------
// MPU6050_write_reg
//
// An extra function to write a single register.
// It is just a wrapper around the MPU_6050_write()
// function, and it is only a convenient function
// to make it easier to write a single register.
//
int MPU6050_write_reg(int reg, uint8_t data)
{
    int error;

    error = MPU6050_write(reg, &data, 1);

    return (error);
}

// Function to read and process data from MPU-6050
int readMPU6050Data(accel_t_gyro_union *data) {

    int error;
    // Read the raw values.
    error = MPU6050_read(MPU6050_ACCEL_XOUT_H, (uint8_t *)data, sizeof(*data));

    // Perform byte swapping
    SWAP(data->reg.x_accel_h, data->reg.x_accel_l);
    SWAP(data->reg.y_accel_h, data->reg.y_accel_l);
    SWAP(data->reg.z_accel_h, data->reg.z_accel_l);
    SWAP(data->reg.t_h, data->reg.t_l);
    SWAP(data->reg.x_gyro_h, data->reg.x_gyro_l);
    SWAP(data->reg.y_gyro_h, data->reg.y_gyro_l);
    SWAP(data->reg.z_gyro_h, data->reg.z_gyro_l);

    return error;
}

void setup_IMU() { MPU6050_write_reg (MPU6050_PWR_MGMT_1, 0); }

void IMU_OP() {
    accel_t_gyro_union accel_t_gyro;
    error = readMPU6050Data(&accel_t_gyro);

    AcX = accel_t_gyro.value.x_accel;
    AcY = accel_t_gyro.value.y_accel;
    AcZ = accel_t_gyro.value.z_accel;
    temperature = ( (double) accel_t_gyro.value.temperature + 12412.0 ) / 340.0;
    GyX = accel_t_gyro.value.x_gyro;
    GyY = accel_t_gyro.value.y_gyro;
    GyZ = accel_t_gyro.value.z_gyro;
}