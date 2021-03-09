/** @defgroup WAVEPLAYER_Private_Defines
 * @{
 */
#define  HEADER_SIZE                44          /* correspond to 44 bytes */
#define  CHUNK_ID                   0x52494646  /* correspond to the letters 'RIFF' */
#define  CHUNK_ID_SIZE              4           /* correspond to 4 bytes */
#define  FILE_FORMAT                0x57415645  /* correspond to the letters 'WAVE' */
#define  FORMAT_ID                  0x666D7420  /* correspond to the letters 'fmt ' */
#define  FORMAT_SIZE                0x10000000  /* correspond to 0x10 in little endian */
#define  DATA_ID                    0x64617461  /* correspond to the letters 'data' */
#define  WAVE_FORMAT_PCM            0x01
#define  CHANNEL_MONO               0x0100
#define  CHANNEL_STEREO             0x0200
#define  SAMPLE_RATE_8000           8000
#define  SAMPLE_RATE_11025          11025
#define  SAMPLE_RATE_22050          22050
#define  SAMPLE_RATE_44100          44100
#define  BITS_PER_SAMPLE_8           8
#define  BITS_PER_SAMPLE_16         16