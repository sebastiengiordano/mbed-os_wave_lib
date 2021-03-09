#include <errno.h>

#include "FATFileSystem.h"

#include "Wave.h"


/***************/
/* Constructor */
/***************/
Wave::Wave() :
    {

}


/**
 * @brief  Initialize the wave header with "RIFF" chunk, "WAVE" format
 *    and "fmt "default parameters.
 *    Initialize the audio format parameters according to the
 *    wave_format provided by user.
 * 
 * @param  None
 * 
 * @retval None
 */
Wave::_header_init(Wave_Format wave_format){

    // [Declaration block for a file in WAVE format]
        /* Write chunkID, must be 'RIFF'  ----------------------------------------*/
        _file_type_bloc_id = CHUNK_ID;

        /* Initialize FileSize, Header_size - chunkID_size -----------------------*/
        _file_size = HEADER_SIZE - CHUNK_ID_SIZE;

        /* Write the file format, must be 'WAVE' ---------------------------------*/
        _file_format_id = FILE_FORMAT;

    // [Block describing the audio format]
        /* Write the format chunk, must be'fmt ' ---------------------------------*/
        _format_bloc_id  = FORMAT_ID;

        /* Write the length of the 'fmt' data, must be 0x10 ----------------------*/
        _format_bloc_size  = FORMAT_SIZE;

        /* Write the audio format, must be 0x01 (PCM) ----------------------------*/
        _audio_format = WAVE_FORMAT_PCM;

        /* Write the number of channels, Mono by default -------------------------*/
        _channel_number = CHANNEL_MONO;


    // [Audio format parameters calculation]
        uint32_t byte_per_bloc;
        uint32_t byte_per_second;
        // Number of bytes per sample block.
        byte_per_bloc = wave_format.number_of_channels * wave_format.bits_per_sample;
        // Number of bytes read per second.
        byte_per_second = wave_format.sample_rate * byte_per_bloc;

        /* Write the Sample Rate in Hz -------------------------------------------*/
        /* Write Little Endian 
                        ie. 8000 = 0x00001F40 =>    byte[3]=0x40, byte[2]=0x1F,
                                                    byte[1]=0x00, byte[0]=0x00 ---*/
        _sample_rate[3]  = (uint8_t)((wave_format.sample_rate & 0xFF));
        _sample_rate[2]  = (uint8_t)((wave_format.sample_rate >> 8) & 0xFF);
        _sample_rate[1]  = (uint8_t)((wave_format.sample_rate >> 16) & 0xFF);
        _sample_rate[0]  = (uint8_t)((wave_format.sample_rate >> 24) & 0xFF);

        /* Write the Byte Rate ---------------------------------------------------*/
        /* Write Little Endian ---------------------------------------------------*/
        _byte_per_second[3]  = (uint8_t)((byte_per_second & 0xFF));
        _byte_per_second[2]  = (uint8_t)((byte_per_second >> 8) & 0xFF);
        _byte_per_second[1]  = (uint8_t)((byte_per_second >> 16) & 0xFF);
        _byte_per_second[0]  = (uint8_t)((byte_per_second >> 24) & 0xFF);
        
        /* Write the block alignment ---------------------------------------------*/
        /* Write Little Endian ---------------------------------------------------*/
        _byte_per_bloc[3]  = (uint8_t)((byte_per_bloc & 0xFF));
        _byte_per_bloc[2]  = (uint8_t)((byte_per_bloc >> 8) & 0xFF);
        _byte_per_bloc[1]  = (uint8_t)((byte_per_bloc >> 16) & 0xFF);
        _byte_per_bloc[0]  = (uint8_t)((byte_per_bloc >> 24) & 0xFF);

        /* Write the number of bits per sample -----------------------------------*/
        /* Write Little Endian ---------------------------------------------------*/
        _bits_per_sample[3]  = (uint8_t)((wave_format.bits_per_sample & 0xFF));
        _bits_per_sample[2]  = (uint8_t)((wave_format.bits_per_sample >> 8) & 0xFF);
        _bits_per_sample[1]  = (uint8_t)((wave_format.bits_per_sample >> 16) & 0xFF);
        _bits_per_sample[0]  = (uint8_t)((wave_format.bits_per_sample >> 24) & 0xFF);


    // [Data block]
        /* Write the Data chunk, must be 'data' ----------------------------------*/
        _data_bloc_id  = DATA_ID;

        /* Write the number of sample data ---------------------------------------*/
        _data_size = 0;
}


