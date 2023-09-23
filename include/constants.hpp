
//Set DEBUG_MODE (1= yes debug mode, 0= production)

//#define DEBUG_MODE 0 //production mode
#define DEBUG_MODE 1 //yes debug mode

//In debug mode, DEBUG_FAKE_BP_VOTES will be the bp votes for any account
#define DEBUG_FAKE_BP_VOTES 50000000


//NAME STRING FOR "NULL" Placeholder name
#define NAME_NULL "name.null...a"

//Used within struct_oracle
#define ORACLE_STATUS_BANNED -2
#define ORACLE_STATUS_KICKED -1
#define ORACLE_STATUS_INACTIVE 0
#define ORACLE_STATUS_ACTIVE 1

#define ORACLE_APPROVAL_RATIO 0.70

#define GLOBAL_TOKEN_MAX 10
#define GLOBAL_RECENT_TRX_MAX 10
#define GLOBAL_MAX_TRX_SECONDS 180