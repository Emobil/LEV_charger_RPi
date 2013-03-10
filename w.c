#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdio.h>
#include <fcntl.h>

#define SLAVE_ADDRESS 0x55

#define I2C_DEV "/dev/i2c-0"
#define I2C_BUFFER_SIZE 10 // Number of Bytes 


unsigned char buffer[I2C_BUFFER_SIZE];

int i2c_read (int slaveaddr, int length, unsigned char *buffer){
	int deviceHandle = open(I2C_DEV,O_RDWR);
	ioctl(deviceHandle, I2C_SLAVE, slaveaddr);
	write(deviceHandle, 0x00, 0);
	read(deviceHandle,buffer,length);	
	close(deviceHandle);
	return 0; 
}

int i2c_write (int slaveaddr, int length, unsigned char *buffer){
	int deviceHandle = open(I2C_DEV,O_WRONLY);
	ioctl(deviceHandle, I2C_SLAVE, slaveaddr);
	write(deviceHandle, buffer, length);
	close(deviceHandle);
	return 0; 
}


/* 
	GPIO nur als su mgl.

	argv[1] -> LEDs am AVR (via I2C)
	argv[2] -> linke LED (GPIO23)
	argv[3] -> rechte LED (GPIO24)

	Vorbereitung via shell als su: 
		echo "23" > /sys/class/gpio/export
		echo "24" > /sys/class/gpio/export
		echo "out" > /sys/class/gpio/gpio23/direction
		echo "out" > /sys/class/gpio/gpio24/direction
*/

int main( int argc, char *argv[] ) {

 int fd;
 char GPIO[3][30] = {"/sys/class/gpio/gpio23/value","/sys/class/gpio/gpio24/value"}; 

 int i =0;
 int res;
 
 float u1, u2, u3, u4;

 if (argc >= 1)                                                  
 {
 	 buffer[2] = atoi(argv[1]);
	 i2c_write(SLAVE_ADDRESS , I2C_BUFFER_SIZE , buffer); 

	 fd = open(GPIO[0], O_WRONLY | O_NOCTTY);
	 res = write(fd,argv[2],1);
	 close (fd);

	 fd = open(GPIO[1], O_WRONLY | O_NOCTTY);
	 res = write(fd,argv[3],1);
	 close (fd);
 }

 while(1){

	i2c_read(SLAVE_ADDRESS , I2C_BUFFER_SIZE , buffer);

//	u1 = ((float)(( buffer[0] << 8 ) | buffer[1]) / 4096.0) ;//* 130000.0 / 1740.0;
//	printf("U_Bat: %.03f V\n",u1);

//	int u11 = ((( buffer[0] << 8 ) | buffer[1]));// * 130000.0 / 1740.0;
//	printf("U_Bat: %u V\n",u11);


//	u2 = (float)((buffer[2] << 8 ) | buffer[3]) / 4096.0;
//	printf("I_Bat: %.03f A\n\n",u2);

	u3 = ((float)(((buffer[4] << 8 ) | buffer[5]) / 4096.0) * (130.0 / 1.74) - 3.8);
	printf(" U_PV: %.03f V\n",u3);

	u4 =  (float)(((buffer[6] << 8 ) | buffer[7]) / ( 4096.0 * 0.02 * (1 + 22000.0 / 4700.0) ) - 1.3);
	printf(" I_PV: %.03f A\n",u4);

//	printf("\n");	
	sleep(1);

}

 return 0;
}
