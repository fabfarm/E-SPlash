#include "TimeHandler.h"
#include "Config.h"

void printCompilationTimestamp()
{
    Serial.println("*****************************************************");
    Serial.printf("* This program was compiled on: %s at %s\n\r", __DATE__, __TIME__);
    Serial.println("*****************************************************\n\r");
}

void printRtcDateTime(const RtcDateTime &dt)
{
    char datestring[20];
    snprintf_P(datestring, countof(datestring), PSTR("%02u/%02u/%04u %02u:%02u:%02u"), dt.Month(), dt.Day(), dt.Year(), dt.Hour(), dt.Minute(), dt.Second());
    Serial.println(datestring);
}

void syncInternalRtcWithExternal(const RtcDateTime &dt)
{
    char datestring[20];
    snprintf_P(datestring, countof(datestring), PSTR("%02u/%02u/%04u %02u:%02u:%02u"), dt.Month(), dt.Day(), dt.Year(), dt.Hour(), dt.Minute(), dt.Second());
    rtc.setTime(dt.Second(), dt.Minute(), dt.Hour(), dt.Day(), dt.Month(), dt.Year());

    Serial.println("*****************************************************");
    Serial.println("* Function syncInternalRtcWithExternal()");
    Serial.printf("* Time from external RTC: %s\n\r", datestring);
    Serial.println("* This function updates the internal RTC with the time");
    Serial.println("* from the external RTC");
    Serial.println("*****************************************************");
}

