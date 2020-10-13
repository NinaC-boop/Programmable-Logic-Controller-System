#using <System.dll>

#include <conio.h>

using namespace System;
using namespace System::Diagnostics;

int main()
{
    __int64 StartingTime, EndingTime, Frequency;
    double TimeStamp, Elapsed;
    Frequency = Stopwatch::Frequency;
    StartingTime = Stopwatch::GetTimestamp();
    Elapsed = Frequency / 2.0f;
    while (!_kbhit())
    {
        StartingTime = Stopwatch::GetTimestamp();
        do
        {
            EndingTime = Stopwatch::GetTimestamp();
        } while ((double)(EndingTime - StartingTime) < Elapsed);
        TimeStamp = (double)(EndingTime) / Frequency;

        Console::WriteLine("TimeStamp = {0,22:F6}", TimeStamp);
    }
    Console::ReadKey();
    Console::ReadKey();
    return 0;
}