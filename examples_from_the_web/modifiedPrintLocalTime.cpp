void modifiedPrintLocalTime()
{ 
    int OnlyYear;
    int onlyMonth;
    int onlyDay;
    int onlyHour;
    int onlyMin;
    int onlySec;
    
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time");
        return;
        }
//Serial.println(&timeinfo, "%m %d %Y / %H:%M:%S");
//scanf(&timeinfo, "%m %d %Y / %H:%M:%S")
        onlyHour = timeinfo.tm_hour;
        onlyMin  = timeinfo.tm_min;
        onlySec  = timeinfo.tm_sec;

        onlyDay = timeinfo.tm_mday;
        onlyMonth = timeinfo.tm_mon + 1;
        OnlyYear = timeinfo.tm_year +1900;
        }