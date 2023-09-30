//_nftscribe.hpp

#include <eosio/eosio.hpp>
#include <eosio/binary_extension.hpp>
#include <eosio/time.hpp>
#include <eosio/transaction.hpp>
#include <eosio/action.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>

#include <eosio/privileged.hpp>

#include "./constants.hpp"

using namespace std;
using namespace eosio;

CONTRACT nftscribe : public contract {

  public:
    using contract::contract;

    const name name_null = name(NAME_NULL);
        
    #include "./utility.hpp"

    #include "./structs/struct_token.hpp"
    #include "./structs/struct_oracle.hpp"
    #include "./structs/struct_post.hpp"
    #include "./structs/struct_nft_user.hpp"

    #include "./manage.hpp"
    #include "./tokens.hpp"
    #include "./producers_tbl.hpp"
    #include "./networks.hpp"
    #include "./nftservice.hpp"
    #include "./nftusers.hpp"
    #include "./posts.hpp"
    #include "./oracles.hpp"

    #include "./transfer.hpp"

};