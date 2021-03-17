#include "Wave_Format.h"
#include "Wave_parameters.h"

/***************/
/* Constructor */
/***************/
Wave_Format::Wave_Format() :
    /* Number of channels set to "Mono" by default" --------------------------*/
    number_of_channels (CHANNEL_MONO),

    /* Sample rate set to 44 100 kHz by default" -----------------------------*/
    sample_rate (SAMPLE_RATE_44100),

    /* Number of bits per sample set to 16 by default" -----------------------*/
    bits_per_sample (BITS_PER_SAMPLE_16)
{

}
