/*

Java code for MSP Parser

Copyright (C) Simon D. Levy 2021

MIT License
*/

package edu.wlu.cs.msppg;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.io.ByteArrayOutputStream;

public class Parser {

    private int state;
    private byte message_id;
    private byte message_length_expected;
    private byte message_length_received;
    private ByteArrayOutputStream message_buffer;
    private byte message_checksum;

    public Parser() {

        this.state = 0;
        this.message_buffer = new ByteArrayOutputStream();
    }

    private static ByteBuffer newByteBuffer(int capacity) {
        ByteBuffer bb = ByteBuffer.allocate(capacity);
        bb.order(ByteOrder.LITTLE_ENDIAN);
        return bb;
    }

   private static byte CRC8(byte [] data, int beg, int end) {

        int crc = 0x00;

        for (int k=beg; k<end; ++k) {

            int extract = (int)data[k] & 0xFF;

            crc ^= extract;
        }

        return (byte)crc;
    }

    public void parse(byte b) {

        switch (this.state) {

            case 0:               // sync char 1
                if (b == 36) { // $
                    this.state++;
                }
                break;        

            case 1:               // sync char 2
                if (b == 77) { // M
                    this.state++;
                }
                else {            // restart and try again
                    this.state = 0;
                }
                break;

            case 2:               // direction (should be >)
                this.state++;
                break;

            case 3:
                this.message_length_expected = b;
                this.message_checksum = b;
                // setup arraybuffer
                this.message_length_received = 0;
                this.state++;
                break;

            case 4:
                this.message_id = b;
                this.message_checksum ^= b;
                this.message_buffer.reset();
                if (this.message_length_expected > 0) {
                    // process payload
                    this.state++;
                }
                else {
                    // no payload
                    this.state += 2;
                }
                break;

            case 5: // payload
                this.message_buffer.write(b);
                this.message_checksum ^= b;
                this.message_length_received++;
                if (this.message_length_received >= this.message_length_expected) {
                    this.state++;
                }
                break;

            case 6:
                this.state = 0;
                if (this.message_checksum == b) {

                    ByteBuffer bb = newByteBuffer(this.message_length_received);
                    bb.put(this.message_buffer.toByteArray(), 0, this.message_length_received);

                    switch (this.message_id) {
                        case (byte)121:
                                this.handle_RECEIVER(
                                bb.getFloat(0),
                                bb.getFloat(4),
                                bb.getFloat(8),
                                bb.getFloat(12),
                                bb.getFloat(16),
                                bb.getFloat(20));
                            break;

                        case (byte)122:
                                this.handle_ATTITUDE_RADIANS(
                                bb.getFloat(0),
                                bb.getFloat(4),
                                bb.getFloat(8));
                            break;

                    }
                }
        }
    }

    public byte [] serialize_RECEIVER_Request() {


        byte [] message = new byte[6];

        message[0] = 36;
        message[1] = 77;
        message[2] = 60;
        message[3] = 0;
        message[4] = (byte)121;
        message[5] = (byte)121;

        return message;
    }

    protected void handle_RECEIVER(float  c1, float  c2, float  c3, float  c4, float  c5, float  c6) { 
    }

    public byte [] serialize_ATTITUDE_RADIANS_Request() {


        byte [] message = new byte[6];

        message[0] = 36;
        message[1] = 77;
        message[2] = 60;
        message[3] = 0;
        message[4] = (byte)122;
        message[5] = (byte)122;

        return message;
    }

    protected void handle_ATTITUDE_RADIANS(float  roll, float  pitch, float  yaw) { 
    }

}
