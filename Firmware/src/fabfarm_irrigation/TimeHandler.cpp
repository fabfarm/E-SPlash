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