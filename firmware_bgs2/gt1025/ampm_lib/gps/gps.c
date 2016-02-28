#include "gps.h"
#include "lib/sys_tick.h"
#define nmea_error(...) 	//DbgCfgPrintf(__VA_ARGS__)



uint8_t gpsCheckSum = 0;
uint8_t gpsCheckSumX = 0;
uint8_t gpsGetCheckSumFlag = 0;
uint8_t GPRMC_gotDataFlag = 0;
uint8_t GPGGA_gotDataFlag = 0;
uint8_t GPGSA_gotDataFlag = 0;
uint8_t GPGSV_gotDataFlag = 0;
uint8_t msgGpsDataGood = 0;
char msgTerm[80];

#define GPS_CMD_COUNT   (sizeof (GPS_ProcessCmd) / sizeof (GPS_ProcessCmd[0]))
#define GPS_CMD_PARSER_SIZE	16
#define GPS_MSG_MAX_SIZE	80



char buffGPRMC[GPS_MSG_MAX_SIZE] = {0};
char buffGPGGA[GPS_MSG_MAX_SIZE] = {0};
char buffGPGSA[GPS_MSG_MAX_SIZE] = {0};
char buffGPGSV[GPS_MSG_MAX_SIZE] = {0};

uint8_t GPS_Buff[128];
nmeaGPRMC *pPackGPRMC;
nmeaGPGGA	*pPackGPGGA;
nmeaGPGSA	*pPackGPGSA;
nmeaGPGSV	*pPackGPGSV;
nmeaINFO	gpsInfo;
nmeaINFO	lastGpsInfo;

uint8_t gpsPulse = 0;

char GPRMC_Process(char c);
char GPGGA_Process(char c);
char GPGSA_Process(char c);
char GPGSV_Process(char c);
/* Command definitions structure. */
typedef struct _GPS_PARSER_STRUCT {
   char cmdInfo[16];
   char (*func)(char c);
} GPS_PARSER_STRUCT;

static const GPS_PARSER_STRUCT GPS_ProcessCmd[] = {
	"GPRMC,", GPRMC_Process,
	"GPGGA,", GPGGA_Process,
  "GPGSA,", GPGSA_Process,
	"GPGSV,", GPGSV_Process
};

uint32_t GPS_cmdCnt[GPS_CMD_COUNT];
uint8_t GPS_numCmd;
uint32_t GPS_cmdRecvLength;


enum{
	GPS_CMD_NEW_STATE,
	GPS_CMD_WAIT_FINISH,
	GPS_CMD_FINISH
}GPS_cmdState = GPS_CMD_FINISH;


void GPS_PulseProcess(uint32_t timeNow)
{
		static uint32_t gpsTime = 0;
		if(timeNow - gpsTime >= 3)
		{
			if(gpsPulse)
			{
				gpsPulse = 0;
				gpsInfo.pulse = 1;
			}
			else
			{
				gpsInfo.pulse = 0;
			}
		}
}

void GPS_SetPulse(void)
{
	gpsPulse = 1;
}

char GPRMC_Process(char c)
{
	
	if(GPRMC_gotDataFlag) return 0;
	
	if(GPS_cmdRecvLength == 0) 
	{
		gpsGetCheckSumFlag = 0;
		gpsCheckSumX = 0;
	}
	switch(c)
	{
		case '\r':
			if(gpsCheckSumX == gpsCheckSum)
				GPRMC_gotDataFlag = 1;
			return 0;
		case '*':
			gpsCheckSumX = 0;
			gpsGetCheckSumFlag = 1;
			break;
		default:
			if(gpsGetCheckSumFlag)
			{
				if((c >= '0') && (c <= '9'))
					gpsCheckSumX |= c - '0';
				else
					gpsCheckSumX |= (c - 'A') + 10;
				if(gpsGetCheckSumFlag == 1)
					gpsCheckSumX <<= 4;
				gpsGetCheckSumFlag++;
			}
			else
			{
				gpsCheckSum ^= c;
			}
			break;
	}
	buffGPRMC[GPS_cmdRecvLength] = c;
	return 0xff;
}

