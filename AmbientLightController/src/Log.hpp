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

    static error(String msg) 
    {
        println("ERROR: " + msg);
    }
};