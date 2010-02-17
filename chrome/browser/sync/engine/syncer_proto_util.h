// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_SYNC_ENGINE_SYNCER_PROTO_UTIL_H_
#define CHROME_BROWSER_SYNC_ENGINE_SYNCER_PROTO_UTIL_H_

#include <string>

#include "chrome/browser/sync/syncable/blob.h"
#include "chrome/browser/sync/syncable/model_type.h"
#include "chrome/browser/sync/util/sync_types.h"
#include "testing/gtest/include/gtest/gtest_prod.h"  // For FRIEND_TEST

namespace syncable {
class Directory;
class Entry;
class ScopedDirLookup;
class SyncName;
}  // namespace syncable

namespace sync_pb {
class ClientToServerResponse;
class EntitySpecifics;
}  // namespace sync_pb

namespace browser_sync {

namespace sessions {
class SyncSession;
}

class AuthWatcher;
class ClientToServerMessage;
class ServerConnectionManager;
class SyncEntity;
class CommitResponse_EntryResponse;

class SyncerProtoUtil {
 public:
  // Posts the given message and fills the buffer with the returned value.
  // Returns true on success.  Also handles store birthday verification:
  // session->status()->syncer_stuck_ is set true if the birthday is
  // incorrect.  A false value will always be returned if birthday is bad.
  static bool PostClientToServerMessage(ClientToServerMessage* msg,
      sync_pb::ClientToServerResponse* response,
      sessions::SyncSession* session);

  // Compares a syncable Entry to SyncEntity, returns true iff the data is
  // identical.
  //
  // TODO(sync): The places where this function is used are arguable big causes
  // of the fragility, because there's a tendency to freak out the moment the
  // local and server values diverge. However, this almost always indicates a
  // sync bug somewhere earlier in the sync cycle.
  static bool Compare(const syncable::Entry& local_entry,
                      const SyncEntity& server_entry);

  // Utility methods for converting between syncable::Blobs and protobuf byte
  // fields.
  static void CopyProtoBytesIntoBlob(const std::string& proto_bytes,
                                     syncable::Blob* blob);
  static bool ProtoBytesEqualsBlob(const std::string& proto_bytes,
                                   const syncable::Blob& blob);
  static void CopyBlobIntoProtoBytes(const syncable::Blob& blob,
                                     std::string* proto_bytes);

  // Extract the name field from a sync entity.
  static const std::string& NameFromSyncEntity(const SyncEntity& entry);


  // Extract the name field from a commit entry response.
  static const std::string& NameFromCommitEntryResponse(
      const CommitResponse_EntryResponse& entry);

  // EntitySpecifics is used as a filter for the GetUpdates message to tell
  // the server which datatypes to send back.  This adds a datatype so that
  // it's included in the filter.
  static void AddToEntitySpecificDatatypesFilter(syncable::ModelType datatype,
      sync_pb::EntitySpecifics* filter);

 private:
  SyncerProtoUtil() {}

  // Helper functions for PostClientToServerMessage.

  // Verifies the store birthday, alerting/resetting as appropriate if there's a
  // mismatch. Return false if the syncer should be stuck.
  static bool VerifyResponseBirthday(syncable::Directory* dir,
      const sync_pb::ClientToServerResponse* response);

  // Pull the birthday from the dir and put it into the msg.
  static void AddRequestBirthday(syncable::Directory* dir,
                                 ClientToServerMessage* msg);

  // Post the message using the scm, and do some processing on the returned
  // headers. Decode the server response.
  static bool PostAndProcessHeaders(browser_sync::ServerConnectionManager* scm,
                                    browser_sync::AuthWatcher* authwatcher,
                                    ClientToServerMessage* msg,
                                    sync_pb::ClientToServerResponse* response);

  friend class SyncerProtoUtilTest;

  FRIEND_TEST(SyncerProtoUtilTest, AddRequestBirthday);
  FRIEND_TEST(SyncerProtoUtilTest, PostAndProcessHeaders);
  FRIEND_TEST(SyncerProtoUtilTest, VerifyResponseBirthday);


  DISALLOW_COPY_AND_ASSIGN(SyncerProtoUtil);
};

}  // namespace browser_sync

#endif  // CHROME_BROWSER_SYNC_ENGINE_SYNCER_PROTO_UTIL_H_
