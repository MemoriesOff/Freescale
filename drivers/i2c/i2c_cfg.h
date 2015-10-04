/******************** (C) COPYRIGHT 2011 蓝宙电子工作室 ********************
 * 文件名       ：i2c_cfg.h
 * 描述         ：I2C头文件
 *
 * 实验平台     ：
 * 库版本       ：
 * 嵌入系统     ：
 *
 * 作者         ：
 * 淘宝店       ：
**********************************************************************************/
#ifndef _I2C_CFG_H_
#define _I2C_CFG_H_

#define MMA7660_I2C_ADDRESS                         0x4C    //MMA7660 三轴加速度模块的地址




#define i2c_DisableAck(I2Cn)        I2C_C1_REG(I2Cx[I2Cn]) |= I2C_C1_TXAK_MASK

//
#define i2c_RepeatedStart(I2Cn)     I2C_C1_REG(I2Cx[I2Cn]) |= I2C_C1_RSTA_MASK


//启动信号
#define i2c_Start(I2Cn)             I2C_C1_REG(I2Cx[I2Cn]) |= I2C_C1_TX_MASK;\
                                    I2C_C1_REG(I2Cx[I2Cn]) |= I2C_C1_MST_MASK

//暂停信号
#define i2c_Stop(I2Cn)              I2C_C1_REG(I2Cx[I2Cn]) &= ~I2C_C1_MST_MASK;\
                                    I2C_C1_REG(I2Cx[I2Cn]) &= ~I2C_C1_TX_MASK

//进入接收模式(应答)
#define i2c_EnterRxMode(I2Cn)       I2C_C1_REG(I2Cx[I2Cn]) &= ~I2C_C1_TX_MASK;\
                                    I2C_C1_REG(I2Cx[I2Cn]) &= ~I2C_C1_TXAK_MASK
//进入接收模式(不应答)
#define i2c_PutinRxMode(I2Cn)       I2C_C1_REG(I2Cx[I2Cn]) &= ~I2C_C1_TX_MASK

//等待 I2C0_S
#define i2c_Wait(I2Cn)              while(( I2C_S_REG(I2Cx[I2Cn]) & I2C_S_IICIF_MASK)==0) {} \
                                    I2C_S_REG(I2Cx[I2Cn]) |= I2C_S_IICIF_MASK;

//写一个字节
#define i2c_write_byte(I2Cn,data)   I2C_D_REG(I2Cx[I2Cn]) = data


#endif //_I2C_CFG_H_



