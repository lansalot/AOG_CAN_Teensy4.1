// ----------------------------------------------------------
// Send a hardware message with proper header, payload, and CRC
// ----------------------------------------------------------
void sendHardwareMessage(const String& message, byte seconds)
{
    uint8_t hardwareMessage[128] = { 0x80, 0x81, 0x7E, 221 };

    int msgLen = message.length();    // byte count (ASCII assumed)
    if (msgLen > 120) {
        Serial.println("Error: Message too long for hardware message buffer");
        return;
    }
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
    for (int i = 2; i < 7 + msgLen; i++)
    {
        CK_A += hardwareMessage[i];
    }
    hardwareMessage[7 + msgLen] = CK_A; // CRC

    // Send via UDP
    Udp.beginPacket(ipDestination, AOGPort);
    Udp.write(hardwareMessage, totalLength);
    Udp.endPacket();
}