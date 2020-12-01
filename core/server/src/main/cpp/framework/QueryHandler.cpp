/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */

/* C++ Headers */
#include <cstdint>
#include <deque>
#include <vector>

/* SAFRN Headers */
#include <EventWrapper/Message.h>

#include <framework/QueryHandler.h>

/* Logging Configuration */
#include <ff/logging.h>

namespace safrn {

QueryHandler::QueryHandler(
    QueryId const & qid,
    StudyConfig const * study,
    dbuid_t const & orgId) :
    context(qid, study, orgId),
    dealer(&this->context, ROLE_DEALER),
    recipient(&this->context, ROLE_RECIPIENT),
    dataowner(&this->context, ROLE_DATAOWNER) {
}

void QueryHandler::init() {
  log_info(
      "Initializing query handler (%s)",
      dbuidToStr(this->context.orgId).c_str());

  std::vector<OutgoingMessageCache> out_msgs;

  this->dealer.init(out_msgs);
  this->recipient.init(out_msgs);
  this->dataowner.init(out_msgs);

  this->sendOutgoingMessages(out_msgs);

  this->initialized = true;

  while (this->messageCache.size() > 0) {
    std::unique_ptr<IncomingMessageCache> msg_cache =
        std::move(this->messageCache.front());
    this->messageCache.pop_front();
    IncomingMessage msg(
        msg_cache->sender, msg_cache->senderRole, msg_cache->msg);

    this->update(
        msg, msg_cache->id, msg_cache->myRole, msg_cache->control);
  }
}

FronctocolManager * QueryHandler::chooseRole(role_t myRole) {
  FronctocolManager * mgr;
  if (myRole == ROLE_DEALER) {
    mgr = &this->dealer;
  } else if (myRole == ROLE_RECIPIENT) {
    mgr = &this->recipient;
  } else if (myRole == ROLE_DATAOWNER) {
    mgr = &this->dataowner;
  } else {
    mgr = nullptr;
    log_fatal(
        "Detected non-singular role where singular role is expected");
  }

  return mgr;
}

void QueryHandler::handleSync(
    IncomingMessage & msg, role_t myRole, fronctocolid_t parentId) {
  FronctocolManager * mgr = this->chooseRole(myRole);
  std::vector<OutgoingMessageCache> out_msgs;

  mgr->handleSyncMsg(msg, parentId, out_msgs);

  this->sendOutgoingMessages(out_msgs);
}

void QueryHandler::handlePayload(
    IncomingMessage & msg, role_t myRole, fronctocolid_t parentId) {
  FronctocolManager * mgr = this->chooseRole(myRole);
  std::vector<OutgoingMessageCache> out_msgs;

  mgr->handlePayloadMsg(msg, parentId, out_msgs);

  this->sendOutgoingMessages(out_msgs);
}

void QueryHandler::handleComplete(
    IncomingMessage & msg, role_t myRole, fronctocolid_t parentId) {
  FronctocolManager * mgr = this->chooseRole(myRole);
  mgr->handleCompleteMsg(msg, parentId);
}

void QueryHandler::update(
    IncomingMessage & msg,
    fronctocolid_t fronctocolId,
    role_t myRole,
    uint8_t controlBlock) {
  if (!this->initialized) {
    this->messageCache.emplace_back(new IncomingMessageCache(
        msg, myRole, fronctocolId, controlBlock));
    return;
  }

#ifndef NDEBUG
  if (myRole != ROLE_DEALER && myRole != ROLE_RECIPIENT &&
      myRole != ROLE_DATAOWNER) {
    log_warn(
        "Received sync message for multiple roles (0x%hhx) from %s",
        myRole,
        dbuidToStr(msg.peer).c_str());
  }
  if (msg.peerRole != ROLE_DEALER && msg.peerRole != ROLE_RECIPIENT &&
      msg.peerRole != ROLE_DATAOWNER) {
    log_warn(
        "Received message from multiple roles (0x%hhx) from %s",
        msg.peerRole,
        dbuidToStr(msg.peer).c_str());
  }
#endif //NDEBUG

  if (controlBlock == CTRLBLK_SYNC) {
    this->handleSync(msg, myRole, fronctocolId);
  } else if (controlBlock == CTRLBLK_PAYLOAD) {
    this->handlePayload(msg, myRole, fronctocolId);
  } else if (controlBlock == CTRLBLK_COMPLETE) {
    this->handleComplete(msg, myRole, fronctocolId);
  } else {
    log_error(
        "Unrecognized message header: 0x%hhx from peer %s",
        controlBlock,
        dbuidToStr(msg.peer).c_str());
  }
}

void QueryHandler::sendOutgoingMessages(
    std::vector<OutgoingMessageCache> & msgs) {
  for (OutgoingMessageCache & omc : msgs) {
    if (omc.msg->peer == this->context.orgId) {
      IncomingMessage imsg(
          omc.msg->peer,
          omc.msg->peerRole,
          omc.msg->getEvBufferDirectAccess(),
          omc.msg->length());
      this->update(imsg, omc.id, omc.myRole, omc.control);
    } else if (map_contains(this->context.peers, omc.msg->peer)) {
      EvBufferWrapper out(bufferevent_get_output(
          this->context.peers[omc.msg->peer]->bufevt.get()));
      out.write<uint8_t>(omc.control);
      out.write<uint8_t>(omc.myRole);
      out.write<uint64_t>(omc.id);
      out.write<uint8_t>(omc.msg->peerRole);
      out.write<uint32_t>((uint32_t)omc.msg->length());
      evbuffer_add_buffer(
          out.getEvBufferDirectAccess(),
          omc.msg->getEvBufferDirectAccess());
    } else {
      log_warn(
          "Attempted send to disconnected peer %s",
          dbuidToStr(omc.msg->peer).c_str());
    }
  }
}

bool QueryHandler::isFinished() {
  return this->dealer.isFinished() && this->recipient.isFinished() &&
      this->dataowner.isFinished();
}

bool QueryHandler::isClosed() {
  return this->dealer.isClosed() && this->recipient.isClosed() &&
      this->dataowner.isClosed();
}

} // namespace safrn
