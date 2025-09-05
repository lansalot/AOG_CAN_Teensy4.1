// ----------------------------------------------------------
// Send a hardware message with proper header, payload, and CRC
// ----------------------------------------------------------
void sendHardwareMessage(const String& message, byte seconds)
{
    uint8_t hardwareMessage[128] = { 0x80, 0x81, 0x7E, 221 };

    int msgLen = message.length();    // byte count (ASCII assumed)
    int totalLength = 7 + msgLen + 1; // header(7) + message + CRC(1)

    hardwareMessage[4] = msgLen + 2;  // message length + display config
    hardwareMessage[5] = seconds;           // seconds to display
    hardwareMessage[6] = 0;           // color (0 = normal, 1 = alt)

    // Copy message into buffer
    message.getBytes(&hardwareMessage[7], msgLen + 1);

    // Copy the range we need for checksum into temp buffer
    uint8_t temp[128];
    int checksumLen = 7 + msgLen - 2; // from index 2 up to 6+msgLen
    memcpy(temp, hardwareMessage + 2, checksumLen);

    // Sum for checksum
    int16_t CK_A = 0;
    for (int i = 0; i < checksumLen; i++)
    {
        CK_A += temp[i];
    }
    hardwareMessage[7 + msgLen] = CK_A; // CRC

    // Debug dump
    Serial.println("Hardware Message Dump:");
    for (int i = 0; i < totalLength; i++)
    {
        if (i % 16 == 0) Serial.print("\n");
        Serial.printf("%02X ", hardwareMessage[i]);
    }
    Serial.println("\n");

    // Send via UDP
    Udp.beginPacket(ipDestination, AOGPort);
    Udp.write(hardwareMessage, totalLength);
    Udp.endPacket();
}