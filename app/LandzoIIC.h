#ifndef _LANDZO_IIC_H_
#define _LANDZO_IIC_H_


#define BFEND      PTE11_OUT
#define BFSDA      PTE12_OUT
#define BFCLK      PTE10_OUT
#define BFSDAI     PTE12_IN

#define IICEorr    (0)
#define IICOK      (1)

void BFdelay_1us(uint8_t us) ;
void BFDly_ms(uint8_t ms) ;
void iic_start(void) ;
void iic_stop(void) ;
void slave_ACK(void) ;
void slave_NOACK(void) ;
uint8_t check_ACK(void);
void IICSendByte(uint8_t ch) ;
uint8_t IICreceiveByte(void) ;
uint8_t writeNbyte(uint8_t slave_add, uint8_t *slave_data,uint16_t n) ;
uint8_t receiveNbyte(uint8_t slave_add,uint8_t *rece_data, uint16_t n) ;
uint8_t LandzoIICEEROM_INIT(void) ;



#endif /* _Landzo_H_*/