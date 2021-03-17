#include <errno.h>

#include "Wave.h"
#include "utils.h"


/***************/
/* Constructor */
/***************/
Wave::Wave() 
    {

}

/**
 * @brief   Create an empty file (if a file with the same name already exists,
 *      its contents are discarded), at file_path location.
 *      Initialize the wave header file according to wave paramaters
 *      contain in wave_format.
 *      Copy the wave header in file_path.
 * 
 * @param   file_path - path where save the file
 * @param   wave_format - Wave_Format which define:
 *              number_of_channels : Number of channels (1: MONO, 2: STEREO...)
 *              sample_rate : Sample Rate in Hz
 *              bits_per_sample :   Number of bits per sample (must be a
 *                                  multiple of 8)          
 * 
 * @retval  Zero value if the function succeed, otherwise it return
 *          a nonzero value which specifies the error code.
 */
int32_t Wave::open(const char* file_path, Wave_Format wave_format){
    // Save the file path
    _file_path = file_path;
    // Create the file
    int32_t err = _file_open(file_path);
    if (err < 0) return err;

    // Initialize the wave header file.
    _header_init(wave_format);

    // Copy the header in the file.
    err = _copy_header();
    if (err < 0){
        // Copy header file return an error.
        return err;
    }
    return RES_OK;
}

/**
 * @brief   This function must be called after Wave::open(...)
 *      Copy the samples in the wave file previously created.
 *      Each sample shall be transmit in little endian format.
 * 
 * @param   samples - Samples to write in the open file
 * @param   number_of_sample - Number of sample contains in samples
 * 
 * @retval  Zero value if the function succeed, otherwise it return
 *          a nonzero value.
 */
int32_t Wave::write(const char* samples, uint8_t number_of_sample){
    uint8_t data_size;
    data_size = (_bits_per_sample / 8) * number_of_sample;
    _file_size += data_size;
    _data_size += data_size;
    return _write_in_file(samples, data_size);
}

/**
 * @brief   This function must be called at the end of the recording,
 *      in order to update the wave header with the amount of data copied.
 * 
 * @param   None
 * 
 * @retval  Zero value if the function succeed, otherwise it return
 *          a nonzero value.
 */
int32_t Wave::rewrite_header_and_close(void){
    int32_t err = _header_update();
    if (err < 0) return err;
    err = _file_close();
    return err;
}

/**
 * @brief   This function returns the file size in numbers of bytes.
 *          Returns the data size + the header size.
 * 
 * @param   None
 * 
 * @retval  File size in numbers of bytes.
 */
int32_t Wave::file_size(void){
    return _file_size + FILE_SIZE_POSITION;
}

/**
 * @brief   Initialize the wave header with "RIFF" chunk, "WAVE" format
 *    and "fmt "default parameters.
 *    Initialize the audio format parameters according to the
 *    wave_format provided by user.
 * 
 * @param   None
 * 
 * @retval  None
 */
void    Wave::_header_init(Wave_Format wave_format){

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
        _channel_number = wave_format.number_of_channels;


    // [Audio format parameters calculation]
        uint32_t byte_per_bloc;
        uint32_t byte_per_second;
        // Number of bytes per sample block.
        byte_per_bloc = wave_format.number_of_channels * wave_format.bits_per_sample / 8;
        // Number of bytes read per second.
        byte_per_second = wave_format.sample_rate * byte_per_bloc;

        /* Write the Sample Rate in Hz -------------------------------------------*/
        _sample_rate = wave_format.sample_rate;

        /* Write the Byte Rate ---------------------------------------------------*/
        _byte_per_second = byte_per_second;
        
        /* Write the block alignment ---------------------------------------------*/
        _byte_per_bloc = byte_per_bloc;

        /* Write the number of bits per sample -----------------------------------*/
        _bits_per_sample = wave_format.bits_per_sample;


    // [Data block]
        /* Write the Data chunk, must be 'data' ----------------------------------*/
        _data_bloc_id  = DATA_ID;

        /* Write the number of sample data ---------------------------------------*/
        _data_size = 0;
}

/**
 * @brief   Re-open the wave file in read/write binary mode.
 *          Update the FileSize and the DataSize.
 * 
 * @param  None
 * 
 * @retval  Zero value if the function succeed, otherwise it return
 *          a nonzero value.
 */
int32_t Wave::_header_update(void){
    int32_t err;
    int32_t copy_header_error = 0;
 
    // Close the wave file
    fclose(_file);
 
    // Open the wave file in binary read/write
    _file = fopen(_file_path, "rb+");
    if (!_file){
        /* File creation error */
        return -errno;
    }

    // File pointer set to "file size" position
    fseek(_file, FILE_SIZE_POSITION, SEEK_SET);
        /* Copy the FileSize -----------------------------------------------------*/
    err = _write_in_file((const char*)&_file_size, sizeof(_file_size));
    if (err < 0) copy_header_error += err;

    // File pointer set to "data size" position
    fseek(_file, DATA_SIZE_POSITION, SEEK_SET);
        /* Copy the DataSize -----------------------------------------------------*/
    err = _write_in_file((const char*)&_data_size, sizeof(_data_size));
    if (err < 0) copy_header_error += err;

    return copy_header_error;
}

/**
 * @brief   Copy each fields of the wave header file at the
 *      beginning of the wave file and returns a zero value.
 *      Otherwise the function fails and the return value is nonzero.
 * 
 * @param   None
 * 
 * @retval  Zero value if the function succeed, otherwise it return
 *          a nonzero value which specifies the error code.
 */
