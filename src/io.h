/* Yet unnamed clonewheel organ for STM32F4 Discovery, working name "Syna"

Copyright (C) 2016 Jani Frilander

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

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