Wave::_header_update(void){

}


/**
 * @brief  Copy each fields of the wave header file at the
 *   beginning of the wave file and returns a zero value.
 *   Otherwise the function fails and the return value is nonzero.
 * 
 * @param  None
 * 
 * @retval Zero value if the function succeed, otherwise it return
 *         a nonzero value which specifies the error code.
 */
Wave::_copy_header(void){
    int err;

    err = fprintf(f, "    %d\n", _file_type_bloc_id);
    if (err < 0) goto copy_header_error;
    err = fprintf(f, "    %d\n", _file_size);
    if (err < 0) goto copy_header_error;
    err = fprintf(f, "    %d\n", _file_format_id);
    if (err < 0) goto copy_header_error;
    err = fprintf(f, "    %d\n", _format_bloc_id);
    if (err < 0) goto copy_header_error;
    err = fprintf(f, "    %d\n", _format_bloc_size);
    if (err < 0) goto copy_header_error;
    err = fprintf(f, "    %d\n", _audio_format);
    if (err < 0) goto copy_header_error;
    err = fprintf(f, "    %d\n", _channel_number);
    if (err < 0) goto copy_header_error;
    err = fprintf(f, "    %d\n", _sample_rate);
    if (err < 0) goto copy_header_error;
    err = fprintf(f, "    %d\n", _byte_per_second);
    if (err < 0) goto copy_header_error;
    err = fprintf(f, "    %d\n", _byte_per_bloc);
    if (err < 0) goto copy_header_error;
    err = fprintf(f, "    %d\n", _bits_per_sample);
    if (err < 0) goto copy_header_error;
    err = fprintf(f, "    %d\n", _data_bloc_id);
    if (err < 0) goto copy_header_error;
    err = fprintf(f, "    %d\n", _data_size);
    if (err < 0) goto copy_header_error;

    return RES_OK;

    copy_header_error:
        return err;
}


Wave::_add_data(void){

}

/**
 * @brief  Checks if there is a file open before created a new wave file.
 *   Check if the file doesn't exist yet.
 *   Initialize the wave header with "RIFF" chunk, "WAVE" format and "fmt "
 *   default parameters.  Create the wave file. Initialize the wave
 *   header file. Copy the header in the file. and returns a zero value.
 *   Otherwise the function fails and the return value is nonzero.
 *   In this case, the return value specifies the cause of the function fails.
 *   The error codes that can be returned by this function are declared
 *   in "mbed_retarget.h".
 * 
 * @param  file_name - name of the file
 * @param  file_path - path where save the file
 * @param  wave_format - describing the audio format
 *                          (number_of_channels)
 *                          (sample_rate)
 *                          (bits_per_sample)
 * 
 * @retval Zero value if the function succeed, otherwise it return
 *         a nonzero value which specifies the error code.
 */
Wave::_file_create(char* file_path, Wave_Format wave_format){

    // Check if there is a file open before created a new wave file.
    if !(_wave_file_status == File_Close) {
        /* There is already a wave file opened or created.
           This class managed onlly one wave file at a time. */
        /* Too many open files */
        return -EMFILE;
    }

    // Check if the file doesn't exist yet.
    FILE *f = fopen(file_path, "r+");
    if (f){
        /* File exists */
        return -EEXIST;
    }

    // Create the wave file.
    f = fopen(file_path, "w+");
    if (!f){
        /* File creation error */
        return -errno;
    }

    // Initialize the wave header file.
    _header_init(wave_format);

    // Copy the header in the file.
    int err _copy_header();
    if (err < 0){
        // Copy header file return an error.
        return err;
    }

    // File creation success
    _wave_file_status = File_Created;
    return RES_OK;
}


Wave::_file_open(void){

    // Check if there is a file open before created a new wave file.
    if !(_wave_file_status == File_Close) {
        /* There is already a wave file opened or created.
           This class managed onlly one wave file at a time. */
        /* Too many open files */
        return -EMFILE;
    }

    // Check if the file doesn't exist yet
    FILE *f = fopen(file_path, "r+");
    if (f){
        /* File exists */
        return -EEXIST;
    }


    // Initialize the wave header file.
    _header_init(wave_format);

    // Create the wave file
    f = fopen(file_path, "w+");
    if (!f){
        /* File creation error */
        return -errno;
    }

    // File creation success
    _wave_file_status = File_Created;
    return RES_OK;

}


Wave::_file_close(void){

}