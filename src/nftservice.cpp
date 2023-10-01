//nftservice.cpp

ACTION nftscribe::nftregister(const name& auth, const name& suffix, const name& network_id, const string& nftcontract, const uint64_t& nft_qty, const vector <string>& contracts, const name& admin, const string& evm_owner, const string& website,  const string& admin_email) {
    require_auth(auth);

    checkfreeze();

    check(evm_owner == "", "Public ACTION nftregister cannot specify the evm_owner string. ");


    _nftregister(auth, suffix, network_id, nftcontract, nft_qty, contracts, admin, evm_owner, website, admin_email);
}

void nftscribe::_nftregister(const name& auth, const name& suffix, const name& network_id, const string& nftcontract, const uint64_t& nft_qty, const vector <string>& contracts, const name& admin, const string& evm_owner, const string& website,  const string& admin_email) {
    
    if(evm_owner == "") {
        check(is_account(admin), "The specified admin account " + admin.to_string() + " does not exist. ");
    } else {
        //verify is registered oracle
        check(is_oracle(auth, network_id), "Only registered oracles may update existing records with evm_owner public key set. "); 
    }

    //--------------------------------------------------------------------------------------------------
    //Charge fees using _calc_register_cost(suffix) to calculate fees
    //Ensure prefunding balance is deposited into contract using 
    asset deposit = getdepamt(auth);
    asset fee = _calc_register_cost(suffix);
    asset prefund = _calc_prefund(nft_qty);
    asset total_dep = fee + prefund;

    check(fee.amount >= 0,  "Oracle fee setting _calc_register_cost returned zero cost, error. ");
    check(deposit.amount >= prefund.amount, "You must first deposit " + prefund.to_string() + " into account " + get_self().to_string() + " before you are able to register an NFT service. ");

    if(fee.amount > 0) { //charge NFT Service registration fee
        if(deposit.amount == fee.amount) {
            deldeposit(auth);
        } else {
            subdeposit(auth, fee);
        }

        asset prior_balance = getglobalast(name("acct.ramfees"));
        asset new_balance = system_asset(prior_balance.amount - fee.amount);
        setglobalast(name("acct.ramfees"), new_balance);
    }
    //--------------------------------------------------------------------------------------------------


    check(nftcontract.size() <= 128, "nftcontract addresses are limited to 128 bytes long. ");
    check(contracts.size() <= 5, "Too many contracts vector is too large, limit of 5 contracts in total. ");
    check(website.size() <= 256, "website is too large, limit of 256 characters. ");
    check(admin_email.size() <= 64, "admin_email is too large, limit of 64 characters. ");

    for(int i=0; i < contracts.size(); i++) {
        check(contracts[i].size() <= 128, "Contract addresses are limited to 128 bytes long. ");
    }

    name scope = network_id; //netwwork_id from networks TABLE is scope
    nftsrv_index _nftsrv_table( get_self(), scope.value );
    auto nftsrv_itr = _nftsrv_table.find(suffix.value);

    if(nftsrv_itr == _nftsrv_table.end()){
        //new registration
        _nftsrv_table.emplace( get_self(), [&]( auto& row ) {
            row.suffix      = suffix;
            row.nftcontract = nftcontract;
            row.nft_qty     = nft_qty;
            row.contracts   = contracts;
            row.active      = 1;
            row.official    = 0;
            row.updates     = 0;
            row.admin       = admin;
            row.evm_owner   = evm_owner;
            row.website     = website;
            row.admin_email = admin_email;

            vector <struct_token> tokens;
            tokens.push_back(struct_token(getglobalint(name("tokentbl.id")), name(getglobalstr(name("tokencontr"))), system_asset(0)));
            row.tokens = tokens;
        });

    } else { //update existing registration

        //verify admin authority
        if(evm_owner == "") {
            check(auth.value == nftsrv_itr->admin.value, "Only the NFT service admin may update the settings. ");
            require_auth(nftsrv_itr->admin);
        }  //else is already verified as an oracle, so they would be updating via post

        check(nftsrv_itr->nftcontract == nftcontract, "You cannot update the NFT Contract (nftcontract), it is a permanent fixed value after registration. ");

        _nftsrv_table.modify( nftsrv_itr, get_self(), [&]( auto& row ) {
            row.contracts = contracts;
            row.website = website;
            row.admin_email = admin_email;
        });
        
    }
}

