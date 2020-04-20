/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:08:47
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-19 10:03:50
 * @Description: MasterModule
 */
#ifndef MASTERMODULE_H
#define MASTERMODULE_H

#include "masterslave.h"

typedef QMap<TagType, TagType> TagsMap;

class MasterModule : public MasterSlave
{
    struct LookUpRange {
        QString min;
        QString max;
        MID dstID;
    };

    // Q_OBJECT // 这个不能加！否则会爆 'QObject' is an ambiguous base of 'MasterModule' 的问题
public:
    MasterModule(QWidget *parent = nullptr);

    friend class FlowControlBase;
    friend class FlowControl_Master1_Slave1;
    friend class LookUpTableDialog;

    virtual MasterModule *newInstanceBySelf(QWidget *parent = nullptr) override;
    virtual void initData() override;
    virtual void setDefaultDataList() override;

    void passOnPackets() override;
    void updatePacketPos() override;
    void updatePacketPosVertical();
    void updatePacketPosHorizone();

    void setLookUpTable(QList<QStringList> table);

protected:
    virtual void packageSendEvent(DataPacket *packet) override;
    void setSrcIDAndDstID(DataPacket *packet);
    void encodePackageTag(DataPacket *package);
    void decodePackageTag(DataPacket *package);

    void paintEvent(QPaintEvent *event) override;
    virtual void drawShapePixmap(QPainter &painter, QRect draw_rect) override;



private:
    QFont big_font, normal_font, bold_font;
    TagsMap tags_map; // 维护 tags 映射，decode: logic中唯一tag => IP设定的tag
    int sended_unitID_pointer;

    QList<LookUpRange> look_up_table;
};

#endif // MASTERMODULE_H
