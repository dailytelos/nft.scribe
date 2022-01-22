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
    #include "./manage.hpp"
  
  public:

    ACTION issuecert(const name& issuer, const string& certid, const asset& supply);

    ACTION claimcert(const name& issuer, const uint64_t& certn);
    
  private:

    //void newcert(const name& issuer, const string& certid, const asset& supply);
    //void getcertfunds(const name& issuer, const uint64_t& certn);

    TABLE certificate {
      uint64_t  certnum; //incrementor for cert# in smart contract
      name      issuer;  //account that issued token
      string    certid;  //ID specified by the certificate provider, limit to 256
      asset     supply;  // asset to specify cert token supply created
      bool      claimed; //false if funds remain unclaimed, true if claimed

      // composite id -- ((uint128_t)issuer.value << 64)|(uint128_t)certnum;
      //auto primary_key() const { return ((uint128_t)issuer.value << 64)|(uint128_t)certnum; };
      uint64_t primary_key() const { return certnum; }
    };

    typedef multi_index<name("certificates"), certificate> certs_index;
};