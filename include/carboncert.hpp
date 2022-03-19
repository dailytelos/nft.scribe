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

    #include "./structs/auth_struct.hpp"
    #include "./structs/cert_struct.hpp"

    #include "./manage.hpp"
    #include "./auths.hpp"
    #include "./transfer.hpp"

  
  public:

    ACTION draftcertpr(const name& creator, const string& strid, const string& cname, const string& exid, const asset& tpot);

    ACTION draftcertcs( const name& creator,
                        const string& strid,
                        const uint16_t& loc,
                        const string& type,
                        const string& desc,
                        const asset& gross,
                        const asset& humidity,
                        const asset& tmin,
                        const asset& tmax,
                        const asset& tavg,
                        const uint32_t& ystart,
                        const uint32_t& yend,
                        const uint64_t& portid,
                        const uint64_t& prodid );
    
  private:

    void _draftcert_p(const name& creator, const string& strid, const string& cname, const string& exid, const asset& tpot);
    void _draftcert_c(  const name& creator,
                        const string& strid,
                        const uint16_t& loc,
                        const string& type,
                        const string& desc,
                        const asset& gross,
                        const asset& humidity,
                        const asset& tmin,
                        const asset& tmax,
                        const asset& tavg,
                        const uint32_t& ystart,
                        const uint32_t& yend,
                        const uint64_t& portid,
                        const uint64_t& prodid);


    TABLE prodcert {
      uint64_t  id;       //numeric id of activity
      cert_production c;  //production cert data

      uint64_t primary_key() const { return id; }
    };
    
    typedef multi_index<name("production"), prodcert> prodcert_index;

    TABLE csinkcert {
      uint64_t  id;       //numeric id of activity
      cert_csink c;       //csink cert data

      uint64_t primary_key() const { return id; }
    };
    
    typedef multi_index<name("csinks"), csinkcert> csinkcert_index;


};