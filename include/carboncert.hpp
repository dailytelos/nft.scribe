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

    const name name_null = name("name.nullaaaa");

    #include "./constants.hpp"
    #include "./utility.hpp"

    #include "./auth_struct.hpp"
    #include "./cert_struct.hpp"

    #include "./manage.hpp"
    #include "./auths.hpp"
    #include "./transfer.hpp"


struct retiredcert {
    uint64_t        certnum;
    string          certid;
    asset           qtyretired;

    EOSLIB_SERIALIZE(retiredcert, (certnum)(certid)(qtyretired));
};
  
  public:
/*
    ACTION draftcert(const name& issuer, const string& certid, const string& certname, const string& extid, const time_point_sec& tstart, const time_point_sec& tend);
    ACTION apprcert(const name& authuser, const uint64_t& certnum); */

    ACTION issuecert(const name& issuer, const string& certid, const asset& supply);

    ACTION claimcert(const name& issuer, const uint64_t& certn);

    ACTION retirefunds(const name& sender, const asset& supply);
    
  private:

    void newcert(const name& issuer, const string& certid, const asset& supply);
    void getcertfunds(const name& issuer, const uint64_t& certn);
    void retiredep(const name& sender, const asset& supply, const uint64_t& orgid);
    asset retireamount(const uint64_t& certn, const asset& supply);
    void retirestat(const uint64_t& orgid, const asset& supplyret);
    void issuestat(const uint64_t& orgid, const asset& supplyiss);
    uint64_t getdataval(const uint64_t& year, const uint64_t& month);
    void reducedep(const name& sender, const asset& supply);

    TABLE certificate {
      uint64_t  certnum; //incrementor for cert# in smart contract
      name      issuer;  //account that issued token
      string    certid;  //ID specified by the certificate provider, limit to 256
      asset     supply;  // asset to specify cert token supply created
      bool      claimed; //false if funds remain unclaimed, true if claimed
      asset     qtyretired;  // asset qty to specify retirement of cert tokens
      bool      retired;     // true when fully retired
      time_point_sec issuedate; //moment certificate was issued
      time_point_sec claimdate; //moment certificate was claimed
      time_point_sec retiredate; //date when set to retired = true

      // composite id -- ((uint128_t)issuer.value << 64)|(uint128_t)certnum;
      //auto primary_key() const { return ((uint128_t)issuer.value << 64)|(uint128_t)certnum; };
      uint64_t primary_key() const { return certnum; }
    };

    typedef multi_index<name("certificates"), certificate> certs_index;

    TABLE retirements {
      uint64_t retirenum;     //incrementor for retire # for user account
      uint64_t orgid;         //organisation id performing retirement
      name account;           //account issuing retirements
      asset supplyret;        //asset amount retired
      vector<carboncert::retiredcert>    certs;  //vector of retired certificate id's inside this retirement
      time_point_sec retiredate; //moment retirement commenced

      auto primary_key() const { return retirenum; };
    };

    typedef multi_index<name("retired"), retirements> retire_index;


    TABLE certstats {
      uint32_t orgid;      //organisation id performing retirement
      uint32_t dateval;    //tracks month/year: 2022012 (12/2022)  2024001 (01/2024)
      asset supplystat;     // asset amount retired

      uint64_t primary_key() const { return ((uint64_t)orgid << 32)|(uint64_t)dateval; };
    };

    typedef multi_index<name("issuestat"), certstats> issuestat_index;
    typedef multi_index<name("retirestat"), certstats> retirestat_index;
};