int32_t Wave::_copy_header(void){
    int32_t err;
    int32_t copy_header_error = 0;
    uint32_t file_type_bloc_id;
    uint32_t file_format_id;
    uint32_t format_bloc_id;
    uint32_t data_bloc_id;

    // [Declaration block for a file in WAVE format]
        /* Write chunkID, must be 'RIFF'  ----------------------------------------*/
        /* Write Big Endian ------------------------------------------------------*/
    file_type_bloc_id = swap_uint32(_file_type_bloc_id);
    err = _write_in_file((const char*)&file_type_bloc_id, sizeof(_file_type_bloc_id));
    if (err < 0) copy_header_error += err;
        /* Initialize FileSize, Header_size - chunkID_size -----------------------*/
    err = _write_in_file((const char*)&_file_size, sizeof(_file_size));
    if (err < 0) copy_header_error += err;
        /* Write the file format, must be 'WAVE' ---------------------------------*/
        /* Write Big Endian ------------------------------------------------------*/
    file_format_id = swap_uint32(_file_format_id);
    err = _write_in_file((const char*)&file_format_id, sizeof(_file_format_id));
    if (err < 0) copy_header_error += err;
    // [Block describing the audio format]
        /* Write the format chunk, must be'fmt ' ---------------------------------*/
        /* Write Big Endian ------------------------------------------------------*/
    format_bloc_id = swap_uint32(_format_bloc_id);
    err = _write_in_file((const char*)&format_bloc_id, sizeof(_format_bloc_id));
    if (err < 0) copy_header_error += err;
        /* Write the length of the 'fmt' data, must be 0x10 ----------------------*/
    err = _write_in_file((const char*)&_format_bloc_size, sizeof(_format_bloc_size));
    if (err < 0) copy_header_error += err;
        /* Write the audio format, must be 0x01 (PCM) ----------------------------*/
    err = _write_in_file((const char*)&_audio_format, sizeof(_audio_format));
    if (err < 0) copy_header_error += err;
        /* Write the number of channels ------------------------------------------*/
    err = _write_in_file((const char*)&_channel_number, sizeof(_channel_number));
    if (err < 0) copy_header_error += err;
        /* Write the Sample Rate in Hz -------------------------------------------*/
    err = _write_in_file((const char*)&_sample_rate, sizeof(_sample_rate));
    if (err < 0) copy_header_error += err;
        /* Write the Byte Rate ---------------------------------------------------*/
    err = _write_in_file((const char*)&_byte_per_second, sizeof(_byte_per_second));
    if (err < 0) copy_header_error += err;
        /* Write the block alignment ---------------------------------------------*/
    err = _write_in_file((const char*)&_byte_per_bloc, sizeof(_byte_per_bloc));
    if (err < 0) copy_header_error += err;
        /* Write the number of bits per sample -----------------------------------*/
    err = _write_in_file((const char*)&_bits_per_sample, sizeof(_bits_per_sample));
    if (err < 0) copy_header_error += err;
        /* Write the Data chunk, must be 'data' ----------------------------------*/
        /* Write Big Endian ------------------------------------------------------*/
    data_bloc_id = swap_uint32(_data_bloc_id);
    err = _write_in_file((const char*)&data_bloc_id, sizeof(_data_bloc_id));
    if (err < 0) copy_header_error += err;
        /* Write the number of sample data ---------------------------------------*/
    err = _write_in_file((const char*)&_data_size, sizeof(_data_size));
    if (err < 0) copy_header_error += err;

    return err;
}

/**
 * @brief   Create an empty file (if a file with the same name already exists,
 *      its contents are discarded), at file_path location, and returns a zero value.
 *      Otherwise the function fails and the return value is nonzero.
 *      In this case, the return value specifies the cause of the function fails.
 *      The error codes that can be returned by this function are declared
 *      in <errno.h>.
 *
 * @param   file_path - path where save the file
 * 
 * @retval  Zero value if the function succeed, otherwise it return
 *          a nonzero value which specifies the error code.
 */
int32_t Wave::_file_open(const char* file_path){
 
    // Create the wave file
    _file = fopen(file_path, "wb+");
    if (!_file){
        /* File creation error */
        return -errno;
    }

    // File creation success
    return RES_OK;
}

/**
 * @brief   Close the wave file and returns a zero value.
 *      Otherwise the function fails and the return value is nonzero.
 *      In this case, the return value specifies the cause of the function fails.
 *      The error codes that can be returned by this function are declared
 *      in <errno.h>.
 *
 * @param   file_path - path where save the file
 * 
 * @retval  Zero value if the function succeed, otherwise it return
 *          a nonzero value which specifies the error code.
 */
int32_t Wave::_file_close(void){
    int32_t err = fclose(_file);
    if (err < 0){
        // File closed return an error.
        return err;
    }
    _file = NULL;
    // File closed successully
    return RES_OK;
}

/**
 * @brief   Write data at the file pointer position.
 *
 * @param   data - data to copy.
 * @param   size_of_data - number of byto to copy.
 * 
 * @retval  Zero value if the function succeed, otherwise it return
 *          a nonzero value.
 */
int32_t Wave::_write_in_file(const char* data, uint32_t size_of_data){
    int32_t err;
    int32_t copy_data_error = 0;

    for (uint8_t i=0; i < size_of_data; i++){
        err = fputc((uint8_t)*(data + i), _file);
        if (err < 0) copy_data_error += err;
    }
    return copy_data_error;
}
