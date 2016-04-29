#define SAMPLERATE                  48000
#define AUDIO_BUFFER_LENGTH_HALF    256
#define AUDIO_BUFFER_LENGTH         512

#define VOLUME                      80
//#define MANUAL_KEYS                 61
#define TONEWHEELS                  91

void Generate_buffer(uint16_t);
void Organ_noteon(uint8_t);
void Organ_noteoff(uint8_t);
void Init_organ();