ACTION nftscribe::nftactive(const name& auth, const name& suffix, const name& network_id, const uint8_t& active) {
    require_auth(auth);

    checkfreeze();

    _nftactive(auth, suffix, network_id, active);
}

void nftscribe::_nftactive(const name& auth, const name& suffix, const name& network_id, const uint8_t& active) {

    name scope = network_id; 
    nftsrv_index _nftsrv_table( get_self(), scope.value );
    auto nftsrv_itr = _nftsrv_table.find(suffix.value);
    
    check((active == 0) || (active == 1), "The active variable was set to a value out of bounds. ");

    if(nftsrv_itr == _nftsrv_table.end()){
        check(false, "NFT service with specified suffix does not exist. ");
    } else {
        //verify admin authority
        check(auth.value == nftsrv_itr->admin.value, "Only the NFT service admin may update the settings. ");
        require_auth(nftsrv_itr->admin);

        _nftsrv_table.modify( nftsrv_itr, get_self(), [&]( auto& row ) {
            row.active = active;
        });
    }
}

ACTION nftscribe::nftaddtoken(const name& auth, const name& suffix, const name& network_id, const uint64_t& token_id) {
    require_auth(auth);

    checkfreeze();

    _nftaddtoken(auth, suffix, network_id, token_id);
}

void nftscribe::_nftaddtoken(const name& auth, const name& suffix, const name& network_id, const uint64_t& token_id) {
    name scope = network_id; 
    nftsrv_index _nftsrv_table( get_self(), scope.value );
    auto nftsrv_itr = _nftsrv_table.find(suffix.value);

    if(nftsrv_itr == _nftsrv_table.end()){
        check(false, "NFT service with specified suffix does not exist. ");
    } else {
        //verify admin authority
        check(auth.value == nftsrv_itr->admin.value, "Only the NFT service admin may update the settings. ");
        require_auth(nftsrv_itr->admin);

        //verify there are not too many tokens in the project, max 10
        uint64_t token_max = GLOBAL_TOKEN_MAX;
        check(nftsrv_itr->tokens.size() < token_max, "This NFT service cannot add more tokens, it has reached the max limit of " + to_string(token_max) + ". ");

        //fetch token data
        struct_token cToken = gettoken(token_id);

        _nftsrv_table.modify( nftsrv_itr, get_self(), [&]( auto& row ) {

            //check for duplicate ID's, prevent adding twice
            int i = 0;
            while(i < row.tokens.size()) {
                if(row.tokens[i].id == token_id) {
                    check(false, "This NFT service already has the specified token_id. ");
                }
                i++;
            }

            row.tokens.push_back(struct_token(cToken.id, cToken.contract, 0, cToken.token.symbol.code().to_string(), cToken.token.symbol.precision()) );
        });
    }
}

ACTION nftscribe::nftdeltoken(const name& auth, const name& suffix, const name& network_id, const uint64_t& token_id) {
    require_auth(auth);

    checkfreeze();

    _nftdeltoken(auth, suffix, network_id, token_id);
}

void nftscribe::_nftdeltoken(const name& auth, const name& suffix, const name& network_id, const uint64_t& token_id) {
    name scope = network_id; 
    nftsrv_index _nftsrv_table( get_self(), scope.value );
    auto nftsrv_itr = _nftsrv_table.find(suffix.value);

    if(nftsrv_itr == _nftsrv_table.end()){
        check(false, "NFT service with specified suffix does not exist. ");
    } else {
        //verify admin authority
        check(auth.value == nftsrv_itr->admin.value, "Only the NFT service admin may update the settings. ");
        require_auth(nftsrv_itr->admin);

        //fetch token data
        struct_token cToken = gettoken(token_id);

        vector <struct_token> vTokens;

        _nftsrv_table.modify( nftsrv_itr, get_self(), [&]( auto& row ) {
            
            bool found = false;
            int i = 0;
            while(i < row.tokens.size()) {
                if(row.tokens[i].id == token_id) {
                    check(row.tokens[i].token.amount == 0, "An NFT service may only delete tokens that have an absolute balance of zero for the project. ");
                    found = true;
                    //do nothing, just doing this will remove the element
                } else {
                    vTokens.push_back(row.tokens[i]);
                }
                i++;
            }
            check(found, "Token id was not found inside vector<struct_token> to be removed. ");
            row.tokens = vTokens;
        });
    }
}

