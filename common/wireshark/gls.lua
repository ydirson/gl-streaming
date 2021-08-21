-- This is a (rough) dissector for GLS protocol packets.
-- WARNING: the protocol is still moving a lot, this dissector may not
-- be always uptodate.

-- Install it in the proper directory/folder for your platform, see
-- https://www.wireshark.org/docs/wsug_html_chunked/ChPluginFolders.html

local p_gls = Proto("gls", "GL-Streaming")

local vs_commands = {
   [0] = "UNDEF",
   [1] = "HANDSHAKE",
   [2] = "SEND_DATA",

   [0x10000] = "eglChooseConfig",
   [0x10008] = "eglGetConfigAttrib",
   [0x10012] = "eglQueryString",

   [0x20000] = "glActiveTexture",
   [0x20003] = "glBindBuffer",
   [0x20006] = "glBindTexture",
   [0x2000c] = "glBufferData",
   [0x2000d] = "glBufferSubData",
   [0x2000e] = "glCheckFramebufferStatus",
   [0x20033] = "glGenFramebuffers",
   [0x20035] = "glGenTextures",
   [0x2003f] = "glGetIntegerv",
   [0x20047] = "glGetString",
   [0x20062] = "glShaderSource",
   [0x20069] = "glTexImage2D",
   [0x2006c] = "glTexParameteri",
}

-- SEND_DATA
local p_gls_senddata = Proto("gls.data_chunk", "GL-Streaming send_data buffer chunk")
local f_data_offset = ProtoField.uint32("gls.data_chunk.offset", "Offset of chunk in buffer", base.DEC)
local f_data_size = ProtoField.uint32("gls.data_chunk.size", "Size of chunk", base.DEC)
local f_is_last = ProtoField.uint32("gls.data_chunk.is_last", "Is last chunk", base.DEC)
local f_data_chunk = ProtoField.bytes("gls.data_chunk.data", "Data chunk", base.NONE)
p_gls_senddata.fields = { f_data_offset, f_data_size, f_is_last, f_data_chunk }

function p_gls_senddata.dissector(buf, pkt, tree)
   length = buf:len()
   if length == 0 then return end

   local subtree = tree:add(p_gls_senddata, buf(0,4))
   subtree:add_le(f_data_offset, buf(0,4))
   subtree:add_le(f_data_size, buf(4,4))
   subtree:add_le(f_is_last, buf(8,4))
   subtree:add(f_data_chunk, buf(12))
end

-- GLS
local f_cmd = ProtoField.uint32("gls.command", "Command", base.HEX, vs_commands)
local f_pktsize = ProtoField.uint32("gls.pktsize", "Packet size", base.DEC)
p_gls.fields = { f_cmd, f_pktsize }

local data_dis = Dissector.get("data")

protos = {
   [2] = p_gls_senddata,
}

function p_gls.dissector(buf, pkt, tree)
   length = buf:len()
   if length == 0 then return end

   pkt.cols.protocol = p_gls.name

   local subtree = tree:add(p_gls, buf(0,4))
   subtree:add_le(f_cmd, buf(0,4))
   subtree:add_le(f_pktsize, buf(4,4))

   local cmd_id = buf(0,4):le_uint()
   local dissector = protos[cmd_id]

   if dissector ~= nil then
      -- Dissector was found, invoke subdissector with a new Tvb,
      -- created from the current buffer (skipping GLS header).
      dissector.dissector:call(buf(8):tvb(), pkt, tree)
   else
      -- fallback dissector that just shows the raw data.
      data_dis:call(buf(8):tvb(), pkt, tree)
   end
end

local udp_encap_table = DissectorTable.get("udp.port")
local tcp_encap_table = DissectorTable.get("tcp.port")

udp_encap_table:add(18145, p_gls)
tcp_encap_table:add(18145, p_gls)