void initializeRtc()
{
    Serial.println("*****************************************************");
    Serial.println("* Running function initializeRtc()");

#ifdef ds_3231
    Wire.begin(sdaPin, sclPin);
#endif
    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

    Serial.println("* ");
    Serial.printf("* Time from internal RTC on boot: %s\n\r", rtc.getTime("%A, %B %d %Y %H:%M:%S").c_str());

    syncInternalRtcWithExternal(Rtc.GetDateTime());

    Serial.println("* ");
    Serial.printf("* Time from internal RTC after external RTC update: %s\n\r", rtc.getTime("%A, %B %d %Y %H:%M:%S").c_str());

    if (!Rtc.IsDateTimeValid())
    {
        Serial.println("* RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

#ifdef ds_3231
#else
    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }
#endif

    if (!Rtc.GetIsRunning())
    {
        Serial.println("*RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled)
    {
        Serial.println("* RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled)
    {
        Serial.println("* RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled)
    {
        Serial.println("* RTC is the same as compile time! (not expected but all is fine)");
    }

    Serial.println("* END of data from first external RTC function");
    Serial.println("*****************************************************");
    
    // Initialize internet time synchronization
    initializeInternetTimeSync();
}

void printRtcDateTimeInLoop()
{
    unsigned long previousMillis = 0;
    const long printTimeInterval = 1000;

    Serial.println("This data comes from void printRtcDateTimeInLoop(). Time will appear every 1 second.");

    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= printTimeInterval)
    {
        previousMillis = currentMillis;

        RtcDateTime now = Rtc.GetDateTime();
        if (now.IsValid())
        {
            printRtcDateTime(now);
            Serial.println();
        }
        else
        {
            Serial.println("RTC lost confidence in the DateTime!");
        }
    }

    Serial.println("End of data from void printRtcDateTimeInLoop().");
}

void initializeInternetTimeSync()
{
    Serial.println("*****************************************************");
    Serial.println("* Initializing Internet Time Synchronization");

    // Get UTC offset from JSON data (default to UTC+1 for Portugal)
    int utcOffsetHours = 1; // Default timezone
    if (doc["data"]["changedtime"][0]["utcOffset"].is<int>()) {
        utcOffsetHours = doc["data"]["changedtime"][0]["utcOffset"];
    }
    
    long utcOffsetSeconds = utcOffsetHours * 3600;

    // Configure NTP with timezone offset
    configTime(utcOffsetSeconds, 0, "pool.ntp.org", "time.nist.gov", "time.google.com");

    Serial.printf("* UTC offset set to: %+d hours (%ld seconds)\n", utcOffsetHours, utcOffsetSeconds);
    Serial.println("* NTP servers configured: pool.ntp.org, time.nist.gov, time.google.com");
    Serial.println("* Internet time sync initialized");
    Serial.println("*****************************************************");
}

void syncWithInternetTime()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("* WiFi not connected - cannot sync with internet time");
        return;
    }

    Serial.println("*****************************************************");
    Serial.println("* Attempting to sync with internet time...");

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo, 2000)) // Reduced to 2 second timeout
    {
        Serial.println("* Failed to get internet time (continuing without sync)");
        Serial.println("*****************************************************");
        return;
    }

    // Update internal ESP32 RTC
    rtc.setTimeStruct(timeinfo);

    // Update external RTC if available
#ifdef ds_3231
    RtcDateTime internetTime = RtcDateTime(
        timeinfo.tm_year + 1900,
        timeinfo.tm_mon + 1,
        timeinfo.tm_mday,
        timeinfo.tm_hour,
        timeinfo.tm_min,
        timeinfo.tm_sec);

    if (Rtc.IsDateTimeValid())
    {
        Rtc.SetDateTime(internetTime);
        Serial.println("* External RTC updated with internet time");
    }
#endif

    Serial.printf("* Time synced: %04d-%02d-%02d %02d:%02d:%02d\n",
                  timeinfo.tm_year + 1900,
                  timeinfo.tm_mon + 1,
                  timeinfo.tm_mday,
                  timeinfo.tm_hour,
                  timeinfo.tm_min,
                  timeinfo.tm_sec);
    Serial.println("* Internet time sync completed successfully");
    Serial.println("*****************************************************");
}

void handleInternetTimeSync()
{
    static unsigned long lastSyncAttempt = 0;
    static unsigned long lastSuccessfulSync = 0;
    const unsigned long syncInterval = 3600000; // 1 hour in milliseconds
    const unsigned long retryInterval = 300000;  // 5 minutes for retry on failure

    unsigned long currentMillis = millis();

    // Check if WiFi is connected and it's time to sync
    if (WiFi.status() == WL_CONNECTED)
    {
        // If we've never synced, or it's been more than the sync interval
        if (lastSuccessfulSync == 0 || (currentMillis - lastSuccessfulSync >= syncInterval))
        {
            // Avoid too frequent attempts - wait at least retry interval between attempts
            if (currentMillis - lastSyncAttempt >= retryInterval)
            {
                lastSyncAttempt = currentMillis;

                // Try to sync
                struct tm timeinfo;
                if (getLocalTime(&timeinfo, 5000)) // 5 second timeout for regular checks
                {
                    // Update internal ESP32 RTC
                    rtc.setTimeStruct(timeinfo);

                    // Update external RTC if available
#ifdef ds_3231
                    RtcDateTime internetTime = RtcDateTime(
                        timeinfo.tm_year + 1900,
                        timeinfo.tm_mon + 1,
                        timeinfo.tm_mday,
                        timeinfo.tm_hour,
                        timeinfo.tm_min,
                        timeinfo.tm_sec);

                    if (Rtc.IsDateTimeValid())
                    {
                        Rtc.SetDateTime(internetTime);
                    }
#endif

                    lastSuccessfulSync = currentMillis;
                    Serial.println("* Automatic internet time sync completed");
                }
                else
                {
                    Serial.println("* Automatic internet time sync failed - will retry later");
                }
            }
        }
    }
}

void updateTimezone()
{
    // Get UTC offset from JSON data
    int utcOffsetHours = 1; // Default timezone
    if (doc["data"]["changedtime"][0]["utcOffset"].is<int>()) {
        utcOffsetHours = doc["data"]["changedtime"][0]["utcOffset"];
    }
    
    long utcOffsetSeconds = utcOffsetHours * 3600;

    // Reconfigure NTP with new timezone offset
    configTime(utcOffsetSeconds, 0, "pool.ntp.org", "time.nist.gov", "time.google.com");

    Serial.printf("* Timezone updated to UTC%+d (%ld seconds)\n", utcOffsetHours, utcOffsetSeconds);
}