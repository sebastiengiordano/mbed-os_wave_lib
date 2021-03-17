#ifndef WAVE_H
#define WAVE_H

// #include "mbed.h"

#include <cstdint>
#include <stdio.h>

#include "Wave_Format.h"
#include "Wave_parameters.h"


class Wave{

    public:
        Wave();
        int32_t     open(const char* file_path, Wave_Format wave_format);
        int32_t     write(const char* samples, uint8_t number_of_sample);
        
        int32_t     rewrite_header_and_close(void);
        int32_t     file_size(void);




    private:
        void        _header_init(Wave_Format wave_format);
        int32_t     _header_update(void);
        int32_t     _copy_header(void);

        // Wave file fields
        uint32_t    _file_type_bloc_id;
        uint32_t    _file_size;
        uint32_t    _file_format_id;
        uint32_t    _format_bloc_id;
        uint32_t    _format_bloc_size;
        uint16_t    _audio_format;
        uint16_t    _channel_number;
        uint32_t    _sample_rate;
        uint32_t    _byte_per_second;
        uint16_t    _byte_per_bloc;
        uint16_t    _bits_per_sample;
        uint32_t    _data_bloc_id;
        uint32_t    _data_size;

        // File handler
        FILE*       _file = NULL;
        const char* _file_path;

    protected:
        int32_t     _file_open(const char* file_path);
        int32_t     _file_close(void);
        int32_t     _write_in_file(const char* data, uint32_t size_of_data);
};

#endif