// Increase the balance of a specific token in the given NFT service
void nftscribe::_nft_incr_token(const name& suffix, const name& network_id, const struct_token& cToken) {
    nftsrv_index nft_services(_self, network_id.value);
    auto srv_itr = nft_services.find(suffix.value);

    // Ensure NFT service exists
    eosio::check(srv_itr != nft_services.end(), "NFT service not found.");

    nft_services.modify(srv_itr, _self, [&](auto& service) {
        // Look for the token in the service's tokens list
        auto token_itr = std::find_if(service.tokens.begin(), service.tokens.end(), 
                                        [&](const struct_token& t) { return t.id == cToken.id; });

        // If the token is found, increase its balance. Otherwise, add the token to the list.
        if (token_itr != service.tokens.end()) {
            token_itr->token += cToken.token;
        } else {
            service.tokens.push_back(cToken);
        }
    });
}

// Decrease the balance of a specific token in the given NFT service
void nftscribe::_nft_decr_token(const name& suffix, const name& network_id, const struct_token& cToken) {
    nftsrv_index nft_services(_self, network_id.value);
    auto srv_itr = nft_services.find(suffix.value);

    // Ensure NFT service exists
    eosio::check(srv_itr != nft_services.end(), "NFT service not found.");

    nft_services.modify(srv_itr, _self, [&](auto& service) {
        // Look for the token in the service's tokens list
        auto token_itr = std::find_if(service.tokens.begin(), service.tokens.end(), 
                                        [&](const struct_token& t) { return t.id == cToken.id; });

        // If the token is found, decrease its balance. If the balance becomes 0 or negative, remove the token.
        eosio::check(token_itr != service.tokens.end(), "Token not found in NFT service.");
        token_itr->token -= cToken.token;

        if (token_itr->token.amount <= 0) {
            service.tokens.erase(token_itr);
        }
    });
}

asset nftscribe::_calc_register_cost(name suffix) {
    size_t suffix_len = suffix.to_string().size();

    // Check for invalid suffix lengths
    check(suffix_len != 0, "Suffix length cannot be 0.");
    check(suffix_len <= 6, "Suffix length cannot be greater than 6.");

    asset price_by_length;
    if (suffix_len == 1) {
        price_by_length = getglobalast(name("reg.price.1"));
    } else if (suffix_len == 2) {
        price_by_length = getglobalast(name("reg.price.2"));
    } else if (suffix_len == 3) {
        price_by_length = getglobalast(name("reg.price.3"));
    } else if (suffix_len == 4) {
        price_by_length = getglobalast(name("reg.price.4"));
    } else if (suffix_len == 5) {
        price_by_length = getglobalast(name("reg.price.5"));
    } else {
        price_by_length = getglobalast(name("reg.price.a"));
    }

    return price_by_length;
}


asset nftscribe::_calc_prefund(uint64_t nft_qty) {

    asset nft_cost = getglobalast(name("reg.cost.nft")) * nft_qty;

    double pre_percentage = static_cast<double>(getglobalint(name("reg.pre.per"))) / 100;

    asset total_cost = nft_cost * pre_percentage;

    return total_cost;
}


bool nftscribe::has_token_in_nftservice(const name& network_id, const name& suffix, const uint64_t& id) {
    // Create an index to the "nftservices" table
    nftsrv_index nftservices(get_self(), network_id.value);

    // Find the service using the provided suffix
    auto service_itr = nftservices.find(suffix.value);
    check(service_itr != nftservices.end(), "NFT service with provided suffix not found");

    // Iterate over all tokens in the found service
    for (const auto& token : service_itr->tokens) {
        if (token.id == id) {
            return true;
        }
    }

    return false;
}

nftscribe::struct_token nftscribe::get_token_from_nftservice(const name& network_id, const name& suffix, const uint64_t& id) {
    // Create an index to the "nftservices" table
    nftsrv_index nftservices(get_self(), network_id.value);

    // Find the service using the provided suffix
    auto service_itr = nftservices.find(suffix.value);
    check(service_itr != nftservices.end(), "NFT service with provided suffix not found");

    // Iterate over all tokens in the found service
    for (const auto& token : service_itr->tokens) {
        if (token.id == id) {
            return token;
        }
    }

    check(false, "Token with the given id not found");
    return struct_token();  // This line will never be reached, but it's here to silence compiler warnings
}