char GPGGA_Process(char c)
{
	if(GPGGA_gotDataFlag) return 0;
	if(GPS_cmdRecvLength == 0) 
	{
		gpsGetCheckSumFlag = 0;
		gpsCheckSumX = 0;
	}
	switch(c)
	{
		case '\r':
			if(gpsCheckSumX == gpsCheckSum)
				GPGGA_gotDataFlag = 1;
			return 0;
		case '*':
			gpsCheckSumX = 0;
			gpsGetCheckSumFlag = 1;
			break;
		default:
			if(gpsGetCheckSumFlag)
			{
				if((c >= '0') && (c <= '9'))
					gpsCheckSumX |= c - '0';
				else
					gpsCheckSumX |= (c - 'A') + 10;
				if(gpsGetCheckSumFlag == 1)
					gpsCheckSumX <<= 4;
				gpsGetCheckSumFlag++;
			}
			else
			{
				gpsCheckSum ^= c;
			}
			break;
	}
	buffGPGGA[GPS_cmdRecvLength] = c;
	return 0xff;
}
char GPGSA_Process(char c)
{
	if(GPGSA_gotDataFlag) return 0;
	if(GPS_cmdRecvLength == 0) 
	{
		gpsGetCheckSumFlag = 0;
		gpsCheckSumX = 0;
	}
	switch(c)
	{
		case '\r':
			if(gpsCheckSumX == gpsCheckSum)
				GPGSA_gotDataFlag = 1;
			return 0;
		case '*':
			gpsCheckSumX = 0;
			gpsGetCheckSumFlag = 1;
			break;
		default:
			if(gpsGetCheckSumFlag)
			{
				if((c >= '0') && (c <= '9'))
					gpsCheckSumX |= c - '0';
				else
					gpsCheckSumX |= (c - 'A') + 10;
				if(gpsGetCheckSumFlag == 1)
					gpsCheckSumX <<= 4;
				gpsGetCheckSumFlag++;
			}
			else
			{
				gpsCheckSum ^= c;
			}
			break;
	}
	buffGPGSA[GPS_cmdRecvLength] = c;
	return 0xff;
}

char GPGSV_Process(char c)
{
	if(GPGSV_gotDataFlag) return 0;
	if(GPS_cmdRecvLength == 0) 
	{
		gpsGetCheckSumFlag = 0;
		gpsCheckSumX = 0;
	}
	switch(c)
	{
		case '\r':
			if(gpsCheckSumX == gpsCheckSum)
				GPGSV_gotDataFlag = 1;
			return 0;
		case '*':
			gpsCheckSumX = 0;
			gpsGetCheckSumFlag = 1;
			break;
		default:
			if(gpsGetCheckSumFlag)
			{
				if((c >= '0') && (c <= '9'))
					gpsCheckSumX |= c - '0';
				else
					gpsCheckSumX |= (c - 'A') + 10;
				if(gpsGetCheckSumFlag == 1)
					gpsCheckSumX <<= 4;
				gpsGetCheckSumFlag++;
			}
			else
			{
				gpsCheckSum ^= c;
			}
			break;
	}
	buffGPGSV[GPS_cmdRecvLength] = c;
	return 0xff;
}

