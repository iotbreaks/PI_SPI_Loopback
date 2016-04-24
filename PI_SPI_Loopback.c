//#########################################################
//## PI_SPI_Loopback.c 
//## Created by Kenny from www.iotbreaks.vn, April 24, 2016.
//## Released into the public domain.
//## Tutorial: 
//#########################################################
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

static int initializePiAsMaster();
static uint8_t* transceiver(int fileDescriptor, uint8_t sendingBytes[], uint8_t sendingByteLen);

static uint32_t speed = 115200;

int main(int argc, char *argv[]) {
	int fileDescriptor = initializePiAsMaster();
	if (fileDescriptor == 0) return 0;
	unsigned char greeting[] = {'H','e','l','l','o',' ','f','r','o','m',' ','w','w','w','.','i','o','t','b','r','e','a','k','s','.','v','n'};
	unsigned char result;
	uint8_t sendingByteLen = 27;
	uint8_t* receivedBytes ;
	receivedBytes = transceiver(fileDescriptor, greeting, sendingByteLen);
	close(fileDescriptor);
	printf("finish\n");
	return 1;
}

// Return file descriptor of SPI
static int initializePiAsMaster(){ 
	const char* device = "/dev/spidev0.0";
	int fileDescriptor = open("/dev/spidev0.0", O_RDWR);
	if (fileDescriptor < 0){
		printf("can't open device\n");
		return 0;
	} else {
		printf("open device spidev0.0 successfully with file description = %d\n", fileDescriptor);
	}
	uint8_t mode =   ((uint8_t)((uint8_t)0 & SPI_READY))
				   | ((uint8_t)((uint8_t)0 & SPI_NO_CS)) 
				   | ((uint8_t)((uint8_t)1 & SPI_LOOP)) 
				   | ((uint8_t)((uint8_t)0 & SPI_3WIRE))
				   | ((uint8_t)((uint8_t)0 & SPI_LSB_FIRST))
				   | ((uint8_t)((uint8_t)0 & SPI_CS_HIGH))
				   | ((uint8_t)((uint8_t)0 & SPI_CPOL)) 
				   | ((uint8_t)((uint8_t)0 & SPI_CPHA)); 

	mode |= SPI_LOOP;
	int ret;
	ret = ioctl(fileDescriptor, SPI_IOC_WR_MODE, &mode);
	ret = ioctl(fileDescriptor, SPI_IOC_RD_MODE, &mode);
	
	uint8_t bitPerWord = 8;
	ret = ioctl(fileDescriptor, SPI_IOC_WR_BITS_PER_WORD, &bitPerWord); //8 bits/word
	ret = ioctl(fileDescriptor, SPI_IOC_RD_BITS_PER_WORD, &bitPerWord);
	
	ret = ioctl(fileDescriptor, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	ret = ioctl(fileDescriptor, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

	return fileDescriptor;
}


// Send and Receive SPI
static uint8_t* transceiver(int fileDescriptor, uint8_t sendingBytes[],uint8_t sendingByteLen)
{
	static uint8_t receivedBytes[255] = {0, }; 
	struct spi_ioc_transfer transceiverMessage = {
		.tx_buf = (unsigned long)sendingBytes,
		.rx_buf = (unsigned long)receivedBytes,
		.len = sendingByteLen,
		.delay_usecs = 1000,
		.speed_hz = speed,
		.bits_per_word = 8,
	};

	int ret = ioctl(fileDescriptor, SPI_IOC_MESSAGE(1), &transceiverMessage);
	if (ret < 1)
		printf("can't send spi message");

	printf("Sending Bytes  = ");
	int i=0;
	for (i = 0; i < sendingByteLen; i++) {
		printf("%c ", sendingBytes[i]);
	}
	puts("");
	
	printf("Received Bytes = ");
	for (i = 0; i < sendingByteLen; i++) {
		printf("%c ", receivedBytes[i]);
	}
	puts("");

	return receivedBytes;
}
