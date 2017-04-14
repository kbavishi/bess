#ifndef BESS_PKTBATCH_H_
#define BESS_PKTBATCH_H_

#include "utils/copy.h"

namespace bess {

class Packet;

class PacketBatch {
 public:
  int cnt() const { return cnt_; }
  void set_cnt(int cnt) { cnt_ = cnt; }
  void incr_cnt(int n = 1) { cnt_ += n; }

  Packet *const *pkts() const { return pkts_; }
  Packet **pkts() { return pkts_; }

  void clear() { cnt_ = 0; }

  // WARNING: this function has no bounds checks and so it's possible to
  // overrun the buffer by calling this. We are not adding bounds check because
  // we want maximum GOFAST.
  void add(Packet *pkt) { pkts_[cnt_++] = pkt; }

  bool empty() { return (cnt_ == 0); }

  bool full() { return (cnt_ == kMaxBurst); }

  void Copy(const PacketBatch *src) {
    cnt_ = src->cnt_;
    bess::utils::Copy(pkts_, src->pkts_, cnt_ * sizeof(Packet *));
  }

  static const size_t kMaxBurst = 32;

 private:
  int cnt_;
  Packet *pkts_[kMaxBurst];
};

static_assert(std::is_pod<PacketBatch>::value, "PacketBatch is not a POD Type");

}  // namespace bess

#endif  // BESS_PKTBATCH_H_