void GPS_ComnandParser(char c)
{
	uint32_t i;
	switch(GPS_cmdState)
	{
		case GPS_CMD_FINISH:
			for(i = 0; i < GPS_CMD_COUNT;i++)
			{
					if(c == GPS_ProcessCmd[i].cmdInfo[GPS_cmdCnt[i]])
					{
							GPS_cmdCnt[i]++;
							if(GPS_ProcessCmd[i].cmdInfo[GPS_cmdCnt[i]] == '\0')
							{
									GPS_numCmd = i;
									GPS_cmdState = GPS_CMD_WAIT_FINISH;
									GPS_cmdRecvLength = 0;
									gpsCheckSum = 0;
									for(i = 0;i < GPS_CMD_PARSER_SIZE;i++)
									{
										gpsCheckSum ^= GPS_ProcessCmd[GPS_numCmd].cmdInfo[i];
										if(GPS_ProcessCmd[GPS_numCmd].cmdInfo[i] == '\0')
											break;
									}
							}
					}
					else
					{
							GPS_cmdCnt[i] = 0;
					}
			}
		break;
		case GPS_CMD_WAIT_FINISH:
			if(GPS_ProcessCmd[GPS_numCmd].func(c) == 0)
			{
				GPS_cmdState = GPS_CMD_FINISH;
				for(i = 0; i < GPS_CMD_COUNT;i++)
				{
					GPS_cmdCnt[i] = 0;
				}
			}
			GPS_cmdRecvLength++;
			if(GPS_cmdRecvLength >= GPS_MSG_MAX_SIZE)
			{
				GPS_cmdState = GPS_CMD_FINISH;
				for(i = 0; i < GPS_CMD_COUNT;i++)
				{
					GPS_cmdCnt[i] = 0;
				}
			}
			break;
		default:
			GPS_cmdState = GPS_CMD_FINISH;
			for(i = 0; i < GPS_CMD_COUNT;i++)
			{
				GPS_cmdCnt[i] = 0;
			}
			break;
	}	
}

double neamFormatLatLng(double v)
{
	uint32_t u32temp = (uint32_t)v;
	double f = v - (double)u32temp;
	f = f*60;
	return f + (double)u32temp*100;
}


double formatLatLng(double v)
{
	double degree = floor(v / 100);
	double min = v - degree * 100;
	
	return degree + min / 60;
}

uint8_t GpsGetInfo(nmeaINFO *info)
{	
	Timeout_Type timeOut;
	nmea_error("\r\nGPS:Get info...\r\n");
	InitTimeout(&timeOut,SYSTICK_TIME_SEC(30));
	while(gpsInfo.fix < 3)
	{
		if(CheckTimeout(&timeOut)) break;
	}
	if(gpsInfo.fix > 1)
	{
			memcpy(info, (nmeaINFO*)&gpsInfo, sizeof(nmeaINFO) - 1);
		return 0;
	}
	return 1;
}

void nmea_zero_INFO(nmeaINFO *info)
{
    memset(info, 0, sizeof(nmeaINFO));
    info->sig = NMEA_SIG_BAD;
    info->fix = NMEA_FIX_BAD;
}

void GPSInit(void)
{	
	nmea_zero_INFO((nmeaINFO*)&gpsInfo);
}


uint8_t GPS_GetInfo(void)
{
	if(GPRMC_gotDataFlag && GPGGA_gotDataFlag && GPGSA_gotDataFlag && GPGSV_gotDataFlag)
	{
		//__disable_irq();
		pPackGPRMC = (nmeaGPRMC *)GPS_Buff;
		nmea_parse_GPRMC(buffGPRMC,sizeof(buffGPRMC),pPackGPRMC);
		nmea_GPRMC2info(pPackGPRMC, &gpsInfo);
		
		pPackGPGGA = (nmeaGPGGA *)GPS_Buff;
		nmea_parse_GPGGA(buffGPGGA,sizeof(buffGPGGA),pPackGPGGA);
		nmea_GPGGA2info(pPackGPGGA, &gpsInfo);
		
		pPackGPGSA = (nmeaGPGSA *)GPS_Buff;
		nmea_parse_GPGSA(buffGPGSA,sizeof(buffGPGSA),pPackGPGSA);
		nmea_GPGSA2info(pPackGPGSA, &gpsInfo);
		
		pPackGPGSV = (nmeaGPGSV *)GPS_Buff;
		nmea_parse_GPGSV(buffGPGSV,sizeof(buffGPGSV),pPackGPGSV);
		nmea_GPGSV2info(pPackGPGSV, &gpsInfo);

		GPRMC_gotDataFlag = 0;
		GPGGA_gotDataFlag = 0;
		GPGSA_gotDataFlag = 0;
		GPGSV_gotDataFlag = 0;
		//__enable_irq();
	}
	return 0;
}

