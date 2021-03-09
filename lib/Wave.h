#ifndef WAVE_H
#define WAVE_H

// #include "mbed.h"

#include <ctype.h>

#include "Wave_Format.h"


/* Enum*/
typedef enum {
    File_Created,
    File_Open,
    File_Close
} eFileStatus;


class Wave{

    public:
        Wave();


    private:
        void _header_init(void);
        void _header_update(void);
        void _copy_header(void);
        void _restore_header(void);

        // Wave file fields
        uint32_t _file_type_bloc_id;
        uint32_t _file_size;
        uint32_t _file_format_id;
        uint32_t _format_bloc_id;
        uint32_t _format_bloc_size;
        uint16_t _audio_format;
        uint16_t _channel_number;
        uint32_t _sample_rate;
        uint32_t _byte_per_second;
        uint16_t _byte_per_bloc;
        uint16_t _bits_per_sample;
        uint32_t _data_bloc_id;
        uint32_t _data_size;
        uint8_t  _datas[];

        // Wave class management variable
        eFileStatus _wave_file_status = File_Close;



    protected:
        void _add_data(uint8_t data[], uint32_t data_size);
        void _file_create(char* file_name, char* file_path, Wave_Format wave_format);
        void _file_open(char* file_name);
        void _file_close(char* file_name);
};

#endif