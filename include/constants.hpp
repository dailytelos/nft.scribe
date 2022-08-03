    // DATA Types for tbldata.d.header.type
    const name DATA_TYPE_CERT_EBC  = name("cert.ebc");
    const name DATA_TYPE_CERT_PRO  = name("cert.prod");
    const name DATA_TYPE_CERT_SNK  = name("cert.csink");
    const name DATA_TYPE_CERT_SNKI = name("cert.csinkis");
    const name DATA_TYPE_PORTF     = name("portfolio");
    const name DATA_TYPE_ACT_SEND  = name("action.send");
    const name DATA_TYPE_ACT_RETR  = name("action.retire");


    //certn counting vars to count certificates (globals)
    const name GLOBAL_COUNT_EBC     = name("certn.ebc");
    const name GLOBAL_COUNT_PRO     = name("certn.prod");
    const name GLOBAL_COUNT_SNK     = name("certn.csink");
    const name GLOBAL_COUNT_SNKI    = name("certn.csinki");
    const name GLOBAL_COUNT_PRT     = name("portfolio");
    const name GLOBAL_COUNT_SND     = name("count.send");
    const name GLOBAL_COUNT_RET     = name("count.retire");
    const name GLOBAL_COUNT_RETTAIL = name("count.rttail");



// Authorization Levels


// Values comprised of uint8_t
// Higher value equals more authority, 255 being max admin authority, 0 being view authority
const uint8_t AUTH_LEVEL_VIEWER         = 0;    //not used

const uint8_t AUTH_LEVEL_CORP_SUBMIT    = 41;   //issue / retire certificates authorisation
const uint8_t AUTH_LEVEL_CORP_APPROVE   = 61;   //send funds initation
const uint8_t AUTH_LEVEL_CORP_ADMIN     = 71;   //normal org administrator account (does both submit and approve)

const uint8_t AUTH_ADMIN_AUTO_APPROVE   = 211;  //limited authority for oracle to auto-approve certain activities
const uint8_t AUTH_ADMIN_MANL_APPROVE   = 215;  //limited authority for admin to approve actions (such as token issues, etc.)
const uint8_t AUTH_ADMIN_MASTER         = 221;  //Full authority to do all things

const uint8_t AUTH_LEVEL_ROOTADMIN      = 255;  //highest administrator level of contract, can assign AUTH_ADMIN_ADMIN_ROLES permission and all other actions


//STATUS NONE
const uint8_t STATUS_NONE                 = 0;

//CERT STATUS
const uint8_t STATUS_DATA_LOCKED          = 3;
const uint8_t STATUS_DATA_DELETION        = 4;

const uint8_t STATUS_DATA_DRAFT           = 40;
const uint8_t STATUS_DATA_SUBMIT          = 41;
const uint8_t STATUS_DATA_CORP_APPROVED   = 61;
const uint8_t STATUS_DATA_ADMIN_APPROVED  = 201;
const uint8_t STATUS_DATA_EXECUTED        = 255;

//GLOBALS FOR ACTIVITIES
    //Activity Types -- pass string as appr_type variable
   /* const string ACTIVITY_DRAFT         = "draft";
    const string ACTIVITY_SUBMIT        = "submit";
    const string ACTIVITY_APPROVE       = "approve";
    const string ACTIVITY_ISSUE         = "issue";
    const string ACTIVITY_CLAIM         = "claim";
    const string ACTIVITY_LOCK          = "locked.status";
    const string ACTIVITY_DELETE_STATUS = "del.status";
    const string ACTIVITY_DELETE_TOTAL  = "del.total";*/  // can be used on drafts to fully delete, otherwise can only be used by admins
                                                        // can not be used on a cert that issued any funds, use "del.status" on that

//ADMIN ORG ID
const uint64_t ORG_ADMIN_ID = 5;