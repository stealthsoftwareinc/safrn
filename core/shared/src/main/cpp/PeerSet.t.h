/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

namespace ff {

template<typename Identity_T>
bool msg_read(
    IncomingMessage<Identity_T> & imsg, safrn::PeerSet & peerset) {
  bool success = true;

  uint8_t has_dlr_8 = 0;
  success = success && imsg.template read<uint8_t>(has_dlr_8);
  if (has_dlr_8 == 0x00) {
    peerset.hasDealer = false;
  } else {
    peerset.hasDealer = true;
    success = success &&
        imsg.template read<safrn::Identity>(peerset.dealer.peer);
  }

  uint32_t num_recips32 = 0;
  success = success && imsg.template read<uint32_t>(num_recips32);
  for (size_t i = 0; i < (size_t)num_recips32; i++) {
    safrn::Identity recip;
    success = success && imsg.template read<safrn::Identity>(recip);
    peerset.recipients.emplace_back(recip);
  }

  uint32_t num_verts32 = 0;
  success = success && imsg.template read<uint32_t>(num_verts32);
  for (size_t i = 0; i < (size_t)num_verts32; i++) {
    peerset.dataowners.emplace_back(0);
    uint32_t num_in_vert32 = 0;
    success = success && imsg.template read<uint32_t>(num_in_vert32);
    for (size_t j = 0; j < (size_t)num_in_vert32; j++) {
      safrn::Identity owner;
      success = success && imsg.template read<safrn::Identity>(owner);
      peerset.dataowners[i].emplace_back(owner);
    }
  }

  return success;
}

template<typename Identity_T>
bool msg_write(
    OutgoingMessage<Identity_T> & omsg,
    safrn::PeerSet const & peerset) {
  bool success = true;
  if (peerset.hasDealer) {
    success = success && omsg.template write<uint8_t>(0x01);
    success = success &&
        omsg.template write<safrn::Identity>(peerset.dealer.peer);
  } else {
    success = success && omsg.template write<uint8_t>(0x00);
  }

  success = success &&
      omsg.template write<uint32_t>(
          (uint32_t)peerset.recipients.size());
  for (size_t i = 0; i < peerset.recipients.size(); i++) {
    success = success &&
        omsg.template write<safrn::Identity>(
            peerset.recipients[i].peer);
  }

  success = success &&
      omsg.template write<uint32_t>(
          (uint32_t)peerset.dataowners.size());
  for (size_t i = 0; i < peerset.dataowners.size(); i++) {
    success = success &&
        omsg.template write<uint32_t>(
            (uint32_t)peerset.dataowners[i].size());
    for (size_t j = 0; j < peerset.dataowners[i].size(); j++) {
      success = success &&
          omsg.template write<safrn::Identity>(
              peerset.dataowners[i][j].peer);
    }
  }

  return success;
}

} // namespace ff
