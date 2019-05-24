// ROM and blob calls without official headers available

#ifdef __cplusplus
extern "C" {
#endif

// ROM

extern void rom_i2c_writeReg_Mask(int, int, int, int, int, int);
extern int rom_i2c_readReg_Mask(int, int, int, int, int);

extern int uart_baudrate_detect(int, int);

extern void ets_delay_us(uint32_t us);

#ifdef __cplusplus
};
#endif
