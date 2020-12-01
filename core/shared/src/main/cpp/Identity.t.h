/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

namespace ff {

template<typename Identity_T>
bool msg_read(
    ff::IncomingMessage<Identity_T> & imsg, safrn::Identity & id) {
  bool ret = true;
  ret = ret && imsg.remove(&id.orgId[0], safrn::DBUID_LENGTH);
  ret = ret && imsg.template read<safrn::role_t>(id.role);
  uint32_t vert32 = 0;
  ret = ret && imsg.template read<uint32_t>(vert32);
  id.vertical = (size_t)vert32;

  return ret;
}

template<typename Identity_T>
bool msg_write(
    ff::OutgoingMessage<Identity_T> & omsg,
    safrn::Identity const & id) {
  bool ret = true;
  ret = ret && omsg.add(&id.orgId[0], safrn::DBUID_LENGTH);
  ret = ret && omsg.template write<safrn::role_t>(id.role);
  ret = ret && omsg.template write<uint32_t>((uint32_t)id.vertical);
  return ret;
}

} // namespace ff