/**
 * \brief Fill nmeaINFO structure by GGA packet data.
 * @param pack a pointer of packet structure.
 * @param info a pointer of summary information structure.
 */
void nmea_GPGGA2info(nmeaGPGGA *pack, nmeaINFO *info)
{
    info->utc.hour = pack->utc.hour;
    info->utc.min = pack->utc.min;
    info->utc.sec = pack->utc.sec;
    info->utc.hsec = pack->utc.hsec;
    info->sig = pack->sig;
    info->HDOP = pack->HDOP;
    info->elv = pack->elv;
		info->mlat = pack->lat;
		info->mlon = pack->lon;
		info->ew = pack->ew;
		info->ns = pack->ns;
    info->lat = ((pack->ns == 'N')?formatLatLng(pack->lat):-formatLatLng(pack->lat));
    info->lon = ((pack->ew == 'E')?formatLatLng(pack->lon):-formatLatLng(pack->lon));
    info->smask |= GPGGA;
}

/**
 * \brief Fill nmeaINFO structure by GSA packet data.
 * @param pack a pointer of packet structure.
 * @param info a pointer of summary information structure.
 */
void nmea_GPGSA2info(nmeaGPGSA *pack, nmeaINFO *info)
{
    int i, j, nuse = 0;
    info->fix = pack->fix_type;
    info->PDOP = pack->PDOP;
    info->HDOP = pack->HDOP;
    info->VDOP = pack->VDOP;

    for(i = 0; i < NMEA_MAXSAT; ++i)
    {
        for(j = 0; j < info->satinfo.inview; ++j)
        {
            if(pack->sat_prn[i] && pack->sat_prn[i] == info->satinfo.sat[j].id)
            {
                info->satinfo.sat[j].in_use = 1;
                nuse++;
            }
        }
    }

    info->satinfo.inuse = nuse;
    info->smask |= GPGSA;
}
/**
 * \brief Fill nmeaINFO structure by GSV packet data.
 * @param pack a pointer of packet structure.
 * @param info a pointer of summary information structure.
 */
void nmea_GPGSV2info(nmeaGPGSV *pack, nmeaINFO *info)
{
    int isat, isi, nsat;

    if(pack->pack_index > pack->pack_count ||
        pack->pack_index * NMEA_SATINPACK > NMEA_MAXSAT)
        return;

    if(pack->pack_index < 1)
        pack->pack_index = 1;

    info->satinfo.inview = pack->sat_count;

    nsat = (pack->pack_index - 1) * NMEA_SATINPACK;
    nsat = (nsat + NMEA_SATINPACK > pack->sat_count)?pack->sat_count - nsat:NMEA_SATINPACK;

    for(isat = 0; isat < nsat; ++isat)
    {
        isi = (pack->pack_index - 1) * NMEA_SATINPACK + isat;
        info->satinfo.sat[isi].id = pack->sat_data[isat].id;
        info->satinfo.sat[isi].elv = pack->sat_data[isat].elv;
        info->satinfo.sat[isi].azimuth = pack->sat_data[isat].azimuth;
        info->satinfo.sat[isi].sig = pack->sat_data[isat].sig;
    }

    info->smask |= GPGSV;
}
/**
 * \brief Fill nmeaINFO structure by RMC packet data.
 * @param pack a pointer of packet structure.
 * @param info a pointer of summary information structure.
 */
void nmea_GPRMC2info(nmeaGPRMC *pack, nmeaINFO *info)
{
    if('A' == pack->status)
    {
        if(NMEA_SIG_BAD == info->sig)
            info->sig = NMEA_SIG_MID;
        if(NMEA_FIX_BAD == info->fix)
            info->fix = NMEA_FIX_2D;
    }
    else if('V' == pack->status)
    {
        info->sig = NMEA_SIG_BAD;
        info->fix = NMEA_FIX_BAD;
    }

    info->utc = pack->utc;
		info->mlat = pack->lat;
		info->mlon = pack->lon;
		info->ew = pack->ew;
		info->ns = pack->ns;
    info->lat = ((pack->ns == 'N')?formatLatLng(pack->lat):-formatLatLng(pack->lat));
    info->lon = ((pack->ew == 'E')?formatLatLng(pack->lon):-formatLatLng(pack->lon));
    info->speed = pack->speed * NMEA_TUD_KNOTS;
    info->direction = pack->direction;
    info->smask |= GPRMC;
}


