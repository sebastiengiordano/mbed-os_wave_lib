#ifndef WAVE_FORMAT_H
#define WAVE_FORMAT_H

#include "mbed.h"

#include <ctype.h>


class Wave_Format{

    public:
        uint16_t number_of_channels;
        uint32_t sample_rate;
        uint16_t bits_per_sample;


    private:


    protected:
};

#endif