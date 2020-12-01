/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

namespace ff {

template<typename Identity_T>
bool msg_read(
    IncomingMessage<Identity_T> & msg,
    ::safrn::dealer::RandomTableLookupInfo & input) {

  bool success = msg.template read<mpc::LargeNum>(input.r_modulus_);
  success =
      success & msg.template read<mpc::SmallNum>(input.table_size_);
  return success;
}

template<typename Identity_T>
bool msg_write(
    OutgoingMessage<Identity_T> & msg,
    ::safrn::dealer::RandomTableLookupInfo const & input) {
  bool success = msg.template write<mpc::LargeNum>(input.r_modulus_);
  success =
      success & msg.template write<mpc::SmallNum>(input.table_size_);
  return success;
}

template<typename Identity_T>
bool msg_read(
    IncomingMessage<Identity_T> & msg,
    ::safrn::dealer::RandomTableLookup & input) {
  log_debug("Calling read");
  bool success = msg.template read<mpc::LargeNum>(input.r_);

  uint64_t local_table_size = 0;
  success = msg.template read<uint64_t>(local_table_size);
  size_t recast_size = (size_t)local_table_size;
  input.u_.resize(recast_size);

  for (size_t i = 0; i < recast_size; i++) {
    success = success & msg.template read<Boolean_t>(input.u_[i]);
  }
  log_debug("success");
  return success;
}

template<typename Identity_T>
bool msg_write(
    OutgoingMessage<Identity_T> & msg,
    ::safrn::dealer::RandomTableLookup const & input) {
  bool success = msg.template write<mpc::LargeNum>(input.r_);

  uint64_t local_table_size = static_cast<uint64_t>(input.u_.size());
  success = msg.template write<uint64_t>(local_table_size);
  size_t recast_size = (size_t)local_table_size;

  for (size_t i = 0; i < recast_size; i++) {
    success = success & msg.template write<Boolean_t>(input.u_.at(i));
  }
  return success;
}

} // namespace ff
