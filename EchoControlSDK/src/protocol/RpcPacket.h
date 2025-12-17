#pragma once
#include "../global.h"
#include "../utils/buffer.h"
#include "../utils/factory.hpp"

ECCS_BEGIN
namespace rpc {

#pragma pack(push, 1)
    struct PacketHeader {
        u32 magic;      // 0xEC55AAEE
        u32 id;         // Packet ID
        u32 bodyLen;    // Data Length
        u32 cseq;       // Sequence Number

        PacketHeader() : magic(0xEC55AAEE), id(0), bodyLen(0), cseq(0) {}
    };
#pragma pack(pop)

    class RpcPacket {
    public:
        virtual ~RpcPacket() = default;

        // 工厂基类宏 (Key = u32)
        FACTORY_ID_BASE(u32)

        virtual bool Encode(Buffer& buf) = 0;
        virtual bool Decode(const u8* data, u32 len) = 0;

        u32 GetID() const { return m_header.id; }
        void SetSeq(u32 seq) { m_header.cseq = seq; }

    protected:
        PacketHeader m_header;
    };

    // 空数据占位符
    struct NoneData {};

}
ECCS_END