/**
 * \brief Parse GSA packet from buffer.
 * @param buff a constant character pointer of packet buffer.
 * @param buff_sz buffer size.
 * @param pack a pointer of packet which will filled by function.
 * @return 1 (true) - if parsed successfully or 0 (false) - if fail.
 */
int nmea_parse_GPGSA(const char *buff, int buff_sz, nmeaGPGSA *pack)
{
    memset(pack, 0, sizeof(nmeaGPGSA));

    if(17 != nmea_scanf(buff, buff_sz,
        "%C,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f*",
        &(pack->fix_mode), &(pack->fix_type),
        &(pack->sat_prn[0]), &(pack->sat_prn[1]), &(pack->sat_prn[2]), &(pack->sat_prn[3]), &(pack->sat_prn[4]), &(pack->sat_prn[5]),
        &(pack->sat_prn[6]), &(pack->sat_prn[7]), &(pack->sat_prn[8]), &(pack->sat_prn[9]), &(pack->sat_prn[10]), &(pack->sat_prn[11]),
        &(pack->PDOP), &(pack->HDOP), &(pack->VDOP)))
    {
        nmea_error("\r\nGPS:GPGSA parse error!\r\n");
        return 0;
    }

    return 1;
}

/**
 * \brief Parse GSV packet from buffer.
 * @param buff a constant character pointer of packet buffer.
 * @param buff_sz buffer size.
 * @param pack a pointer of packet which will filled by function.
 * @return 1 (true) - if parsed successfully or 0 (false) - if fail.
 */
int nmea_parse_GPGSV(const char *buff, int buff_sz, nmeaGPGSV *pack)
{
    int nsen, nsat;
	
    memset(pack, 0, sizeof(nmeaGPGSV));

    nsen = nmea_scanf(buff, buff_sz,
        "%d,%d,%d,"
        "%d,%d,%d,%d,"
        "%d,%d,%d,%d,"
        "%d,%d,%d,%d,"
        "%d,%d,%d,%d*",
        &(pack->pack_count), &(pack->pack_index), &(pack->sat_count),
        &(pack->sat_data[0].id), &(pack->sat_data[0].elv), &(pack->sat_data[0].azimuth), &(pack->sat_data[0].sig),
        &(pack->sat_data[1].id), &(pack->sat_data[1].elv), &(pack->sat_data[1].azimuth), &(pack->sat_data[1].sig),
        &(pack->sat_data[2].id), &(pack->sat_data[2].elv), &(pack->sat_data[2].azimuth), &(pack->sat_data[2].sig),
        &(pack->sat_data[3].id), &(pack->sat_data[3].elv), &(pack->sat_data[3].azimuth), &(pack->sat_data[3].sig));

    nsat = (pack->pack_index - 1) * NMEA_SATINPACK;
    nsat = (nsat + NMEA_SATINPACK > pack->sat_count)?pack->sat_count - nsat:NMEA_SATINPACK;
    nsat = nsat * 4 + 3 /* first three sentence`s */;

    if(nsen < nsat || nsen > (NMEA_SATINPACK * 4 + 3))
    {
        nmea_error("GPGSV parse error!");
        return 0;
    }

    return 1;
}

/**
 * \brief Parse GGA packet from buffer.
 * @param buff a constant character pointer of packet buffer.
 * @param buff_sz buffer size.
 * @param pack a pointer of packet which will filled by function.
 * @return 1 (true) - if parsed successfully or 0 (false) - if fail.
 */
