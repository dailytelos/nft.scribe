#include <eosio/eosio.hpp>
#include <eosio/time.hpp>
#include <eosio/transaction.hpp>
#include <eosio/action.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>


using namespace std;
using namespace eosio;

CONTRACT carboncert : public contract {
  public:
    using contract::contract;

    const name name_null = name("name.null...a");
        
    // DATA Types for tbldata.d.header.type
    const name DATA_TYPE_CERT_EBC = name("cert.ebc");
    const name DATA_TYPE_CERT_PRO = name("cert.prod");
    const name DATA_TYPE_CERT_SNK = name("cert.csink");
    const name DATA_TYPE_PORTF    = name("portfolio");
    const name DATA_TYPE_ACT_SEND = name("action.send");

    //certn counting vars to count certificates (globals)
    const name GLOBAL_COUNT_EBC = name("certn.ebc");
    const name GLOBAL_COUNT_PRO = name("certn.prod");
    const name GLOBAL_COUNT_SNK = name("certn.csink");
    const name GLOBAL_COUNT_PRT = name("portn");
    const name GLOBAL_COUNT_SND = name("count.send");
    
    //Activity Types -- pass string as appr_type variable
    const string ACTIVITY_DRAFT         = "draft";
    const string ACTIVITY_SUBMIT        = "submit";
    const string ACTIVITY_DELETE_STATUS = "del.status";
    const string ACTIVITY_DELETE_TOTAL  = "del.total";  // can be used on drafts to fully delete, otherwise can only be used by admins
                                                        // can not be used on a cert that issued any funds, use "del.status" on that

    //vector<string> -- Data Definitions
    vector<string> VARDEF_CERT_EBC      = {"s_company","s_company_num","s_cert_num","n_tissue","n_texpire"};
    vector<string> VARDEF_CERT_PROD     = {"n_ebc_certn","a_tpot"};
    vector<string> VARDEF_CERT_CSNK     = {"n_prod_certn","n_port_certn","s_loc","s_type","s_desc","a_gross","a_humidity","a_tmin","a_tmax","a_tavg","n_ystart","n_yend","a_qtyretired","b_retired"};
    vector<string> VARDEF_DATA_PORTF    = {"s_title","s_desc","i_img","a_csinks","a_retired"};
    vector<string> VARDEF_ACT_SEND      = {"s_contract","s_from","s_to","a_qty","s_memo"};
    
    #include "./constants.hpp"
    #include "./utility.hpp"

    #include "./structs/auth_struct.hpp"
    #include "./structs/data_struct.hpp"

    #include "./manage.hpp"
    #include "./auths.hpp"
    #include "./transfer.hpp"


  public:

  // Intitial Data Creation
  ACTION datadraft(const name& creator, const name& type, const string& strid, const string& data, const string& token, const uint8_t& edit, uint64_t& id);

  // Submission / Approval / Deletion Updates to Data
  ACTION datasubmit(const name& approver, const name& type, const uint64_t& id, const string& appr_type);


  private:

  void _datadraft(const name& creator, const name& type, const string& strid, const string& data, const string& token, const uint8_t& edit, uint64_t& id);
  
  void _datasubmit(const name& approver, const name& type, const uint64_t& id, const string& appr_type);

    TABLE tbldata {
      uint64_t   id;      //numeric id of activity
      struct_data d;      //cert data structure

      uint64_t primary_key() const { return id; }
    };
    
    typedef multi_index<name("data"), tbldata> data_index; 
};