#ifndef BYTEARRAYUTIL_H
#define BYTEARRAYUTIL_H

#include <QByteArray>

class ByteArrayUtil
{
public:
    static QByteArray intToByte(int i)
    {
        QByteArray abyte0;
        abyte0.resize(4);
        abyte0[0] = (uchar) (0x000000ff & i);
        abyte0[1] = (uchar) ((0x0000ff00 & i) >> 8);
        abyte0[2] = (uchar) ((0x00ff0000 & i) >> 16);
        abyte0[3] = (uchar) ((0xff000000 & i) >> 24);
        return abyte0;
    }

    static int bytesToInt(QByteArray bytes)
    {
        int addr = bytes[0] & 0x000000FF;
        addr |= ((bytes[1] << 8) & 0x0000FF00);
        addr |= ((bytes[2] << 16) & 0x00FF0000);
        addr |= ((bytes[3] << 24) & 0xFF000000);
        return addr;
    }

    static QByteArray int64ToByte(qint64 i)
    {
        QByteArray abyte0;
        abyte0.resize(8);
        abyte0[0] = (uchar)  (0x00000000000000ff & i);
        abyte0[1] = (uchar) ((0x000000000000ff00 & i) >> 8);
        abyte0[2] = (uchar) ((0x0000000000ff0000 & i) >> 16);
        abyte0[3] = (uchar) ((0x00000000ff000000 & i) >> 24);
        abyte0[4] = (uchar) ((0x000000ff00000000 & i) >> 32);
        abyte0[5] = (uchar) ((0x0000ff0000000000 & i) >> 40);
        abyte0[6] = (uchar) ((0x00ff000000000000 & i) >> 48);
        abyte0[7] = (uchar) ((0xff00000000000000 & i) >> 56);
        return abyte0;
    }

    static qint64 bytesToInt64(QByteArray bytes)
    {
        qint64 addr = bytes[0]    & 0x00000000000000FF;
        addr |= ((bytes[1] << 8)  & 0x000000000000FF00);
        addr |= ((bytes[2] << 16) & 0x0000000000FF0000);
        addr |= ((bytes[3] << 24) & 0x00000000FF000000);
        addr |= ((bytes[4] << 32) & 0x000000FF00000000);
        addr |= ((bytes[5] << 40) & 0x0000FF0000000000);
        addr |= ((bytes[6] << 48) & 0x00FF000000000000);
        addr |= ((bytes[7] << 56) & 0xFF00000000000000);
        return addr;
    }
};

#endif // BYTEARRAYUTIL_H