int nmea_parse_GPGGA(const char *buff, int buff_sz, nmeaGPGGA *pack)
{
    char time_buff[NMEA_TIMEPARSE_BUF];

    memset(pack, 0, sizeof(nmeaGPGGA));

    if(14 != nmea_scanf(buff, buff_sz,
        "%s,%f,%C,%f,%C,%d,%d,%f,%f,%C,%f,%C,%f,%d*",
        &(time_buff[0]),
        &(pack->lat), &(pack->ns), &(pack->lon), &(pack->ew),
        &(pack->sig), &(pack->satinuse), &(pack->HDOP), &(pack->elv), &(pack->elv_units),
        &(pack->diff), &(pack->diff_units), &(pack->dgps_age), &(pack->dgps_sid)))
    {
        nmea_error("\r\nGPS:GPGGA parse error!\r\n");
        return 0;
    }

    if(0 != _nmea_parse_time(&time_buff[0], (int)strlen(&time_buff[0]), &(pack->utc)))
    {
        nmea_error("\r\nGPS:GPGGA time parse error!\r\n");
        return 0;
    }

    return 1;
}


/**
 * \brief Parse RMC packet from buffer.
 * @param buff a constant character pointer of packet buffer.
 * @param buff_sz buffer size.
 * @param pack a pointer of packet which will filled by function.
 * @return 1 (true) - if parsed successfully or 0 (false) - if fail.
 */
int nmea_parse_GPRMC(const char *buff, int buff_sz, nmeaGPRMC *pack)
{
    int nsen;
    char time_buff[NMEA_TIMEPARSE_BUF];

    memset(pack, 0, sizeof(nmeaGPRMC));

    nsen = nmea_scanf(buff, buff_sz,
        "%s,%C,%f,%C,%f,%C,%f,%f,%2d%2d%2d,%f,%C,%C*",
        &(time_buff[0]),
        &(pack->status), &(pack->lat), &(pack->ns), &(pack->lon), &(pack->ew),
        &(pack->speed), &(pack->direction),
        &(pack->utc.day), &(pack->utc.mon), &(pack->utc.year),
        &(pack->declination), &(pack->declin_ew), &(pack->mode));

    if(nsen != 13 && nsen != 14)
    {
        nmea_error("\r\nGPS:GPRMC parse error!\r\n");
        return 0;
    }

    if(0 != _nmea_parse_time(&time_buff[0], (int)strlen(&time_buff[0]), &(pack->utc)))
    {
        nmea_error("\r\nGPS:GPRMC time parse error!\r\n");
        return 0;
    }

    if(pack->utc.year < 90)
        pack->utc.year += 100;
    pack->utc.mon -= 1;

    return 1;
}


int _nmea_parse_time(const char *buff, int buff_sz, nmeaTIME *res)
{
    int success = 0;

    switch(buff_sz)
    {
    case sizeof("hhmmss") - 1:
        success = (3 == nmea_scanf(buff, buff_sz,
            "%2d%2d%2d", &(res->hour), &(res->min), &(res->sec)
            ));
        break;
    case sizeof("hhmmss.s") - 1:
    case sizeof("hhmmss.ss") - 1:
    case sizeof("hhmmss.sss") - 1:
        success = (4 == nmea_scanf(buff, buff_sz,
            "%2d%2d%2d.%d", &(res->hour), &(res->min), &(res->sec), &(res->hsec)
            ));
        break;
    default:
        nmea_error("\r\nGPS:Parse of time error (format error)!\r\n");
        success = 0;
        break;
    }

    return (success?0:-1);        
}


/**
 * \brief Calculate control sum of binary buffer
 */
int nmea_calc_crc(const char *buff, int buff_sz)
{
    int chsum = 0,
        it;

    for(it = 0; it < buff_sz; ++it)
        chsum ^= (int)buff[it];

    return chsum;
}

/**
 * \brief Convert string to number
 */
int nmea_atoi(const char *str, int str_sz, int radix)
{
    char *tmp_ptr;
    char buff[NMEA_CONVSTR_BUF];
    int res = 0;

    if(str_sz < NMEA_CONVSTR_BUF)
    {
        memcpy(&buff[0], str, str_sz);
        buff[str_sz] = '\0';
        res = strtol(&buff[0], &tmp_ptr, radix);
    }

    return res;
}

