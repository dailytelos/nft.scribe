// Authorization Levels
// Values comprised of uint8_t
// Higher value equals more authority, 255 being max admin authority, 0 being view authority
const uint8_t AUTH_LEVEL_VIEWER         = 0;    //very limited
const uint8_t AUTH_LEVEL_CORP_CERTS     = 41;   //issue / retire certificates authorisation
const uint8_t AUTH_LEVEL_CORP_SEND      = 51;   //send funds initation
const uint8_t AUTH_LEVEL_CORP_APPROVE   = 61;   //2nd approver for corporate actions
const uint8_t AUTH_LEVEL_CORP_ADMIN     = 121;  //user roles assignment

const uint8_t AUTH_ADMIN_CORP_ROLES     = 191;  //user can change / assign user roles for CORPs
const uint8_t AUTH_ADMIN_CERTS          = 201;  //user can be final approval for certs - bluefield admin
const uint8_t AUTH_ADMIN_SEND           = 211;  //user can be final approval for send actions -- bluefield admin (ie. large value bridge actions)
const uint8_t AUTH_ADMIN_APPROVALS      = 215;  //user can be final approval for certs and send actions -- bluefield admin
const uint8_t AUTH_ADMIN_ADMIN_ROLES    = 221;  //user can change all admin and CORP roles

const uint8_t AUTH_LEVEL_ROOTADMIN      = 255;  //highest administrator level of contract, can assign AUTH_ADMIN_ADMIN_ROLES permission and all other actions


//STATUS NONE
const uint8_t STATUS_NONE                 = 0;

//CERT STATUS
const uint8_t STATUS_CERT_DRAFT           = 40;
const uint8_t STATUS_CERT_SUBMIT          = 41;
const uint8_t STATUS_CERT_CORP_APPROVED   = 61;
const uint8_t STATUS_CERT_ADMIN_APPROVED  = 201;

//SEND STATUS
const uint8_t STATUS_SEND_DRAFT           = 50;
const uint8_t STATUS_SEND_SUBMIT          = 51;
const uint8_t STATUS_SEND_CORP_APPROVED   = 61;
const uint8_t STATUS_SEND_ADMIN_APPROVED  = 211;