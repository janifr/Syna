//void Init_adc();
void Display_character_clock(uint8_t);
void Display_command_no_clock(uint8_t);
void Display_character_no_clock(uint8_t);
void Display_data_no_clock(uint16_t);
void Display_clock();
void Init_hid(void);
void Init_misc();
void EVAL_AUDIO_TransferComplete_CallBack(uint32_t, uint32_t);
void EVAL_AUDIO_HalfTransfer_CallBack(uint32_t, uint32_t);
uint16_t EVAL_AUDIO_GetSampleCallBack(void);
uint32_t Codec_TIMEOUT_UserCallback(void);
void USART3_IRQHandler(void);
void DMA1_Stream1_IRQHandler(void);

