-- This is a (rough) dissector for GLS protocol packets.
-- WARNING: the protocol is still moving a lot, this dissector may not
-- be always uptodate.

-- Install it in the proper directory/folder for your platform, see
-- https://www.wireshark.org/docs/wsug_html_chunked/ChPluginFolders.html

local p_gls = Proto("gls", "GL-Streaming")

local vs_commands = {
   [0] = "UNDEF",
   [1] = "NOP",
   [2] = "HANDSHAKE",
   [3] = "SEND_DATA",
   [4] = "SEND_DATA_FRAGMENT",
   [5] = "CREATE_WINDOW",
   [6] = "SHARE_RING",

   [0x10000] = "eglChooseConfig",
   [0x10008] = "eglGetConfigAttrib",
   [0x10009] = "eglGetConfigs",
   [0x1000c] = "eglGetDisplay",
   [0x1000e] = "eglGetProcAddress",
   [0x10012] = "eglQueryString",
   [0x10015] = "eglTerminate",
   [0x1001f] = "eglReleaseThread",

   [0x20000] = "glActiveTexture",
   [0x20003] = "glBindBuffer",
   [0x20006] = "glBindTexture",
   [0x2000c] = "glBufferData",
   [0x2000d] = "glBufferSubData",
   [0x2000e] = "glCheckFramebufferStatus",
   [0x20010] = "glClearColor",
   [0x20011] = "glClearDepthf",
   [0x20012] = "glClearStencil",
   [0x2001a] = "glCreateShader",
   [0x20029] = "glDrawElements",
   [0x2002b] = "glEnableVertexAttribArray",
   [0x20033] = "glGenFramebuffers",
   [0x20035] = "glGenTextures",
   [0x2003c] = "glGetError",
   [0x2003f] = "glGetIntegerv",
   [0x20044] = "glGetShaderInfoLog",
   [0x2004c] = "glGetUniformLocation",
   [0x20047] = "glGetString",
   [0x20062] = "glShaderSource",
   [0x20069] = "glTexImage2D",
   [0x2006c] = "glTexParameteri",
   [0x20085] = "glVertexAttribPointer",
   [0x20086] = "glViewport",
}

local gl_enum = {
   [0x1406] = "GL_FLOAT",
}

-- SEND_DATA
local p_gls_senddata = Proto("gls.data_chunk", "GL-Streaming send_data buffer chunk")
local f_zero = ProtoField.uint64("gls.data_chunk.zero", "Must be zero", base.HEX)
local f_data_chunk = ProtoField.bytes("gls.data_chunk.data", "Data chunk", base.NONE)
p_gls_senddata.fields = { f_zero, f_data_chunk }

function p_gls_senddata.dissector(buf, pkt, tree)
   length = buf:len()
   if length == 0 then return end

   local subtree = tree:add(p_gls_senddata, buf())
   subtree:add_le(f_zero, buf(0,8))
   subtree:add(f_data_chunk, buf(8))
end

-- glVertexAttribPointer
local p_gls_glVertexAttribPointer = Proto("gls.glVertexAttribPointer", "GL-Streaming glVertexAttribPointer")
local f_index = ProtoField.uint32("gls.glVertexAttribPointer.index", "Attribute index", base.DEC)
local f_size = ProtoField.int32("gls.glVertexAttribPointer.size", "Attribute size", base.DEC)
local f_type = ProtoField.uint32("gls.glVertexAttribPointer.type", "Attribute type", base.HEX, gl_enum)
local f_normalized = ProtoField.uint32("gls.glVertexAttribPointer.normalized", "Attribute normalized flag", base.HEX)
local f_stride = ProtoField.int32("gls.glVertexAttribPointer.stride", "Attribute buffer stride", base.DEC)
local f_offset = ProtoField.uint64("gls.glVertexAttribPointer.offset", "Offset of first attribute in buffer", base.DEC)
p_gls_senddata.fields = { f_index, f_size, f_type, f_normalized, f_stride, f_offset }

function p_gls_glVertexAttribPointer.dissector(buf, pkt, tree)
   length = buf:len()
   if length == 0 then return end

   local subtree = tree:add(p_gls_senddata, buf())
   subtree:add_le(f_index, buf(0,4))
   subtree:add_le(f_size, buf(4,4))
   subtree:add_le(f_type, buf(8,4))
   subtree:add_le(f_stride, buf(12,4))
   subtree:add_le(f_offset, buf(16,8))
   subtree:add_le(f_normalized, buf(24,4))
end

-- GLS
local f_cmd = ProtoField.uint32("gls.command", "Command", base.HEX, vs_commands)
local f_pktsize = ProtoField.uint32("gls.pktsize", "Packet size", base.DEC)
p_gls.fields = { f_cmd, f_pktsize }

local data_dis = Dissector.get("data")

protos = {
   [2] = p_gls_senddata,
   [0x20085] = p_gls_glVertexAttribPointer,
}

function p_gls.dissector(buf, pinfo, tree)
   length = buf:len()
   if length == 0 then return end

   pinfo.cols.protocol = p_gls.name

   local offset = pinfo.desegment_offset or 0
   while true do
      -- if buf:len() < offset + 8 then
      --    pinfo.desegment_len =  -- how much ?  cannot know !
      --    pinfo.desegment_offset = offset
      --    return
      -- end

      local cmd_id = buf(offset, 4):le_uint()
      local pktsize = buf(offset+4, 4):le_uint()

      local nxtpdu = offset + pktsize

      if nxtpdu > buf:len() then
         pinfo.desegment_len = nxtpdu - buf:len()
         pinfo.desegment_offset = offset
         return
      end

      local subtree = tree:add(p_gls, buf(offset, pktsize))
      subtree:add_le(f_cmd, buf(offset, 4))
      subtree:add_le(f_pktsize, buf(offset+4, 4))

      local dissector = protos[cmd_id]

      if dissector ~= nil then
         -- Dissector was found, invoke subdissector with a new Tvb,
         -- created from the current buffer (skipping GLS header).
         dissector.dissector:call(buf(offset+8, pktsize-8):tvb(), pinfo, tree)
      else
         -- fallback dissector that just shows the raw data.
         data_dis:call(buf(offset+8, pktsize-8):tvb(), pinfo, tree)
      end

      offset = nxtpdu
      if nxtpdu == buf:len() then
         return
      end
   end
end

local udp_encap_table = DissectorTable.get("udp.port")
local tcp_encap_table = DissectorTable.get("tcp.port")

udp_encap_table:add(18145, p_gls)
tcp_encap_table:add(18145, p_gls)
