// Authorization Levels
// Values comprised of uint8_t
// Higher value equals more authority, 255 being max admin authority, 0 being view authority
const uint8_t AUTH_LEVEL_VIEWER         = 0;    //very limited
const uint8_t AUTH_LEVEL_CORP_ADMIN     = 41;   //issue / retire certificates

const uint8_t AUTH_LEVEL_ROOTADMIN      = 255;  //highest administrator level of contract
