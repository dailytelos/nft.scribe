// Authorization Levels
// Values comprised of uint8_t
// Higher value equals more authority, 255 being max admin authority, 0 being view authority
const uint8_t AUTH_LEVEL_VIEWER         = 0;    //very limited
//const uint8_t AUTH_LEVEL_SEND           = 21;   //allows for sending of token
const uint8_t AUTH_LEVEL_RETIRE         = 40;   //retire certificate permission
const uint8_t AUTH_LEVEL_ISSUE          = 41;   //issue certificate permission

//const uint8_t AUTH_LEVEL_ORG_APPROVAL   = 200;  //for implementing tierd approvals

const uint8_t AUTH_LEVEL_ROOTADMIN      = 255;  //highest administrator level of contract
