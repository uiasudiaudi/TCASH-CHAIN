#pragma once

namespace tcash {

namespace elect {

enum ElectVhostSendErrorCode {
    kVhostSendSuccess = 0,
    kVhostSendFailed = 1,
    kVhostSendSrcInvalid = 2,
    kVhostSendDstInvalid = 3,
    kVHostSendWrouterFailed = 4,
};

}  // namespace elect

}  // namespace tcash
