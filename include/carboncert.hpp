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
        
    #include "./constants.hpp"
    #include "./utility.hpp"

    #include "./structs/struct_approval.hpp"
    #include "./structs/struct_header.hpp"
    #include "./structs/struct_data.hpp"

    #include "./manage.hpp"
    #include "./auths.hpp"
    #include "./orgs.hpp"
    #include "./transfer.hpp"
    #include "./datamgr.hpp"
};