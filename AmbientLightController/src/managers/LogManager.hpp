#pragma once

// Different naming convention for convinience
/* 
Interface for logging operations
 */
struct Log
{
public:
    static void init()
    {
        Serial.begin(115200);
    }

    static void println(String msg)
    {
        Serial.print("[");
        Serial.print(millis());
        Serial.print("] ");
        Serial.println(msg);
    }

    static void error(String msg) 
    {
        println("ERROR: " + msg);
    }
};