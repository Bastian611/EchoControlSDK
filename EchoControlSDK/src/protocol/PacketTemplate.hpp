#pragma once
#include "RpcPacket.h"
#include <cstring>

ECCS_BEGIN
namespace rpc {

    template<u32 ID, typename TData>
    class Packet : public RpcPacket {
    public:
        const static u32 _FACTORY_ID_ = ID;
        using Data_Type = TData;

        // 必须匹配 factory.hpp 中要求的签名
        static RpcPacket* createInstance() {
            return new Packet<ID, TData>();
        }

        virtual u32 typeId() const override { return ID; }
        virtual const char* typeName() const override { return "Packet"; }

        Packet() { m_header.id = ID; }
        Packet(const TData& d) : data(d) { m_header.id = ID; }

        TData data;

        virtual bool Encode(Buffer& buf) override {
            u32 dataSize = std::is_empty<TData>::value ? 0 : sizeof(TData);
            m_header.bodyLen = dataSize;
            buf.push_back((char*)&m_header, sizeof(PacketHeader));
            if (dataSize > 0) buf.push_back((char*)&data, dataSize);
            return true;
        }

        virtual bool Decode(const u8* ptr, u32 len) override {
            u32 dataSize = std::is_empty<TData>::value ? 0 : sizeof(TData);
            if (len < dataSize) return false;
            if (dataSize > 0) memcpy(&data, ptr, dataSize);
            return true;
        }
    };

}
ECCS_END