/**
 * \brief Convert string to fraction number
 */
double nmea_atof(const char *str, int str_sz)
{
    char *tmp_ptr;
    char buff[NMEA_CONVSTR_BUF];
    double res = 0;

    if(str_sz < NMEA_CONVSTR_BUF)
    {
        memcpy(&buff[0], str, str_sz);
        buff[str_sz] = '\0';
        res = strtod(&buff[0], &tmp_ptr);
    }

    return res;
}

/**
 * \brief Analyse string (specificate for NMEA sentences)
 */
int nmea_scanf(const char *buff, int buff_sz, const char *format, ...)
{
    const char *beg_tok;
    const char *end_buf = buff + buff_sz;

    va_list arg_ptr;
    int tok_type = NMEA_TOKS_COMPARE;
    int width = 0;
    const char *beg_fmt = 0;
    int snum = 0, unum = 0;

    int tok_count = 0;
    void *parg_target;

    va_start(arg_ptr, format);
    
    for(; *format && buff < end_buf; ++format)
    {
        switch(tok_type)
        {
        case NMEA_TOKS_COMPARE:
            if('%' == *format)
                tok_type = NMEA_TOKS_PERCENT;
            else if(*buff++ != *format)
                goto fail;
            break;
        case NMEA_TOKS_PERCENT:
            width = 0;
            beg_fmt = format;
            tok_type = NMEA_TOKS_WIDTH;
        case NMEA_TOKS_WIDTH:
            if(isdigit(*format))
                break;
            {
                tok_type = NMEA_TOKS_TYPE;
                if(format > beg_fmt)
                    width = nmea_atoi(beg_fmt, (int)(format - beg_fmt), 10);
            }
        case NMEA_TOKS_TYPE:
            beg_tok = buff;

            if(!width && ('c' == *format || 'C' == *format) && *buff != format[1])
                width = 1;

            if(width)
            {
                if(buff + width <= end_buf)
                    buff += width;
                else
                    goto fail;
            }
            else
            {
                if(!format[1] || (0 == (buff = (char *)memchr(buff, format[1], end_buf - buff))))
                    buff = end_buf;
            }

            if(buff > end_buf)
                goto fail;

            tok_type = NMEA_TOKS_COMPARE;
            tok_count++;

            parg_target = 0; width = (int)(buff - beg_tok);

            switch(*format)
            {
            case 'c':
            case 'C':
                parg_target = (void *)va_arg(arg_ptr, char *);
                if(width && 0 != (parg_target))
                    *((char *)parg_target) = *beg_tok;
                break;
            case 's':
            case 'S':
                parg_target = (void *)va_arg(arg_ptr, char *);
                if(width && 0 != (parg_target))
                {
                    memcpy(parg_target, beg_tok, width);
                    ((char *)parg_target)[width] = '\0';
                }
                break;
            case 'f':
            case 'g':
            case 'G':
            case 'e':
            case 'E':
                parg_target = (void *)va_arg(arg_ptr, double *);
                if(width && 0 != (parg_target))
                    *((double *)parg_target) = nmea_atof(beg_tok, width);
                break;
            };

            if(parg_target)
                break;
            if(0 == (parg_target = (void *)va_arg(arg_ptr, int *)))
                break;
            if(!width)
                break;

            switch(*format)
            {
            case 'd':
            case 'i':
                snum = nmea_atoi(beg_tok, width, 10);
                memcpy(parg_target, &snum, sizeof(int));
                break;
            case 'u':
                unum = nmea_atoi(beg_tok, width, 10);
                memcpy(parg_target, &unum, sizeof(unsigned int));
                break;
            case 'x':
            case 'X':
                unum = nmea_atoi(beg_tok, width, 16);
                memcpy(parg_target, &unum, sizeof(unsigned int));
                break;
            case 'o':
                unum = nmea_atoi(beg_tok, width, 8);
                memcpy(parg_target, &unum, sizeof(unsigned int));
                break;
            default:
                goto fail;
            };

            break;
        };
    }

fail:

    va_end(arg_ptr);

    return tok_count;
}


