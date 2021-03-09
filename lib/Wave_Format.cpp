#include "Wave_Format.h"


/***************/
/* Constructor */
/***************/
Wave_Format::Wave_Format() :
    /* Number of channels set to "Mono" by default" --------------------------*/
    NumChannels = CHANNEL_MONO;

    /* Sample rate set to 44 100 kHz by default" -----------------------------*/
    SampleRate = SAMPLE_RATE_44100;

    /* Number of bits per sample set to 16 by default" -----------------------*/
    BitsPerSample = BITS_PER_SAMPLE_16;
{

}
